/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/tglbtn.h
// Purpose:     wxTogleButton class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TGLBTN_H_
#define _WX_WASM_TGLBTN_H_

#include "wx/tglbtn.h"


class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton();
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    virtual void SetValue(bool state) wxOVERRIDE;
    virtual bool GetValue() const wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(wxToggleButton);

};



class WXDLLIMPEXP_CORE wxBitmapToggleButton: public wxToggleButton
{
public:
    wxBitmapToggleButton();
    wxBitmapToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));


private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapToggleButton);

};

#endif // _WX_WASM_TGLBTN_H_
