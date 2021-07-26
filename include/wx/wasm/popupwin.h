/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/popwin.h
// Purpose:     wxPopupWindow class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
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
