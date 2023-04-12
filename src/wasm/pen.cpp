/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/pen.cpp
// Author:      Peter Most, Javier Torres
// Copyright:   (c) Peter Most, Javier Torres
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/pen.h"
#include "wx/colour.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxPen,wxGDIObject);

//-----------------------------------------------------------------------------
// wxPen
//-----------------------------------------------------------------------------

class wxPenRefData: public wxGDIRefData
{
    public:
        void defaultPen()
        {

            m_dashesSize = 0;
        }

        wxPenRefData()
        {
            defaultPen();
        }

        wxPenRefData( const wxPenRefData& data )
        : wxGDIRefData()
            , m_qtPen(data.m_qtPen)
        {
            defaultPen();
        }

        bool operator == (const wxPenRefData& data) const
        {
             return m_qtPen == data.m_qtPen;
        }

        const wxDash *m_dashes;
        int m_dashesSize;
};

//-----------------------------------------------------------------------------

wxPen::wxPen()
{
    m_refData = new wxPenRefData();
}

wxPen::wxPen( const wxColour &colour, int width, wxPenStyle style)
{
    m_refData = new wxPenRefData();
    M_PENDATA.setWidth(width);
    M_PENDATA.setStyle(ConvertPenStyle(style));
    M_PENDATA.setColor(colour.GetQColor());
}

wxPen::wxPen(const wxColour& col, int width, int style)
{
    m_refData = new wxPenRefData();
    M_PENDATA.setWidth(width);
    M_PENDATA.setStyle(ConvertPenStyle((wxPenStyle)style));
    M_PENDATA.setColor(col.GetQColor());
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
    M_PENDATA.setColor(col.GetQColor());
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
    M_PENDATA.setColor(QColor(r, g, b));
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();
    M_PENDATA.setWidth(width);
}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();
    M_PENDATA.setStyle(ConvertPenStyle(style));
}

void wxPen::SetStipple(const wxBitmap& WXUNUSED(stipple))
{
    wxFAIL_MSG( "stippled pens not supported" );
}

void wxPen::SetDashes(int nb_dashes, const wxDash *dash)
{
    AllocExclusive();
    ((wxPenRefData *)m_refData)->m_dashes = dash;
    ((wxPenRefData *)m_refData)->m_dashesSize = nb_dashes;

    QVector<qreal> dashes;
    if (dash)
    {
        for (int i = 0; i < nb_dashes; i++)
            dashes << dash[i];
    }

    M_PENDATA.setDashPattern(dashes);
}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();
    M_PENDATA.setJoinStyle(ConvertPenJoinStyle(join));
}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();
    M_PENDATA.setCapStyle(ConvertPenCapStyle(cap));
}

wxColour wxPen::GetColour() const
{
    wxColour c(M_PENDATA.color());
    return c;
}

wxBitmap *wxPen::GetStipple() const
{
    return nullptr;
}

wxPenStyle wxPen::GetStyle() const
{
    return ConvertPenStyle(M_PENDATA.style());
}

wxPenJoin wxPen::GetJoin() const
{
    return ConvertPenJoinStyle(M_PENDATA.joinStyle());
}

wxPenCap wxPen::GetCap() const
{
    return ConvertPenCapStyle(M_PENDATA.capStyle());
}

int wxPen::GetWidth() const
{
    return M_PENDATA.width();
}

int wxPen::GetDashes(wxDash **ptr) const
{
    *ptr = (wxDash *)((wxPenRefData *)m_refData)->m_dashes;
    return ((wxPenRefData *)m_refData)->m_dashesSize;
}

QPen wxPen::GetHandle() const
{
    return M_PENDATA;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
