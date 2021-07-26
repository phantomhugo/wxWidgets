/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/fontdlg.h
// Purpose:     wxFontDialog class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FONTDLG_H_
#define _WX_WASM_FONTDLG_H_

class WXDLLIMPEXP_CORE wxFontDialog : public wxFontDialogBase
{
public:
    wxFontDialog() { }
    wxFontDialog(wxWindow *parent) { Create(parent); }
    wxFontDialog(wxWindow *parent, const wxFontData& data) { Create(parent, data); }

protected:
    bool DoCreate(wxWindow *parent) wxOVERRIDE;

private:

    wxFontData m_data;

    wxDECLARE_DYNAMIC_CLASS(wxFontDialog);
};

#endif // _WX_WASM_FONTDLG_H_
