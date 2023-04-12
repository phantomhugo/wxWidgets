/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/filedlg.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) 2014 Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/filename.h"

#include "wx/filedlg.h"
#include "wx/dirdlg.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxDialog);

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
{
    Create(parent, message, defaultDir, defaultFile,
           wildCard, style, pos, sz, name);
}

bool wxFileDialog::Create(wxWindow *parent,
                          const wxString& message,
                          const wxString& defaultDir,
                          const wxString& defaultFile,
                          const wxString& wildCard,
                          long style,
                          const wxPoint& pos,
                          const wxSize& sz,
                          const wxString& name)
{
    return wxTopLevelWindow::Create( parent, wxID_ANY, message, pos, sz, style, name );
}

wxString wxFileDialog::GetPath() const
{
    wxCHECK_MSG( !HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetPaths() instead" );

    wxArrayString paths;
    GetPaths(paths);
    if (paths.empty())
        return "";

    return paths[0];
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
}

wxString wxFileDialog::GetFilename() const
{
    wxCHECK_MSG( !HasFlag(wxFD_MULTIPLE), wxString(), "When using wxFD_MULTIPLE, must call GetFilenames() instead" );

    wxArrayString filenames;
    GetFilenames(filenames);
    if ( filenames.empty() )
        return "";

    return filenames[0];
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{

}

int wxFileDialog::GetFilterIndex() const
{
    int index = 0;
    return -1;
}

void wxFileDialog::SetMessage(const wxString& message)
{
}

void wxFileDialog::SetPath(const wxString& path)
{
}

void wxFileDialog::SetDirectory(const wxString& dir)
{
}

void wxFileDialog::SetFilename(const wxString& name)
{
    SetPath(name);
}

void wxFileDialog::SetWildcard(const wxString& wildCard)
{
}

void wxFileDialog::SetFilterIndex(int filterIndex)
{
}

wxIMPLEMENT_DYNAMIC_CLASS(wxDirDialog, wxDialog);

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name)
{
    Create(parent, message, defaultPath, style, pos, size, name);
}

bool wxDirDialog::Create(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name)
{
    return wxTopLevelWindow::Create( parent, wxID_ANY, message, pos, size, style, name );
}

wxString wxDirDialog::GetPath() const
{
    return "";
}

void wxDirDialog::SetPath(const wxString& path)
{
}
