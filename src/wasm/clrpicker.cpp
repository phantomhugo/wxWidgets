/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/clrpicker.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/clrpicker.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxColourPickerWidget, wxControl);

wxColourPickerWidget::wxColourPickerWidget()
{
}

wxColourPickerWidget::wxColourPickerWidget(wxWindow *parent,
               wxWindowID id,
               const wxColour& initial,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxValidator& validator,
               const wxString& name )
{
    Create( parent, id, initial, pos, size, style, validator, name );
}

bool wxColourPickerWidget::Create(wxWindow *parent,
            wxWindowID id,
            const wxColour& initial,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_colour = initial;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var input = document.createElement('input');
        input.type = 'color';
        input.className = 'wxColourPickerWidget';
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';
        input.style.border = 'none';
        input.style.padding = '0';
        input.style.background = 'none';

        input.addEventListener('input', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'input', 0, 0]);
            }
        });

        input.addEventListener('change', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(input);
    }, GetId());

    UpdateColour();
    return true;
}

void wxColourPickerWidget::UpdateColour()
{
    wxString colStr = m_colour.GetAsString(wxC2S_HTML_SYNTAX);
    wxCharBuffer buf = colStr.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxColourPickerWidget');
        if (input) input.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxColourPickerWidget::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId)
    {
        if (event.eventType == "input" || event.eventType == "change")
        {
            char* val = (char*)EM_ASM_INT({
                var container = document.getElementById($0);
                if (!container) return 0;
                var input = container.querySelector('.wxColourPickerWidget');
                if (!input) return 0;
                var str = input.value;
                var len = lengthBytesUTF8(str) + 1;
                var buf = Module._malloc(len);
                stringToUTF8(str, buf, len);
                return buf;
            }, GetId());

            if (val)
            {
                m_colour.Set(val);
                free(val);
            }

            wxColourPickerEvent evt(this, GetId(), m_colour);
            HandleWindowEvent(evt);
        }
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
