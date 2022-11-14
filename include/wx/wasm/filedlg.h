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

    virtual wxString GetPath() const override;
    virtual void GetPaths(wxArrayString& paths) const override;
    virtual wxString GetFilename() const override;
    virtual void GetFilenames(wxArrayString& files) const override;
    virtual int GetFilterIndex() const override;

    virtual void SetMessage(const wxString& message) override;
    virtual void SetPath(const wxString& path) override;
    virtual void SetDirectory(const wxString& dir) override;
    virtual void SetFilename(const wxString& name) override;
    virtual void SetWildcard(const wxString& wildCard) override;
    virtual void SetFilterIndex(int filterIndex) override;

    virtual bool SupportsExtraControl() const override { return true; }

private:

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_WASM_FILEDLG_H_
