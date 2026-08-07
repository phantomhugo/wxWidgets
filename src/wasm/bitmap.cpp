/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/bitmap.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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

#include <emscripten.h>

//-----------------------------------------------------------------------------
// wxBitmapRefData
//-----------------------------------------------------------------------------

class wxBitmapRefData: public wxGDIRefData
{
public:
    wxBitmapRefData()
        : m_mask(nullptr), m_width(0), m_height(0), m_depth(0),
          m_pixels(nullptr), m_hasAlpha(false) {}

    wxBitmapRefData(int width, int height, int depth)
        : m_mask(nullptr), m_width(width), m_height(height), m_depth(depth),
          m_pixels(nullptr), m_hasAlpha(false)
    {
        // depth <= 0 means "screen depth": the browser canvas is always
        // 32bpp, so treat it as 32
        if (m_depth <= 0)
            m_depth = 32;

        // the pixels are always stored as RGBA (4 bytes per pixel),
        // whatever the logical depth
        if (m_width > 0 && m_height > 0)
        {
            m_pixels = new unsigned char[m_width * m_height * 4]();
            m_hasAlpha = (m_depth == 32);
        }
    }

    wxBitmapRefData(const wxBitmapRefData& other)
        : wxGDIRefData(),
          m_mask(other.m_mask ? new wxMask(*other.m_mask) : nullptr),
          m_width(other.m_width),
          m_height(other.m_height),
          m_depth(other.m_depth),
          m_pixels(nullptr),
          m_hasAlpha(other.m_hasAlpha)
    {
        if (other.m_pixels && m_width > 0 && m_height > 0)
        {
            const size_t size = m_width * m_height * 4;
            m_pixels = new unsigned char[size];
            memcpy(m_pixels, other.m_pixels, size);
        }
    }

    virtual ~wxBitmapRefData()
    {
        delete m_mask;
        delete[] m_pixels;
    }

    wxMask *m_mask;
    int m_width;
    int m_height;
    int m_depth;
    unsigned char *m_pixels;
    bool m_hasAlpha;

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
#define M_PIXELS ((wxBitmapRefData *)m_refData)->m_pixels
#define M_HASALPHA ((wxBitmapRefData *)m_refData)->m_hasAlpha

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
#if wxUSE_IMAGE
    if (bits)
    {
        // wxImage knows how to parse XPM data, reuse it to rasterize the
        // pixels into our RGBA buffer
        wxImage image(bits);
        if (image.IsOk())
            CreateFromImage(image);
    }
#else
    wxUnusedVar(bits);
#endif
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type )
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const wxImage& image, int depth, double WXUNUSED(scale))
{
    if (image.IsOk())
    {
        CreateFromImage(image, depth);
    }
}

wxBitmap::wxBitmap(const wxImage& image, const wxDC& WXUNUSED(dc))
{
    if (image.IsOk())
    {
        CreateFromImage(image);
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
    if (!IsOk() || !M_PIXELS)
        return wxImage();

    const int w = M_WIDTH;
    const int h = M_HEIGHT;
    wxImage image(w, h);

    // propagate the alpha channel when the bitmap has one
    if (M_HASALPHA)
        image.SetAlpha();

    unsigned char *rgb = image.GetData();
    unsigned char *alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;

    const unsigned char *src = M_PIXELS;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const int dstIdx = (y * w + x) * 3;
            const int srcIdx = ((y * w + x) * 4);
            rgb[dstIdx + 0] = src[srcIdx + 0]; // R
            rgb[dstIdx + 1] = src[srcIdx + 1]; // G
            rgb[dstIdx + 2] = src[srcIdx + 2]; // B
            if (alpha)
                alpha[y * w + x] = src[srcIdx + 3]; // A
        }
    }

    // Fold the mask into the image alpha channel: the mask is a 1bpp bit
    // array, MSB first, where a set bit means the pixel is shown and a clear
    // one that it is masked out (fully transparent). When the bitmap also
    // has real alpha, both are combined (a masked-out pixel stays masked).
    if (M_MASK)
    {
        const unsigned char *bits =
            static_cast<const unsigned char*>(M_MASK->GetBitmap());
        if (bits)
        {
            if (!image.HasAlpha())
                image.SetAlpha();
            alpha = image.GetAlpha();

            const int stride = (w + 7) / 8;
            for (int y = 0; y < h; ++y)
            {
                for (int x = 0; x < w; ++x)
                {
                    if ( !(bits[y * stride + x / 8] & (0x80 >> (x % 8))) )
                        alpha[y * w + x] = 0;
                }
            }
        }
    }

    return image;
}

