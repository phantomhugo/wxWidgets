/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/bmpbuttn.h
// Purpose:     wxBitmapButton class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_BMPBUTTN_H_
#define _WX_WASM_BMPBUTTN_H_

class WXDLLIMPEXP_CORE wxBitmapButton : public wxBitmapButtonBase
{
public:
    wxBitmapButton();

    wxBitmapButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmapBundle& bitmap,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& bitmap,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxButtonNameStr));

    bool CreateCloseButton(wxWindow* parent,
                           wxWindowID winid,
                           const wxString& name = wxString());

protected:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapButton);

private:
    // We re-use wxButton
};

#endif // _WX_WASM_BMPBUTTN_H_
