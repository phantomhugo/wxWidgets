/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/apptraits.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/apptrait.h"
#include "wx/stdpaths.h"
#include "wx/evtloop.h"

#include "wx/timer.h"

// wxEventLoopBase *wxConsoleAppTraits::CreateEventLoop()
// {
//     printf("En CreateEventLoop\n");
//     return new wxEventLoop();
// }

wxEventLoopBase *wxGUIAppTraits::CreateEventLoop()
{
    return new wxEventLoop();
}

#if wxUSE_TIMER
wxTimerImpl *wxGUIAppTraits::CreateTimerImpl(wxTimer *timer)
{
    return nullptr;
}
#endif

// #if wxUSE_THREADS
// void wxGUIAppTraits::MutexGuiEnter()
// {
// }
//
// void wxGUIAppTraits::MutexGuiLeave()
// {
// }

// #endif

// #if wxUSE_CONSOLE_EVENTLOOP && wxUSE_EVENTLOOP_SOURCE
// wxEventLoopSourcesManagerBase* wxGUIAppTraits::GetEventLoopSourcesManager()
// {
//
// }
// #endif

wxPortId wxGUIAppTraits::GetToolkitVersion(int *majVer,
                                           int *minVer,
                                           int *microVer) const
{
    if ( majVer )
        *majVer = 0;
    if ( minVer )
        *minVer = 0;
    if ( microVer )
        *microVer = 0;

    return wxPORT_WASM;
}

wxString wxGUIAppTraits::GetDesktopEnvironment() const
{
    return wxEmptyString;
}

int wxGUIAppTraits::WaitForChild(wxExecuteData& WXUNUSED(execData))
{
    return -1;
}
