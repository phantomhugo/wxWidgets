/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/evtloop.cpp
// Author:      Mariano Reingart, Peter Most, Sean D'Epagnier, Javier Torres
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/evtloop.h"
#include "wx/private/eventloopsourcesmanager.h"

#if wxUSE_EVENTLOOP_SOURCE



#if !wxUSE_CONSOLE_EVENTLOOP

// Use the GUI event loop sources manager if console support is disabled
// (needed by some common code, will raise an undefinied reference if not done)

wxEventLoopSourcesManagerBase* wxAppTraits::GetEventLoopSourcesManager()
{

}

#endif

#endif // wxUSE_EVENTLOOP_SOURCE

//#############################################################################

#if wxUSE_GUI


#endif // wxUSE_GUI
