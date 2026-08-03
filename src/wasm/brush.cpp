/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/brush.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxBrush,wxGDIObject);

//-----------------------------------------------------------------------------
// wxBrushRefData
//-----------------------------------------------------------------------------

class wxBrushRefData: public wxGDIRefData
{
public:
    wxBrushRefData()
        : m_style(wxBRUSHSTYLE_INVALID)
    {
    }

    wxBrushRefData( const wxBrushRefData& data )
        : wxGDIRefData(),
          m_colour(data.m_colour),
          m_style(data.m_style),
          m_stipple(data.m_stipple)
    {
    }

    bool operator == (const wxBrushRefData& data) const
    {
        if ( m_colour != data.m_colour || m_style != data.m_style )
            return false;

        // for stippled brushes the stipple bitmaps must match too: we only
        // compare the bitmap identity, not the pixel contents
        if ( m_style == wxBRUSHSTYLE_STIPPLE &&
             m_stipple.GetRefData() != data.m_stipple.GetRefData() )
            return false;

        return true;
    }

    wxColour m_colour;
    wxBrushStyle m_style;
    wxBitmap m_stipple;
};

#define M_BRUSHDATA ((wxBrushRefData *)m_refData)

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

wxBrush::wxBrush()
{
    m_refData = new wxBrushRefData();
}

wxBrush::wxBrush(const wxColour& col, wxBrushStyle style )
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA->m_colour = col;
    M_BRUSHDATA->m_style = style;
}

wxBrush::wxBrush(const wxColour& col, int style)
    : wxBrush(col, (wxBrushStyle)style)
{
}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA->m_style = wxBRUSHSTYLE_STIPPLE;
    M_BRUSHDATA->m_stipple = stipple;
}


void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();

    M_BRUSHDATA->m_colour = col;
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();

    M_BRUSHDATA->m_colour.Set(r, g, b);
}

void wxBrush::SetStyle(wxBrushStyle style)
{
    AllocExclusive();

    M_BRUSHDATA->m_style = style;
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();

    M_BRUSHDATA->m_stipple = stipple;
    M_BRUSHDATA->m_style = wxBRUSHSTYLE_STIPPLE;
}


bool wxBrush::operator==(const wxBrush& brush) const
{
    if (m_refData == brush.m_refData) return true;

    if (!m_refData || !brush.m_refData) return false;

    return ( *(wxBrushRefData*)m_refData == *(wxBrushRefData*)brush.m_refData );
}


wxColour wxBrush::GetColour() const
{
    wxCHECK_MSG( IsOk(), wxNullColour, "invalid brush" );

    return M_BRUSHDATA->m_colour;
}

wxBrushStyle wxBrush::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxBRUSHSTYLE_INVALID, "invalid brush" );

    return M_BRUSHDATA->m_style;
}

wxBitmap *wxBrush::GetStipple() const
{
    wxCHECK_MSG( IsOk(), nullptr, "invalid brush" );

    // the caller owns the returned copy
    return new wxBitmap(M_BRUSHDATA->m_stipple);
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
