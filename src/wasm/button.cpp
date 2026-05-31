/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/button.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/button.h"
#include "wx/stockitem.h"
#include <emscripten.h>

wxButton::wxButton()
{
}

wxButton::wxButton(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxButton::Create(wxWindow *parent, wxWindowID id,
       const wxString& label,
       const wxPoint& pos,
       const wxSize& size, long style,
       const wxValidator& validator,
       const wxString& name )
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    int domId = GetId();
    wxString btnLabel = label;
    if (btnLabel.empty() && wxIsStockID(id))
        btnLabel = wxGetStockLabel(id);

    wxCharBuffer labelBuffer = btnLabel.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var btn = document.createElement('button');
        btn.className = 'wxButton';
        btn.textContent = UTF8ToString($1);

        btn.onclick = function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'click', 0, 0]);
            }
        };

        container.appendChild(btn);
    }, domId, labelBuffer.data());

    return true;
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    // Quitar clase default del botón anterior
    if (oldDefault) {
        EM_ASM_({
            var container = document.getElementById($0);
            if (container) {
                var btn = container.querySelector('.wxButton');
                if (btn) btn.classList.remove('default');
            }
        }, oldDefault->GetId());
    }

    // Agregar clase default al botón actual
    EM_ASM_({
        var container = document.getElementById($0);
        if (container) {
            var btn = container.querySelector('.wxButton');
            if (btn) btn.classList.add('default');
        }
    }, GetId());

    return oldDefault;
}

/* static */
wxSize wxButtonBase::GetDefaultSize(wxWindow* WXUNUSED(win))
{
    static wxSize size = wxDefaultSize;
    if (size == wxDefaultSize)
    {
        // Approximate default button size in pixels
        size = wxSize(80, 28);
    }
    return size;
}

void wxButton::WasmNotifyEvent(const wxWasmEvent& event)
{
    if(event.id==m_windowId&&event.eventType=="click")
    {
        wxCommandEvent generatedEvent(wxEVT_BUTTON,event.id);
        HandleWindowEvent(generatedEvent);
    }
}
