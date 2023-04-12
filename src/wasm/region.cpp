/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/region.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"
#include "wx/bitmap.h"
#include "wx/scopedarray.h"

class wxRegionRefData: public wxGDIRefData
{
public:
    wxRegionRefData()
    {
    }

    wxRegionRefData( const wxRegionRefData& data )
        : wxGDIRefData()
    {
    }

    bool operator == (const wxRegionRefData& data) const
    {

    }
};

#define M_REGIONDATA ((wxRegionRefData *)m_refData)->m_qtRegion

wxIMPLEMENT_DYNAMIC_CLASS(wxRegion,wxGDIObject);

wxRegion::wxRegion()
{
    m_refData = nullptr;
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{

}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{

}

wxRegion::wxRegion(const wxRect& rect)
{

}

wxRegion::wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle)
{

}

wxRegion::wxRegion(const wxBitmap& bmp)
{

}

wxRegion::wxRegion(const wxBitmap& bmp, const wxColour& transp, int tolerance)
{

}

bool wxRegion::IsEmpty() const
{

}

void wxRegion::Clear()
{

}

wxGDIRefData *wxRegion::CreateGDIRefData() const
{
    return new wxRegionRefData;
}

wxGDIRefData *wxRegion::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxRegionRefData(*(wxRegionRefData *)data);
}

bool wxRegion::DoIsEqual(const wxRegion& region) const
{
    wxCHECK_MSG( IsOk(), false, "Invalid region" );
    wxCHECK_MSG( region.IsOk(), false, "Invalid parameter region" );
}

bool wxRegion::DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    return true;
}

wxRegionContain wxRegion::DoContainsPoint(wxCoord x, wxCoord y) const
{
}

wxRegionContain wxRegion::DoContainsRect(const wxRect& rect) const
{
}

bool wxRegion::DoOffset(wxCoord x, wxCoord y)
{
    return true;
}

bool wxRegion::DoUnionWithRegion(const wxRegion& region)
{
}

bool wxRegion::DoIntersect(const wxRegion& region)
{
}

bool wxRegion::DoSubtract(const wxRegion& region)
{
}

bool wxRegion::DoXor(const wxRegion& region)
{
}

bool wxRegion::DoUnionWithRect(const wxRect& rect)
{
    return true;
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

wxRegionIterator::wxRegionIterator()
{

}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
{

}

void wxRegionIterator::Reset()
{
     m_current = 0u;
}

void wxRegionIterator::Reset(const wxRegion& region)
{
}

bool wxRegionIterator::HaveRects() const
{

}

wxRegionIterator::operator bool () const
{
    return HaveRects();
}

void wxRegionIterator::operator ++ ()
{

}

void wxRegionIterator::operator ++ (int)
{
    ++*this;
}

wxCoord wxRegionIterator::GetX() const
{

}

wxCoord wxRegionIterator::GetY() const
{

}

wxCoord wxRegionIterator::GetW() const
{
    return GetWidth();
}

wxCoord wxRegionIterator::GetWidth() const
{
}

wxCoord wxRegionIterator::GetH() const
{
    return GetHeight();
}

wxCoord wxRegionIterator::GetHeight() const
{
}

wxRect wxRegionIterator::GetRect() const
{
}
