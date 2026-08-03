/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/region.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/region.h"
#include "wx/bitmap.h"
#include "wx/utils.h"
#include "wx/vector.h"

#include <algorithm>
#include <cmath>

namespace
{

bool RectsEqual(const wxVector<wxRect>& a, const wxVector<wxRect>& b)
{
    if ( a.size() != b.size() )
        return false;
    for ( size_t n = 0; n < a.size(); ++n )
    {
        if ( a[n] != b[n] )
            return false;
    }
    return true;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxRegionRefData: a region is simply a list of rectangles
// ----------------------------------------------------------------------------

class wxRegionRefData: public wxGDIRefData
{
public:
    wxRegionRefData()
    {
    }

    wxRegionRefData( const wxRegionRefData& data )
        : wxGDIRefData(),
          m_rects(data.m_rects)
    {
    }

    bool operator == (const wxRegionRefData& data) const
    {
        return RectsEqual(m_rects, data.m_rects);
    }

    wxVector<wxRect> m_rects;
};

// helpers for the rect-list algebra used by the DoXXX() operations below
namespace
{

const wxVector<wxRect>& GetRects(const wxRegionRefData *data)
{
    static const wxVector<wxRect> s_empty;
    return data ? data->m_rects : s_empty;
}

// Subtract sub from r, appending the (up to 4) remaining pieces to out.
void SubtractRect(const wxRect& r, const wxRect& sub, wxVector<wxRect>& out)
{
    if ( !r.Intersects(sub) )
    {
        out.push_back(r);
        return;
    }

    // use half-open coordinates for clarity
    const int l = r.GetLeft(),   t = r.GetTop(),
              rt = r.GetRight() + 1, b = r.GetBottom() + 1;
    const wxRect i = r.Intersect(sub);
    const int il = i.GetLeft(),   it = i.GetTop(),
              ir = i.GetRight() + 1, ib = i.GetBottom() + 1;

    if ( it > t )
        out.push_back(wxRect(l, t, rt - l, it - t));
    if ( ib < b )
        out.push_back(wxRect(l, ib, rt - l, b - ib));
    if ( il > l )
        out.push_back(wxRect(l, it, il - l, ib - it));
    if ( ir < rt )
        out.push_back(wxRect(ir, it, rt - ir, ib - it));
}

// result = a - b for rect lists a and b
wxVector<wxRect> SubtractRects(const wxVector<wxRect>& a,
                               const wxVector<wxRect>& b)
{
    wxVector<wxRect> result = a;
    for ( size_t n = 0; n < b.size(); ++n )
    {
        wxVector<wxRect> next;
        for ( size_t m = 0; m < result.size(); ++m )
            SubtractRect(result[m], b[n], next);
        result = next;
    }
    return result;
}

// result = a intersected with b
wxVector<wxRect> IntersectRects(const wxVector<wxRect>& a,
                                const wxVector<wxRect>& b)
{
    wxVector<wxRect> result;
    for ( size_t n = 0; n < a.size(); ++n )
    {
        for ( size_t m = 0; m < b.size(); ++m )
        {
            if ( a[n].Intersects(b[m]) )
            {
                const wxRect i = a[n].Intersect(b[m]);
                if ( !i.IsEmpty() )
                    result.push_back(i);
            }
        }
    }
    return result;
}

} // anonymous namespace

#define M_REGIONDATA ((wxRegionRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxRegion,wxGDIObject);

wxRegion::wxRegion()
{
    m_refData = nullptr;
}

void wxRegion::InitRect(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    m_refData = new wxRegionRefData;

    if ( w > 0 && h > 0 )
        M_REGIONDATA->m_rects.push_back(wxRect(x, y, w, h));
}

wxRegion::wxRegion(wxCoord x, wxCoord y, wxCoord w, wxCoord h)
{
    InitRect(x, y, w, h);
}

wxRegion::wxRegion(const wxPoint& topLeft, const wxPoint& bottomRight)
{
    InitRect(topLeft.x, topLeft.y,
             bottomRight.x - topLeft.x + 1, bottomRight.y - topLeft.y + 1);
}

wxRegion::wxRegion(const wxRect& rect)
{
    InitRect(rect.x, rect.y, rect.width, rect.height);
}

wxRegion::wxRegion(size_t n, const wxPoint *points, wxPolygonFillMode fillStyle)
{
    m_refData = nullptr;

    if ( !points || n < 3 )
        return;

    // decompose the polygon into one-pixel-high scanline spans: a pixel is
    // inside if its centre is inside the polygon
    wxCoord minY = points[0].y, maxY = points[0].y;
    for ( size_t i = 1; i < n; ++i )
    {
        minY = wxMin(minY, points[i].y);
        maxY = wxMax(maxY, points[i].y);
    }

    m_refData = new wxRegionRefData;

    for ( wxCoord y = minY; y <= maxY; ++y )
    {
        const double ys = y + 0.5;

        // x coordinates (and directions, for the winding rule) of the
        // crossings of the polygon edges with this scanline
        wxVector<double> crossings;
        wxVector<int> dirs;
        for ( size_t i = 0; i < n; ++i )
        {
            const wxPoint p1 = points[i];
            const wxPoint p2 = points[(i + 1) % n];

            if ( p1.y == p2.y )
                continue;

            if ( (p1.y <= ys && p2.y > ys) || (p2.y <= ys && p1.y > ys) )
            {
                crossings.push_back(p1.x + (ys - p1.y) * (p2.x - p1.x) /
                                           (double)(p2.y - p1.y));
                dirs.push_back(p2.y > p1.y ? 1 : -1);
            }
        }

        // sort crossings (keeping directions in sync) by x
        for ( size_t i = 1; i < crossings.size(); ++i )
        {
            for ( size_t j = i;
                  j > 0 && crossings[j - 1] > crossings[j]; --j )
            {
                std::swap(crossings[j - 1], crossings[j]);
                std::swap(dirs[j - 1], dirs[j]);
            }
        }

        // walk the crossings collecting the inside spans
        int winding = 0;
        bool inside = false;
        double spanStart = 0.0;
        for ( size_t i = 0; i < crossings.size(); ++i )
        {
            if ( fillStyle == wxWINDING_RULE )
            {
                winding += dirs[i];
                if ( (winding != 0) != inside )
                {
                    if ( !inside )
                        spanStart = crossings[i];
                    else
                    {
                        const int l = (int)std::ceil(spanStart - 0.5);
                        const int r = (int)std::ceil(crossings[i] - 0.5);
                        if ( r > l )
                            M_REGIONDATA->m_rects.push_back(wxRect(l, y, r - l, 1));
                    }
                    inside = !inside;
                }
            }
            else // wxODDEVEN_RULE
            {
                if ( !inside )
                    spanStart = crossings[i];
                else
                {
                    const int l = (int)std::ceil(spanStart - 0.5);
                    const int r = (int)std::ceil(crossings[i] - 0.5);
                    if ( r > l )
                        M_REGIONDATA->m_rects.push_back(wxRect(l, y, r - l, 1));
                }
                inside = !inside;
            }
        }
    }
}

wxRegion::wxRegion(const wxBitmap& bmp)
{
    m_refData = nullptr;

#if wxUSE_IMAGE
    Union(bmp);
#else
    (void)bmp;
#endif
}

wxRegion::wxRegion(const wxBitmap& bmp, const wxColour& transp, int tolerance)
{
    m_refData = nullptr;

#if wxUSE_IMAGE
    Union(bmp, transp, tolerance);
#else
    (void)bmp;
    (void)transp;
    (void)tolerance;
#endif
}

bool wxRegion::IsEmpty() const
{
    return GetRects((wxRegionRefData *)m_refData).empty();
}

void wxRegion::Clear()
{
    UnRef();
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

    return RectsEqual(M_REGIONDATA->m_rects,
                      ((wxRegionRefData *)region.m_refData)->m_rects);
}

bool wxRegion::DoGetBox(wxCoord& x, wxCoord& y, wxCoord& w, wxCoord& h) const
{
    const wxVector<wxRect>& rects = GetRects((wxRegionRefData *)m_refData);
    if ( rects.empty() )
    {
        x = y = w = h = 0;
        return false;
    }

    wxCoord l = rects[0].GetLeft(), t = rects[0].GetTop(),
            r = rects[0].GetRight(), b = rects[0].GetBottom();
    for ( size_t n = 1; n < rects.size(); ++n )
    {
        l = wxMin(l, rects[n].GetLeft());
        t = wxMin(t, rects[n].GetTop());
        r = wxMax(r, rects[n].GetRight());
        b = wxMax(b, rects[n].GetBottom());
    }

    x = l;
    y = t;
    w = r - l + 1;
    h = b - t + 1;
    return true;
}

wxRegionContain wxRegion::DoContainsPoint(wxCoord x, wxCoord y) const
{
    const wxVector<wxRect>& rects = GetRects((wxRegionRefData *)m_refData);
    for ( size_t n = 0; n < rects.size(); ++n )
    {
        if ( rects[n].Contains(x, y) )
            return wxInRegion;
    }
    return wxOutRegion;
}

wxRegionContain wxRegion::DoContainsRect(const wxRect& rect) const
{
    const wxVector<wxRect>& rects = GetRects((wxRegionRefData *)m_refData);

    bool anyIntersection = false;
    for ( size_t n = 0; n < rects.size(); ++n )
    {
        if ( rects[n].Contains(rect) )
            return wxInRegion;
        if ( rects[n].Intersects(rect) )
            anyIntersection = true;
    }

    // approximation: a rect fully covered by the union of several region
    // rects, but by no single one, is reported as wxPartRegion
    return anyIntersection ? wxPartRegion : wxOutRegion;
}

bool wxRegion::DoOffset(wxCoord x, wxCoord y)
{
    if ( !m_refData )
        return true;

    AllocExclusive();

    wxVector<wxRect>& rects = M_REGIONDATA->m_rects;
    for ( size_t n = 0; n < rects.size(); ++n )
        rects[n].Offset(x, y);

    return true;
}

bool wxRegion::DoUnionWithRegion(const wxRegion& region)
{
    const wxVector<wxRect>& other = GetRects((wxRegionRefData *)region.m_refData);
    if ( other.empty() )
        return true;

    if ( !m_refData )
        m_refData = new wxRegionRefData;
    else
        AllocExclusive();

    // no attempt is made at merging overlapping rectangles: correctness of
    // Contains()/GetBox() doesn't require it
    wxVector<wxRect>& rects = M_REGIONDATA->m_rects;
    for ( size_t n = 0; n < other.size(); ++n )
        rects.push_back(other[n]);

    return true;
}

bool wxRegion::DoIntersect(const wxRegion& region)
{
    const wxVector<wxRect> result =
        IntersectRects(GetRects((wxRegionRefData *)m_refData),
                       GetRects((wxRegionRefData *)region.m_refData));

    if ( !m_refData )
        m_refData = new wxRegionRefData;
    else
        AllocExclusive();

    M_REGIONDATA->m_rects = result;
    return true;
}

bool wxRegion::DoSubtract(const wxRegion& region)
{
    if ( !m_refData )
        return true;

    const wxVector<wxRect> result =
        SubtractRects(M_REGIONDATA->m_rects,
                      GetRects((wxRegionRefData *)region.m_refData));

    AllocExclusive();
    M_REGIONDATA->m_rects = result;
    return true;
}

bool wxRegion::DoXor(const wxRegion& region)
{
    // Xor(A, B) = (A - B) union (B - A)
    const wxVector<wxRect>& mine = GetRects((wxRegionRefData *)m_refData);
    const wxVector<wxRect>& other = GetRects((wxRegionRefData *)region.m_refData);

    wxVector<wxRect> result = SubtractRects(mine, other);
    const wxVector<wxRect> rest = SubtractRects(other, mine);
    for ( size_t n = 0; n < rest.size(); ++n )
        result.push_back(rest[n]);

    if ( !m_refData )
        m_refData = new wxRegionRefData;
    else
        AllocExclusive();

    M_REGIONDATA->m_rects = result;
    return true;
}

bool wxRegion::DoUnionWithRect(const wxRect& rect)
{
    if ( rect.IsEmpty() )
        return true;

    if ( !m_refData )
        m_refData = new wxRegionRefData;
    else
        AllocExclusive();

    M_REGIONDATA->m_rects.push_back(rect);
    return true;
}

#undef M_REGIONDATA

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxRegionIterator,wxObject);

wxRegionIterator::wxRegionIterator()
    : m_current(0u)
{
}

wxRegionIterator::wxRegionIterator(const wxRegion& region)
    : m_current(0u),
      m_region(region)
{
}

void wxRegionIterator::Reset()
{
     m_current = 0u;
}

void wxRegionIterator::Reset(const wxRegion& region)
{
    m_region = region;
    m_current = 0u;
}

bool wxRegionIterator::HaveRects() const
{
    if ( !m_region.IsOk() )
        return false;

    const wxRegionRefData *data = (wxRegionRefData *)m_region.m_refData;
    return data && m_current < data->m_rects.size();
}

wxRegionIterator::operator bool () const
{
    return HaveRects();
}

void wxRegionIterator::operator ++ ()
{
    if ( HaveRects() )
        ++m_current;
}

void wxRegionIterator::operator ++ (int)
{
    ++*this;
}

wxCoord wxRegionIterator::GetX() const
{
    return GetRect().x;
}

wxCoord wxRegionIterator::GetY() const
{
    return GetRect().y;
}

wxCoord wxRegionIterator::GetW() const
{
    return GetWidth();
}

wxCoord wxRegionIterator::GetWidth() const
{
    return GetRect().width;
}

wxCoord wxRegionIterator::GetH() const
{
    return GetHeight();
}

wxCoord wxRegionIterator::GetHeight() const
{
    return GetRect().height;
}

wxRect wxRegionIterator::GetRect() const
{
    wxCHECK_MSG( HaveRects(), wxRect(), "Invalid region iterator" );

    return ((wxRegionRefData *)m_region.m_refData)->m_rects[m_current];
}
