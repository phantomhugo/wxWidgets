/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/clrpicker.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/clrpicker.h"

wxColourPickerWidget::wxColourPickerWidget()
{
}

wxColourPickerWidget::wxColourPickerWidget(wxWindow *parent,
               wxWindowID id,
               const wxColour& initial,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name )
{
    Create( parent, id, initial, pos, size, style, validator, name );
}


bool wxColourPickerWidget::Create(wxWindow *parent,
            wxWindowID id,
            const wxColour& initial,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    return wxGenericColourButton::Create( parent, id, initial, pos, size, style, validator, name );
}


void wxColourPickerWidget::UpdateColour()
{
    wxGenericColourButton::UpdateColour();
}

