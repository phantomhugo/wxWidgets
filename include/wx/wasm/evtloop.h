/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/evtloop.h
// Author:      Hugo Armando Castellanos
// Copyright:   (c) 2023 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_EVTLOOP_H_
#define _WX_WASM_EVTLOOP_H_

class wxWasmEventSink;
class WXDLLIMPEXP_CORE wxWasmEventLoopBase : public wxEventLoopBase
{
public:
    wxWasmEventLoopBase();
    ~wxWasmEventLoopBase()=default;

    virtual int DoRun() override;
    virtual void ScheduleExit(int rc = 0) override;
    virtual bool Pending() const override;
    virtual bool Dispatch() override;
    virtual int DispatchTimeout(unsigned long timeout) override;
    virtual void WakeUp() override;
    virtual void DoYieldFor(long eventsToProcess) override;

    void ScheduleIdleCheck();
    friend void addEventFriend(int id,const std::string& eventType,int x,int y);
private:
    static std::unique_ptr<wxWasmEventSink> m_sink;
    bool m_shouldExit;
    wxDECLARE_NO_COPY_CLASS(wxWasmEventLoopBase);
};


#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIEventLoop : public wxWasmEventLoopBase
{
public:
    wxGUIEventLoop();
    ~wxGUIEventLoop() = default;
};

#endif // wxUSE_GUI

#endif // _WX_WASM_EVTLOOP_H_

