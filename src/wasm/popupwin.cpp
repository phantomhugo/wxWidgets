/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/popupwin.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/popupwin.h"
#include "wx/window.h"

#include <emscripten.h>

// NB: wxIMPLEMENT_DYNAMIC_CLASS(wxPopupWindow) lives in
// src/common/popupcmn.cpp, as for the other ports.

// Dismissal of transient popups: the native ports get it from the platform
// (pointer grab/activation loss); here a capture-phase DOM listener watches
// for pointer presses outside the popup and calls DismissAndNotify().
// (The C++ linkage helper is the friend; the extern "C" wrapper is what
// the DOM listener calls via ccall.)
void wxWasmPopupOutsideClickCpp(int domId)
{
    extern wxWindowWasm* wxWasmFindWindowByDomId(int domId);
    wxWindow* win = wxWasmFindWindowByDomId(domId);
    if ( !win || !win->IsShown() )
        return;

    // NB: the friendship is declared on wxPopupTransientWindow (the derived
    // class), so the cast must be to it for the protected DismissAndNotify()
    // to be accessible.
    if ( auto* trans = wxDynamicCast(win, wxPopupTransientWindow) )
        trans->DismissAndNotify();
}

extern "C" EMSCRIPTEN_KEEPALIVE void wxWasmPopupOutsideClick(int domId)
{
    wxWasmPopupOutsideClickCpp(domId);
}

wxPopupWindow::wxPopupWindow()
{
}

wxPopupWindow::wxPopupWindow(wxWindow *parent, int flags)
{
    Create(parent, flags);
}

bool wxPopupWindow::Create(wxWindow *parent, int flags)
{
    if ( !wxPopupWindowBase::Create(parent, flags) )
        return false;

    if ( !wxWindow::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           flags, "wxPopupWindow") )
        return false;

    // Popups float above the rest of the UI, attached to the document body.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        elem.classList.add('wxPopupWindow');
        document.body.appendChild(elem);
        elem.style.position = 'fixed';
        elem.style.zIndex = '1500';

        // Watch for pointer presses outside the popup: transient popups
        // dismiss themselves then (see wxWasmPopupOutsideClick). The check
        // happens in the capture phase so it runs even when the press is
        // swallowed elsewhere; the guard on display skips hidden popups.
        document.addEventListener('pointerdown', function(e) {
            var el = document.getElementById($0);
            if (!el || el.style.display === 'none') return;
            if (el.contains(e.target)) return;
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('wxWasmPopupOutsideClick', null, ['number'], [$0]);
            }
        }, true);
    }, GetDomWindowId());

    return true;
}

