/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menuitem.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/bitmap.h"

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}



wxMenuItem::wxMenuItem(wxMenu *parentMenu, int id, const wxString& text,
        const wxString& help, wxItemKind kind, wxMenu *subMenu)
    : wxMenuItemBase( parentMenu, id, text, help, kind, subMenu )
{

}



void wxMenuItem::SetItemLabel( const wxString &label )
{
    wxMenuItemBase::SetItemLabel( label );

}

void wxMenuItem::SetCheckable( bool checkable )
{
    wxMenuItemBase::SetCheckable( checkable );

}



void wxMenuItem::Enable( bool enable )
{
    wxMenuItemBase::Enable( enable );

}



bool wxMenuItem::IsEnabled() const
{

}



void wxMenuItem::Check( bool checked )
{
    wxMenuItemBase::Check( checked );

}



bool wxMenuItem::IsChecked() const
{

}

void wxMenuItem::SetBitmap(const wxBitmapBundle& bitmap)
{

}

void wxMenuItem::SetFont(const wxFont& font)
{
}
