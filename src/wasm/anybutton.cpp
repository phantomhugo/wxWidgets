/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/anybutton.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_ANY_BUTTON

#ifndef WX_PRECOMP
    #include "wx/anybutton.h"
#endif

#include "wx/bitmap.h"

wxAnyButton::wxAnyButton()
{
}

void wxAnyButton::SetLabel( const wxString &label )
{
}

wxString wxAnyButton::GetLabel() const
{
}

void *wxAnyButton::GetHandle() const
{

}

wxBitmap wxAnyButton::DoGetBitmap(State state) const
{
    return state < State_Max ? m_bitmaps[state].GetBitmap(wxDefaultSize) : wxNullBitmap;
}

void wxAnyButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET(which < State_Max, "Invalid state");

    // Cache the bitmap.
    m_bitmaps[which] = bitmap;
}

#endif // wxHAS_ANY_BUTTON
