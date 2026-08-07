/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/evtloop.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/wasm/evtloop.h"

// NOTE: wxAppTraits::GetEventLoopSourcesManager() is not defined here: the
// port uses the implementation from src/unix/evtloopunix.cpp (compiled as
// part of BASE_WASM_SRC), which provides a real manager based on
// wxFDIODispatcher. Defining it here too would duplicate the symbol.

#if wxUSE_EVENTLOOP_SOURCE
#include <emscripten.h>
#include <queue>
#include <string>

// Idle sleep of the main loop, in milliseconds. It bounds the worst-case
// latency between a DOM event being queued by addEvent() and its dispatch:
// 10 ms is below a single frame at 60 fps, so it is imperceptible, while
// still yielding the browser main thread between iterations.
#define wxWASM_EVTLOOP_SLEEP_MS 10

class wxWasmEventSink
{
private:
    friend class wxWasmEventLoopBase;
    friend void addEventFriend(const wxWasmEvent& event);
    std::queue<wxWasmEvent> m_pendingEvents;
public:
    void Add(const wxWasmEvent& event)
    {
        // Coalesce bursts of high-frequency events: if the last queued
        // event is of the same replaceable type for the same window,
        // overwrite it instead of letting the queue grow faster than
        // DoRun() drains it (a scroll or drag can fire dozens of DOM
        // events per second, but only the latest position matters). The
        // button slot is part of the key because for "scroll" it carries
        // the orientation: a horizontal scroll arriving in the same cycle
        // as a vertical one must not overwrite it.
        if ( (event.eventType == "scroll" || event.eventType == "mousemove") &&
             !m_pendingEvents.empty() )
        {
            wxWasmEvent& last = m_pendingEvents.back();
            if ( last.id == event.id && last.eventType == event.eventType &&
                 last.button == event.button )
            {
                last = event;
                return;
            }
        }
        m_pendingEvents.push(event);
    }
};
std::unique_ptr<wxWasmEventSink> wxWasmEventLoopBase::m_sink;
void addEventFriend(const wxWasmEvent& event)
{
    if(wxWasmEventLoopBase::m_sink.get()!=nullptr)
    {
        wxWasmEventLoopBase::m_sink->Add(event);
    }
}
extern "C"
{
    /**
     * Add an event from html
     * @param id id of the tag
     * @param eventType String containing the event type
     * @param x If applicable, the x coordinate where the event happened.
     * @param y If applicable, the y coordinate where the event happened.
     */
    // NOTE: the event is only enqueued here, not dispatched immediately.
    // Dispatching right away would run wx event handlers nested inside an
    // arbitrary DOM listener, possibly reentering the handler that triggered
    // the DOM change (e.g. setting a control value from C++ fires an "input"
    // event synchronously). The queue keeps the dispatch context flat and
    // predictable; DoRun() drains it completely each iteration, so the extra
    // latency is bounded by wxWASM_EVTLOOP_SLEEP_MS.
    EMSCRIPTEN_KEEPALIVE
    void addEvent(int id,const char* eventType,int x,int y)
    {
        wxWasmEvent event;
        event.id=id;
        event.eventType=eventType;
        event.x=x;
        event.y=y;
        addEventFriend(event);
    }

    /**
     * Add a mouse input event from the global DOM listeners registered in
     * wxApp::Initialize (see src/wasm/app.cpp). Same queuing discipline as
     * addEvent(); the extra payload becomes a wxMouseEvent in
     * wxWindowWasm::WasmNotifyEvent().
     * @param id id of the target window tag
     * @param eventType "mousedown", "mouseup", "mousemove", "wheel",
     *        "dblclick", "mouseenter", "mouseleave", "scroll",
     *        "setfocus" or "killfocus"
     * @param x, y coordinates relative to the target window; for "scroll",
     *        the new scrollLeft/scrollTop of the window element
     * @param button DOM MouseEvent.button (0=left, 1=middle, 2=right);
     *        for "scroll", the orientation (wxHORIZONTAL or wxVERTICAL)
     * @param buttons DOM MouseEvent.buttons bitmask
     * @param wheelDelta signed wheel rotation (multiple of 120)
     * @param mods modifier bits: 1=shift, 2=ctrl, 4=alt, 8=meta
     */
    EMSCRIPTEN_KEEPALIVE
    void addInputEvent(int id,const char* eventType,int x,int y,int button,int buttons,int wheelDelta,int mods)
    {
        wxWasmEvent event;
        event.id=id;
        event.eventType=eventType;
        event.x=x;
        event.y=y;
        event.button=button;
        event.buttons=buttons;
        event.wheelDelta=wheelDelta;
        event.mods=mods;
        addEventFriend(event);
    }

    /**
     * Add a keyboard input event from the global DOM listeners registered in
     * wxApp::Initialize (see src/wasm/app.cpp). Becomes a wxKeyEvent in
     * wxWindowWasm::WasmNotifyEvent().
     * @param id id of the focused window tag
     * @param eventType "keydown" or "keyup"
     * @param key KeyboardEvent.key value
     * @param mods modifier bits: 1=shift, 2=ctrl, 4=alt, 8=meta
     */
    EMSCRIPTEN_KEEPALIVE
    void addKeyEvent(int id,const char* eventType,const char* key,int mods)
    {
        wxWasmEvent event;
        event.id=id;
        event.eventType=eventType;
        event.key=key;
        event.mods=mods;
        addEventFriend(event);
    }
}

