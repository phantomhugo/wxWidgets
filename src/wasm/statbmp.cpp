/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statbmp.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/statbmp.h"
#include "wx/bitmap.h"
#include "wx/icon.h"

#if wxUSE_IMAGE
    #include "wx/image.h"
#endif

#include <emscripten.h>

// Renders raw RGB(A) pixels into the <img> found under the given window
// container, using a canvas to produce a PNG data URL. Declared here and
// reused by other controls that need to show a wxBitmap (e.g. wxBitmapButton
// and wxBitmapComboBox, which declare the prototype locally).
void wxWasmSetImgFromPixels(int domId, const char* selector,
                            unsigned char* rgb, unsigned char* alpha,
                            int w, int h)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var img = container.querySelector(UTF8ToString($1));
        if (!img) return;

        var canvas = document.createElement('canvas');
        canvas.width = $4;
        canvas.height = $5;
        var ctx = canvas.getContext('2d');
        var imgData = ctx.createImageData($4, $5);
        var rgb = $2;
        var alpha = $3;
        var w = $4;
        var h = $5;
        for (var i = 0; i < w * h; i++) {
            imgData.data[i * 4 + 0] = HEAPU8[rgb + i * 3 + 0];
            imgData.data[i * 4 + 1] = HEAPU8[rgb + i * 3 + 1];
            imgData.data[i * 4 + 2] = HEAPU8[rgb + i * 3 + 2];
            imgData.data[i * 4 + 3] = alpha ? HEAPU8[alpha + i] : 255;
        }
        ctx.putImageData(imgData, 0, 0);
        img.src = canvas.toDataURL('image/png');
    }, domId, selector, rgb, alpha, w, h);
}

wxStaticBitmap::wxStaticBitmap()
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBitmap::Create( wxWindow *parent,
             wxWindowID id,
             const wxBitmapBundle& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var img = document.createElement('img');
        img.className = 'wxStaticBitmap';
        img.style.width = '100%';
        img.style.height = '100%';
        img.style.objectFit = 'contain';
        container.appendChild(img);
    }, GetId());

    if ( label.IsOk() )
        SetBitmap( label );

    return true;
}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{
    SetBitmap( icon );
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    m_bitmapBundle = bitmap;

#if wxUSE_IMAGE
    if ( bitmap.IsOk() )
    {
        wxImage image = bitmap.GetBitmapFor(this).ConvertToImage();
        if ( image.IsOk() )
        {
            wxWasmSetImgFromPixels(GetId(), ".wxStaticBitmap",
                image.GetData(),
                image.HasAlpha() ? image.GetAlpha() : nullptr,
                image.GetWidth(), image.GetHeight());
        }
    }
#endif // wxUSE_IMAGE

    InvalidateBestSize();
}


wxBitmap wxStaticBitmap::GetBitmap() const
{
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetBitmap(m_bitmapBundle.GetDefaultSize());

    return wxBitmap();
}

wxIcon wxStaticBitmap::GetIcon() const
{
    wxIcon icon;
    if ( m_bitmapBundle.IsOk() )
        icon.CopyFromBitmap(GetBitmap());

    return icon;
}

wxSize wxStaticBitmap::DoGetBestSize() const
{
    if ( m_bitmapBundle.IsOk() )
        return m_bitmapBundle.GetDefaultSize();

    return wxSize(0, 0);
}

WXWidget wxStaticBitmap::GetHandle() const
{
    return nullptr;
}
