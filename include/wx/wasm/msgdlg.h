/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/msgdlg.h
// Purpose:     wxMessageDialog class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021-2024 Logo Texo S.A.S. (www.logotexo.com)
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

    // Implementación usando elemento <dialog> de HTML5
    virtual int ShowModal() override;

private:
    // Crear el elemento DOM del diálogo
    void CreateDialogElement();
    
    // Helper para obtener etiquetas de botones
    static wxString GetButtonLabel(int id, const wxString& customLabel);
    
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMessageDialog);
};

#endif // _WX_WASM_MSGDLG_H_
