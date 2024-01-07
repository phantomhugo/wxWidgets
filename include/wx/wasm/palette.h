/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/palette.h
// Purpose:     wxPalette class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PALETTE_H_
#define _WX_PALETTE_H_

#include "wx/list.h"

class WXDLLIMPEXP_FWD_CORE wxPalette;

// Palette for one display
class wxXPalette : public wxObject
{
    wxDECLARE_DYNAMIC_CLASS(wxXPalette);

public:
    wxXPalette();

    WXDisplay*        m_display;
    int               m_pix_array_n;
    unsigned char*    m_red;
    unsigned char*    m_green;
    unsigned char*    m_blue;
    unsigned long*    m_pix_array;
    WXColormap        m_cmap;
    bool              m_destroyable;
};

class WXDLLIMPEXP_CORE wxPaletteRefData: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxPalette;
public:
    wxPaletteRefData();
    virtual ~wxPaletteRefData();

protected:
    wxList  m_palettes;
};

#define M_PALETTEDATA ((wxPaletteRefData *)m_refData)

class WXDLLIMPEXP_CORE wxPalette : public wxPaletteBase
{
    wxDECLARE_DYNAMIC_CLASS(wxPalette);

public:
    wxPalette();

    wxPalette(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    virtual ~wxPalette() = default;
    bool Create(int n, const unsigned char *red, const unsigned char *green, const unsigned char *blue);
    int GetPixel(unsigned char red, unsigned char green, unsigned char blue) const;
    bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;

    // X-specific
    WXColormap GetXColormap(WXDisplay* display = NULL) const;
    bool TransferBitmap(void *data, int depth, int size);
    bool TransferBitmap8(unsigned char *data, unsigned long size, void *dest, unsigned int bpp);
    unsigned long *GetXPixArray(WXDisplay* display, int *pix_array_n);
    void PutXColormap(WXDisplay* display, WXColormap cmap, bool destroyable);
    virtual int GetColoursCount() const override;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;
};

#endif // _WX_PALETTE_H_
