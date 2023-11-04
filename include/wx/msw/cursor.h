/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/cursor.h
// Purpose:     wxCursor class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_
#define _WX_CURSOR_H_

class WXDLLIMPEXP_FWD_CORE wxImage;

// Cursor
class WXDLLIMPEXP_CORE wxCursor : public wxCursorBase
{
public:
    // constructors
    wxCursor();
#if wxUSE_IMAGE
    wxCursor(const wxImage& image);
    wxCursor(const char* const* xpmData);
#endif // wxUSE_IMAGE
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(wxStockCursor id) { InitFromStock(id); }

    virtual wxPoint GetHotSpot() const override;

    virtual ~wxCursor();

    // implementation only
    void SetHCURSOR(WXHCURSOR cursor) { SetHandle((WXHANDLE)cursor); }
    WXHCURSOR GetHCURSOR() const { return (WXHCURSOR)GetHandle(); }

protected:
    void InitFromStock(wxStockCursor);

    virtual wxGDIImageRefData *CreateData() const override;

private:
#if wxUSE_IMAGE
    void InitFromImage(const wxImage& image);
#endif // wxUSE_IMAGE

    wxDECLARE_DYNAMIC_CLASS(wxCursor);
};

#endif
    // _WX_CURSOR_H_
