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
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/cursor.h"
#include <emscripten.h>

// ----------------------------------------------------------------------------
// global cursor functions
// ----------------------------------------------------------------------------

// Set the cursor for the whole document (used for override cursors).
static void wxWasmSetDocumentCursor(const char *cssCursor)
{
    EM_ASM_({
        document.documentElement.style.cursor = UTF8ToString($0);
    }, cssCursor);
}

void wxSetCursor(const wxCursorBundle& cursors)
{
    const wxCursor& cursor = cursors.GetCursorForMainWindow();

    wxWasmSetDocumentCursor(cursor.IsOk()
                                ? wxWasmCursorToCSS(cursor.GetStockCursorId())
                                : "default");
}

// Count of nested wxBeginBusyCursor() calls.
static int gs_busyCursorCount = 0;

void wxBeginBusyCursor(const wxCursor *cursor)
{
    if ( gs_busyCursorCount++ == 0 )
    {
        wxWasmSetDocumentCursor(cursor && cursor->IsOk()
                                    ? wxWasmCursorToCSS(cursor->GetStockCursorId())
                                    : "wait");
    }
}

bool wxIsBusy()
{
    return gs_busyCursorCount > 0;
}

void wxEndBusyCursor()
{
    wxCHECK_RET( gs_busyCursorCount > 0, wxT("no matching wxBeginBusyCursor()") );

    if ( --gs_busyCursorCount == 0 )
        wxWasmSetDocumentCursor("default");
}

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxGDIRefData
{
public:
    wxCursorRefData() : m_stockCursorId(wxCURSOR_NONE) {}
    wxCursorRefData( const wxCursorRefData& data )
        : wxGDIRefData(), m_stockCursorId(data.m_stockCursorId) {}

    wxStockCursor m_stockCursorId;
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
    wxUnusedVar(cursor_file);
    wxUnusedVar(type);
    wxUnusedVar(hotSpotX);
    wxUnusedVar(hotSpotY);
}

#if wxUSE_IMAGE
wxCursor::wxCursor( const wxImage & image )
{
    wxUnusedVar(image);
}
#endif

void wxCursor::InitFromStock( wxStockCursor cursorId )
{
    m_refData = new wxCursorRefData;

    static_cast<wxCursorRefData*>(m_refData)->m_stockCursorId = cursorId;
}

WXCursor wxCursor::GetCursor() const
{
    return m_refData;
}

wxStockCursor wxCursor::GetStockCursorId() const
{
    if ( !m_refData )
        return wxCURSOR_NONE;

    return static_cast<wxCursorRefData*>(m_refData)->m_stockCursorId;
}

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}

// Map a stock cursor id to the equivalent CSS cursor value.
const char *wxWasmCursorToCSS(wxStockCursor cursorId)
{
    switch ( cursorId )
    {
        case wxCURSOR_NONE:
        case wxCURSOR_BLANK:          return "none";
        case wxCURSOR_HAND:           return "pointer";
        case wxCURSOR_IBEAM:          return "text";
        case wxCURSOR_WAIT:
        case wxCURSOR_WATCH:          return "wait";
        case wxCURSOR_ARROWWAIT:      return "progress";
        case wxCURSOR_CROSS:
        case wxCURSOR_BULLSEYE:
        case wxCURSOR_PENCIL:
        case wxCURSOR_PAINT_BRUSH:
        case wxCURSOR_SPRAYCAN:       return "crosshair";
        case wxCURSOR_MAGNIFIER:      return "zoom-in";
        case wxCURSOR_NO_ENTRY:       return "not-allowed";
        case wxCURSOR_QUESTION_ARROW: return "help";
        case wxCURSOR_SIZING:         return "nwse-resize";
        case wxCURSOR_SIZENESW:       return "nesw-resize";
        case wxCURSOR_SIZENWSE:       return "nwse-resize";
        case wxCURSOR_SIZEWE:         return "ew-resize";
        case wxCURSOR_SIZENS:         return "ns-resize";
        case wxCURSOR_ARROW:
        case wxCURSOR_RIGHT_ARROW:
        case wxCURSOR_CHAR:
        case wxCURSOR_LEFT_BUTTON:
        case wxCURSOR_MIDDLE_BUTTON:
        case wxCURSOR_RIGHT_BUTTON:
        case wxCURSOR_POINT_LEFT:
        case wxCURSOR_POINT_RIGHT:
        default:                      return "default";
    }
}
