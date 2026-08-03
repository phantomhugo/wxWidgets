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
        int winId = m_window->GetId();

        // The canvas is shared by all the DCs created for this window (and
        // persists after each DC is destroyed, so what was drawn with a
        // local wxPaintDC/wxClientDC stays visible): use a deterministic id
        // derived from the window id instead of a global counter.
        m_canvasId = "wx_canvas_" + std::to_string(winId);
        m_canvasOwnedByDC = false;
        EM_ASM_({
            var parent = document.getElementById($0);
            if (!parent) parent = document.body;
            var canvas = document.getElementById(UTF8ToString($1));
            if (!canvas)
            {
                canvas = document.createElement('canvas');
                canvas.id = UTF8ToString($1);
                canvas.style.position = 'absolute';
                canvas.style.left = '0px';
                canvas.style.top = '0px';
                canvas.style.width = '100%';
                canvas.style.height = '100%';
                canvas.style.pointerEvents = 'none';
                parent.appendChild(canvas);
            }
            else if (canvas.parentNode !== parent)
            {
                // The window was reparented: move its canvas along.
                parent.appendChild(canvas);
            }
            // Only touch the buffer size when it actually differs: assigning
            // width/height resets the context state and clears the canvas.
            if (canvas.width !== $2) canvas.width = $2;
            if (canvas.height !== $3) canvas.height = $3;
        }, winId, m_canvasId.c_str(), size.x, size.y);
    }
}

//##############################################################################

void wxWindowDCImpl::SyncCanvasBuffer()
{
    if ( m_canvasId.empty() || !m_window )
        return;

    const wxSize size = m_window->GetClientSize();
    if ( size.x <= 0 || size.y <= 0 || size == m_size )
        return;

    // The window was resized since the buffer was last set: resize it (this
    // also clears it; the wxPaintEvent scheduled by wxWindowWasm::DoSetSize
    // repaints the contents).
    m_size = size;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        canvas.width = $1;
        canvas.height = $2;
    }, m_canvasId.c_str(), size.x, size.y);
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
    if (m_canvasId.empty())
        return;

    // This port uses a full-repaint model: OnPaint redraws the whole window
    // contents (there is no erase-background phase), so start each paint
    // from a clean canvas and drop any clipping state left over by a
    // previous DC on this shared per-window canvas.
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        if (canvas._wxClipSaved)
        {
            ctx.restore();
            canvas._wxClipSaved = false;
        }
        canvas._wxClip = null;
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    }, m_canvasId.c_str());
}
