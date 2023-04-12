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

}


void wxBrush::SetColour(const wxColour& col)
{
    AllocExclusive();
}

void wxBrush::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
}

void wxBrush::SetStyle(wxBrushStyle style)
{
    AllocExclusive();
}

void wxBrush::SetStipple(const wxBitmap& stipple)
{
    AllocExclusive();
}


bool wxBrush::operator==(const wxBrush& brush) const
{
    if (m_refData == brush.m_refData) return true;

    if (!m_refData || !brush.m_refData) return false;

    return ( *(wxBrushRefData*)m_refData == *(wxBrushRefData*)brush.m_refData );
}


wxColour wxBrush::GetColour() const
{

}

wxBrushStyle wxBrush::GetStyle() const
{
}

wxBitmap *wxBrush::GetStipple() const
{
    if (p.isNull())
        return new wxBitmap();
}

wxGDIRefData *wxBrush::CreateGDIRefData() const
{
    return new wxBrushRefData;
}

wxGDIRefData *wxBrush::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxBrushRefData(*(wxBrushRefData *)data);
}
