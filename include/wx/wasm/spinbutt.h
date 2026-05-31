/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/spinbutt.h
// Purpose:     wxSpingButton class
// Author:      Hugo Armando Castellanos Morales
// Created:     26.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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

    virtual int GetValue() const override;
    virtual void SetValue(int val) override;
    virtual void SetRange(int min, int max) override;

    virtual WXWidget GetHandle() const override;

private:

    wxDECLARE_DYNAMIC_CLASS( wxSpinButton );
};

#endif // _WX_WASM_SPINBUTT_H_
