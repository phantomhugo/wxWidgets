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
    virtual void ScheduleExit(int rc = 0);
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual void DoYieldFor(long eventsToProcess) override;
    virtual void DoStop(int rc) override;

    friend void addEventFriend(const wxWasmEvent& event);
private:
    static std::unique_ptr<wxWasmEventSink> m_sink;
    bool m_shouldExit;
    int m_exitcode;
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

