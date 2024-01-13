/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/apptraits.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
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