wxWasmEventLoopBase::wxWasmEventLoopBase():m_shouldExit(false), m_exitcode(0)
{
    m_sink.reset(new wxWasmEventSink);
}

int wxWasmEventLoopBase::DoRun()
{
    m_shouldExit=false;
    // NOTE: emscripten_set_main_loop() is not usable here: it never returns
    // (or unwinds via Asyncify) and cannot support nested event loops (modal
    // dialogs), which must run to completion and return their exit code.
    // Instead we poll the queue, draining ALL pending events each iteration
    // (so bursts are processed at full speed, not one event per cycle) and
    // then sleeping briefly to yield the browser main thread.
    while(!m_shouldExit)
    {
        while(!m_shouldExit&&Pending())
        {
            Dispatch();
        }
        // Process the events pending at wx level (wxCallAfter, wxQueueEvent)
        // and generate idle events (which in turn drive wxUpdateUIEvent).
        // wxEventLoopBase::DoYieldFor(wxEVT_CATEGORY_ALL) does exactly
        // wxTheApp->ProcessPendingEvents() plus ProcessIdle() (see
        // src/common/evtloopcmn.cpp); calling it once per cycle matches the
        // usual "idle when the queue is empty" behaviour of other ports, and
        // the 10 ms sleep below bounds the idle rate. This also covers the
        // nested modal loops (wxModalEventLoop inherits wxGUIEventLoop,
        // hence this same DoRun()).
        wxEventLoopBase::DoYieldFor(wxEVT_CATEGORY_ALL);
        emscripten_sleep(wxWASM_EVTLOOP_SLEEP_MS);
    }
    return m_exitcode;
}

void wxWasmEventLoopBase::ScheduleExit(int rc)
{
    m_shouldExit=true;
    DoStop(rc);
}

bool wxWasmEventLoopBase::Pending() const
{
    return m_sink.get()!=nullptr&&!m_sink->m_pendingEvents.empty();
}

bool wxWasmEventLoopBase::Dispatch()
{
    wxWindow* controlToNotify = wxWindow::FindWindowById(m_sink->m_pendingEvents.front().id);
    if(controlToNotify!=nullptr)
    {
        controlToNotify->WasmNotifyEvent(m_sink->m_pendingEvents.front());
    }
    //Always remove from the queue
    m_sink->m_pendingEvents.pop();
    return !m_sink->m_pendingEvents.empty();
}

int wxWasmEventLoopBase::DispatchTimeout(unsigned long timeout)
{
    // Return true if an event was dispatched, -1 if the timeout expired
    // without any event arriving (see wxEventLoopBase::DispatchTimeout()).
    if(Pending())
    {
        Dispatch();
        return 1;
    }
    emscripten_sleep(timeout);
    if(Pending())
    {
        Dispatch();
        return 1;
    }
    return -1;
}

void wxWasmEventLoopBase::DoStop(int rc)
{
    m_exitcode = rc;
}

void wxWasmEventLoopBase::WakeUp()
{
    // Nothing to do: DoRun() polls the queue every wxWASM_EVTLOOP_SLEEP_MS,
    // so WakeUp() only needs to guarantee that the loop observes new events
    // promptly, which the short sleep already does (worst case 10 ms). There
    // is no blocking wait to interrupt in this port.
}

void wxWasmEventLoopBase::DoYieldFor(long eventsToProcess)
{
    // Drain the pending native (DOM) events first, then let the base class
    // process the events pending at wx level and the idle events, as it
    // filters them according to eventsToProcess.
    while(!m_shouldExit&&Pending())
    {
        Dispatch();
    }
    wxEventLoopBase::DoYieldFor(eventsToProcess);
}

#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI

wxGUIEventLoop::wxGUIEventLoop()
{

}

#endif // wxUSE_GUI
