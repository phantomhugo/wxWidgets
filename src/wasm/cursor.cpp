/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/cursor.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
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
    wxCursorRefData( const wxCursorRefData& data ) : m_qtCursor(data.m_qtCursor) {}
    wxCursorRefData( QCursor &c ) : m_qtCursor(c) {}

    QCursor m_qtCursor;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject);


wxCursor::wxCursor(const wxString& cursor_file,
                   wxBitmapType type,
                   int hotSpotX, int hotSpotY)
{
#if wxUSE_IMAGE
    wxImage img;
    if (!img.LoadFile(cursor_file, type))
        return;

    // eventually set the hotspot:
    if (!img.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X))
        img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX);
    if (!img.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y))
        img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotSpotY);

    InitFromImage(img);
#endif // wxUSE_IMAGE
}

#if wxUSE_IMAGE
wxCursor::wxCursor(const wxImage& img)
{
    InitFromImage(img);
}

wxCursor::wxCursor(const char* const* xpmData)
{
    InitFromImage(wxImage(xpmData));
}
#endif // wxUSE_IMAGE

wxPoint wxCursor::GetHotSpot() const
{
}

void wxCursor::InitFromStock( wxStockCursor cursorId )
{

}

#if wxUSE_IMAGE

void wxCursor::InitFromImage( const wxImage & image )
{
    AllocExclusive();
    GetHandle() = QCursor(*wxBitmap(image).GetHandle(),
                           image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X) ?
                           image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X) : -1,
                           image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y) ?
                           image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y) : -1);
}

#endif // wxUSE_IMAGE

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}
