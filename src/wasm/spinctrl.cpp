/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/spinctrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"
#include <emscripten.h>

wxSpinCtrl::wxSpinCtrl()
{
    Init();
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    Init();
    Create( parent, id, value, pos, size, style, min, max, initial, name );
}

bool wxSpinCtrl::Create( wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    int initVal = initial;
    if (!value.empty())
    {
        long lval;
        if (value.ToLong(&lval))
            initVal = static_cast<int>(lval);
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var spin = document.createElement('input');
        spin.type = 'number';
        spin.className = 'wxSpinCtrl';
        spin.min = $1;
        spin.max = $2;
        spin.step = $3;
        spin.value = $4;

        spin.style.width = '100%';
        spin.style.height = '100%';
        spin.style.boxSizing = 'border-box';

        spin.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(spin);
    }, GetId(), min, max, 1, initVal);

    return true;
}

bool wxSpinCtrl::GetSnapToTicks() const
{
    return false;
}

wxString wxSpinCtrl::GetTextValue() const
{
    char* valStr = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var spin = container.querySelector('.wxSpinCtrl');
        if (!spin) return 0;
        var str = spin.value;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    if (!valStr)
        return wxString();

    wxString result = wxString::FromUTF8(valStr);
    free(valStr);
    return result;
}

int wxSpinCtrl::GetValue() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseInt(spin.value, 10) : 0;
    }, GetId());
}

int wxSpinCtrl::GetMin() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseInt(spin.min, 10) : 0;
    }, GetId());
}

int wxSpinCtrl::GetMax() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseInt(spin.max, 10) : 0;
    }, GetId());
}

int wxSpinCtrl::GetIncrement() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 1;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseInt(spin.step, 10) : 1;
    }, GetId());
}

void wxSpinCtrl::SetIncrement(int inc)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.step = $1;
    }, GetId(), inc);
}

void wxSpinCtrl::SetRange(int min, int max)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) {
            spin.min = $1;
            spin.max = $2;
        }
    }, GetId(), min, max);
}

void wxSpinCtrl::SetSnapToTicks(bool WXUNUSED(snap_to_ticks))
{
}

bool wxSpinCtrl::SetBase(int base)
{
    if (base != 10 && base != 16)
        return false;
    m_base = base;
    return true;
}

void wxSpinCtrl::SetSelection(long from, long to)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) {
            spin.selectionStart = $1;
            spin.selectionEnd = $2;
        }
    }, GetId(), (int)from, (int)to);
}

void wxSpinCtrl::SetValue( const wxString &value )
{
    wxCharBuffer buf = value.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxSpinCtrl::SetValue(int val)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.value = $1;
    }, GetId(), val);
}

WXWidget wxSpinCtrl::GetHandle() const
{
    return nullptr;
}

wxSize wxSpinCtrl::DoGetBestSize() const
{
    return wxSize(60, 24);
}

void wxSpinCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxSpinEvent evt(wxEVT_SPINCTRL, m_windowId);
        evt.SetValue(GetValue());
        HandleWindowEvent(evt);
    }
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxSpinCtrlDouble, wxSpinCtrl);

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{
    Create( parent, id, value, pos, size, style, min, max, initial, inc, name );
}

bool wxSpinCtrlDouble::Create(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{
    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    double initVal = initial;
    if (!value.empty())
    {
        value.ToDouble(&initVal);
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var spin = document.createElement('input');
        spin.type = 'number';
        spin.className = 'wxSpinCtrl';
        spin.min = $1;
        spin.max = $2;
        spin.step = $3;
        spin.value = $4;

        spin.style.width = '100%';
        spin.style.height = '100%';
        spin.style.boxSizing = 'border-box';

        spin.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(spin);
    }, GetId(), min, max, inc, initVal);

    return true;
}

void wxSpinCtrlDouble::SetDigits(unsigned WXUNUSED(digits))
{
}

unsigned wxSpinCtrlDouble::GetDigits() const
{
    return 0;
}

void wxSpinCtrlDouble::SetValue( const wxString &value )
{
    wxCharBuffer buf = value.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.value = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxSpinCtrlDouble::SetValue(double val)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.value = $1;
    }, GetId(), val);
}

double wxSpinCtrlDouble::GetValue() const
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 0.0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseFloat(spin.value) : 0.0;
    }, GetId());
}

double wxSpinCtrlDouble::GetMin() const
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 0.0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseFloat(spin.min) : 0.0;
    }, GetId());
}

double wxSpinCtrlDouble::GetMax() const
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 0.0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseFloat(spin.max) : 0.0;
    }, GetId());
}

double wxSpinCtrlDouble::GetIncrement() const
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 1.0;
        var spin = container.querySelector('.wxSpinCtrl');
        return spin ? parseFloat(spin.step) : 1.0;
    }, GetId());
}

void wxSpinCtrlDouble::SetIncrement(double inc)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) spin.step = $1;
    }, GetId(), inc);
}

void wxSpinCtrlDouble::SetRange(double min, double max)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var spin = container.querySelector('.wxSpinCtrl');
        if (spin) {
            spin.min = $1;
            spin.max = $2;
        }
    }, GetId(), min, max);
}

void wxSpinCtrlDouble::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxSpinDoubleEvent evt(wxEVT_SPINCTRLDOUBLE, m_windowId, GetValue());
        HandleWindowEvent(evt);
    }
}

#endif // wxUSE_SPINCTRL
