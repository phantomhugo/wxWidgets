/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/stattext.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/stattext.h"

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
             wxWindowID id,
             const wxString &label,
             const wxPoint &pos,
             const wxSize &size,
             long style,
             const wxString &name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &label,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxString &name)
{

}

void wxStaticText::SetLabel(const wxString& label)
{

}

void wxStaticText::WXSetVisibleLabel(const wxString& label)
{

}

wxString wxStaticText::WXGetVisibleLabel() const
{

}

WXWidget wxStaticText::GetHandle() const
{

}
