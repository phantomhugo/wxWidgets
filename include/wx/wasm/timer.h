/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/timer.h
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TIMER_H_
#define _WX_WASM_TIMER_H_

#include "wx/private/timer.h"

#if wxUSE_TIMER

// ----------------------------------------------------------------------------
// wxWasmTimerImpl: wxTimer implementation based on JS setTimeout/setInterval
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWasmTimerImpl : public wxTimerImpl
{
public:
    wxWasmTimerImpl(wxTimer *timer);
    virtual ~wxWasmTimerImpl();

    virtual bool Start(int milliseconds = -1, bool oneShot = false) override;
    virtual void Stop() override;
    virtual bool IsRunning() const override { return m_isRunning; }

    // called from the exported wxWasmTimerNotify() when the JS timer expires
    void OnFired();

private:
    bool m_isRunning;

    wxDECLARE_NO_COPY_CLASS(wxWasmTimerImpl);
};

#endif // wxUSE_TIMER

#endif // _WX_WASM_TIMER_H_
