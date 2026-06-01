/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/anybutton.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef wxHAS_ANY_BUTTON

#ifndef WX_PRECOMP
    #include "wx/anybutton.h"
#endif

#include "wx/bitmap.h"
#include "wx/mstream.h"
#include "wx/base64.h"
#include "wx/image.h"
#include <emscripten.h>

static wxString wxBitmapToDataURL(const wxBitmap& bmp)
{
    if (!bmp.IsOk()) return wxString();
    wxImage img = bmp.ConvertToImage();
    if (!img.IsOk()) return wxString();
    wxMemoryOutputStream memStream;
    if (!img.SaveFile(memStream, wxBITMAP_TYPE_PNG))
        return wxString();
    size_t size = memStream.GetSize();
    if (size == 0) return wxString();
    wxCharBuffer buf(size);
    memStream.CopyTo(buf.data(), size);
    return wxString::Format("data:image/png;base64,%s", wxBase64Encode(buf.data(), size));
}

wxAnyButton::wxAnyButton()
{
}

void wxAnyButton::SetLabel( const wxString &label )
{
}

wxString wxAnyButton::GetLabel() const
{
}

void *wxAnyButton::GetHandle() const
{

}

wxBitmap wxAnyButton::DoGetBitmap(State state) const
{
    return state < State_Max ? m_bitmaps[state].GetBitmap(wxDefaultSize) : wxNullBitmap;
}

void wxAnyButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxCHECK_RET(which < State_Max, "Invalid state");

    // Cache the bitmap.
    m_bitmaps[which] = bitmap;

    if (which == State_Normal)
    {
        wxBitmap bmp = bitmap.GetBitmap(wxDefaultSize);
        wxString dataUrl = wxBitmapToDataURL(bmp);
        if (!dataUrl.IsEmpty())
        {
            wxCharBuffer urlBuffer = dataUrl.ToUTF8();
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var btn = container.querySelector('.wxButton');
                if (!btn) return;
                var img = btn.querySelector('img');
                if (!img) {
                    img = document.createElement('img');
                    img.style.maxWidth = '100%';
                    img.style.maxHeight = '100%';
                    btn.appendChild(img);
                }
                img.src = UTF8ToString($1);
            }, GetId(), urlBuffer.data());
        }
    }
}

#endif // wxHAS_ANY_BUTTON