bool wxBitmap::CreateFromImage(const wxImage& image, int depth)
{
    wxCHECK_MSG( image.IsOk(), false, "invalid image" );

    UnRef();

    const int w = image.GetWidth();
    const int h = image.GetHeight();

    m_refData = new wxBitmapRefData(w, h, depth);

    // the alpha is real only if it comes from the image
    M_HASALPHA = image.HasAlpha();

    const unsigned char *rgb = image.GetData();
    const unsigned char *alpha = image.GetAlpha(); // may be nullptr

    unsigned char *dst = M_PIXELS;
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const int srcIdx = (y * w + x) * 3;
            const int dstIdx = (y * w + x) * 4;
            dst[dstIdx + 0] = rgb[srcIdx + 0]; // R
            dst[dstIdx + 1] = rgb[srcIdx + 1]; // G
            dst[dstIdx + 2] = rgb[srcIdx + 2]; // B
            dst[dstIdx + 3] = alpha ? alpha[y * w + x] : 255; // A
        }
    }

    return true;
}

#endif // wxUSE_IMAGE

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{
#if wxUSE_IMAGE
    if (icon.IsOk())
    {
        // the generic wxIcon used by this port is just a wxBitmap, so copy
        // its pixel buffer through a wxImage
        const wxImage image = static_cast<const wxBitmap&>(icon).ConvertToImage();
        if (image.IsOk())
            return CreateFromImage(image);
    }
#else
    wxUnusedVar(icon);
#endif
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

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect_) const
{
    wxBitmap bmp;
    if (IsOk())
    {
        // clamp the requested rectangle to the bitmap bounds to avoid
        // reading or writing out of the pixel buffer
        const wxRect rect = rect_.Intersect(wxRect(0, 0, M_WIDTH, M_HEIGHT));
        if (rect.width > 0 && rect.height > 0)
        {
            bmp.Create(rect.width, rect.height, GetDepth());
            wxBitmapRefData *srcData = (wxBitmapRefData *)m_refData;
            wxBitmapRefData *dstData = (wxBitmapRefData *)bmp.m_refData;
            if (srcData->m_pixels && dstData->m_pixels)
            {
                dstData->m_hasAlpha = srcData->m_hasAlpha;
                for (int y = 0; y < rect.height; ++y)
                {
                    for (int x = 0; x < rect.width; ++x)
                    {
                        int sx = rect.x + x;
                        int sy = rect.y + y;
                        int srcIdx = ((sy * srcData->m_width) + sx) * 4;
                        int dstIdx = ((y * dstData->m_width) + x) * 4;
                        memcpy(&dstData->m_pixels[dstIdx], &srcData->m_pixels[srcIdx], 4);
                    }
                }
            }
        }
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
#if wxUSE_IMAGE
    // The Emscripten MEMFS is a real POSIX filesystem for us: preloaded or
    // uploaded files can simply be read through wxImage (wxFileSystem).
    wxImage image;
    if ( image.LoadFile(name, type) )
        return CreateFromImage(image);
#else
    wxUnusedVar(name);
    wxUnusedVar(type);
#endif

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
    // real alpha only for 32bpp bitmaps or bitmaps created from an image
    // with an alpha channel
    return m_refData && M_HASALPHA;
}

//-----------------------------------------------------------------------------
// raw bitmap access support
//-----------------------------------------------------------------------------

void *wxBitmap::GetRawData(wxPixelDataBase& data, int bpp)
{
    if (!IsOk() || !M_PIXELS)
        return nullptr;

    if (bpp != 32 || M_DEPTH != 32)
        return nullptr;

    data.m_width = M_WIDTH;
    data.m_height = M_HEIGHT;
    data.m_stride = M_WIDTH * 4;

    return M_PIXELS;
}

void wxBitmap::UngetRawData(wxPixelDataBase& WXUNUSED(data))
{
    // nothing to do, we keep the pixel buffer in the bitmap
}

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxMask, wxObject);

