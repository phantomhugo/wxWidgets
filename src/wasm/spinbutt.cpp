/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/spinbutt.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/spinbutt.h"

#include <emscripten.h>

wxSpinButton::wxSpinButton()
{
}

wxSpinButton::wxSpinButton(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxSpinButton::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    // The buttons need a sensible default size if none was given.
    wxSize newSize(size);
    if ( newSize.x == wxDefaultCoord )
        newSize.x = 18;
    if ( newSize.y == wxDefaultCoord )
        newSize.y = (style & wxSP_VERTICAL) ? 24 : 18;

    if ( !wxControl::Create(parent, id, pos, newSize, style,
                            wxDefaultValidator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var vertical = ($1 & 0x0008) !== 0; // wxSP_VERTICAL == wxVERTICAL

        container.classList.add('wxSpinButton');
        container.classList.add(vertical ? 'wxSpinButton-vertical'
                                         : 'wxSpinButton-horizontal');
        container.dataset.value = '0';

        function makeButton(cls, text, eventType) {
            var btn = document.createElement('button');
            btn.type = 'button';
            btn.className = cls;
            btn.textContent = text;
            btn.addEventListener('click', function(e) {
                e.stopPropagation();
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, eventType, 0, 0]);
                }
            });
            return btn;
        }

        container.appendChild(makeButton('wxSpinButton-up',
            vertical ? '▲' : '◀', 'spinup'));
        container.appendChild(makeButton('wxSpinButton-down',
            vertical ? '▼' : '▶', 'spindown'));
    }, GetId(), style);

    return true;
}

void wxSpinButton::SetRange(int min, int max)
{
    wxSpinButtonBase::SetRange(min, max); // cache the values

    // Make sure the current value still lies inside the new range.
    SetValue(GetValue());
}

int wxSpinButton::GetValue() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var v = parseInt(container.dataset.value, 10);
        return isNaN(v) ? 0 : v;
    }, GetId());
}

void wxSpinButton::SetValue(int val)
{
    // Clamp to the valid range.
    if ( val < m_min )
        val = m_min;
    if ( val > m_max )
        val = m_max;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        container.dataset.value = String($1);
    }, GetId(), val);
}

void wxSpinButton::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id != m_windowId )
        return;

    const bool up = event.eventType == "spinup";
    if ( up || event.eventType == "spindown" )
    {
        const int value = GetValue() + (up ? 1 : -1);
        SetValue(value);

        wxSpinEvent evt(up ? wxEVT_SPIN_UP : wxEVT_SPIN_DOWN, m_windowId);
        evt.SetPosition(GetValue());
        HandleWindowEvent(evt);

        wxSpinEvent evtChanged(wxEVT_SPIN, m_windowId);
        evtChanged.SetPosition(GetValue());
        HandleWindowEvent(evtChanged);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

void *wxSpinButton::GetHandle() const
{
    return nullptr;
}
