/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/pen.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject);

//-----------------------------------------------------------------------------
// wxPenRefData
//-----------------------------------------------------------------------------

class wxPenRefData: public wxGDIRefData
{
public:
    wxPenRefData()
        : m_width(1),
          m_style(wxPENSTYLE_SOLID),
          m_join(wxJOIN_ROUND),
          m_cap(wxCAP_ROUND),
          m_dashes(nullptr),
          m_dashesSize(0)
    {
    }

    wxPenRefData( const wxPenRefData& data )
        : wxGDIRefData(),
          m_colour(data.m_colour),
          m_width(data.m_width),
          m_style(data.m_style),
          m_join(data.m_join),
          m_cap(data.m_cap),
          m_dashes(data.m_dashes),
          m_dashesSize(data.m_dashesSize)
    {
    }

    bool operator == (const wxPenRefData& data) const
    {
        // the dash array is owned by the caller, so we can only compare the
        // pointer and the count, not the dash values themselves
        return m_colour == data.m_colour &&
               m_width == data.m_width &&
               m_style == data.m_style &&
               m_join == data.m_join &&
               m_cap == data.m_cap &&
               m_dashes == data.m_dashes &&
               m_dashesSize == data.m_dashesSize;
    }

    wxColour m_colour;
    int m_width;
    wxPenStyle m_style;
    wxPenJoin m_join;
    wxPenCap m_cap;
    const wxDash *m_dashes;
    int m_dashesSize;
};

#define M_PENDATA ((wxPenRefData *)m_refData)

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

wxPen::wxPen()
{
    m_refData = new wxPenRefData();
}

wxPen::wxPen( const wxColour &colour, int width, wxPenStyle style)
{
    m_refData = new wxPenRefData();
    M_PENDATA->m_colour = colour;
    M_PENDATA->m_width = width;
    M_PENDATA->m_style = style;
}

wxPen::wxPen(const wxColour& col, int width, int style)
    : wxPen(col, width, (wxPenStyle)style)
{
}

wxPen::wxPen(const wxBitmap& WXUNUSED(stipple), int width)
{
    // stippled pens are not supported in this port (see SetStipple), but we
    // still keep the rest of the pen parameters
    m_refData = new wxPenRefData();
    M_PENDATA->m_width = width;
}

wxPen::wxPen(const wxPenInfo& info)
{
    m_refData = new wxPenRefData();
    M_PENDATA->m_colour = info.GetColour();
    M_PENDATA->m_width = info.GetWidth();
    M_PENDATA->m_style = info.GetStyle();
    M_PENDATA->m_join = info.GetJoin();
    M_PENDATA->m_cap = info.GetCap();

    wxDash *dash = nullptr;
    M_PENDATA->m_dashesSize = info.GetDashes(&dash);
    M_PENDATA->m_dashes = dash;
}

bool wxPen::operator==(const wxPen& pen) const
{
    if (m_refData == pen.m_refData) return true;

    if (!m_refData || !pen.m_refData) return false;

    return ( *(wxPenRefData*)m_refData == *(wxPenRefData*)pen.m_refData );
}

bool wxPen::operator!=(const wxPen& pen) const
{
    return !(*this == pen);
}

void wxPen::SetColour(const wxColour& col)
{
    AllocExclusive();

    M_PENDATA->m_colour = col;
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();

    M_PENDATA->m_colour.Set(r, g, b);
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();

    M_PENDATA->m_width = width;
}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();

    M_PENDATA->m_style = style;
}

void wxPen::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( "stippled pens not supported" );
}

void wxPen::SetDashes(int nb_dashes, const wxDash *dash)
{
    AllocExclusive();

    M_PENDATA->m_dashes = dash;
    M_PENDATA->m_dashesSize = nb_dashes;
}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();

    M_PENDATA->m_join = join;
}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();

    M_PENDATA->m_cap = cap;
}

wxColour wxPen::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, "invalid pen" );

    return M_PENDATA->m_colour;
}

wxBitmap *wxPen::GetStipple() const
{
    return nullptr;
}

wxPenStyle wxPen::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxPENSTYLE_INVALID, "invalid pen" );

    return M_PENDATA->m_style;
}

wxPenJoin wxPen::GetJoin() const
{
    wxCHECK_MSG( IsOk(), wxJOIN_INVALID, "invalid pen" );

    return M_PENDATA->m_join;
}

wxPenCap wxPen::GetCap() const
{
    wxCHECK_MSG( IsOk(), wxCAP_INVALID, "invalid pen" );

    return M_PENDATA->m_cap;
}

int wxPen::GetWidth() const
{
    wxCHECK_MSG( IsOk(), -1, "invalid pen" );

    return M_PENDATA->m_width;
}

int wxPen::GetDashes(wxDash **ptr) const
{
    *ptr = (wxDash *)M_PENDATA->m_dashes;
    return M_PENDATA->m_dashesSize;
}

int wxPen::GetDashCount() const
{
    wxCHECK_MSG( IsOk(), -1, "invalid pen" );

    return M_PENDATA->m_dashesSize;
}

wxDash *wxPen::GetDash() const
{
    wxCHECK_MSG( IsOk(), nullptr, "invalid pen" );

    return (wxDash *)M_PENDATA->m_dashes;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
