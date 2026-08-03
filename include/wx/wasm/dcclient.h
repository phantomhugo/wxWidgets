/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dc.h
// Purpose:     wxWindowDcImpl class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DCCLIENT_H_
#define _WX_WASM_DCCLIENT_H_

#include "wx/wasm/dc.h"

#include "wx/scopedptr.h"


class WXDLLIMPEXP_CORE wxWindowDCImpl : public wxWasmDCImpl
{
public:
    wxWindowDCImpl( wxDC *owner );
    wxWindowDCImpl( wxDC *owner, wxWindow *win );

    // NB: the canvas of a window DC is shared by all the DCs of that window
    // (id "wx_canvas_<windowId>") and is NOT removed when the DC is
    // destroyed, so what was drawn on it stays visible; it dies with the
    // window DOM element.
    ~wxWindowDCImpl() = default;

protected:
    wxWindow *m_window;

    // Re-synchronize the canvas pixel buffer with the current client size
    // of the window: the CSS stretches the canvas to 100% of its container,
    // so without this the buffer is scaled (blurry) after a resize.
    virtual void SyncCanvasBuffer() override;

private:
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};


class WXDLLIMPEXP_CORE wxClientDCImpl : public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *win );

    ~wxClientDCImpl() = default;
    static bool CanBeUsedForDrawing(const wxWindow* WXUNUSED(window)) { return true; }
private:
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};


class WXDLLIMPEXP_CORE wxPaintDCImpl : public wxWindowDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );
private:
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};

#endif // _WX_WASM_DCCLIENT_H_
