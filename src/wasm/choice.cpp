/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/choice.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2012 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/choice.h"


wxChoice::wxChoice()
{
}



wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}


wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, choices, style, validator, name );
}


bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    return Create( parent, id, pos, size, choices.size(), choices.size() ? &choices[ 0 ] : nullptr, style,
        validator, name );
}


bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{

}

wxSize wxChoice::DoGetBestSize() const
{
    wxSize basesize = wxChoiceBase::DoGetBestSize();
    wxSize size = wxControl::DoGetBestSize();
    // mix calculated size by wx base prioritizing qt hint (max):
    if (size.GetWidth() < basesize.GetWidth())
        size.SetWidth(basesize.GetWidth());
    if (size.GetHeight() < basesize.GetHeight())
        size.SetHeight(basesize.GetHeight());
    return size;
}


unsigned wxChoice::GetCount() const
{
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG(n < GetCount(), wxString(), "invalid index");

}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
}


void wxChoice::SetSelection(int n)
{
}

int wxChoice::GetSelection() const
{
}


int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    InvalidateBestSize();

    // Hack: to avoid resorting the model many times in DoInsertOneItem(),
    // which will be called for each item from DoInsertItemsInLoop(), reset the
    // wxCB_SORT style if we have it temporarily and only sort once at the end.
    bool wasSorted = false;
    if ( IsSorted() )
    {
        wasSorted = true;
        ToggleWindowStyle(wxCB_SORT);
    }

    int n = DoInsertItemsInLoop(items, pos, clientData, type);

    if ( wasSorted )
    {
        // Restore the flag turned off above.
        ToggleWindowStyle(wxCB_SORT);

    }

    return n;
}

int wxChoice::DoInsertOneItem(const wxString& item, unsigned int pos)
{
}

void wxChoice::DoSetItemClientData(unsigned int n, void *clientData)
{

}

void *wxChoice::DoGetItemClientData(unsigned int n) const
{

}


void wxChoice::DoClear()
{

}

void wxChoice::DoDeleteOneItem(unsigned int pos)
{

}

void *wxChoice::GetHandle() const
{
    return nullptr;
}
