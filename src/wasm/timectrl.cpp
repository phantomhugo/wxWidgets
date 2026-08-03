/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/timectrl.cpp
// Purpose:     wxTimePickerCtrl for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/timectrl.h"
#include "wx/dateevt.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxTimePickerCtrl, wxControl);

bool wxTimePickerCtrl::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxDateTime& date,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator,
                              const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var input = document.createElement('input');
        input.type = 'time';
        input.className = 'wxTimePickerCtrl';
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';

        input.addEventListener('change', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(input);
    }, GetId());

    SetValue(date.IsValid() ? date : wxDateTime::Now());
    return true;
}

void wxTimePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxString str = dt.FormatISOTime();
    wxCharBuffer buf = str.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxTimePickerCtrl');
        if (input) input.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

wxDateTime wxTimePickerCtrl::GetValue() const
{
    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxTimePickerCtrl');
        if (!input) return 0;
        var str = input.value;
        if (!str) return 0;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    wxDateTime dt;
    if (val)
    {
        dt.ParseISOTime(wxString::FromUTF8(val));
        free(val);
    }
    return dt;
}

void wxTimePickerCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxDateEvent evt(this, GetValue(), wxEVT_TIME_CHANGED);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
