/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/button.h
// Purpose:     wxButton class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_BUTTON_H_
#define _WX_WASM_BUTTON_H_

#include "wx/control.h"
#include "wx/button.h"

class WXDLLIMPEXP_CORE wxButton : public wxButtonBase
{
public:
    wxButton();
    wxButton(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& label = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxButtonNameStr));

    virtual wxWindow *SetDefault() override;
    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxButton);
};


#endif // _WX_wasm_BUTTON_H_
