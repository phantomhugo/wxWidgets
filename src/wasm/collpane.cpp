/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/collpane.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_COLLPANE

#include "wx/collpane.h"

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
    #include "wx/sizer.h"
    #include "wx/panel.h"
#endif // !WX_PRECOMP

#include <emscripten.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const char wxCollapsiblePaneNameStr[] = "collapsiblePane";

// ----------------------------------------------------------------------------
// wxCollapsiblePane
// ----------------------------------------------------------------------------

wxDEFINE_EVENT( wxEVT_COLLAPSIBLEPANE_CHANGED, wxCollapsiblePaneEvent );
wxIMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePane, wxControl);
wxIMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePaneEvent, wxCommandEvent);

void wxCollapsiblePane::Init()
{
    m_pPane = nullptr;
    m_collapsed = true;
}

bool wxCollapsiblePane::Create(wxWindow *parent,
          wxWindowID id,
          const wxString& label,
          const wxPoint& pos,
          const wxSize& size,
          long style,
          const wxValidator& val,
          const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, val, name) )
        return false;

    int domId = GetDomWindowId();
    wxCharBuffer labelBuffer = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        container.classList.add('wxCollapsiblePane');

        var btn = document.createElement('button');
        btn.type = 'button';
        btn.className = 'wxCollapsiblePane-button';

        var arrow = document.createElement('span');
        arrow.className = 'wxCollapsiblePane-arrow';
        arrow.textContent = '▶'; // the pane starts collapsed
        btn.appendChild(arrow);

        var text = document.createElement('span');
        text.className = 'wxCollapsiblePane-label';
        text.textContent = UTF8ToString($1);
        btn.appendChild(text);

        btn.addEventListener('click', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'toggle', 0, 0]);
            }
        });

        container.appendChild(btn);

        var pane = document.createElement('div');
        pane.className = 'wxCollapsiblePane-pane';
        container.appendChild(pane);
    }, domId, labelBuffer.data());

    m_label = label;

    // create the container window which is the parent of the pane contents
    m_pPane = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                          wxTAB_TRAVERSAL|wxNO_BORDER, "wxCollapsiblePanePane");

    // move the pane window element inside our own pane div
    int paneId = m_pPane->GetId();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var pane = container.querySelector('.wxCollapsiblePane-pane');
        var paneElem = document.getElementById($1);
        if (pane && paneElem) {
            paneElem.style.position = 'relative';
            paneElem.style.width = '100%';
            pane.appendChild(paneElem);
        }
    }, domId, paneId);

    // start as collapsed:
    m_pPane->Hide();

    // Lay out the pane window when we are resized (the generic version does
    // this in its OnSize handler): the header button keeps its DOM height
    // and the pane gets the rest of our client area.
    Bind(wxEVT_SIZE, [this](wxSizeEvent& event)
    {
        if ( m_pPane )
        {
            const int domId = GetDomWindowId();
            const int btnH = EM_ASM_INT({
                var container = document.getElementById($0);
                if (!container) return 26;
                var btn = container.querySelector('.wxCollapsiblePane-button');
                return btn ? btn.offsetHeight : 26;
            }, domId);

            const wxSize sz = GetClientSize();
            const int paneH = wxMax(0, sz.y - btnH);

            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var pane = container.querySelector('.wxCollapsiblePane-pane');
                if (pane) {
                    pane.style.position = 'relative';
                    pane.style.height = $2 + 'px';
                    pane.style.overflow = 'hidden';
                }
            }, domId, 0, paneH);

            // the panel element sits at (0,0) inside the pane div (it was
            // moved there in Create and positioned relative)
            m_pPane->SetSize(0, 0, sz.x, paneH);
        }
        event.Skip();
    });

    return true;
}

void wxCollapsiblePane::Collapse(bool collapse)
{
    // optimization
    if ( IsCollapsed() == collapse )
        return;

    InvalidateBestSize();

    // update our state
    m_collapsed = collapse;
    m_pPane->Show(!collapse);

    // update the arrow in the DOM
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var arrow = container.querySelector('.wxCollapsiblePane-arrow');
        if (arrow) arrow.textContent = $1 ? '▶' : '▼';
    }, GetId(), collapse ? 1 : 0);

    OnStateChange(GetBestSize());
}

void wxCollapsiblePane::SetLabel(const wxString& label)
{
    m_label = label;

    wxCharBuffer buf = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var text = container.querySelector('.wxCollapsiblePane-label');
        if (text) text.textContent = UTF8ToString($1);
    }, GetId(), buf.data());

    InvalidateBestSize();
}

wxSize wxCollapsiblePane::DoGetBestSize() const
{
    // approximate size of the button: arrow, gap and the label text
    int width = GetTextExtent(m_label).x + 30;
    int height = 26;

    // when expanded, we need more space
    if ( IsExpanded() )
    {
        const wxSize szPane = m_pPane->GetBestSize();
        width = wxMax(width, szPane.x);
        height += szPane.y;
    }

    return wxSize(width, height);
}

void wxCollapsiblePane::OnStateChange(const wxSize& sz)
{
    // minimal size has priority over the best size so set here our min size
    SetSize(sz);

    if ( this->HasFlag(wxCP_NO_TLW_RESIZE) )
    {
        // the user asked to explicitly handle the resizing itself...
        return;
    }

    wxTopLevelWindow *top =
        wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
    if ( !top )
        return;

    wxSizer *sizer = top->GetSizer();
    if ( !sizer )
        return;

    const wxSize newBestSize = sizer->ComputeFittingClientSize(top);
    top->SetMinClientSize(newBestSize);

    // we shouldn't attempt to resize a maximized window, whatever happens
    if ( !top->IsMaximized() )
        top->SetClientSize(newBestSize);
}

void wxCollapsiblePane::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "toggle" )
    {
        Collapse(!IsCollapsed());

        // this change was generated by the user - send the event
        wxCollapsiblePaneEvent evt(this, GetId(), IsCollapsed());
        HandleWindowEvent(evt);
        return;
    }

    wxWindowWasm::WasmNotifyEvent(event);
}

#endif // wxUSE_COLLPANE
