/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dirdlg.h
// Purpose:     wxDirDialog class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DIRDLG_H_
#define _WX_WASM_DIRDLG_H_

class QFileDialog;

class WXDLLIMPEXP_CORE wxDirDialog : public wxDirDialogBase
{
public:
    wxDirDialog() { }

    wxDirDialog(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));

    bool Create(wxWindow *parent,
                const wxString& message = wxASCII_STR(wxDirSelectorPromptStr),
                const wxString& defaultPath = wxEmptyString,
                long style = wxDD_DEFAULT_STYLE,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxASCII_STR(wxDirDialogNameStr));

public:     // overrides from wxGenericDirDialog

    wxString GetPath() const wxOVERRIDE;
    void SetPath(const wxString& path) wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS(wxDirDialog);
};

#endif // _WX_WASM_DIRDLG_H_
