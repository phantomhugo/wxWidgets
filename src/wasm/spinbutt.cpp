/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/spinbutt.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/spinbutt.h"
wxSpinButton::wxSpinButton()
{
}

wxSpinButton::wxSpinButton(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxSpinButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    // Modify the size so that the text field is not visible.
    // TODO: Find out the width of the buttons i.e. take the style into account (QStyleOptionSpinBox).
    wxSize newSize( size );
    newSize.SetWidth( 18 );

    return true;
}

void wxSpinButton::SetRange(int min, int max)
{
    wxSpinButtonBase::SetRange(min, max); // cache the values


}

int wxSpinButton::GetValue() const
{

}

void wxSpinButton::SetValue(int val)
{

}

void *wxSpinButton::GetHandle() const
{

}
