/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/utils.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/cursor.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/utils.h"

void wxBell()
{
}

#if wxUSE_GUI
void wxGetMousePosition( int *x, int *y )
{
    // WASM doesn't have native mouse position access from arbitrary contexts.
    if ( x ) *x = 0;
    if ( y ) *y = 0;
}
#endif

#if wxUSE_GUI
bool wxGetKeyState(wxKeyCode WXUNUSED(key))
{
    return false;
}

wxMouseState wxGetMouseState()
{
    wxMouseState ms;
    return ms;
}
#endif

wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint( pt );
}

wxWindow *wxGetActiveWindow()
{

    return nullptr;
}

bool wxLaunchDefaultApplication(const wxString& path, int WXUNUSED( flags ) )
{
}
