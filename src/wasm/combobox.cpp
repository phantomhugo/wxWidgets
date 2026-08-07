/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/combobox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/combobox.h"
#include "wx/window.h"

#include <emscripten.h>

void wxComboBox::SetSelection( int n )
{
    if ( IsReadOnly() )
    {
        wxChoice::SetSelection( n );
    }
    else if ( n >= 0 && (unsigned int)n < GetCount() )
    {
        SetActualValue( GetString( n ) );
    }
}

wxComboBox::wxComboBox()
{
}

wxComboBox::wxComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, n, choices, style, validator, name );
}


wxComboBox::wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name )
{
    Create( parent, id, value, pos, size, choices, style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    const wxString *pChoices = choices.size() ? &choices[ 0 ] : nullptr;
    return Create(parent, id, value, pos, size, choices.size(), pChoices,
                  style, validator, name );
}


bool wxComboBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name )
{
    if ( style & wxCB_READONLY )
    {
        // A read-only combobox is exactly a wxChoice.
        if ( !wxChoice::Create(parent, id, pos, size, n, choices, style,
                               validator, name) )
            return false;

        if ( !value.empty() )
            SetStringSelection( value );

        return true;
    }

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    wxCharBuffer valueBuf = value.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var listId = 'wxComboBoxList_' + $0;

        var input = document.createElement('input');
        input.type = 'text';
        input.className = 'wxComboBox';
        input.setAttribute('list', listId);
        input.value = UTF8ToString($1);
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';
        input.style.fontFamily = 'inherit';
        input.style.fontSize = 'inherit';

        var datalist = document.createElement('datalist');
        datalist.id = listId;
        datalist.className = 'wxComboBox-list';

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

        input.addEventListener('keydown', function(e) {
            if (e.key === 'Enter') {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'enter', 0, 0]);
                }
            }
        });

        container.appendChild(input);
        container.appendChild(datalist);
    }, GetId(), valueBuf.data());

    for (int i = 0; i < n; ++i)
    {
        DoInsertOneItem(choices[i], GetCount());
    }

    return true;
}

void wxComboBox::SetActualValue(const wxString &value)
{
    if ( IsReadOnly() )
    {
        SetStringSelection( value );
    }
    else
    {
        wxTextEntry::SetValue(value);

    }
}

bool wxComboBox::IsReadOnly() const
{
    return HasFlag( wxCB_READONLY );
}

void wxComboBox::SetValue(const wxString& value)
{
    SetActualValue( value );

    if ( !IsReadOnly() )
        SetInsertionPoint( 0 );
}

void wxComboBox::ChangeValue(const wxString &value)
{
    SetValue( value );
}

void wxComboBox::AppendText(const wxString &value)
{
    SetActualValue( GetValue() + value );
}

void wxComboBox::Replace(long from, long to, const wxString &value)
{
    const wxString original( GetValue() );

    if ( to < 0 )
    {
        to = original.length();
    }

    if ( from == 0 )
    {
        SetActualValue( value + original.substr(to, original.length()) );
    }

    wxString front = original.substr( 0, from ) + value;

    long iPoint = front.length();
    if ( front.length() <= original.length() )
    {
        SetActualValue( front + original.substr(to, original.length()) );
    }
    else
    {
        SetActualValue( front );
    }
    SetInsertionPoint( iPoint );
}

void wxComboBox::WriteText(const wxString &value)
{
    if ( IsReadOnly() )
        return;

    wxCharBuffer buf = value.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox');
        if (elem) {
            var start = elem.selectionStart;
            var end = elem.selectionEnd;
            var text = UTF8ToString($1);
            var val = elem.value;
            elem.value = val.substring(0, start) + text + val.substring(end);
            elem.selectionStart = elem.selectionEnd = start + text.length;
        }
    }, GetId(), buf.data());
}

wxString wxComboBox::DoGetValue() const
{
    if ( IsReadOnly() )
        return wxItemContainer::GetStringSelection();

    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxComboBox');
        if (!elem) return 0;
        var str = elem.value;
        var len = lengthBytesUTF8(str) + 1;
        var buf = _malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    if (!val)
        return wxString();

    wxString result = wxString::FromUTF8(val);
    free(val);
    return result;
}

void wxComboBox::DoSetValue(const wxString& value, int WXUNUSED(flags))
{
    if ( IsReadOnly() )
    {
        SetStringSelection( value );
        return;
    }

    wxCharBuffer buf = value.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox');
        if (elem) elem.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxComboBox::Popup()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox, .wxChoice');
        if (!elem) return;
        elem.focus();
        if (elem.showPicker) {
            try { elem.showPicker(); } catch (e) { /* requires user gesture */ }
        }
    }, GetId());
}

void wxComboBox::Dismiss()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox, .wxChoice');
        if (elem) elem.blur();
    }, GetId());
}

