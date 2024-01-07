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
        wxBitmapRefData() { m_mask = nullptr; }

        wxBitmapRefData( int width, int height, int depth )
        {

        }

        virtual ~wxBitmapRefData() { delete m_mask; }

        wxMask *m_mask;

private:
    wxBitmapRefData(const wxBitmapRefData&other);
    wxBitmapRefData& operator=(const wxBitmapRefData&other);
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmap, wxObject);

#define M_PIXDATA ((wxBitmapRefData *)m_refData)->m_qtPixmap
#define M_MASK ((wxBitmapRefData *)m_refData)->m_mask

void wxBitmap::InitStandardHandlers()
{
}

wxBitmap::wxBitmap()
{
}

wxBitmap::wxBitmap(const char bits[], int width, int height, int depth )
{
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
}

wxBitmap::wxBitmap(const wxString &filename, wxBitmapType type )
{
    LoadFile(filename, type);
}

wxBitmap::wxBitmap(const wxImage& image, int depth, double scale)
{

}

wxBitmap::wxBitmap(const wxImage& image, const wxDC& dc)
{

}

wxBitmap::wxBitmap(const wxIcon& icon)
{

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
}

int wxBitmap::GetWidth() const
{
}

int wxBitmap::GetDepth() const
{
}

#if wxUSE_IMAGE
wxImage wxBitmap::ConvertToImage() const
{

}

#endif // wxUSE_IMAGE

bool wxBitmap::CopyFromIcon(const wxIcon& icon)
{

}

wxMask *wxBitmap::GetMask() const
{
    return M_MASK;
}

void wxBitmap::SetMask(wxMask *mask)
{
    AllocExclusive();
    delete M_MASK;
    M_MASK = mask;
}

wxBitmap wxBitmap::GetSubBitmap(const wxRect& rect) const
{
}


bool wxBitmap::SaveFile(const wxString &name, wxBitmapType type,
              const wxPalette *WXUNUSED(palette) ) const
{
    #if wxUSE_IMAGE
    //Try to save using wx
    wxImage image = ConvertToImage();
    if (image.IsOk() && image.SaveFile(name, type))
        return true;
    #endif

    //Try to save using Qt
    const char* type_name = nullptr;
    switch (type)
    {
        case wxBITMAP_TYPE_BMP:  type_name = "bmp";  break;
        case wxBITMAP_TYPE_ICO:  type_name = "ico";  break;
        case wxBITMAP_TYPE_JPEG: type_name = "jpeg"; break;
        case wxBITMAP_TYPE_PNG:  type_name = "png";  break;
        case wxBITMAP_TYPE_GIF:  type_name = "gif";  break;
        case wxBITMAP_TYPE_CUR:  type_name = "cur";  break;
        case wxBITMAP_TYPE_TIFF: type_name = "tif";  break;
        case wxBITMAP_TYPE_XBM:  type_name = "xbm";  break;
        case wxBITMAP_TYPE_PCX:  type_name = "pcx";  break;
        case wxBITMAP_TYPE_BMP_RESOURCE:
        case wxBITMAP_TYPE_ICO_RESOURCE:
        case wxBITMAP_TYPE_CUR_RESOURCE:
        case wxBITMAP_TYPE_XBM_DATA:
        case wxBITMAP_TYPE_XPM:
        case wxBITMAP_TYPE_XPM_DATA:
        case wxBITMAP_TYPE_TIFF_RESOURCE:
        case wxBITMAP_TYPE_GIF_RESOURCE:
        case wxBITMAP_TYPE_PNG_RESOURCE:
        case wxBITMAP_TYPE_JPEG_RESOURCE:
        case wxBITMAP_TYPE_PNM:
        case wxBITMAP_TYPE_PNM_RESOURCE:
        case wxBITMAP_TYPE_PCX_RESOURCE:
        case wxBITMAP_TYPE_PICT:
        case wxBITMAP_TYPE_PICT_RESOURCE:
        case wxBITMAP_TYPE_ICON:
        case wxBITMAP_TYPE_ICON_RESOURCE:
        case wxBITMAP_TYPE_ANI:
        case wxBITMAP_TYPE_IFF:
        case wxBITMAP_TYPE_TGA:
        case wxBITMAP_TYPE_MACCURSOR:
        case wxBITMAP_TYPE_MACCURSOR_RESOURCE:
        case wxBITMAP_TYPE_MAX:
        case wxBITMAP_TYPE_ANY:
        default:
            break;
    }
    return type_name;
}

bool wxBitmap::LoadFile(const wxString &name, wxBitmapType type)
{

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

}

void wxBitmap::SetWidth(int width)
{

}

void wxBitmap::SetDepth(int depth)
{

}
#endif

wxGDIRefData *wxBitmap::CreateGDIRefData() const
{
    return new wxBitmapRefData;
}

wxGDIRefData *wxBitmap::CloneGDIRefData(const wxGDIRefData *data) const
{

}

bool wxBitmap::HasAlpha() const
{

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

}

wxMask::wxMask(const wxBitmap& bitmap, const wxColour& colour)
{

}

wxMask::wxMask(const wxBitmap& bitmap, int paletteIndex)
{

}

wxMask::wxMask(const wxBitmap& bitmap)
{

}

wxMask::~wxMask()
{

}


WXPixmap wxMask::GetBitmap() const
{

}

