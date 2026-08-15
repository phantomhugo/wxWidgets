/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/evtloop.h
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_EVTLOOP_H_
#define _WX_WASM_EVTLOOP_H_

class wxWasmEventSink;
struct wxWasmEvent;
class WXDLLIMPEXP_CORE wxWasmEventLoopBase : public wxEventLoopBase
{
public:
    wxWasmEventLoopBase();
    ~wxWasmEventLoopBase()=default;

    virtual int DoRun() override;
    // NOTE: ScheduleExit() is NOT overridden on purpose: the base class
    // method is not virtual and sets wxEventLoopBase::m_shouldExit, which is
    // the flag DoRun() reads (overriding it with a shadow flag left modal
    // loops running forever).
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual void DoYieldFor(long eventsToProcess) override;
    virtual void DoStop(int rc) override;

    friend void addEventFriend(const wxWasmEvent& event);
private:
    static std::unique_ptr<wxWasmEventSink> m_sink;
    // Exit code returned by DoRun(): set by our DoStop() (called from the
    // base ScheduleExit()). wxEventLoopBase has no such member — it lives in
    // wxEventLoopManual, which this class does not inherit from. The exit
    // FLAG is the base m_shouldExit instead (see the ScheduleExit note above).
    int m_exitcode = 0;
    wxDECLARE_NO_COPY_CLASS(wxWasmEventLoopBase);
};


// Always declared, also for base-only builds (wxUSE_GUI=0): wx/evtloop.h
// maps wxEventLoop to wxGUIEventLoop for wasm in that case.
class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxWasmEventLoopBase
{
public:
    wxGUIEventLoop();
    ~wxGUIEventLoop() = default;
};

#endif // _WX_WASM_EVTLOOP_H_

