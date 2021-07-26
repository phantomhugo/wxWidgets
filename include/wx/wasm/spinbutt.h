/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/spinbutt.h
// Purpose:     wxSpingButton class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_SPINBUTT_H_
#define _WX_WASM_SPINBUTT_H_

#include "wx/spinbutt.h"

class WXDLLIMPEXP_CORE wxSpinButton : public wxSpinButtonBase
{
public:
    wxSpinButton();
    wxSpinButton(wxWindow *parent,
                 wxWindowID id = -1,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxSP_VERTICAL,
                 const wxString& name = wxSPIN_BUTTON_NAME);

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_VERTICAL,
                const wxString& name = wxSPIN_BUTTON_NAME);

    virtual int GetValue() const wxOVERRIDE;
    virtual void SetValue(int val) wxOVERRIDE;
    virtual void SetRange(int min, int max) wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS( wxSpinButton );
};

#endif // _WX_WASM_SPINBUTT_H_
