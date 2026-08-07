/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/fontpicker.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/fontpicker.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxFontPickerWidget, wxControl);

wxFontPickerWidget::wxFontPickerWidget(wxWindow *parent,
                                       wxWindowID id,
                                       const wxFont& initial,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long style,
                                       const wxValidator& validator,
                                       const wxString& name)
{
    Create(parent, id, initial, pos, size, style, validator, name);
}

bool wxFontPickerWidget::Create(wxWindow *parent,
                                wxWindowID id,
                                const wxFont& initial,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxValidator& validator,
                                const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_selectedFont = initial.IsOk() ? initial : *wxNORMAL_FONT;
    m_selectedColour = *wxBLACK;

    EM_ASM_(({
        var container = document.getElementById($0);
        if (!container) return;

        var wrap = document.createElement('div');
        wrap.className = 'wxFontPickerWidget';
        wrap.style.width = '100%';
        wrap.style.height = '100%';
        wrap.style.boxSizing = 'border-box';
        wrap.style.display = 'flex';
        wrap.style.flexDirection = 'column';
        wrap.style.gap = '2px';
        wrap.style.overflow = 'auto';

        // Family select
        var selFamily = document.createElement('select');
        selFamily.className = 'wxFontPicker-family';
        var families = ['sans-serif', 'serif', 'monospace', 'cursive', 'fantasy'];
        families.forEach(function(f) {
            var opt = document.createElement('option');
            opt.value = f;
            opt.textContent = f;
            selFamily.appendChild(opt);
        });
        wrap.appendChild(selFamily);

        // Size input
        var inpSize = document.createElement('input');
        inpSize.type = 'number';
        inpSize.className = 'wxFontPicker-size';
        inpSize.value = '12';
        inpSize.min = '1';
        inpSize.max = '96';
        inpSize.style.width = '100%';
        wrap.appendChild(inpSize);

        // Style select
        var selStyle = document.createElement('select');
        selStyle.className = 'wxFontPicker-style';
        ['normal', 'italic'].forEach(function(s) {
            var opt = document.createElement('option');
            opt.value = s;
            opt.textContent = s;
            selStyle.appendChild(opt);
        });
        wrap.appendChild(selStyle);

        // Weight select
        var selWeight = document.createElement('select');
        selWeight.className = 'wxFontPicker-weight';
        [['normal','400'],['bold','700']].forEach(function(w) {
            var opt = document.createElement('option');
            opt.value = w[1];
            opt.textContent = w[0];
            selWeight.appendChild(opt);
        });
        wrap.appendChild(selWeight);

        function emitChange() {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        }

        selFamily.addEventListener('change', emitChange);
        inpSize.addEventListener('change', emitChange);
        selStyle.addEventListener('change', emitChange);
        selWeight.addEventListener('change', emitChange);

        container.appendChild(wrap);
    }), GetId());

    UpdateFont();
    return true;
}

wxColour wxFontPickerWidget::GetSelectedColour() const
{
    return m_selectedColour;
}

void wxFontPickerWidget::SetSelectedColour(const wxColour& colour)
{
    m_selectedColour = colour;
}

void wxFontPickerWidget::UpdateFont()
{
    wxString family = m_selectedFont.GetFaceName();
    if (family.empty()) family = "sans-serif";
    int size = m_selectedFont.GetPointSize();
    if (size <= 0) size = 12;
    wxString style = (m_selectedFont.GetStyle() == wxFONTSTYLE_ITALIC) ? "italic" : "normal";
    int weight = m_selectedFont.GetNumericWeight();
    if (weight < 500) weight = 400; else weight = 700;

    wxCharBuffer famBuf = family.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var wrap = container.querySelector('.wxFontPickerWidget');
        if (!wrap) return;
        var selFamily = wrap.querySelector('.wxFontPicker-family');
        var inpSize   = wrap.querySelector('.wxFontPicker-size');
        var selStyle  = wrap.querySelector('.wxFontPicker-style');
        var selWeight = wrap.querySelector('.wxFontPicker-weight');
        if (selFamily) selFamily.value = UTF8ToString($1);
        if (inpSize)   inpSize.value = $2;
        if (selStyle)  selStyle.value = UTF8ToString($3);
        if (selWeight) selWeight.value = $4.toString();
    }, GetId(), famBuf.data(), size, style.ToUTF8().data(), weight);
}

void wxFontPickerWidget::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        char* family = (char*)EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 0;
            var wrap = container.querySelector('.wxFontPickerWidget');
            if (!wrap) return 0;
            var sel = wrap.querySelector('.wxFontPicker-family');
            var str = sel ? sel.value : 'sans-serif';
            var len = lengthBytesUTF8(str) + 1;
            var buf = _malloc(len);
            stringToUTF8(str, buf, len);
            return buf;
        }, GetId());

        int size = EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 12;
            var wrap = container.querySelector('.wxFontPickerWidget');
            if (!wrap) return 12;
            var inp = wrap.querySelector('.wxFontPicker-size');
            return inp ? parseInt(inp.value) || 12 : 12;
        }, GetId());

        char* styleStr = (char*)EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 0;
            var wrap = container.querySelector('.wxFontPickerWidget');
            if (!wrap) return 0;
            var sel = wrap.querySelector('.wxFontPicker-style');
            var str = sel ? sel.value : 'normal';
            var len = lengthBytesUTF8(str) + 1;
            var buf = _malloc(len);
            stringToUTF8(str, buf, len);
            return buf;
        }, GetId());

        int weight = EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 400;
            var wrap = container.querySelector('.wxFontPickerWidget');
            if (!wrap) return 400;
            var sel = wrap.querySelector('.wxFontPicker-weight');
            return sel ? parseInt(sel.value) || 400 : 400;
        }, GetId());

        wxFontStyle fstyle = wxFONTSTYLE_NORMAL;
        if (styleStr)
        {
            if (wxString::FromUTF8(styleStr) == "italic")
                fstyle = wxFONTSTYLE_ITALIC;
            free(styleStr);
        }

        wxFontFamily ffam = wxFONTFAMILY_SWISS;
        if (family)
        {
            wxString f = wxString::FromUTF8(family);
            if (f == "serif") ffam = wxFONTFAMILY_ROMAN;
            else if (f == "monospace") ffam = wxFONTFAMILY_MODERN;
            else if (f == "cursive") ffam = wxFONTFAMILY_SCRIPT;
            else if (f == "fantasy") ffam = wxFONTFAMILY_DECORATIVE;
            free(family);
        }

        m_selectedFont = wxFont(size, ffam, fstyle, static_cast<wxFontWeight>(weight));
        m_selectedFont.SetFaceName(wxString::FromUTF8(family ? family : "sans-serif"));
        m_data.SetChosenFont(m_selectedFont);

        wxFontPickerEvent evt(this, GetId(), m_selectedFont);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
