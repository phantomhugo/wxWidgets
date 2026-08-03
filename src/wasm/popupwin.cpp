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
    }, GetId());

    return true;
}

