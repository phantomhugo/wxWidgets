/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/brush.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/brush.h"
#include "wx/colour.h"
#include "wx/bitmap.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxBrush,wxGDIObject);

//-----------------------------------------------------------------------------
// wxBrush
//-----------------------------------------------------------------------------

class wxBrushRefData: public wxGDIRefData
{
    public:
        wxBrushRefData() :
            m_style(wxBRUSHSTYLE_INVALID)
        {
        }

        wxBrushRefData( const wxBrushRefData& data )
        {
            m_style = data.m_style;
        }

        bool operator == (const wxBrushRefData& data) const
        {

        }

        // To keep if mask is stippled
        wxBrushStyle m_style;
};

//-----------------------------------------------------------------------------


wxBrush::wxBrush()
{
    m_refData = new wxBrushRefData();
}

wxBrush::wxBrush(const wxColour& col, wxBrushStyle style )
{

}

wxBrush::wxBrush(const wxColour& col, int style)
{

}

wxBrush::wxBrush(const wxBitmap& stipple)
{
    m_refData = new wxBrushRefData();
    M_BRUSHDATA.setTexture(*stipple.GetHandle());
    if (stipple.GetMask() != nullptr)
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE;
    else
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE;
}


void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
    M_BRUSHDATA.setColor(col.GetQColor());
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_BRUSHDATA.setColor(QColor(r, g, b));
}

void wxBrush::SetStyle(wxBrushStyle style)
{
    AllocExclusive();
    M_BRUSHDATA.setStyle(ConvertBrushStyle((wxBrushStyle)style));
    M_STYLEDATA = style;
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
    M_BRUSHDATA.setTexture(*stipple.GetHandle());

    if (stipple.GetMask() != nullptr)
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE_MASK_OPAQUE;
    else
        M_STYLEDATA = wxBRUSHSTYLE_STIPPLE;
}


bool wxBrush::operator==(const wxBrush& brush) const
{
    if (m_refData == brush.m_refData) return true;

    if (!m_refData || !brush.m_refData) return false;

    return ( *(wxBrushRefData*)m_refData == *(wxBrushRefData*)brush.m_refData );
}


wxColour wxBrush::GetColour() const
{
    return wxColour(M_BRUSHDATA.color());
}

wxBrushStyle wxBrush::GetStyle() const
{
    return M_STYLEDATA;
}

wxBitmap *wxBrush::GetStipple() const
{
    QPixmap p = M_BRUSHDATA.texture();

    if (p.isNull())
        return new wxBitmap();
    else
        return new wxBitmap(p);
}

QBrush wxBrush::GetHandle() const
{
    return M_BRUSHDATA;
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
