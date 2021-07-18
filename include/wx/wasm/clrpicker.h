/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/clipbrd.h
// Purpose:     wxClipboard class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
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
    virtual void UpdateColour() wxOVERRIDE;

private:
};

#endif // _WX_WASM_CLRPICKER_H_
