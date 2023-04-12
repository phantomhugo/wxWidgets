/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dialog.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart, Sean D'Epagnier
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/modalhook.h"
#include "wx/dialog.h"

wxDialog::wxDialog()
{
}

wxDialog::wxDialog( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    Create( parent, id, title, pos, size, style, name );
}

wxDialog::~wxDialog()
{
}


bool wxDialog::Create( wxWindow *parent, wxWindowID id,
        const wxString &title,
        const wxPoint &pos,
        const wxSize &size,
        long style,
        const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);

    // all dialogs should have tab traversal enabled
    style |= wxTAB_TRAVERSAL;


    return true;
}

int wxDialog::ShowModal()
{

}

void wxDialog::EndModal(int retCode)
{

}

bool wxDialog::IsModal() const
{
    wxCHECK_MSG( GetDialogHandle() != nullptr, false, "Invalid dialog" );

    return GetDialogHandle()->isModal();
}

bool wxDialog::Show(bool show)
{
    if ( show == IsShown() )
        return false;

    if ( !show && IsModal() )
        EndModal(wxID_CANCEL);

    if ( show && CanDoLayoutAdaptation() )
        DoLayoutAdaptation();

    const bool ret = wxDialogBase::Show(show);

    if (show)
        InitDialog();

    return ret;
}

void *wxDialog::GetDialogHandle() const
{
}

