/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/popwin.h
// Purpose:     wxPopupWindow class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_POPUPWIN_H_
#define _WX_WASM_POPUPWIN_H_

class WXDLLIMPEXP_CORE wxPopupWindow : public wxPopupWindowBase
{
public:
    wxPopupWindow();
    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE);

protected:

private:
    wxDECLARE_DYNAMIC_CLASS(wxPopupWindow);
};

#endif // _WX_WASM_POPUPWIN_H_
