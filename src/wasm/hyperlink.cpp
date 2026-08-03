/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/hyperlink.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_HYPERLINKCTRL

#include "wx/hyperlink.h"

#include <emscripten.h>

// NOTE: wxIMPLEMENT_DYNAMIC_CLASS(wxHyperlinkCtrl) lives in
// src/common/hyperlnkcmn.cpp, as for the other ports.

void wxHyperlinkCtrl::Init()
{
    // Same colours as the .wxHyperlinkCtrl CSS rules (GTK3 theme).
    m_normalColour = wxColour(0x2a, 0x76, 0xc6);
    m_hoverColour = m_normalColour;
    m_visitedColour = wxColour(0x91, 0x41, 0xac);

    m_visited = false;
}

bool wxHyperlinkCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& label,
            const wxString& url,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    CheckParams(label, url, style);

    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    m_url = url;

    wxCharBuffer labelBuf = (label.empty() ? url : label).ToUTF8();
    wxCharBuffer urlBuf = url.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var link = document.createElement('a');
        link.className = 'wxHyperlinkCtrl';
        link.href = UTF8ToString($2);
        link.textContent = UTF8ToString($1);
        link.target = '_blank';
        link.rel = 'noopener';

        // wxHL_ALIGN_LEFT == 0x0002, wxHL_ALIGN_RIGHT == 0x0004
        var style = $3;
        container.style.textAlign = (style & 0x0002) ? 'left'
                                  : (style & 0x0004) ? 'right' : 'center';

        link.addEventListener('mouseenter', function() {
            link.style.color = link.dataset.hover;
        });
        link.addEventListener('mouseleave', function() {
            link.style.color = link.classList.contains('visited')
                ? link.dataset.visited : link.dataset.normal;
        });

        link.addEventListener('click', function(e) {
            // Do not preventDefault(): the browser opens the link itself.
            e.stopPropagation();
            link.classList.add('visited');
            link.style.color = link.dataset.visited;
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'click', 0, 0]);
            }
        });

        container.appendChild(link);
    }, GetId(), labelBuf.data(), urlBuf.data(), style);

    SetLabel(label);
    UpdateLinkColour();

    return true;
}

void wxHyperlinkCtrl::SetLabel(const wxString& label)
{
    const wxString text = label.empty() ? m_url : label;

    // Bypass wxControl::SetLabel() accelerator handling: the text may be
    // an URL containing '&' characters.
    wxWindow::SetLabel(text);

    wxCharBuffer buf = text.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var link = container.querySelector('.wxHyperlinkCtrl');
        if (link) link.textContent = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxHyperlinkCtrl::SetURL(const wxString &url)
{
    m_url = url;

    wxCharBuffer buf = url.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var link = container.querySelector('.wxHyperlinkCtrl');
        if (link) link.href = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxHyperlinkCtrl::SetVisited(bool visited)
{
    m_visited = visited;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var link = container.querySelector('.wxHyperlinkCtrl');
        if (!link) return;
        link.classList.toggle('visited', $1 !== 0);
        link.style.color = ($1 !== 0) ? link.dataset.visited
                                      : link.dataset.normal;
    }, GetId(), visited ? 1 : 0);
}

void wxHyperlinkCtrl::SetNormalColour(const wxColour &colour)
{
    m_normalColour = colour;
    UpdateLinkColour();
}

void wxHyperlinkCtrl::SetHoverColour(const wxColour &colour)
{
    m_hoverColour = colour;
    UpdateLinkColour();
}

void wxHyperlinkCtrl::SetVisitedColour(const wxColour &colour)
{
    m_visitedColour = colour;
    UpdateLinkColour();
}

void wxHyperlinkCtrl::UpdateLinkColour()
{
    wxCharBuffer normalBuf = m_normalColour.GetAsString(wxC2S_HTML_SYNTAX).ToUTF8();
    wxCharBuffer hoverBuf = m_hoverColour.GetAsString(wxC2S_HTML_SYNTAX).ToUTF8();
    wxCharBuffer visitedBuf = m_visitedColour.GetAsString(wxC2S_HTML_SYNTAX).ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var link = container.querySelector('.wxHyperlinkCtrl');
        if (!link) return;

        link.dataset.normal = UTF8ToString($1);
        link.dataset.hover = UTF8ToString($2);
        link.dataset.visited = UTF8ToString($3);
        link.style.color = ($4 !== 0) ? link.dataset.visited
                                      : link.dataset.normal;
    }, GetId(), normalBuf.data(), hoverBuf.data(), visitedBuf.data(),
       m_visited ? 1 : 0);
}

wxSize wxHyperlinkCtrl::DoGetBestSize() const
{
    int w = 0, h = 0;
    GetTextExtent(GetLabel(), &w, &h);

    if ( h <= 0 )
        h = GetCharHeight();

    return wxSize(w + 2, h + 2);
}

void wxHyperlinkCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "click" )
    {
        SetVisited(true);

        wxHyperlinkEvent evt(this, m_windowId, GetURL());
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

#endif // wxUSE_HYPERLINKCTRL
