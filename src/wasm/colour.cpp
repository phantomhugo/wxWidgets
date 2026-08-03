/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colour.cpp
// Purpose:     wxColour class implementation for wxWasm
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include "wx/colour.h"

namespace
{

// reference counted data storing the RGBA channels of a wxColour
class wxColourRefData : public wxGDIRefData
{
public:
    wxColourRefData() = default;
    wxColourRefData(const wxColourRefData& other)
        : wxGDIRefData(),
          m_red(other.m_red),
          m_green(other.m_green),
          m_blue(other.m_blue),
          m_alpha(other.m_alpha) {}

    wxColourBase::ChannelType m_red = 0;
    wxColourBase::ChannelType m_green = 0;
    wxColourBase::ChannelType m_blue = 0;
    wxColourBase::ChannelType m_alpha = wxALPHA_OPAQUE;
};

} // anonymous namespace

#define M_COLOUR_DATA ((wxColourRefData *)m_refData)

wxIMPLEMENT_DYNAMIC_CLASS(wxColourImpl, wxColourBase);

bool wxColourImpl::operator==(const wxColourImpl& color) const
{
    if ( m_refData == color.m_refData )
        return true;

    if ( !m_refData || !color.m_refData )
        return false;

    return Red() == color.Red() &&
           Green() == color.Green() &&
           Blue() == color.Blue() &&
           Alpha() == color.Alpha();
}

unsigned long wxColourImpl::GetPixel() const
{
    return GetRGB();
}

wxGDIRefData *wxColourImpl::CreateGDIRefData() const
{
    return new wxColourRefData;
}

wxGDIRefData *wxColourImpl::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxColourRefData(*(const wxColourRefData *)data);
}

void wxColourImpl::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
    AllocExclusive();

    if ( !m_refData )
        m_refData = CreateGDIRefData();

    M_COLOUR_DATA->m_red = r;
    M_COLOUR_DATA->m_green = g;
    M_COLOUR_DATA->m_blue = b;
    M_COLOUR_DATA->m_alpha = a;
}

bool wxColourImpl::FromString(const wxString& str)
{
    // the base class parses colour names and #rgb/#rrggbb syntax
    return wxColourBase::FromString(str);
}

unsigned char wxColourImpl::Red() const
{
    return m_refData ? M_COLOUR_DATA->m_red : 0;
}

unsigned char wxColourImpl::Green() const
{
    return m_refData ? M_COLOUR_DATA->m_green : 0;
}

unsigned char wxColourImpl::Blue() const
{
    return m_refData ? M_COLOUR_DATA->m_blue : 0;
}

unsigned char wxColourImpl::Alpha() const
{
    return m_refData ? M_COLOUR_DATA->m_alpha : wxALPHA_OPAQUE;
}

void wxColourImpl::CalcPixel( WXColormap WXUNUSED(cmap) )
{
    // no colormaps in the browser: nothing to do
}

WXColor *wxColourImpl::GetColor() const
{
    // there is no native colour handle in this port
    return nullptr;
}
