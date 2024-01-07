/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statbox.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbox.h"
#include "wx/window.h"


wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{

}

void wxStaticBox::SetLabel(const wxString& label)
{

}

wxString wxStaticBox::GetLabel() const
{

}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space for the label:
    *borderTop += GetCharHeight();
}

WXWidget wxStaticBox::GetHandle() const
{

}
