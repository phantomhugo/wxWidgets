/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/bitmap.h
// Purpose:     wxBitmap class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_
#define _WX_BITMAP_H_

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/palette.h"
#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxMask;
class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxImage;

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMask: public wxObject
{
public:
    wxMask();
    wxMask(const wxMask& mask);
    wxMask( const wxBitmap& bitmap, const wxColour& colour );
    wxMask( const wxBitmap& bitmap, int paletteIndex );
    wxMask( const wxBitmap& bitmap );
    virtual ~wxMask();

    bool Create( const wxBitmap& bitmap, const wxColour& colour );
    bool Create( const wxBitmap& bitmap, int paletteIndex );
    bool Create( const wxBitmap& bitmap );

    // implementation
    WXPixmap GetBitmap() const              { return m_bitmap; }
    void SetBitmap( WXPixmap bitmap )       { m_bitmap = bitmap; }

private:
    WXPixmap    m_bitmap;
    wxSize m_size;

private:
    wxDECLARE_DYNAMIC_CLASS(wxMask);
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
public:
    wxBitmap() {}
    wxBitmap( int width, int height, int depth = -1 ) { Create( width, height, depth ); }
    wxBitmap( const wxSize& sz, int depth = -1 ) { Create( sz, depth ); }

    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( const char* const* bits );
    wxBitmap( const wxString &filename, wxBitmapType type = wxBITMAP_DEFAULT_TYPE );
    virtual ~wxBitmap();

    static void InitStandardHandlers();

    bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH)
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }
    bool Create(int width, int height, const wxDC& WXUNUSED(dc))
        { return Create(width,height); }

    bool Create(const void* data, wxBitmapType type,
                int width, int height, int depth = -1);
    // create the wxBitmap using a _copy_ of the pixmap
    bool Create(WXPixmap pixmap);

    int GetHeight() const;
    int GetWidth() const;
    int GetDepth() const;

#if wxUSE_IMAGE
    wxBitmap( const wxImage& image, int depth = -1, double WXUNUSED(scale) = 1.0 ) { (void)CreateFromImage(image, depth); }
    wxImage ConvertToImage() const;
    bool CreateFromImage(const wxImage& image, int depth = -1);
#endif // wxUSE_IMAGE

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);

    wxMask *GetMask() const;
    void SetMask( wxMask *mask );

    wxBitmap GetSubBitmap( const wxRect& rect ) const;

    bool SaveFile( const wxString &name, wxBitmapType type, const wxPalette *palette = NULL ) const;
    bool LoadFile( const wxString &name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE );

    wxPalette *GetPalette() const;
    wxPalette *GetColourMap() const
        { return GetPalette(); }
    virtual void SetPalette(const wxPalette& palette);

    // implementation
    // --------------

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED(void SetHeight( int height ));
    wxDEPRECATED(void SetWidth( int width ));
    wxDEPRECATED(void SetDepth( int depth ));
#endif
    void SetPixmap( WXPixmap pixmap );
    void SetBitmap( WXPixmap bitmap );

    WXPixmap GetPixmap() const;
    WXPixmap GetBitmap() const;

    WXPixmap GetDrawable() const;

    // This is provided only for compatibility with the other ports, there is
    // no alpha support in X11 bitmaps.
    bool HasAlpha() const { return false; }

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmap);
};

#endif // _WX_BITMAP_H_
