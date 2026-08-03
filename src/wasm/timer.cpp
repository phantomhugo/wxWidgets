/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/timer.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_TIMER

#include "wx/wasm/timer.h"

#include <emscripten.h>

#include <unordered_map>

namespace
{

// Map from the timer id to the live implementation. The id is the
// wxWasmTimerImpl pointer itself: it is unique among the live timers and
// needs no separate id generator. This map mirrors window.wxWasmTimers on
// the JS side and is used to route wxWasmTimerNotify() back to the right
// object, ignoring notifications for timers already stopped or destroyed.
std::unordered_map<intptr_t, wxWasmTimerImpl*> s_wasmTimers;

} // anonymous namespace

// Called from JS (via Module.ccall) when a timer created in
// wxWasmTimerImpl::Start() expires.
extern "C" EMSCRIPTEN_KEEPALIVE
void wxWasmTimerNotify(int id)
{
    const auto it = s_wasmTimers.find(static_cast<intptr_t>(id));
    if ( it == s_wasmTimers.end() )
        return; // the timer was stopped or destroyed before it fired

    it->second->OnFired();
}

wxWasmTimerImpl::wxWasmTimerImpl(wxTimer *timer)
    : wxTimerImpl(timer),
      m_isRunning(false)
{
}

wxWasmTimerImpl::~wxWasmTimerImpl()
{
    Stop();
}

bool wxWasmTimerImpl::Start(int milliseconds, bool oneShot)
{
    // the base class version stops the timer if it is already running and
    // updates m_milli/m_oneShot
    if ( !wxTimerImpl::Start(milliseconds, oneShot) )
        return false;

    // NB: the commas inside the JS code must be parenthesized, otherwise the
    // preprocessor would split the code into several macro arguments
    EM_ASM_({
        var id = $0;
        var ms = $1;
        var oneShot = $2;
        var timers = window.wxWasmTimers || (window.wxWasmTimers = {});
        var fire = function()
        {
            Module.ccall('wxWasmTimerNotify', null, ['number'], [id]);
        };
        var handle = oneShot ? setTimeout(fire, ms) : setInterval(fire, ms);
        timers[id] = {};
        timers[id].handle = handle;
        timers[id].oneShot = !!oneShot;
    }, reinterpret_cast<intptr_t>(this), m_milli, oneShot ? 1 : 0);

    s_wasmTimers[reinterpret_cast<intptr_t>(this)] = this;
    m_isRunning = true;

    return true;
}

void wxWasmTimerImpl::Stop()
{
    if ( !m_isRunning )
        return;

    EM_ASM_({
        var id = $0;
        var timers = window.wxWasmTimers;
        if ( timers && timers[id] )
        {
            // clearTimeout() and clearInterval() share the same id pool in
            // the browsers, so calling both works for one-shot and repeating
            // timers alike
            clearTimeout(timers[id].handle);
            clearInterval(timers[id].handle);
            delete timers[id];
        }
    }, reinterpret_cast<intptr_t>(this));

    s_wasmTimers.erase(reinterpret_cast<intptr_t>(this));
    m_isRunning = false;
}

void wxWasmTimerImpl::OnFired()
{
    if ( m_oneShot )
    {
        // a one-shot timer is no longer running once it has expired; do the
        // cleanup before notifying as the event handler may restart (or even
        // destroy) the timer
        m_isRunning = false;
        s_wasmTimers.erase(reinterpret_cast<intptr_t>(this));
        EM_ASM_({
            var timers = window.wxWasmTimers;
            if ( timers ) delete timers[$0];
        }, reinterpret_cast<intptr_t>(this));
    }

    Notify();
}

#endif // wxUSE_TIMER
