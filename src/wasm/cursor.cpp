/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/cursor.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/cursor.h"

void wxSetCursor(const wxCursor& cursor)
{

}

void wxBeginBusyCursor(const wxCursor *cursor)
{

}

bool wxIsBusy()
{
}

void wxEndBusyCursor()
{
}

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxGDIRefData
{
public:
    wxCursorRefData() {}
    wxCursorRefData( const wxCursorRefData& data )  {}
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject);

wxCursor::wxCursor()
{

}

wxCursor::wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot)
{
    wxUnusedVar(bitmap);
    wxUnusedVar(hotSpot);
}

wxCursor::wxCursor(const wxString& cursor_file,
                   wxBitmapType type,
                   int hotSpotX, int hotSpotY)
{
}

#if wxUSE_IMAGE
wxCursor::wxCursor( const wxImage & image )
{

}
#endif

void wxCursor::InitFromStock( wxStockCursor cursorId )
{

}

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}
