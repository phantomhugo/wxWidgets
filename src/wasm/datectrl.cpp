/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/datectrl.cpp
// Purpose:     wxDatePickerCtrl for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/datectrl.h"
#include "wx/dateevt.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl);

bool wxDatePickerCtrl::Create(wxWindow *parent,
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
        input.type = 'date';
        input.className = 'wxDatePickerCtrl';
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

    SetValue(date.IsValid() ? date : wxDateTime::Today());
    return true;
}

void wxDatePickerCtrl::SetValue(const wxDateTime& dt)
{
    wxString str = dt.FormatISODate();
    wxCharBuffer buf = str.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxDatePickerCtrl');
        if (input) input.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

wxDateTime wxDatePickerCtrl::GetValue() const
{
    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxDatePickerCtrl');
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
        dt.ParseISODate(wxString::FromUTF8(val));
        free(val);
    }
    return dt;
}

void wxDatePickerCtrl::SetRange(const wxDateTime& dt1, const wxDateTime& dt2)
{
    wxString minStr = dt1.IsValid() ? dt1.FormatISODate() : wxString();
    wxString maxStr = dt2.IsValid() ? dt2.FormatISODate() : wxString();
    wxCharBuffer minBuf = minStr.ToUTF8();
    wxCharBuffer maxBuf = maxStr.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxDatePickerCtrl');
        if (!input) return;
        var min = UTF8ToString($1);
        var max = UTF8ToString($2);
        if (min) input.min = min; else input.removeAttribute('min');
        if (max) input.max = max; else input.removeAttribute('max');
    }, GetId(), minBuf.data(), maxBuf.data());
}

bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    char* minVal = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxDatePickerCtrl');
        if (!input || !input.min) return 0;
        var str = input.min;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    char* maxVal = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxDatePickerCtrl');
        if (!input || !input.max) return 0;
        var str = input.max;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    if (dt1 && minVal)
    {
        dt1->ParseISODate(wxString::FromUTF8(minVal));
        free(minVal);
    }
    if (dt2 && maxVal)
    {
        dt2->ParseISODate(wxString::FromUTF8(maxVal));
        free(maxVal);
    }
    return (minVal != nullptr) || (maxVal != nullptr);
}

void wxDatePickerCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxDateEvent evt(this, GetValue(), wxEVT_DATE_CHANGED);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
