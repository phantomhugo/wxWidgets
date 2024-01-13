///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/apptrait.h
// Purpose:     class implementing wxAppTraits for WASM
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 Hugo Castellanos
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_APPTRAIT_H_
#define _WX_WASM_APPTRAIT_H_

// ----------------------------------------------------------------------------
// wxGUI/ConsoleAppTraits: must derive from wxAppTraits, not wxAppTraitsBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxConsoleAppTraits : public wxConsoleAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
};

#if wxUSE_GUI

class WXDLLIMPEXP_CORE wxGUIAppTraits : public wxGUIAppTraitsBase
{
public:
    virtual wxEventLoopBase *CreateEventLoop() override;
#if wxUSE_TIMER
    virtual wxTimerImpl *CreateTimerImpl(wxTimer *timer) override;
#endif // wxUSE_TIMER
    int WaitForChild(wxExecuteData& execData) override;
    wxPortId GetToolkitVersion(int *majVer = nullptr,
                               int *minVer = nullptr,
                               int *microVer = nullptr) const override;

};

#endif // wxUSE_GUI

#endif // _WX_WASM_APPTRAIT_H_

