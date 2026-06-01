/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/radiobut.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobut.h"
#include <emscripten.h>

wxRadioButton::wxRadioButton()
{
}

wxRadioButton::wxRadioButton( wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name)
{
    Create( parent, id, label, pos, size, style, validator, name );
}

bool wxRadioButton::Create( wxWindow *parent,
             wxWindowID id,
             const wxString& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxValidator& validator,
             const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    int domId = GetId();
    wxCharBuffer labelBuffer = label.ToUTF8();
    int groupId = parent ? parent->GetId() : 0;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var label = document.createElement('label');
        label.className = 'wxRadioButton';

        var input = document.createElement('input');
        input.type = 'radio';
        input.name = 'wxRadioGroup_' + $2;

        var span = document.createElement('span');
        span.textContent = UTF8ToString($1);

        label.appendChild(input);
        label.appendChild(span);

        input.onchange = function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        };

        container.appendChild(label);
    }, domId, labelBuffer.data(), groupId);

    return true;
}

void wxRadioButton::SetValue(bool value)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var radio = container.querySelector('.wxRadioButton input[type="radio"]');
        if (radio) radio.checked = $1;
    }, GetId(), static_cast<int>(value));
}

bool wxRadioButton::GetValue() const
{
    int checked = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var radio = container.querySelector('.wxRadioButton input[type="radio"]');
        return radio ? radio.checked : 0;
    }, GetId());

    return checked != 0;
}

WXWidget wxRadioButton::GetHandle() const
{
    return NULL;
}

void wxRadioButton::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "change" )
    {
        wxCommandEvent evt(wxEVT_RADIOBUTTON, m_windowId);
        HandleWindowEvent(evt);
    }
}
