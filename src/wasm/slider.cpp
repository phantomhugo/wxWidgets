/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/slider.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/slider.h"
#include <emscripten.h>

wxSlider::wxSlider()
{
}

wxSlider::wxSlider(wxWindow *parent,
         wxWindowID id,
         int value, int minValue, int maxValue,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxValidator& validator,
         const wxString& name)
{
    Create( parent, id, value, minValue, maxValue, pos, size, style, validator, name );
}

bool wxSlider::Create(wxWindow *parent,
            wxWindowID id,
            int value, int minValue, int maxValue,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_lineSize = 1;
    m_pageSize = wxMax(1, (maxValue - minValue) / 10);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var slider = document.createElement('input');
        slider.type = 'range';
        slider.className = 'wxSlider';
        slider.min = $1;
        slider.max = $2;
        slider.step = $3;
        slider.value = $4;

        slider.style.width = '100%';
        slider.style.height = '100%';

        slider.addEventListener('input', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'input', 0, 0]);
            }
        });

        slider.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(slider);
    }, GetId(), minValue, maxValue, m_lineSize, value);

    return true;
}

int wxSlider::GetValue() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var slider = container.querySelector('.wxSlider');
        return slider ? parseInt(slider.value, 10) : 0;
    }, GetId());
}

void wxSlider::SetValue(int value)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var slider = container.querySelector('.wxSlider');
        if (slider) slider.value = $1;
    }, GetId(), value);
}

void wxSlider::SetRange(int minValue, int maxValue)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var slider = container.querySelector('.wxSlider');
        if (slider) {
            slider.min = $1;
            slider.max = $2;
        }
    }, GetId(), minValue, maxValue);
}

int wxSlider::GetMin() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var slider = container.querySelector('.wxSlider');
        return slider ? parseInt(slider.min, 10) : 0;
    }, GetId());
}

int wxSlider::GetMax() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var slider = container.querySelector('.wxSlider');
        return slider ? parseInt(slider.max, 10) : 0;
    }, GetId());
}

void wxSlider::DoSetTickFreq(int WXUNUSED(freq))
{
}

int wxSlider::GetTickFreq() const
{
    return 0;
}

void wxSlider::SetLineSize(int lineSize)
{
    m_lineSize = lineSize;
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var slider = container.querySelector('.wxSlider');
        if (slider) slider.step = $1;
    }, GetId(), lineSize);
}

void wxSlider::SetPageSize(int pageSize)
{
    m_pageSize = pageSize;
}

int wxSlider::GetLineSize() const
{
    return m_lineSize;
}

int wxSlider::GetPageSize() const
{
    return m_pageSize;
}

void wxSlider::SetThumbLength(int WXUNUSED(lenPixels))
{
}

int wxSlider::GetThumbLength() const
{
    return 0;
}

WXWidget wxSlider::GetHandle() const
{
    return nullptr;
}

wxSize wxSlider::DoGetBestSize() const
{
    if (GetWindowStyle() & wxSL_VERTICAL)
        return wxSize(24, 100);
    return wxSize(100, 24);
}

void wxSlider::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId)
    {
        if (event.eventType == "input" || event.eventType == "change")
        {
            wxCommandEvent evt(wxEVT_SLIDER, m_windowId);
            evt.SetInt(GetValue());
            HandleWindowEvent(evt);
        }
    }
}
