/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/filedlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/fontdlg.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

bool wxFontDialog::DoCreate(wxWindow *parent)
{

    return wxFontDialogBase::DoCreate(parent);
}


