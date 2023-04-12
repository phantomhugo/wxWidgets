/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/checkbox.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
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
    m_qtCheckBox = new wxQtCheckBox( parent, this );
    m_qtCheckBox->setText( wxQtConvertString( label ) );

    // Do the initialization here as WXValidateStyle may fail in unit tests
    bool ok = QtCreateControl( parent, id, pos, size, style, validator, name );

    WXValidateStyle(&style);

    if ( style & wxCHK_2STATE )
        m_qtCheckBox->setTristate( false );
    else if ( style & wxCHK_3STATE )
        m_qtCheckBox->setTristate( true );
    if ( style & wxALIGN_RIGHT )
        m_qtCheckBox->setLayoutDirection( Qt::RightToLeft );

    return ok;
}


void wxCheckBox::SetValue(bool value)
{
    m_qtCheckBox->setChecked( value );
}

bool wxCheckBox::GetValue() const
{
    return m_qtCheckBox->isChecked();
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{

}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{

    return wxCHK_UNDETERMINED;
}

QWidget *wxCheckBox::GetHandle() const
{
    return m_qtCheckBox;
}

wxString wxCheckBox::GetLabel() const
{
    return wxQtConvertString( m_qtCheckBox->text() );
}

void wxCheckBox::SetLabel(const wxString& label)
{
    m_qtCheckBox->setText( wxQtConvertString(label) );
}

