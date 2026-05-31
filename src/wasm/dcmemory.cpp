/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcmemory.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/dcmemory.h"
#include <emscripten.h>

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner)
    : wxWasmDCImpl(owner)
{
    m_canvasId = GenerateCanvasId();
    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = 1;
        canvas.height = 1;
        canvas.style.display = 'none';
        document.body.appendChild(canvas);
    }, m_canvasId.c_str());
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner, wxBitmap& bitmap)
    : wxWasmDCImpl(owner)
{
    DoSelect(bitmap);
}

wxMemoryDCImpl::wxMemoryDCImpl(wxMemoryDC *owner, wxDC *WXUNUSED(dc))
    : wxWasmDCImpl(owner)
{
    m_canvasId = GenerateCanvasId();
    EM_ASM_({
        var canvas = document.createElement('canvas');
        canvas.id = UTF8ToString($0);
        canvas.width = 1;
        canvas.height = 1;
        canvas.style.display = 'none';
        document.body.appendChild(canvas);
    }, m_canvasId.c_str());
}

wxMemoryDCImpl::~wxMemoryDCImpl()
{
    // canvas cleanup handled by ~wxWasmDCImpl
}

wxBitmap wxMemoryDCImpl::DoGetAsBitmap(const wxRect *WXUNUSED(subrect)) const
{
    return m_selected;
}

void wxMemoryDCImpl::DoSelect(const wxBitmap& bitmap)
{
    m_selected = bitmap;
    if (m_canvasId.empty())
    {
        m_canvasId = GenerateCanvasId();
    }
    if (bitmap.IsOk())
    {
        m_size = bitmap.GetSize();
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas)
            {
                canvas = document.createElement('canvas');
                canvas.id = UTF8ToString($0);
                canvas.style.display = 'none';
                document.body.appendChild(canvas);
            }
            canvas.width = $1;
            canvas.height = $2;
        }, m_canvasId.c_str(), m_size.x, m_size.y);

        // If bitmap has raw pixel data, copy it to the canvas
        wxImage image = bitmap.ConvertToImage();
        if (image.IsOk())
        {
            int w = image.GetWidth();
            int h = image.GetHeight();
            unsigned char* rgb = image.GetData();
            unsigned char* alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;
            EM_ASM_({
                var canvas = document.getElementById(UTF8ToString($0));
                if (!canvas) return;
                var ctx = canvas.getContext('2d');
                var w = $1;
                var h = $2;
                var imgData = ctx.createImageData(w, h);
                var rgb = $3;
                var alpha = $4;
                for (var i = 0; i < w * h; i++) {
                    imgData.data[i * 4 + 0] = HEAPU8[rgb + i * 3 + 0];
                    imgData.data[i * 4 + 1] = HEAPU8[rgb + i * 3 + 1];
                    imgData.data[i * 4 + 2] = HEAPU8[rgb + i * 3 + 2];
                    imgData.data[i * 4 + 3] = alpha ? HEAPU8[alpha + i] : 255;
                }
                ctx.putImageData(imgData, 0, 0);
            }, m_canvasId.c_str(), w, h, rgb, alpha);
        }
    }
    else
    {
        m_size = wxSize(0, 0);
    }
}

const wxBitmap& wxMemoryDCImpl::GetSelectedBitmap() const
{
    return m_selected;
}

wxBitmap& wxMemoryDCImpl::GetSelectedBitmap()
{
    return m_selected;
}
