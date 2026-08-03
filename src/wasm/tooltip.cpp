/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/tooltip.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/tooltip.h"

#include <emscripten.h>

// NOTE: the tooltips are implemented with the native "title" attribute of
// the DOM element, so their timing and appearance are controlled by the
// browser and the global functions below have no effect.

/* static */ void wxToolTip::Enable(bool WXUNUSED(flag))
{
}

/* static */ void wxToolTip::SetDelay(long WXUNUSED(milliseconds))
{
}

/* static */ void wxToolTip::SetAutoPop(long WXUNUSED(milliseconds))
{
}

/* static */ void wxToolTip::SetReshow(long WXUNUSED(milliseconds))
{
}



wxToolTip::wxToolTip(const wxString &tip)
{
    m_window = nullptr;
    SetTip(tip);
}

void wxToolTip::SetTip(const wxString& tip)
{
    m_text = tip;

    if ( m_window )
    {
        wxCharBuffer buf = m_text.ToUTF8();
        EM_ASM_({
            var elem = document.getElementById($0);
            if (!elem) return;

            var tip = UTF8ToString($1);
            if (tip.length > 0)
                elem.title = tip;
            else
                elem.removeAttribute('title');
        }, m_window->GetId(), buf.data());
    }
}

const wxString &wxToolTip::GetTip() const
{
    return m_text;
}


void wxToolTip::SetWindow(wxWindow *win)
{
    m_window = win;
}
