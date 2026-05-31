/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/bitmap.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/dc.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif // WX_PRECOMP

#include "wx/bitmap.h"
#include "wx/cursor.h"
#include "wx/rawbmp.h"

//-----------------------------------------------------------------------------
// wxBitmapRefData
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxGDIRefData
{
public:
    wxBitmapRefData()
        : m_mask(nullptr), m_width(0), m_height(0), m_depth(0) {}

    wxBitmapRefData(int width, int height, int depth)
        : m_mask(nullptr), m_width(width), m_height(height), m_depth(depth) {}

    wxBitmapRefData(const wxBitmapRefData& other)
        : wxGDIRefData(),
          m_mask(other.m_mask ? new wxMask(*other.m_mask) : nullptr),
          m_width(other.m_width),
          m_height(other.m_height),
          m_depth(other.m_depth) {}

    virtual ~wxBitmapRefData() { delete m_mask; }

    wxMask *m_mask;
    int m_width;
    int m_height;
    int m_depth;

private:
    wxBitmapRefData& operator=(const wxBitmapRefData& other);
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxObject);

#define M_MASK ((wxBitmapRefData *)m_refData)->m_mask
#define M_WIDTH ((wxBitmapRefData *)m_refData)->m_width
#define M_HEIGHT ((wxBitmapRefData *)m_refData)->m_height
#define M_DEPTH ((wxBitmapRefData *)m_refData)->m_depth

void wxBitmap::InitStandardHandlers()
{
}

wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth )
{
    Create(width, height, depth);
}

wxBitmap::wxBitmap(int width, int height, int depth)
{
    Create(width, height, depth);
}

wxBitmap::wxBitmap(const wxSize& sz, int depth )
{
    Create(sz, depth);
}

wxBitmap::wxBitmap(int width, int height, const wxDC& dc)
{
    Create(width, height, dc);
}

// Create a wxBitmap from xpm data
wxBitmap::wxBitmap(const char* const* bits)
{
    if (bits)
    {
        int width = 0, height = 0, ncolors = 0, cpp = 0;
        sscanf(bits[0], "%d %d %d %d", &width, &height, &ncolors, &cpp);
        Create(width, height, 32);
    }
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type )
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const wxImage& image, int depth, double scale)
{
    if (image.IsOk())
    {
        Create(image.GetWidth(), image.GetHeight(), depth);
    }
}

wxBitmap::wxBitmap(const wxImage& image, const wxDC& dc)
{
    if (image.IsOk())
    {
        Create(image.GetWidth(), image.GetHeight());
    }
}

wxBitmap::wxBitmap(const wxIcon& icon)
{
    CopyFromIcon(icon);
}

wxBitmap::wxBitmap(const wxCursor& cursor)
{
}

bool wxBitmap::Create(int width, int height, int depth )
{
    UnRef();
    m_refData = new wxBitmapRefData(width, height, depth);

    return true;
}

bool wxBitmap::Create(const wxSize& sz, int depth )
{
    return Create(sz.GetWidth(), sz.GetHeight(), depth);
}

bool wxBitmap::Create(int width, int height, const wxDC& WXUNUSED(dc))
{
    return Create(width, height);
}

int wxBitmap::GetHeight() const
{
    return m_refData ? M_HEIGHT : 0;
}

int wxBitmap::GetWidth() const
{
    return m_refData ? M_WIDTH : 0;
}

int wxBitmap::GetDepth() const
{
    return m_refData ? M_DEPTH : 0;
}

#if wxUSE_IMAGE
wxImage wxBitmap::ConvertToImage() const
{
    // TODO: real pixel conversion when WASM bitmap stores pixel data
    return wxImage();
}

#endif // wxUSE_IMAGE

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
    if (icon.IsOk())
    {
        Create(icon.GetWidth(), icon.GetHeight());
        return true;
    }
    return false;
}

wxMask *wxBitmap::GetMask() const
{
    return m_refData ? M_MASK : nullptr;
}

void wxBitmap::SetMask(wxMask *mask)
{
    AllocExclusive();
    delete M_MASK;
    M_MASK = mask;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
    wxBitmap bmp;
    if (IsOk() && rect.width > 0 && rect.height > 0)
    {
        bmp.Create(rect.width, rect.height, GetDepth());
    }
    return bmp;
}


bool wxBitmap::SaveFile(const wxString &name, wxBitmapType type,
              const wxPalette *WXUNUSED(palette) ) const
{
    wxUnusedVar(name);
    wxUnusedVar(type);

    #if wxUSE_IMAGE
    wxImage image = ConvertToImage();
    if (image.IsOk() && image.SaveFile(name, type))
        return true;
    #endif

    return false;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{
    wxUnusedVar(name);
    wxUnusedVar(type);
    //There is no real filesystem here, inmemory filesystem could be used, but in other time...
    return false;
}


#if wxUSE_PALETTE
wxPalette *wxBitmap::GetPalette() const
{
    return nullptr;
}

void wxBitmap::SetPalette(const wxPalette& WXUNUSED(palette))
{
}

#endif // wxUSE_PALETTE

#if WXWIN_COMPATIBILITY_3_0
void wxBitmap::SetHeight(int height)
{
    if (m_refData)
        M_HEIGHT = height;
}

void wxBitmap::SetWidth(int width)
{
    if (m_refData)
        M_WIDTH = width;
}

void wxBitmap::SetDepth(int depth)
{
    if (m_refData)
        M_DEPTH = depth;
}
#endif

wxGDIRefData *wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData;
}

wxGDIRefData *wxBitmap::CloneGDIRefData(const wxGDIRefData *data) const
{
    const wxBitmapRefData *bitmapData = dynamic_cast<const wxBitmapRefData *>(data);
    if (bitmapData)
        return new wxBitmapRefData(*bitmapData);
    return new wxBitmapRefData;
}

bool wxBitmap::HasAlpha() const
{
    return false;
}

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject);

wxMask::wxMask()
{
}

wxMask::wxMask(const wxMask &mask)
{
    wxUnusedVar(mask);
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{
    wxUnusedVar(bitmap);
    wxUnusedVar(colour);
}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{
    wxUnusedVar(bitmap);
    wxUnusedVar(paletteIndex);
}

wxMask::wxMask(const wxBitmap& bitmap)
{
    wxUnusedVar(bitmap);
}

wxMask::~wxMask()
{
}


WXPixmap wxMask::GetBitmap() const
{
    return 0;
}
