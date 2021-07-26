/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/radiobut.h
// Purpose:     wxRadioButton class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
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

    virtual void SetValue(bool value) wxOVERRIDE;
    virtual bool GetValue() const wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

private:


    wxDECLARE_DYNAMIC_CLASS( wxRadioButton );
};

#endif // _WX_WASM_RADIOBUT_H_
