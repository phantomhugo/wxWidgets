/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/radiobox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobox.h"
wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);


wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, choices, majorDim, style, val, name );
}



bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{
    return Create( parent, id, title, pos, size, choices.size(), &choices[ 0 ],
        majorDim, style, val, name );

}

bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{

}

#define INVALID_INDEX_MESSAGE wxT( "invalid radio box index" )

#define CHECK_BUTTON( button, rc ) \
    wxCHECK_MSG( button != nullptr, rc, INVALID_INDEX_MESSAGE )

bool wxRadioBox::Enable(unsigned int n, bool enable)
{

    return true;
}

bool wxRadioBox::Enable( bool enable )
{

    return true;
}

bool wxRadioBox::Show(unsigned int n, bool show)
{

    return true;
}

bool wxRadioBox::Show( bool show )
{

    return true;
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{

}

bool wxRadioBox::IsItemShown(unsigned int n) const
{

}

unsigned int wxRadioBox::GetCount() const
{

}

wxString wxRadioBox::GetString(unsigned int n) const
{

}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{

}

void wxRadioBox::SetSelection(int n)
{

}

int wxRadioBox::GetSelection() const
{

}

void *wxRadioBox::GetHandle() const
{
    return nullptr;
}

