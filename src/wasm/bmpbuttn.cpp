/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/bmpbuttn.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bmpbuttn.h"
#include "wx/bitmap.h"

#if wxUSE_IMAGE
    #include "wx/image.h"
#endif

#include <emscripten.h>

// defined in src/wasm/statbmp.cpp
void wxWasmSetImgFromPixels(int domId, const char* selector,
                            unsigned char* rgb, unsigned char* alpha,
                            int w, int h);

wxBitmapButton::wxBitmapButton()
{
}


wxBitmapButton::wxBitmapButton(wxWindow *parent,
               wxWindowID id,
               const wxBitmapBundle& bitmap,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name )
{
    Create( parent, id, bitmap, pos, size, style, validator, name );
}


bool wxBitmapButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxBitmapBundle& bitmap,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    if ( !wxBitmapButtonBase::Create( parent, id, pos, size, style, validator, name ))
        return false;
    // Show the initial bitmap and resize accordingly:
    if ( bitmap.IsOk() )
    {
        wxBitmapButtonBase::SetBitmapLabel(bitmap);

        // we need to adjust the size after setting the bitmap as it may be too
        // big for the default button size
        SetInitialSize(size);
    }
    return true;
}

void wxBitmapButton::DoSetBitmap(const wxBitmapBundle& bitmap, State which)
{
    wxBitmapButtonBase::DoSetBitmap(bitmap, which);

    if ( which != State_Normal || !bitmap.IsOk() )
        return;

#if wxUSE_IMAGE
    wxImage image = bitmap.GetBitmapFor(this).ConvertToImage();
    if ( !image.IsOk() )
        return;

    // Make sure there is an <img> inside the <button> to show the bitmap.
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var btn = container.querySelector('.wxButton');
        if (!btn) return;

        btn.classList.add('wxBitmapButton');
        if (!btn.querySelector('img')) {
            var img = document.createElement('img');
            btn.textContent = "";
            btn.appendChild(img);
        }
    }, GetId());

    wxWasmSetImgFromPixels(GetId(), ".wxButton img",
        image.GetData(),
        image.HasAlpha() ? image.GetAlpha() : nullptr,
        image.GetWidth(), image.GetHeight());
#endif // wxUSE_IMAGE
}

void wxBitmapButton::WasmNotifyEvent(const wxWasmEvent& event)
{
    wxButton::WasmNotifyEvent(event);
}
