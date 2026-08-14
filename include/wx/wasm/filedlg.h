/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/filedlg.h
// Purpose:     wxFileDialog class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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

    virtual int ShowModal() override;

    // The selected files are copied into the Emscripten MEMFS under "/tmp",
    // so this is the default directory when none was explicitly set.
    virtual wxString GetDirectory() const override
        { return m_dir.empty() ? wxString(wxASCII_STR("/tmp")) : m_dir; }
    virtual void GetPaths(wxArrayString& paths) const override;
    virtual void GetFilenames(wxArrayString& files) const override;

    // The native browser file picker cannot host wx extra controls.
    virtual bool SupportsExtraControl() const override { return false; }

private:
    // Results of the last accepted wxFD_MULTIPLE selection.
    wxArrayString m_paths;
    wxArrayString m_fileNames;

    wxDECLARE_DYNAMIC_CLASS(wxFileDialog);
};

#endif // _WX_WASM_FILEDLG_H_
