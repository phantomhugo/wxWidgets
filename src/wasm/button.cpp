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

    // Store the label (and invalidate the best size computed with an empty
    // label during wxControl::Create) and show it without accel markers.
    SetLabel(btnLabel);

    return true;
}

wxWindow *wxButton::SetDefault()
{
    wxWindow *oldDefault = wxButtonBase::SetDefault();

    // Remove default class from the previous button
    if (oldDefault) {
        EM_ASM_({
            var container = document.getElementById($0);
            if (container) {
                var btn = container.querySelector('.wxButton');
                if (btn) btn.classList.remove('default');
            }
        }, oldDefault->GetId());
    }

    // Add default class to the current button
    EM_ASM_({
        var container = document.getElementById($0);
        if (container) {
            var btn = container.querySelector('.wxButton');
            if (btn) btn.classList.add('default');
        }
    }, GetId());

    return oldDefault;
}

wxSize wxButton::DoGetBestSize() const
{
    // Base size of a GTK3 button, grown to fit the measured label with
    // horizontal/vertical padding similar to the GTK3 theme.
    wxSize best = GetDefaultSize(const_cast<wxButton*>(this));

    wxString label = GetLabel();
    if ( label.empty() && wxIsStockID(GetId()) )
        label = wxGetStockLabel(GetId());

    if ( !label.empty() )
    {
        int textW = 0, textH = 0;
        GetTextExtent(wxControlBase::GetLabelText(label), &textW, &textH);
        best.x = wxMax(best.x, textW + 40);
        best.y = wxMax(best.y, textH + 14);
    }

    return best;
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
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
