/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colordlg.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/colordlg.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog,wxDialog)

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data )
{


    if ( data )
        m_data = *data;

    if ( m_data.GetChooseFull() )
    {
    }

    return wxTopLevelWindow::Create( parent, wxID_ANY, "");
}

wxColourData &wxColourDialog::GetColourData()
{
    for (int i=0; i<wxColourData::NUM_CUSTOM; i++)
        m_data.SetCustomColour(i, GetQColorDialog()->customColor(i));

    m_data.SetColour(GetQColorDialog()->currentColor());

    return m_data;
}
