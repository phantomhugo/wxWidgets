/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/region.h
// Purpose:     wxRegion class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_REGION_H_
#define _WX_REGION_H_

#include "wx/list.h"

// ----------------------------------------------------------------------------
// wxRegion
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRegion : public wxRegionBase
{
public:
    wxRegion();

    wxRegion( wxCoord x, wxCoord y, wxCoord w, wxCoord h );

    wxRegion( const wxPoint& topLeft, const wxPoint& bottomRight );

    wxRegion( const wxRect& rect );

    wxRegion( size_t n, const wxPoint *points, wxPolygonFillMode fillStyle = wxODDEVEN_RULE );

    wxRegion( const wxBitmap& bmp);

    wxRegion( const wxBitmap& bmp,
              const wxColour& transColour, int tolerance = 0);

    virtual ~wxRegion();

    // wxRegionBase methods
    virtual void Clear();
    virtual bool IsEmpty() const;

public:

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    // wxRegionBase pure virtuals
    virtual bool DoIsEqual(const wxRegion& region) const;
    virtual bool DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const;
    virtual wxRegionContain DoContainsPoint(wxCoord x, wxCoord y) const;
    virtual wxRegionContain DoContainsRect(const wxRect& rect) const;

    virtual bool DoOffset(wxCoord x, wxCoord y);
    virtual bool DoUnionWithRect(const wxRect& rect);
    virtual bool DoUnionWithRegion(const wxRegion& region);
    virtual bool DoIntersect(const wxRegion& region);
    virtual bool DoSubtract(const wxRegion& region);
    virtual bool DoXor(const wxRegion& region);

    // common part of ctors for a rectangle region
    void InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h);

private:
    wxDECLARE_DYNAMIC_CLASS(wxRegion);
};

// ----------------------------------------------------------------------------
// wxRegionIterator: decomposes a region into rectangles
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxRegionIterator: public wxObject
{
public:
    wxRegionIterator();
    wxRegionIterator(const wxRegion& region);

    void Reset();
    void Reset(const wxRegion& region);

    operator bool () const;
    bool HaveRects() const;

    void operator ++ ();
    void operator ++ (int);

    wxCoord GetX() const;
    wxCoord GetY() const;
    wxCoord GetW() const;
    wxCoord GetWidth() const;
    wxCoord GetH() const;
    wxCoord GetHeight() const;
    wxRect GetRect() const;

private:
    size_t   m_current;
    wxRegion m_region;

private:
    wxDECLARE_DYNAMIC_CLASS(wxRegionIterator);
};

#endif
// _WX_REGION_H_
