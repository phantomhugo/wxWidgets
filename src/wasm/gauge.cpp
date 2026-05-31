/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/gauge.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gauge.h"
#include <emscripten.h>

wxGauge::wxGauge()
{
}

wxGauge::wxGauge(wxWindow *parent,
        wxWindowID id,
        int range,
        const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, range, pos, size, style, validator, name );
}

bool wxGauge::Create(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos,
            const wxSize& size, long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    // Reemplazar el <div> creado por wxControl/wxWindow con <progress>
    int domId = GetId();
    bool isVertical = HasFlag(wxGA_VERTICAL);

    EM_ASM_({
        var oldDiv = document.getElementById($0);
        if (oldDiv && oldDiv.tagName !== 'PROGRESS') {
            var progress = document.createElement('progress');
            progress.id = $0;
            progress.className = oldDiv.className + ' wxGauge';
            if ($1) {
                progress.classList.add('wxGauge-vertical');
            } else {
                progress.classList.add('wxGauge-horizontal');
            }
            progress.max = $2;
            progress.value = 0;

            // Transferir hijos existentes (por si acaso)
            while (oldDiv.firstChild) {
                progress.appendChild(oldDiv.firstChild);
            }
            if (oldDiv.parentNode) {
                oldDiv.parentNode.replaceChild(progress, oldDiv);
            }
        }
    }, domId, isVertical ? 1 : 0, range);

    // Inicializar estado base
    SetRange(range);
    SetValue(0);

    return true;
}

void *wxGauge::GetHandle() const
{
    return nullptr;
}

void wxGauge::SetRange(int range)
{
    wxGaugeBase::SetRange(range);

    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem && elem.tagName === 'PROGRESS') {
            elem.max = $1;
        }
    }, GetId(), range);
}

int wxGauge::GetRange() const
{
    return wxGaugeBase::GetRange();
}

void wxGauge::SetValue(int pos)
{
    wxGaugeBase::SetValue(pos);

    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem && elem.tagName === 'PROGRESS') {
            elem.value = $1;
            // Asegurar que el modo indeterminado se desactive si se setea un valor
            elem.removeAttribute('aria-valuenow');
        }
    }, GetId(), pos);
}

int wxGauge::GetValue() const
{
    return wxGaugeBase::GetValue();
}