// The mask is stored in m_bitmap (WXPixmap is void* in this port) as a 1bpp
// bit array, MSB first: a set bit means the corresponding pixel is shown, a
// clear bit that it is masked out. wxBitmap::ConvertToImage() folds it into
// the image alpha channel, so it is honoured when drawing with useMask.

static int wxWasmMaskStride(int width)
{
    return (width + 7) / 8;
}

wxMask::wxMask()
    : m_bitmap(nullptr), m_size(0, 0)
{
}

wxMask::wxMask(const wxMask &mask)
    : wxObject(),
      m_bitmap(nullptr),
      m_size(mask.m_size)
{
    if (mask.m_bitmap && m_size.x > 0 && m_size.y > 0)
    {
        const size_t size = wxWasmMaskStride(m_size.x) * m_size.y;
        unsigned char *bits = new unsigned char[size];
        memcpy(bits, mask.m_bitmap, size);
        m_bitmap = bits;
    }
}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
    : m_bitmap(nullptr), m_size(0, 0)
{
    Create(bitmap, colour);
}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
    : m_bitmap(nullptr), m_size(0, 0)
{
    Create(bitmap, paletteIndex);
}

wxMask::wxMask(const wxBitmap& bitmap)
    : m_bitmap(nullptr), m_size(0, 0)
{
    Create(bitmap);
}

wxMask::~wxMask()
{
    delete[] static_cast<unsigned char*>(m_bitmap);
}

bool wxMask::Create(const wxBitmap& bitmap, const wxColour& colour)
{
#if wxUSE_IMAGE
    wxCHECK_MSG( bitmap.IsOk(), false, "invalid bitmap" );

    const wxImage image = bitmap.ConvertToImage();
    if ( !image.IsOk() )
        return false;

    delete[] static_cast<unsigned char*>(m_bitmap);

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int stride = wxWasmMaskStride(w);
    unsigned char *bits = new unsigned char[stride * h]();

    const unsigned char *rgb = image.GetData();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const int idx = (y * w + x) * 3;
            // pixels matching the colour are masked out, the rest are shown
            if ( rgb[idx] != colour.Red() ||
                 rgb[idx + 1] != colour.Green() ||
                 rgb[idx + 2] != colour.Blue() )
            {
                bits[y * stride + x / 8] |= 0x80 >> (x % 8);
            }
        }
    }

    m_bitmap = bits;
    m_size = wxSize(w, h);

    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(bitmap);
    wxUnusedVar(colour);

    return false;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

bool wxMask::Create(const wxBitmap& WXUNUSED(bitmap), int WXUNUSED(paletteIndex))
{
    // palettes are not supported in this port (wxBitmap::GetPalette()
    // always returns nullptr), so a mask can't be created from an index
    return false;
}

bool wxMask::Create(const wxBitmap& bitmap)
{
#if wxUSE_IMAGE
    // the bitmap is the mask itself: white pixels are shown, black ones are
    // masked out (see the generic wxMask::InitFromMonoBitmap())
    wxCHECK_MSG( bitmap.IsOk(), false, "invalid bitmap" );

    const wxImage image = bitmap.ConvertToImage();
    if ( !image.IsOk() )
        return false;

    delete[] static_cast<unsigned char*>(m_bitmap);

    const int w = image.GetWidth();
    const int h = image.GetHeight();
    const int stride = wxWasmMaskStride(w);
    unsigned char *bits = new unsigned char[stride * h]();

    const unsigned char *rgb = image.GetData();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            const int idx = (y * w + x) * 3;
            if ( rgb[idx] + rgb[idx + 1] + rgb[idx + 2] >= 3 * 128 )
                bits[y * stride + x / 8] |= 0x80 >> (x % 8);
        }
    }

    m_bitmap = bits;
    m_size = wxSize(w, h);

    return true;
#else // !wxUSE_IMAGE
    wxUnusedVar(bitmap);

    return false;
#endif // wxUSE_IMAGE/!wxUSE_IMAGE
}

WXPixmap wxMask::GetBitmap() const
{
    return m_bitmap;
}
