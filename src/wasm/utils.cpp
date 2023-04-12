/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/utils.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
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

#if wxUSE_GUI
wxPoint wxGetMousePosition()
{
}

void wxGetMousePosition( int *x, int *y )
{
    wxPoint position = wxGetMousePosition();

    *x = position.x;
    *y = position.y;
}
#endif

#if wxUSE_GUI
wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    return ms;
}
#endif


wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    /* Another option is to use QApplication::topLevelAt()
     * but that gives the QWidget so the wxWindow list must
     * be traversed comparing with this, or use the pointer from
     * a wxQtWidget/wxQtFrame to the window, but they have
     * no standard interface to return that. */
    return wxGenericFindWindowAtPoint( pt );
}

wxWindow *wxFindWindowAtPointer(wxPoint& pt)
{
    pt = wxQtConvertPoint( QCursor::pos() );

    return wxFindWindowAtPoint( pt );
}

wxWindow *wxGetActiveWindow()
{

    return nullptr;
}

bool wxLaunchDefaultApplication(const wxString& path, int WXUNUSED( flags ) )
{
}
