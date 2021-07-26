/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/spinbutt.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
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
