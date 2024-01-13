/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/evtloop.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/wasm/evtloop.h"
#include "wx/private/eventloopsourcesmanager.h"

#if wxUSE_EVENTLOOP_SOURCE
#include <emscripten.h>
#include <queue>
#include <string>

class wxWasmEventSink
{
private:
    friend class wxWasmEventLoopBase;
    friend void addEventFriend(int id,const std::string& eventType,int x,int y);
    std::queue<wxWasmEvent> m_pendingEvents;
public:
    void Add(const wxWasmEvent& event)
    {
        m_pendingEvents.push(event);
    }
};
std::unique_ptr<wxWasmEventSink> wxWasmEventLoopBase::m_sink;
void addEventFriend(int id,const std::string& eventType,int x,int y)
{
    if(wxWasmEventLoopBase::m_sink.get()!=nullptr)
    {
        wxWasmEvent event;
        event.id=id;
        event.eventType=eventType;
        event.x=x;
        event.y=y;
        wxWasmEventLoopBase::m_sink->Add(event);
    }
}
extern "C"
{
    /**
     * Add an event from html
     * @param id id of the tag
     * @param eventType String containing the event type
     * @param x If applicable, the x coordinate where the vent happened.
     * @param y If applicable, the x coordinate where the vent happened.
     */
    void addEvent(int id,const std::string& eventType,int x,int y)
    {
        addEventFriend(id,eventType,x,y);
    }
}

wxWasmEventLoopBase::wxWasmEventLoopBase():m_shouldExit(false)
{
    m_sink.reset(new wxWasmEventSink);
}

int wxWasmEventLoopBase::DoRun()
{
    m_shouldExit=false;
    //emscripten_set_main_loop(processLoop,0,1);
    while(!m_shouldExit)
    {
        if(Pending())
        {
            Dispatch();
        }
        emscripten_sleep(200);
    }
    return true;
}

void wxWasmEventLoopBase::ScheduleExit(int rc)
{
    m_shouldExit=true;
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

}

void wxWasmEventLoopBase::WakeUp()
{

}

void wxWasmEventLoopBase::DoYieldFor(long eventsToProcess)
{

}

#if !wxUSE_CONSOLE_EVENTLOOP

// Use the GUI event loop sources manager if console support is disabled
// (needed by some common code, will raise an undefinied reference if not done)

wxEventLoopSourcesManagerBase* wxAppTraits::GetEventLoopSourcesManager()
{
    return wxAppTraits::GetEventLoopSourcesManager();
}

#endif

#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI

wxGUIEventLoop::wxGUIEventLoop()
{

}

#endif // wxUSE_GUI
