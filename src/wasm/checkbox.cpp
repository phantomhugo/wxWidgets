/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/checkbox.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checkbox.h"

wxCheckBox::wxCheckBox()
{
}

wxCheckBox::wxCheckBox( wxWindow *parent, wxWindowID id, const wxString& label,
        const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
            const wxString& name )
{
    return true;
}


void wxCheckBox::SetValue(bool value)
{
}

bool wxCheckBox::GetValue() const
{

}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{

}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return wxCHK_UNDETERMINED;
}

void wxCheckBox::Press()
{

}

void wxCheckBox::Release()
{

}

wxBitmap wxCheckBox::GetBitmap(State state, Status status) const
{

}

void wxCheckBox::ChangeValue(bool value)
{

}

void wxCheckBox::SendEvent()
{

}

void wxCheckBox::OnCheck()
{

}

wxSize wxCheckBox::DoGetBestClientSize() const
{

}

wxSize wxCheckBox::GetBitmapSize() const
{

}
