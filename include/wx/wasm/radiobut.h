/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_RADIOBUT_H_
#define _WX_WASM_RADIOBUT_H_

class WXDLLIMPEXP_CORE wxRadioButton : public wxRadioButtonBase
{
public:
    wxRadioButton();
    wxRadioButton( wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxRadioButtonNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxASCII_STR(wxRadioButtonNameStr) );

    virtual void SetValue(bool value) override;
    virtual bool GetValue() const override;

    virtual WXWidget GetHandle() const override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:


    wxDECLARE_DYNAMIC_CLASS( wxRadioButton );
};

#endif // _WX_WASM_RADIOBUT_H_
