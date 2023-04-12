/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/radiobut.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobut.h"

wxRadioButton::wxRadioButton()
{
}

wxRadioButton::wxRadioButton( wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxRadioButton::Create( wxWindow *parent,
             wxWindowID id,
             const wxString& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxValidator& validator,
             const wxString& name)
{

    return true;
}

void wxRadioButton::SetValue(bool value)
{

}

bool wxRadioButton::GetValue() const
{

}

void *wxRadioButton::GetHandle() const
{

}
