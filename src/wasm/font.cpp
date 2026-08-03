/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/font.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/font.h"
#include "wx/fontutil.h"

//-----------------------------------------------------------------------------
// wxFontRefData
//-----------------------------------------------------------------------------

// reference counted data storing the full generic wxNativeFontInfo of a
// wxFont (point/pixel size, family, style, weight, underlined, strikethrough,
// face name and encoding)
class wxFontRefData: public wxGDIRefData
{
public:
    wxFontRefData() {}

    wxFontRefData(const wxFontInfo& info)
    {
        InitFromInfo(info);
    }

    wxFontRefData( const wxFontRefData& data )
        : wxGDIRefData(),
          m_nativeFontInfo(data.m_nativeFontInfo)
    {
    }

    void InitFromInfo(const wxFontInfo& info)
    {
        if ( info.IsUsingSizeInPixels() )
            m_nativeFontInfo.SetPixelSize(info.GetPixelSize());
        else if ( info.GetFractionalPointSize() > 0 )
            m_nativeFontInfo.SetFractionalPointSize(info.GetFractionalPointSize());

        if ( info.HasFaceName() )
            m_nativeFontInfo.SetFaceName(info.GetFaceName());
        else
            m_nativeFontInfo.SetFamily(info.GetFamily());

        m_nativeFontInfo.SetStyle(info.GetStyle());
        m_nativeFontInfo.SetNumericWeight(info.GetNumericWeight());
        m_nativeFontInfo.SetUnderlined(info.IsUnderlined());
        m_nativeFontInfo.SetStrikethrough(info.IsStrikethrough());
        m_nativeFontInfo.SetEncoding(info.GetEncoding());
    }

    wxNativeFontInfo m_nativeFontInfo;
};

#define M_FONTDATA ((wxFontRefData *)m_refData)->m_nativeFontInfo

wxFont::wxFont(const wxNativeFontInfo& info)
{
    m_refData = new wxFontRefData();
    M_FONTDATA = info;
}

bool wxFont::Create(wxSize size, wxFontFamily family, wxFontStyle style,
        wxFontWeight weight, bool underlined, const wxString& face,
        wxFontEncoding encoding)
{
    UnRef();

    m_refData = new wxFontRefData(InfoFromLegacyParams(size, family,
                                                       style, weight, underlined,
                                                       face, encoding));

    return true;
}

bool wxFont::Create(const wxString& fontname, wxFontEncoding fontenc)
{
    UnRef();

    m_refData = new wxFontRefData();

    // the string is normally a serialized wxNativeFontInfo (as produced by
    // GetNativeFontInfoDesc()); if parsing it fails, treat it as a face name
    if ( !M_FONTDATA.FromString(fontname) )
    {
        if ( fontname.empty() )
        {
            UnRef();
            return false;
        }

        M_FONTDATA.SetFaceName(fontname);
    }

    if ( fontenc != wxFONTENCODING_DEFAULT )
        M_FONTDATA.SetEncoding(fontenc);

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

wxSize wxFont::GetPixelSize() const
{
    wxCHECK_MSG( IsOk(), wxDefaultSize, "invalid font" );

    return M_FONTDATA.GetPixelSize();
}

bool wxFont::IsUsingSizeInPixels() const
{
    return IsOk() && M_FONTDATA.pixelSize != wxDefaultSize;
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( IsOk(), false, "invalid font" );

    // the browser cannot tell us whether a font is monospaced, so use a
    // heuristic: the font is considered fixed-width if its family is
    // teletype or its face name contains "mono" (case-insensitive), e.g.
    // "monospace" or "DejaVu Sans Mono"
    if ( M_FONTDATA.GetFamily() == wxFONTFAMILY_TELETYPE )
        return true;

    return M_FONTDATA.GetFaceName().Lower().Contains("mono");
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
    AllocExclusive();

    // the generic wxNativeFontInfo is a plain struct, just copy it wholly
    // (this also preserves the pixel size)
    M_FONTDATA = info;
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
    if ( pixelSize != wxDefaultSize )
        return pixelSize;

    // the size was specified in points: approximate the pixel size assuming
    // 96 DPI, which is the CSS reference resolution
    return wxSize(wxDefaultCoord, pointSize > 0 ? wxRound(pointSize * 96.0 / 72.0) : 0);
}

void wxNativeFontInfo::SetPixelSize(const wxSize& size)
{
    pixelSize = size;

    // keep a coherent point size: approximate it assuming 96 DPI (CSS)
    if ( size.GetHeight() > 0 )
        pointSize = size.GetHeight() * 72.0 / 96.0;
}
