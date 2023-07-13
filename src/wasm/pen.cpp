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
        {
            defaultPen();
        }

        bool operator == (const wxPenRefData& data) const
        {
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

}

wxPen::wxPen(const wxColour& col, int width, int style)
{
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
}

void wxPen::SetColour(unsigned char r, unsigned char g, unsigned char b)
{
    AllocExclusive();
}

void wxPen::SetWidth(int width)
{
    AllocExclusive();

}

void wxPen::SetStyle(wxPenStyle style)
{
    AllocExclusive();

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


}

void wxPen::SetJoin(wxPenJoin join)
{
    AllocExclusive();

}

void wxPen::SetCap(wxPenCap cap)
{
    AllocExclusive();

}

wxColour wxPen::GetColour() const
{

}

wxBitmap *wxPen::GetStipple() const
{
    return nullptr;
}

wxPenStyle wxPen::GetStyle() const
{

}

wxPenJoin wxPen::GetJoin() const
{

}

wxPenCap wxPen::GetCap() const
{

}

int wxPen::GetWidth() const
{

}

int wxPen::GetDashes(wxDash **ptr) const
{
    *ptr = (wxDash *)((wxPenRefData *)m_refData)->m_dashes;
    return ((wxPenRefData *)m_refData)->m_dashesSize;
}

wxGDIRefData *wxPen::CreateGDIRefData() const
{
    return new wxPenRefData;
}

wxGDIRefData *wxPen::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxPenRefData(*(wxPenRefData *)data);
}
