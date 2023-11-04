/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/taskbar.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/taskbar.h"

//=============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxTaskBarIcon, wxEvtHandler);

wxTaskBarIcon::wxTaskBarIcon(wxTaskBarIconType WXUNUSED(iconType))
{
}

wxTaskBarIcon::~wxTaskBarIcon()
{
}

bool wxTaskBarIcon::SetIcon(const wxBitmapBundle& WXUNUSED(icon),
             const wxString& WXUNUSED(tooltip))
{
    return false;
}

bool wxTaskBarIcon::RemoveIcon()
{
    return false;
}

bool wxTaskBarIcon::PopupMenu(wxMenu *WXUNUSED(menu))
{
    return false;
}