void wxComboBox::Clear()
{
    if ( !IsReadOnly() )
        wxTextEntry::Clear();

    wxItemContainer::Clear();
}

void wxComboBox::SetSelection( long from, long to )
{
    if ( IsReadOnly() )
        return;

    if ( from == -1 )
    {
        from = 0;
    }
    if ( to == -1 )
    {
        to = GetValue().length();
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox');
        if (elem) {
            elem.selectionStart = $1;
            elem.selectionEnd = $2;
        }
    }, GetId(), (int)from, (int)to);
}

void wxComboBox::SetInsertionPoint( long pos )
{
    if ( IsReadOnly() )
        return;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox');
        if (elem) elem.selectionStart = elem.selectionEnd = $1;
    }, GetId(), (int)pos);
}

long wxComboBox::GetInsertionPoint() const
{
    if ( IsReadOnly() )
        return 0;

    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxComboBox');
        return elem ? elem.selectionStart : 0;
    }, GetId());
}

void wxComboBox::GetSelection(long* from, long* to) const
{
    if ( IsReadOnly() )
    {
        wxTextEntry::GetSelection(from, to);
        return;
    }

    if (from) *from = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxComboBox');
        return elem ? elem.selectionStart : 0;
    }, GetId());

    if (to) *to = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxComboBox');
        return elem ? elem.selectionEnd : 0;
    }, GetId());
}

void wxComboBox::SetEditable(bool editable)
{
    if ( IsReadOnly() )
        return;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxComboBox');
        if (elem) elem.readOnly = $1 === 0;
    }, GetId(), editable ? 1 : 0);
}

void wxComboBox::SetString(unsigned int n, const wxString& s)
{
    wxChoice::SetString(n, s);

    if ( !IsReadOnly() )
    {
        wxCharBuffer buf = s.ToUTF8();
        EM_ASM_({
            var datalist = document.getElementById('wxComboBoxList_' + $0);
            if (datalist && $1 < datalist.options.length) {
                datalist.options[$1].value = UTF8ToString($2);
            }
        }, GetId(), (int)n, buf.data());
    }
}

int wxComboBox::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    const int idx = wxChoice::DoInsertOneItem(item, pos);

    // Mirror the item into the <datalist> of the editable combobox.
    if ( !IsReadOnly() && idx != wxNOT_FOUND )
    {
        wxCharBuffer buf = item.ToUTF8();
        EM_ASM_({
            var datalist = document.getElementById('wxComboBoxList_' + $0);
            if (!datalist) return;

            var option = document.createElement('option');
            option.value = UTF8ToString($1);

            if ($2 >= datalist.options.length) {
                datalist.appendChild(option);
            } else {
                datalist.insertBefore(option, datalist.options[$2]);
            }
        }, GetId(), buf.data(), (int)pos);
    }

    return idx;
}

void wxComboBox::DoClear()
{
    wxChoice::DoClear();

    if ( !IsReadOnly() )
    {
        EM_ASM_({
            var datalist = document.getElementById('wxComboBoxList_' + $0);
            if (datalist) {
                while (datalist.options.length > 0) {
                    datalist.remove(0);
                }
            }
        }, GetId());
    }
}

void wxComboBox::DoDeleteOneItem(unsigned int pos)
{
    wxChoice::DoDeleteOneItem(pos);

    if ( !IsReadOnly() )
    {
        EM_ASM_({
            var datalist = document.getElementById('wxComboBoxList_' + $0);
            if (datalist && $1 < datalist.options.length) {
                datalist.remove($1);
            }
        }, GetId(), (int)pos);
    }
}

void wxComboBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id != m_windowId )
        return;

    if ( event.eventType == "change" )
    {
        if ( IsReadOnly() )
        {
            wxCommandEvent evt(wxEVT_COMBOBOX, m_windowId);
            evt.SetInt(GetSelection());
            evt.SetString(GetStringSelection());
            HandleWindowEvent(evt);
        }
        else
        {
            const wxString value = GetValue();
            const int sel = FindString(value);

            if ( sel != wxNOT_FOUND )
            {
                wxCommandEvent evt(wxEVT_COMBOBOX, m_windowId);
                evt.SetInt(sel);
                evt.SetString(value);
                HandleWindowEvent(evt);
            }

            wxCommandEvent evtText(wxEVT_TEXT, m_windowId);
            evtText.SetString(value);
            HandleWindowEvent(evtText);
        }
    }
    else if ( event.eventType == "input" )
    {
        wxCommandEvent evt(wxEVT_TEXT, m_windowId);
        evt.SetString(GetValue());
        HandleWindowEvent(evt);
    }
    else if ( event.eventType == "enter" )
    {
        wxCommandEvent evt(wxEVT_TEXT_ENTER, m_windowId);
        evt.SetString(GetValue());
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
