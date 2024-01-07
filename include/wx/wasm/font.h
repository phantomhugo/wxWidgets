/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/font.h
// Purpose:     wxFont class
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 Hugo Castellanos
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
    }

    wxFont(int size,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
    }

    wxFont(const wxSize& pixelSize,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        SetPixelSize(pixelSize);
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

    virtual bool IsFixedWidth() const;

    virtual void SetFractionalPointSize(double pointSize);
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
    // font to list in the private data for future reference.

    // TODO This is a fairly basic implementation, that doesn't
    // allow for different facenames, and also doesn't do a mapping
    // between 'standard' facenames (e.g. Arial, Helvetica, Times Roman etc.)
    // and the fonts that are available on a particular system.
    // Maybe we need to scan the user's machine to build up a profile
    // of the fonts and a mapping file.

    // Return font struct, and optionally the Motif font list
    void *GetInternalFont(double scale = 1.0,
        WXDisplay* display = NULL) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );
    virtual wxFontFamily DoGetFamily() const;

    void Unshare();

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif
    // _WX_FONT_H_
