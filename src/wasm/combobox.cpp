/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/combobox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"
#include "wx/window.h"

void wxComboBox::SetSelection( int n )
{
    wxChoice::SetSelection( n );
}

wxComboBox::wxComboBox()
{
}

wxComboBox::wxComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, n, choices, style, validator, name );
}


wxComboBox::wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, choices, style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    const wxString *pChoices = choices.size() ? &choices[ 0 ] : nullptr;
    return Create(parent, id, value, pos, size, choices.size(), pChoices,
                  style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name )
{

}

void wxComboBox::SetActualValue(const wxString &value)
{
    if ( IsReadOnly() )
    {
        SetStringSelection( value );
    }
    else
    {
        wxTextEntry::SetValue(value);

    }
}

bool wxComboBox::IsReadOnly() const
{
    return HasFlag( wxCB_READONLY );
}

void wxComboBox::SetValue(const wxString& value)
{
    SetActualValue( value );

    if ( !IsReadOnly() )
        SetInsertionPoint( 0 );
}

void wxComboBox::ChangeValue(const wxString &value)
{
    SetValue( value );
}

void wxComboBox::AppendText(const wxString &value)
{
    SetActualValue( GetValue() + value );
}

void wxComboBox::Replace(long from, long to, const wxString &value)
{
    const wxString original( GetValue() );

    if ( to < 0 )
    {
        to = original.length();
    }

    if ( from == 0 )
    {
        SetActualValue( value + original.substr(to, original.length()) );
    }

    wxString front = original.substr( 0, from ) + value;

    long iPoint = front.length();
    if ( front.length() <= original.length() )
    {
        SetActualValue( front + original.substr(to, original.length()) );
    }
    else
    {
        SetActualValue( front );
    }
    SetInsertionPoint( iPoint );
}

void wxComboBox::WriteText(const wxString &value)
{

}

wxString wxComboBox::DoGetValue() const
{

}

void wxComboBox::Popup()
{

}

void wxComboBox::Dismiss()
{
}

void wxComboBox::Clear()
{
    if ( !IsReadOnly() )
        wxTextEntry::Clear();

    wxItemContainer::Clear();
}

void wxComboBox::SetSelection( long from, long to )
{
    if ( from == -1 )
    {
        from = 0;
    }
    if ( to == -1 )
    {
        to = GetValue().length();
    }

    SetInsertionPoint( from );
}

void wxComboBox::SetInsertionPoint( long pos )
{

}

long wxComboBox::GetInsertionPoint() const
{

}

void wxComboBox::GetSelection(long* from, long* to) const
{
    wxTextEntry::GetSelection(from, to);
}
