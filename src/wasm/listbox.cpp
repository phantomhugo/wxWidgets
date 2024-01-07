/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/listbox.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listbox.h"
wxListBox::wxListBox(): wxScrollHelper(this)
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name): wxScrollHelper(this)
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name) : wxScrollHelper(this)
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

wxListBox::~wxListBox()
{
    Clear();
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{


    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

void wxListBox::Init()
{
#if wxUSE_CHECKLISTBOX

#endif // wxUSE_CHECKLISTBOX
}

void wxListBox::EnsureVisible(int n)
{

}

bool wxListBox::IsSelected(int n) const
{

}

wxBorder wxListBox::GetDefaultBorder() const
{

}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.clear();

    return aSelections.size();
}

unsigned wxListBox::GetCount() const
{

}

bool wxListBox::SetFont(const wxFont& font)
{

}

wxString wxListBox::GetString(unsigned int n) const
{

}

void wxListBox::SetString(unsigned int n, const wxString& s)
{

}
int wxListBox::FindString(const wxString& s, bool bCase) const
{

}
int wxListBox::GetSelection() const
{

}

void wxListBox::DoDrawRange(wxControlRenderer *renderer, int itemFirst, int itemLast)
{

}

void wxListBox::DoEnsureVisible(int n)
{

}

void wxListBox::DoSetFirstItem(int n)
{
}

void wxListBox::DoSetSelection(int n, bool select)
{

}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                          unsigned int pos,
                          void **clientData,
                          wxClientDataType type)
{
    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    UpdateOldSelections();
    return n;
}

int wxListBox::DoListHitTest(const wxPoint& point) const
{

}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
}

wxSize wxListBox::DoGetBestClientSize() const
{

}

void wxListBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{

}

void wxListBox::DoClear()
{
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
}

void wxListBox::OnInternalIdle()
{

}
