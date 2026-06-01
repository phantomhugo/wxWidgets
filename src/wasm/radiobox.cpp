/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/radiobox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/radiobox.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl);

wxRadioBox::wxRadioBox()
{
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
}

wxRadioBox::wxRadioBox(wxWindow *parent,
           wxWindowID id,
           const wxString& title,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           int majorDim,
           long style,
           const wxValidator& val,
           const wxString& name)
{
    Create( parent, id, title, pos, size, choices, majorDim, style, val, name );
}

bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            int majorDim,
            long style,
            const wxValidator& val,
            const wxString& name)
{
    return Create( parent, id, title, pos, size, choices.size(), &choices[ 0 ],
        majorDim, style, val, name );
}

bool wxRadioBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            int WXUNUSED(majorDim),
            long style,
            const wxValidator& val,
            const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, val, name))
        return false;

    wxCharBuffer titleBuf = title.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var fieldset = document.createElement('fieldset');
        fieldset.className = 'wxRadioBox';
        fieldset.style.width = '100%';
        fieldset.style.height = '100%';
        fieldset.style.boxSizing = 'border-box';

        var legend = document.createElement('legend');
        legend.textContent = UTF8ToString($1);
        fieldset.appendChild(legend);

        var groupName = 'wxRadioBox_' + $0;

        for (var i = 0; i < $2; i++) {
            var label = document.createElement('label');
            label.style.display = 'block';
            label.style.cursor = 'pointer';

            var radio = document.createElement('input');
            radio.type = 'radio';
            radio.name = groupName;
            radio.value = i;
            if (i === 0) radio.checked = true;

            radio.addEventListener('change', function(e) {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'change', 0, 0]);
                }
            });

            label.appendChild(radio);
            label.appendChild(document.createTextNode(' '));
            var span = document.createElement('span');
            span.className = 'wxRadioBox-label-' + i;
            label.appendChild(span);

            fieldset.appendChild(label);
        }

        container.appendChild(fieldset);
    }, GetId(), titleBuf.data(), n);

    for (int i = 0; i < n; i++)
    {
        SetString(i, choices[i]);
    }

    return true;
}

bool wxRadioBox::Enable(unsigned int n, bool enable)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return;
        var radios = fieldset.querySelectorAll('input[type="radio"]');
        if (radios[$1]) radios[$1].disabled = !$2;
    }, GetId(), (int)n, enable ? 1 : 0);
    return true;
}

bool wxRadioBox::Enable( bool enable )
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var fieldset = container.querySelector('.wxRadioBox');
        if (fieldset) fieldset.disabled = !$1;
    }, GetId(), enable ? 1 : 0);
    return wxControl::Enable(enable);
}

bool wxRadioBox::Show(unsigned int WXUNUSED(n), bool WXUNUSED(show))
{
    return true;
}

bool wxRadioBox::Show( bool show )
{
    return wxControl::Show(show);
}

bool wxRadioBox::IsItemEnabled(unsigned int n) const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 1;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return 1;
        var radios = fieldset.querySelectorAll('input[type="radio"]');
        return radios[$1] ? !radios[$1].disabled : 1;
    }, GetId(), (int)n) != 0;
}

bool wxRadioBox::IsItemShown(unsigned int WXUNUSED(n)) const
{
    return true;
}

unsigned int wxRadioBox::GetCount() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return 0;
        return fieldset.querySelectorAll('input[type="radio"]').length;
    }, GetId());
}

wxString wxRadioBox::GetString(unsigned int n) const
{
    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return 0;
        var spans = fieldset.querySelectorAll('.wxRadioBox-label-' + $1);
        if (!spans.length) return 0;
        var str = spans[0].textContent;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId(), (int)n);

    if (!val) return wxString();
    wxString result = wxString::FromUTF8(val);
    free(val);
    return result;
}

void wxRadioBox::SetString(unsigned int n, const wxString& s)
{
    wxCharBuffer buf = s.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return;
        var spans = fieldset.querySelectorAll('.wxRadioBox-label-' + $1);
        if (spans.length) spans[0].textContent = UTF8ToString($2);
    }, GetId(), (int)n, buf.data());
}

void wxRadioBox::SetSelection(int n)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return;
        var radios = fieldset.querySelectorAll('input[type="radio"]');
        if (radios[$1]) radios[$1].checked = true;
    }, GetId(), n);
}

int wxRadioBox::GetSelection() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var fieldset = container.querySelector('.wxRadioBox');
        if (!fieldset) return 0;
        var radios = fieldset.querySelectorAll('input[type="radio"]');
        for (var i = 0; i < radios.length; i++) {
            if (radios[i].checked) return i;
        }
        return 0;
    }, GetId());
}

void *wxRadioBox::GetHandle() const
{
    return nullptr;
}

void wxRadioBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxCommandEvent evt(wxEVT_RADIOBOX, m_windowId);
        evt.SetInt(GetSelection());
        HandleWindowEvent(evt);
    }
}
