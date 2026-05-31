/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/colour.h
// Purpose:     wxColour class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/palette.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxPaintDC;
class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_FWD_CORE wxColour;

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxColourImpl : public wxColourBase
{
public:
    // constructors
    // ------------
    wxColourImpl() = default;
    wxColourImpl(ChannelType r, ChannelType g, ChannelType b, ChannelType a = wxALPHA_OPAQUE)
        { Set(r, g, b, a); }
    wxColourImpl(const wxColourImpl& col) = default;

    virtual ~wxColourImpl() = default;

    bool operator==(const wxColourImpl& col) const;
    bool operator!=(const wxColourImpl& col) const { return !(*this == col); }

    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue() const;

    // Implementation part

    void CalcPixel( WXColormap cmap );
    unsigned long GetPixel() const;
    WXColor *GetColor() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void
    InitRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    virtual bool FromString(const wxString& str);

private:
    wxDECLARE_DYNAMIC_CLASS(wxColourImpl);
};

#endif // _WX_COLOUR_H_
