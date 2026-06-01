/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/textctrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textctrl.h"
#include "wx/settings.h"
#include <emscripten.h>

wxTextCtrl::wxTextCtrl()
{
}

wxTextCtrl::wxTextCtrl(wxWindow *parent,
           wxWindowID id,
           const wxString &value,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxValidator& validator,
           const wxString &name)
{
    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &value,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxValidator& validator,
            const wxString &name)
{
    if (!wxTextCtrlBase::Create(parent, id, pos, size, style, validator, name))
        return false;

    wxCharBuffer valueBuf = value.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var style = $1;
        var elem;
        if (style & 0x0020) { // wxTE_MULTILINE
            elem = document.createElement('textarea');
            elem.style.resize = 'none';
        } else if (style & 0x0800) { // wxTE_PASSWORD
            elem = document.createElement('input');
            elem.type = 'password';
        } else {
            elem = document.createElement('input');
            elem.type = 'text';
        }

        elem.className = 'wxTextCtrl';
        elem.value = UTF8ToString($2);
        elem.style.width = '100%';
        elem.style.height = '100%';
        elem.style.boxSizing = 'border-box';
        elem.style.fontFamily = 'inherit';
        elem.style.fontSize = 'inherit';

        if (style & 0x0004) { // wxTE_READONLY
            elem.readOnly = true;
        }

        elem.addEventListener('input', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'input', 0, 0]);
            }
        });

        elem.addEventListener('change', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        elem.addEventListener('keydown', function(e) {
            if (e.key === 'Enter') {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'enter', 0, 0]);
                }
            }
        });

        container.appendChild(elem);
    }, GetId(), style, valueBuf.data());

    return true;
}

wxTextCtrl::~wxTextCtrl()
{
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return wxSize(100, (GetWindowStyle() & wxTE_MULTILINE) ? 80 : 24);
}

int wxTextCtrl::GetLineLength(long WXUNUSED(lineNo)) const
{
    return GetValue().length();
}

wxString wxTextCtrl::GetLineText(long WXUNUSED(lineNo)) const
{
    return GetValue();
}

int wxTextCtrl::GetNumberOfLines() const
{
    return 1;
}

bool wxTextCtrl::IsModified() const
{
    return true; // Simplified
}

void wxTextCtrl::MarkDirty()
{
}

void wxTextCtrl::DiscardEdits()
{
}

bool wxTextCtrl::SetStyle(long WXUNUSED(start), long WXUNUSED(end), const wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::GetStyle(long WXUNUSED(position), wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& WXUNUSED(style))
{
    return false;
}

long wxTextCtrl::XYToPosition(long WXUNUSED(x), long WXUNUSED(y)) const
{
    return GetInsertionPoint();
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( x == nullptr || y == nullptr || pos < 0 )
        return false;
    *x = pos;
    *y = 0;
    return true;
}

void wxTextCtrl::ShowPosition(long WXUNUSED(pos))
{
}

bool wxTextCtrl::DoLoadFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

bool wxTextCtrl::DoSaveFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxTextCtrl');
        if (elem) elem.selectionStart = elem.selectionEnd = $1;
    }, GetId(), (int)pos);
}

long wxTextCtrl::GetInsertionPoint() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxTextCtrl');
        return elem ? elem.selectionStart : 0;
    }, GetId());
}

wxString wxTextCtrl::DoGetValue() const
{
    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxTextCtrl');
        if (!elem) return 0;
        var str = elem.value;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    if (!val)
        return wxString();

    wxString result = wxString::FromUTF8(val);
    free(val);
    return result;
}

void wxTextCtrl::SetSelection( long from, long to )
{
    if ( to == -1 )
        to = GetValue().length();
    if ( from == -1 )
        from = 0;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxTextCtrl');
        if (elem) {
            elem.selectionStart = $1;
            elem.selectionEnd = $2;
        }
    }, GetId(), (int)from, (int)to);
}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    if (from) *from = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxTextCtrl');
        return elem ? elem.selectionStart : 0;
    }, GetId());

    if (to) *to = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var elem = container.querySelector('.wxTextCtrl');
        return elem ? elem.selectionEnd : 0;
    }, GetId());
}

void wxTextCtrl::WriteText( const wxString &text )
{
    wxCharBuffer buf = text.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxTextCtrl');
        if (elem) {
            var start = elem.selectionStart;
            var end = elem.selectionEnd;
            var val = elem.value;
            elem.value = val.substring(0, start) + UTF8ToString($1) + val.substring(end);
            elem.selectionStart = elem.selectionEnd = start + UTF8ToString($1).length;
        }
    }, GetId(), buf.data());
}

void wxTextCtrl::DoSetValue( const wxString &text, int WXUNUSED(flags) )
{
    wxCharBuffer buf = text.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxTextCtrl');
        if (elem) elem.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

WXWidget wxTextCtrl::GetHandle() const
{
    return nullptr;
}

void wxTextCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId)
    {
        if (event.eventType == "input")
        {
            wxCommandEvent evt(wxEVT_TEXT, m_windowId);
            evt.SetString(GetValue());
            HandleWindowEvent(evt);
        }
        else if (event.eventType == "change")
        {
            wxCommandEvent evt(wxEVT_TEXT_ENTER, m_windowId);
            evt.SetString(GetValue());
            HandleWindowEvent(evt);
        }
        else if (event.eventType == "enter")
        {
            if (m_windowStyle & wxTE_PROCESS_ENTER)
            {
                wxCommandEvent evt(wxEVT_TEXT_ENTER, m_windowId);
                evt.SetString(GetValue());
                HandleWindowEvent(evt);
            }
        }
    }
}
