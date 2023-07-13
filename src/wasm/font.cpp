/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/font.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
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

wxFont::wxFont()
{
    m_refData = new wxFontRefData();
}

wxFont::wxFont(const wxFontInfo& info)
{
    m_refData = new wxFontRefData(info);
}

wxFont::wxFont(const wxString& nativeFontInfoString)
{
    m_refData = new wxFontRefData();
}

wxFont::wxFont(const wxNativeFontInfo& info)
{

}

wxFont::wxFont(int size,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    m_refData = new wxFontRefData();
    Create(wxSize(0, size), family, style, weight, underlined, face, encoding);
}

wxFont::wxFont(const wxSize& pixelSize,
       wxFontFamily family,
       wxFontStyle style,
       wxFontWeight weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    Create(pixelSize, family, style, weight, underlined, face, encoding);
}

wxFont::wxFont(int size,
       int family,
       int style,
       int weight,
       bool underlined,
       const wxString& face,
       wxFontEncoding encoding)
{
    Create(wxSize(0, size), (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
}


bool wxFont::Create(wxSize size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding )
{
    UnRef();

    m_refData = new wxFontRefData(InfoFromLegacyParams(size.GetHeight(), family,
                                                       style, weight, underlined,
                                                       face, encoding));

    return true;
}

int wxFont::GetPointSize() const
{
    return M_FONTDATA.wxNativeFontInfo::GetPointSize();
}

double wxFont::GetFractionalPointSize() const
{
    return M_FONTDATA.GetFractionalPointSize();
}

wxSize wxFont::GetPixelSize() const
{
    return M_FONTDATA.GetPixelSize();
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


void wxFont::SetFractionalPointSize(double pointSize)
{
    AllocExclusive();

    M_FONTDATA.SetFractionalPointSize(pointSize);
}

void wxFont::SetPixelSize(const wxSize& pixelSize)
{
    AllocExclusive();

    M_FONTDATA.SetPixelSize(pixelSize);
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

void wxNativeFontInfo::Init()
{
}

double wxNativeFontInfo::GetFractionalPointSize() const
{

}

wxSize wxNativeFontInfo::GetPixelSize() const
{

}

wxFontStyle wxNativeFontInfo::GetStyle() const
{

    wxFAIL_MSG( "Invalid font style value" );
    return wxFontStyle();
}

int wxNativeFontInfo::GetNumericWeight() const
{

}

bool wxNativeFontInfo::GetUnderlined() const
{

}

bool wxNativeFontInfo::GetStrikethrough() const
{

}

wxString wxNativeFontInfo::GetFaceName() const
{
    // use font info to get the matched face name (not the family given)

}

wxFontFamily wxNativeFontInfo::GetFamily() const
{

    return wxFONTFAMILY_UNKNOWN;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return wxFONTENCODING_UTF8;
}

void wxNativeFontInfo::SetFractionalPointSize(double pointsize)
{

}

void wxNativeFontInfo::SetPixelSize(const wxSize& size)
{

}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{

}

void wxNativeFontInfo::SetNumericWeight(int weight)
{

}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{

}

void wxNativeFontInfo::SetStrikethrough(bool strikethrough)
{

}

bool wxNativeFontInfo::SetFaceName(const wxString& facename)
{

    return true;
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{

}

void wxNativeFontInfo::SetEncoding(wxFontEncoding WXUNUSED(encoding))
{

}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    return true;
}

wxString wxNativeFontInfo::ToString() const
{

}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromString(s);
}

wxString wxNativeFontInfo::ToUserString() const
{
    return ToString();
}
