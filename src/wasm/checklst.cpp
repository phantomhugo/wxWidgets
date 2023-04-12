/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/checklst.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checklst.h"

wxCheckListBox::wxCheckListBox()
{
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int nStrings,
        const wxString *choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, nStrings, choices, style, validator, name );
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              int n, const wxString choices[],
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return wxCheckListBoxBase::Create( parent, id, pos, size, n, choices, style, validator, name );
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return wxCheckListBoxBase::Create( parent, id, pos, size, choices, style, validator, name );
}

void wxCheckListBox::Init()
{

}

bool wxCheckListBox::IsChecked(unsigned int n) const
{

}

void wxCheckListBox::Check(unsigned int n, bool check )
{

}

