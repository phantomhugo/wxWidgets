/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/font.h
// Purpose:     wxFont class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

// Font
class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

    wxFont(const wxFontInfo& info)
    {
        if ( info.IsUsingSizeInPixels() )
            SetPixelSize(info.GetPixelSize());
        else if ( info.GetFractionalPointSize() > 0 )
            SetFractionalPointSize(info.GetFractionalPointSize());

        if ( info.HasFaceName() )
            SetFaceName(info.GetFaceName());
        else
            SetFamily(info.GetFamily());

        SetStyle(info.GetStyle());
        SetNumericWeight(info.GetNumericWeight());
        SetUnderlined(info.IsUnderlined());
        SetStrikethrough(info.IsStrikethrough());
        SetEncoding(info.GetEncoding());
    }

    wxFont(int size,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
        : wxFont(InfoFromLegacyParams(size, family, style, weight,
                                      underlined, face, encoding))
    {
    }

    wxFont(const wxSize& pixelSize,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
        : wxFont(InfoFromLegacyParams(pixelSize, family, style, weight,
                                      underlined, face, encoding))
    {
    }

    bool Create(wxSize size,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    wxFont(const wxNativeFontInfo& info);

    wxFont(const wxString &nativeInfoString)
    {
        Create(nativeInfoString);
    }

    bool Create(const wxString& fontname,
        wxFontEncoding fontenc = wxFONTENCODING_DEFAULT);

    virtual ~wxFont() = default;

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const;
    virtual wxFontStyle GetStyle() const;
    virtual int GetNumericWeight() const;
    virtual bool GetUnderlined() const;
    virtual bool GetStrikethrough() const override;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    // accessors with a default implementation in the base class that we can
    // implement more accurately using the stored font description
    virtual wxSize GetPixelSize() const override;
    virtual bool IsUsingSizeInPixels() const override;

    virtual bool IsFixedWidth() const;

    virtual void SetFractionalPointSize(double pointSize);
    virtual void SetPixelSize(const wxSize& pixelSize) override;
    virtual void SetFamily(wxFontFamily family);
    virtual void SetStyle(wxFontStyle style);
    virtual void SetNumericWeight(int weight);
    virtual bool SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetStrikethrough(bool strikethrough) override;
    virtual void SetEncoding(wxFontEncoding encoding);

    wxDECLARE_COMMON_FONT_METHODS();

    // Implementation

    // Find an existing, or create a new, XFontStruct
    // based on this wxFont and the given scale. Append the
protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );
    virtual wxFontFamily DoGetFamily() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif
    // _WX_FONT_H_
