/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/scrolbar.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/scrolbar.h"
#include <emscripten.h>

wxScrollBar::wxScrollBar()
{
    m_pageSize = 0;
}

wxScrollBar::wxScrollBar( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    Create( parent, id, pos, size, style, validator, name );
}

bool wxScrollBar::Create( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    m_pageSize = 0;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var scrollbar = document.createElement('input');
        scrollbar.type = 'range';
        scrollbar.className = 'wxScrollBar';
        scrollbar.min = 0;
        scrollbar.max = 0;
        scrollbar.step = 1;
        scrollbar.value = 0;

        scrollbar.style.width = '100%';
        scrollbar.style.height = '100%';
        scrollbar.style.margin = '0';
        scrollbar.style.padding = '0';

        if ($1) {
            scrollbar.style.appearance = 'slider-vertical';
            scrollbar.style.webkitAppearance = 'slider-vertical';
        }

        scrollbar.addEventListener('input', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'input', 0, 0]);
            }
        });

        scrollbar.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(scrollbar);
    }, GetId(), IsVertical() ? 1 : 0);

    return true;
}

int wxScrollBar::GetThumbPosition() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var scrollbar = container.querySelector('.wxScrollBar');
        return scrollbar ? parseInt(scrollbar.value, 10) : 0;
    }, GetId());
}

int wxScrollBar::GetThumbSize() const
{
    return 1;
}

int wxScrollBar::GetPageSize() const
{
    return m_pageSize;
}

int wxScrollBar::GetRange() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var scrollbar = container.querySelector('.wxScrollBar');
        return scrollbar ? parseInt(scrollbar.max, 10) : 0;
    }, GetId());
}

void wxScrollBar::SetThumbPosition(int viewStart)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var scrollbar = container.querySelector('.wxScrollBar');
        if (scrollbar) scrollbar.value = $1;
    }, GetId(), viewStart);
}

void wxScrollBar::SetScrollbar(int position, int WXUNUSED(thumbSize),
                          int range, int pageSize,
                          bool WXUNUSED(refresh))
{
    m_pageSize = pageSize;

    int max = wxMax(0, range - 1);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var scrollbar = container.querySelector('.wxScrollBar');
        if (scrollbar) {
            scrollbar.min = 0;
            scrollbar.max = $1;
            scrollbar.step = 1;
            scrollbar.value = $2;
        }
    }, GetId(), max, position);
}

WXWidget wxScrollBar::GetHandle() const
{
    return nullptr;
}

wxSize wxScrollBar::DoGetBestSize() const
{
    if (GetWindowStyle() & wxVERTICAL)
        return wxSize(16, 100);
    return wxSize(100, 16);
}

void wxScrollBar::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId)
    {
        if (event.eventType == "input")
        {
            wxScrollEvent evt(wxEVT_SCROLL_THUMBTRACK, m_windowId,
                              GetThumbPosition(),
                              IsVertical() ? wxVERTICAL : wxHORIZONTAL);
            evt.SetEventObject(this);
            HandleWindowEvent(evt);
        }
        else if (event.eventType == "change")
        {
            wxScrollEvent evt(wxEVT_SCROLL_THUMBRELEASE, m_windowId,
                              GetThumbPosition(),
                              IsVertical() ? wxVERTICAL : wxHORIZONTAL);
            evt.SetEventObject(this);
            HandleWindowEvent(evt);
        }
    }
}
