/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/clipbrd.h
// Purpose:     wxClipboard class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CLRPICKER_H_
#define _WX_WASM_CLRPICKER_H_

#include "wx/generic/clrpickerg.h"

class WXDLLIMPEXP_CORE wxColourPickerWidget : public wxGenericColourButton
{
public:
    wxColourPickerWidget();
    wxColourPickerWidget(wxWindow *parent,
                   wxWindowID id,
                   const wxColour& initial = *wxBLACK,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCLRBTN_DEFAULT_STYLE,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxColour& initial = *wxBLACK,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

protected:
    virtual void UpdateColour() override;

private:
};

#endif // _WX_WASM_CLRPICKER_H_
