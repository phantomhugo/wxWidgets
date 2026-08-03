/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/msgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MSGDLG_H_
#define _WX_WASM_MSGDLG_H_

#include "wx/msgdlg.h"


class WXDLLIMPEXP_CORE wxMessageDialog : public wxMessageDialogBase
{
public:
    wxMessageDialog(wxWindow *parent, const wxString& message,
                    const wxString& caption = wxASCII_STR(wxMessageBoxCaptionStr),
                    long style = wxOK|wxCENTRE,
                    const wxPoint& pos = wxDefaultPosition);
    virtual ~wxMessageDialog();

    // Implementation using the HTML5 <dialog> element
    virtual int ShowModal() override;

private:
    // Create the DOM element of the dialog
    void CreateDialogElement();
    
    // Helper to get button labels
    static wxString GetButtonLabel(int id, const wxString& customLabel);
    
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMessageDialog);
};

#endif // _WX_WASM_MSGDLG_H_
