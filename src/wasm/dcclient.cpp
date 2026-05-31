/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcclient.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/wasm/dcclient.h"
#include <emscripten.h>

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl(wxDC *owner)
    : wxWasmDCImpl(owner),
      m_window(nullptr)
{
}

wxWindowDCImpl::wxWindowDCImpl(wxDC *owner, wxWindow *win)
    : wxWasmDCImpl(owner),
      m_window(win)
{
    if (m_window)
    {
        wxSize size = m_window->GetClientSize();
        m_size = size;
        m_canvasId = GenerateCanvasId();
        int winId = m_window->GetId();
        EM_ASM_({
            var parent = document.getElementById($0);
            if (!parent) parent = document.body;
            var canvas = document.createElement('canvas');
            canvas.id = UTF8ToString($1);
            canvas.width = $2;
            canvas.height = $3;
            canvas.style.position = 'absolute';
            canvas.style.left = '0px';
            canvas.style.top = '0px';
            canvas.style.width = '100%';
            canvas.style.height = '100%';
            canvas.style.pointerEvents = 'none';
            parent.appendChild(canvas);
        }, winId, m_canvasId.c_str(), size.x, size.y);
    }
}

//##############################################################################

wxClientDCImpl::wxClientDCImpl(wxDC *owner)
    : wxWindowDCImpl(owner)
{
}

wxClientDCImpl::wxClientDCImpl(wxDC *owner, wxWindow *win)
    : wxWindowDCImpl(owner, win)
{
}

//##############################################################################

wxPaintDCImpl::wxPaintDCImpl(wxDC *owner)
    : wxWindowDCImpl(owner)
{
}

wxPaintDCImpl::wxPaintDCImpl(wxDC *owner, wxWindow *win)
    : wxWindowDCImpl(owner, win)
{
}
