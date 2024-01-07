/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/font.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"
#include "wx/fontutil.h"

// Older versions of QT don't define all the QFont::Weight enum values, so just
// do it ourselves here for all case instead.
class wxFontRefData: public wxGDIRefData
{
public:
    wxFontRefData() {}

    wxFontRefData(const wxFontInfo& info)
    {
    }

    wxFontRefData( const wxFontRefData& data )
    : wxGDIRefData()
    {
    }

    wxNativeFontInfo m_nativeFontInfo;
};

#define M_FONTDATA ((wxFontRefData *)m_refData)->m_nativeFontInfo

wxFont::wxFont(const wxNativeFontInfo& info)
{

}

bool wxFont::Create(wxSize size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(InfoFromLegacyParams(size.GetHeight(), family,
                                                       style, weight, underlined,
                                                       face, encoding));

    return true;
}

double wxFont::GetFractionalPointSize() const
{
    return M_FONTDATA.GetFractionalPointSize();
}

wxFontStyle wxFont::GetStyle() const
{
    return M_FONTDATA.GetStyle();
}

int wxFont::GetNumericWeight() const
{
    return M_FONTDATA.GetNumericWeight();
}

bool wxFont::GetUnderlined() const
{
    return M_FONTDATA.GetUnderlined();
}

wxString wxFont::GetFaceName() const
{
    return M_FONTDATA.GetFaceName();
}

wxFontEncoding wxFont::GetEncoding() const
{
    return M_FONTDATA.GetEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    return &M_FONTDATA;
}

bool wxFont::GetStrikethrough() const
{
    return M_FONTDATA.GetStrikethrough();
}

bool wxFont::IsFixedWidth() const
{

}

void wxFont::SetFractionalPointSize(double pointSize)
{
    AllocExclusive();

    M_FONTDATA.SetFractionalPointSize(pointSize);
}

bool wxFont::SetFaceName(const wxString& facename)
{
    AllocExclusive();

    return M_FONTDATA.SetFaceName(facename);
}

void wxFont::SetFamily( wxFontFamily family )
{
    AllocExclusive();

    M_FONTDATA.SetFamily(family);
}

void wxFont::SetStyle( wxFontStyle style )
{
    AllocExclusive();

    M_FONTDATA.SetStyle(style);
}

void wxFont::SetNumericWeight(int weight)
{
    AllocExclusive();

    M_FONTDATA.SetNumericWeight(weight);
}

void wxFont::SetUnderlined( bool underlined )
{
    AllocExclusive();

    M_FONTDATA.SetUnderlined(underlined);
}

void wxFont::SetStrikethrough(bool strikethrough)
{
    AllocExclusive();

    M_FONTDATA.SetStrikethrough(strikethrough);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA.SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    SetFractionalPointSize(info.GetPointSize());
    SetFamily(info.GetFamily());
    SetStyle(info.GetStyle());
    SetNumericWeight(info.GetWeight());
    SetUnderlined(info.GetUnderlined());
    SetStrikethrough(info.GetStrikethrough());
    SetFaceName(info.GetFaceName());
    SetEncoding(info.GetEncoding());
}

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData;
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*(wxFontRefData *)data);
}

wxFontFamily wxFont::DoGetFamily() const
{
    return M_FONTDATA.GetFamily();
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

wxSize wxNativeFontInfo::GetPixelSize() const
{

}

void wxNativeFontInfo::SetPixelSize(const wxSize& size)
{

}
