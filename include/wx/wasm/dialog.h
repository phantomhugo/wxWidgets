/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dialog.h
// Purpose:     wxDialog class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DIALOG_H_
#define _WX_WASM_DIALOG_H_

#include "wx/dialog.h"
class QDialog;

class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
public:
    wxDialog();
    wxDialog( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );

    virtual ~wxDialog();

    bool Create( wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_DIALOG_STYLE,
            const wxString &name = wxASCII_STR(wxDialogNameStr) );

    virtual int ShowModal() wxOVERRIDE;
    virtual void EndModal(int retCode) wxOVERRIDE;
    virtual bool IsModal() const wxOVERRIDE;
    virtual bool Show(bool show = true) wxOVERRIDE;

    QDialog *GetDialogHandle() const;

private:

    wxDECLARE_DYNAMIC_CLASS( wxDialog );
};


#endif // _WX_WASM_DIALOG_H_
