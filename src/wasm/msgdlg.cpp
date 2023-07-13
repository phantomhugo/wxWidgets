/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/msgdlg.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/modalhook.h"
#include "wx/msgdlg.h"


wxMessageDialog::wxMessageDialog( wxWindow *parent, const wxString& message,
        const wxString& caption, long style, const wxPoint& pos )
    : wxMessageDialogBase( parent, message, caption, style )
{
    PostCreation();

    Centre(wxBOTH | wxCENTER_FRAME);
}

wxIMPLEMENT_CLASS(wxMessageDialog,wxDialog);

int wxMessageDialog::ShowModal()
{
}

wxMessageDialog::~wxMessageDialog()
{
}

