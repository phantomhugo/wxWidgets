/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/pen.h
// Purpose:     wxPen class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PEN_H_
#define _WX_PEN_H_

#include "wx/gdicmn.h"
#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxPen;
class WXDLLIMPEXP_FWD_CORE wxColour;
class WXDLLIMPEXP_FWD_CORE wxBitmap;

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxPen: public wxPenBase
{
public:
    wxPen();

    wxPen( const wxColour &colour, int width = 1, wxPenStyle style = wxPENSTYLE_SOLID );

    wxPen( const wxBitmap &stipple, int width );

    wxPen( const wxPenInfo& info );

    virtual ~wxPen();

    bool operator == ( const wxPen& pen ) const;
    bool operator != (const wxPen& pen) const;

    void SetColour( const wxColour &colour );
    void SetColour( unsigned char red, unsigned char green, unsigned char blue );
    void SetCap( wxPenCap capStyle );
    void SetJoin( wxPenJoin joinStyle );
    void SetStyle( wxPenStyle style );
    void SetWidth( int width );
    void SetDashes( int number_of_dashes, const wxDash *dash );
    void SetStipple( const wxBitmap& stipple );

    wxColour GetColour() const;
    wxPenCap GetCap() const;
    wxPenJoin GetJoin() const;
    wxPenStyle GetStyle() const;
    int GetWidth() const;
    int GetDashes(wxDash **ptr) const;
    int GetDashCount() const;
    wxDash* GetDash() const;
    wxBitmap* GetStipple() const;


    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    wxPen(const wxColour& col, int width, int style);
    wxDEPRECATED_MSG("use wxPENSTYLE_XXX constants")
    void SetStyle(int style) { SetStyle((wxPenStyle)style); }

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    wxDECLARE_DYNAMIC_CLASS(wxPen);
};

#endif // _WX_PEN_H_
