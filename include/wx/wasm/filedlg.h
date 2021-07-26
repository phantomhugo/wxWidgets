/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FILEDLG_H_
#define _WX_WASM_FILEDLG_H_

class WXDLLIMPEXP_CORE wxFileDialog : public wxFileDialogBase
{
public:
    wxFileDialog() { }
    wxFileDialog(wxWindow *parent,
                 const wxString& message = wxASCII_STR(wxFileSelectorPromptStr),
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxASCII_STR(wxFileDialogNameStr));
    bool Create(wxWindow *parent,
                 const wxString& message = wxASCII_STR(wxFileSelectorPromptStr),
                 const wxString& defaultDir = wxEmptyString,
                 const wxString& defaultFile = wxEmptyString,
                 const wxString& wildCard = wxASCII_STR(wxFileSelectorDefaultWildcardStr),
                 long style = wxFD_DEFAULT_STYLE,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& sz = wxDefaultSize,
                 const wxString& name = wxASCII_STR(wxFileDialogNameStr));

    virtual wxString GetPath() const wxOVERRIDE;
    virtual void GetPaths(wxArrayString& paths) const wxOVERRIDE;
    virtual wxString GetFilename() const wxOVERRIDE;
    virtual void GetFilenames(wxArrayString& files) const wxOVERRIDE;
    virtual int GetFilterIndex() const wxOVERRIDE;

    virtual void SetMessage(const wxString& message) wxOVERRIDE;
    virtual void SetPath(const wxString& path) wxOVERRIDE;
    virtual void SetDirectory(const wxString& dir) wxOVERRIDE;
    virtual void SetFilename(const wxString& name) wxOVERRIDE;
    virtual void SetWildcard(const wxString& wildCard) wxOVERRIDE;
    virtual void SetFilterIndex(int filterIndex) wxOVERRIDE;

    virtual bool SupportsExtraControl() const wxOVERRIDE { return true; }

private:

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_WASM_FILEDLG_H_
