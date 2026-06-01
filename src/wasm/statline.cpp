/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statline.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/statline.h"
#include <emscripten.h>

wxStaticLine::wxStaticLine()
{
}

wxStaticLine::wxStaticLine(wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString &name)
{
    Create(parent, id, pos, size, style, name);
}

bool wxStaticLine::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString &name)
{
    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    bool isHorizontal = (style & wxLI_VERTICAL) == 0;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var line = document.createElement('div');
        line.className = 'wxStaticLine';
        line.style.width = '100%';
        line.style.height = '100%';
        line.style.boxSizing = 'border-box';

        if ($1) {
            line.style.borderTop = '1px solid #bfb8b1';
            line.style.minHeight = '2px';
        } else {
            line.style.borderLeft = '1px solid #bfb8b1';
            line.style.minWidth = '2px';
        }

        container.appendChild(line);
    }, GetId(), isHorizontal ? 1 : 0);

    return true;
}
