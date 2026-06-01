/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/tglbtn.cpp
// Purpose:     wxToggleButton implementation for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/tglbtn.h"
#include <emscripten.h>

wxDEFINE_EVENT( wxEVT_TOGGLEBUTTON, wxCommandEvent );

// ============================================================================
// wxToggleButton
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxToggleButton, wxControl);

wxToggleButton::wxToggleButton()
{
}

wxToggleButton::wxToggleButton(wxWindow *parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxPoint& pos,
                               const wxSize& size, long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

bool wxToggleButton::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxString& label,
                            const wxPoint& pos,
                            const wxSize& size, long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    wxCharBuffer labelBuf = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var btn = document.createElement('button');
        btn.className = 'wxToggleButton';
        btn.type = 'button';
        btn.textContent = UTF8ToString($1);
        btn.style.width = '100%';
        btn.style.height = '100%';
        btn.style.boxSizing = 'border-box';

        btn.addEventListener('click', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'click', 0, 0]);
            }
        });

        container.appendChild(btn);
    }, GetId(), labelBuf.data());

    return true;
}

void wxToggleButton::SetValue(bool state)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var btn = container.querySelector('.wxToggleButton');
        if (btn) {
            btn.classList.toggle('pressed', $1);
            btn.setAttribute('aria-pressed', $1 ? 'true' : 'false');
        }
    }, GetId(), state ? 1 : 0);
}

bool wxToggleButton::GetValue() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var btn = container.querySelector('.wxToggleButton');
        return btn ? (btn.classList.contains('pressed') ? 1 : 0) : 0;
    }, GetId()) != 0;
}

void wxToggleButton::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "click")
    {
        SetValue(!GetValue());
        wxCommandEvent evt(wxEVT_TOGGLEBUTTON, m_windowId);
        evt.SetInt(GetValue() ? 1 : 0);
        HandleWindowEvent(evt);
    }
}

// ============================================================================
// wxBitmapToggleButton
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapToggleButton, wxToggleButton);

wxBitmapToggleButton::wxBitmapToggleButton()
{
}

wxBitmapToggleButton::wxBitmapToggleButton(wxWindow *parent,
                                           wxWindowID id,
                                           const wxBitmapBundle& label,
                                           const wxPoint& pos,
                                           const wxSize& size, long style,
                                           const wxValidator& validator,
                                           const wxString& name)
{
    Create(parent, id, label, pos, size, style, validator, name);
}

bool wxBitmapToggleButton::Create(wxWindow *parent,
                                  wxWindowID id,
                                  const wxBitmapBundle& WXUNUSED(label),
                                  const wxPoint& pos,
                                  const wxSize& size, long style,
                                  const wxValidator& validator,
                                  const wxString& name)
{
    return wxToggleButton::Create(parent, id, wxString(), pos, size, style, validator, name);
}
