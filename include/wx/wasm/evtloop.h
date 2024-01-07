/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/evtloop.h
// Author:      Hugo Armando Castellanos
// Copyright:   (c) 2023 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_EVTLOOP_H_
#define _WX_WASM_EVTLOOP_H_

class WXDLLIMPEXP_CORE wxWasmEventLoopBase : public wxEventLoopBase
{
public:
    wxWasmEventLoopBase() = default;
    ~wxWasmEventLoopBase()=default;

    virtual int DoRun() override;
    virtual void ScheduleExit(int rc = 0) override;
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual void DoYieldFor(long eventsToProcess) override;

    void ScheduleIdleCheck();

private:

    wxDECLARE_NO_COPY_CLASS(wxWasmEventLoopBase);
};


#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxWasmEventLoopBase
{
public:
    wxGUIEventLoop();
    ~wxGUIEventLoop() = default;
    bool Dispatch() override;
    bool Pending() const override;
    void ScheduleExit(int rc = 0) override;
protected:
    // real implementation of Run()
    virtual int DoRun() override;
};

#endif // wxUSE_GUI

#endif // _WX_WASM_EVTLOOP_H_

