/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menu.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/menu.h"
#include <emscripten.h>
wxMenu::wxMenu(long style)
    : wxMenuBase( style )
{
}

wxMenu::wxMenu(const wxString& title, long style)
    : wxMenuBase( title, style )
{

}



wxMenuItem *wxMenu::DoAppend(wxMenuItem *item)
{
    // Get the previous/successive items *before* we call the base class methods,
    // because afterwards it is less clear where these items end up:

    if ( wxMenuBase::DoAppend( item ) == nullptr )
        return nullptr;
    return item;
}


wxMenuItem *wxMenu::DoInsert(size_t insertPosition, wxMenuItem *item)
{
    if ( wxMenuBase::DoInsert( insertPosition, item ) == nullptr )
        return nullptr;

    return item;
}


wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    if ( wxMenuBase::DoRemove( item ) == nullptr )
        return nullptr;


    return item;
}


void *wxMenu::GetHandle() const
{

}


//##############################################################################

wxMenuBar::wxMenuBar( long style)
{
    wxWasmCreateMenuBar(style);
    PostCreation(false);
}

void wxMenuBar::wxWasmCreateMenuBar(long style)
{
     if (!CreateBase( nullptr, -1, wxDefaultPosition, wxDefaultSize, style, wxDefaultValidator, wxT("menubar") ))
    {
        wxFAIL_MSG( wxT("wxMenuBar creation failed") );
        return;
    }

    EM_ASM_INT(
        {
            const menuBar=document.createElement("div");
            menuBar.id= $0;
            menuBar.className="wxMenuBar";
            menuBar.style.clear="both";
            const parentlessDiv=document.getElementById("wxParentlessTags");
            parentlessDiv.append(menuBar);
            return 1;
        },
        GetId()
    );
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long style)
{
    wxWasmCreateMenuBar(style);

    for ( size_t i = 0; i < count; ++i )
        Append( menus[ i ], titles[ i ] );

    PostCreation(false);
}

bool wxMenuBar::Append( wxMenu *menu, const wxString& title )
{
    if ( !wxMenuBarBase::Append( menu, title ))
        return false;

    return true;
}


bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if ( !wxMenuBarBase::Insert( pos, menu, title ))
        return false;

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu;

    if (( menu = wxMenuBarBase::Remove( pos )) == nullptr )
        return nullptr;

    return menu;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{

}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{

}


void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{

}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{

}

void wxMenuBar::Attach(wxFrame *frame)
{
    // sanity check as setMenuBar takes ownership

    wxMenuBarBase::Attach(frame);
}

void wxMenuBar::Detach()
{
    // the QMenuBar probably was deleted by Qt as setMenuBar takes ownership

    wxMenuBarBase::Detach();
}

WXWidget wxMenuBar::GetHandle() const
{

}
