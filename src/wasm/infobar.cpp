/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/infobar.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_INFOBAR

#ifndef WX_PRECOMP
    #include "wx/stockitem.h"
#endif // WX_PRECOMP

#include "wx/infobar.h"

#include <emscripten.h>

wxDEFINE_EVENT( wxEVT_INFOBAR_DISMISSED, wxCommandEvent );

wxIMPLEMENT_DYNAMIC_CLASS(wxInfoBar, wxControl);

namespace
{

// map the wxICON_XXX flags to the index of the corresponding CSS variant
int IconIndexFromFlags(int flags)
{
    switch ( flags & wxICON_MASK )
    {
        case wxICON_INFORMATION:
            return 0; // wxInfoBar-info
        case wxICON_WARNING:
            return 1; // wxInfoBar-warning
        case wxICON_ERROR:
            return 2; // wxInfoBar-error
        case wxICON_QUESTION:
            return 3; // wxInfoBar-question
        default:
            return 4; // wxInfoBar-none
    }
}

} // anonymous namespace

void wxInfoBar::Init()
{
    m_showEffect =
    m_hideEffect = wxSHOW_EFFECT_MAX;

    // use default effect duration
    m_effectDuration = 0;
}

bool wxInfoBar::Create(wxWindow *parent, wxWindowID winid, long style)
{
    if ( !wxControl::Create(parent, winid, wxDefaultPosition, wxDefaultSize,
                            style, wxDefaultValidator, "wxInfoBar") )
        return false;

    int domId = GetDomWindowId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        container.classList.add('wxInfoBar');
        container.classList.add('wxInfoBar-info');

        // the info bar is created hidden and is only shown by ShowMessage()
        container.style.display = 'none';

        var message = document.createElement('span');
        message.className = 'wxInfoBar-message';
        container.appendChild(message);

        var close = document.createElement('button');
        close.type = 'button';
        close.className = 'wxInfoBar-close';
        close.textContent = '✕';
        close.addEventListener('click', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'dismiss', 0, 0]);
            }
        });
        container.appendChild(close);
    }, domId);

    // keep the internal visibility flag in sync with the DOM
    wxWindowBase::Show(false);

    return true;
}

void wxInfoBar::DoShowBar(bool show)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        container.style.display = $1 ? 'flex' : 'none';
    }, GetId(), show ? 1 : 0);

    // just change the internal flag indicating that the window is visible,
    // without touching the DOM again
    wxWindowBase::Show(show);

    // update the parent to take our new or changed size into account
    if ( wxWindow * const parent = GetParent() )
        parent->Layout();
}

void wxInfoBar::ShowMessage(const wxString& msg, int flags)
{
    wxCharBuffer buf = msg.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        // NB: an array literal can't be used here, the commas inside square
        // brackets are not protected from the EM_ASM_ macro argument split
        container.classList.remove('wxInfoBar-info');
        container.classList.remove('wxInfoBar-warning');
        container.classList.remove('wxInfoBar-error');
        container.classList.remove('wxInfoBar-question');
        container.classList.remove('wxInfoBar-none');

        var classes = [];
        classes.push('info');
        classes.push('warning');
        classes.push('error');
        classes.push('question');
        classes.push('none');
        container.classList.add('wxInfoBar-' + classes[$2]);

        var message = container.querySelector('.wxInfoBar-message');
        if (message) message.textContent = UTF8ToString($1);
    }, GetId(), buf.data(), IconIndexFromFlags(flags));

    DoShowBar(true);
}

void wxInfoBar::Dismiss()
{
    DoShowBar(false);
}

void wxInfoBar::ShowCheckBox(const wxString& checkBoxText, bool checked)
{
    wxCharBuffer buf = checkBoxText.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var label = container.querySelector('.wxInfoBar-checkbox');
        var text = UTF8ToString($1);

        if (text.length === 0) {
            if (label) label.remove();
            return;
        }

        if (!label) {
            label = document.createElement('label');
            label.className = 'wxInfoBar-checkbox';

            var input = document.createElement('input');
            input.type = 'checkbox';
            label.appendChild(input);
            label.appendChild(document.createTextNode(""));

            // insert before the close button, like the extra buttons
            var close = container.querySelector('.wxInfoBar-close');
            if (close) {
                container.insertBefore(label, close);
            } else {
                container.appendChild(label);
            }
        }

        label.lastChild.nodeValue = ' ' + text;
        label.firstChild.checked = $2 !== 0;
    }, GetId(), buf.data(), checked ? 1 : 0);
}

bool wxInfoBar::IsCheckBoxChecked() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var label = container.querySelector('.wxInfoBar-checkbox');
        if (!label) return 0;
        return label.firstChild.checked ? 1 : 0;
    }, GetId()) != 0;
}

void wxInfoBar::AddButton(wxWindowID btnid, const wxString& label)
{
    wxCHECK_RET( !HasButtonId(btnid), "button with this id already exists" );

    wxString btnLabel = label;
    if ( btnLabel.empty() && wxIsStockID(btnid) )
        btnLabel = wxGetStockLabel(btnid);

    wxCharBuffer buf = btnLabel.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var btn = document.createElement('button');
        btn.type = 'button';
        btn.className = 'wxButton';
        btn.dataset.id = String($1);
        btn.textContent = UTF8ToString($2);
        btn.addEventListener('click', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'button', $1, 0]);
            }
        });

        // extra buttons replace the default close button
        var close = container.querySelector('.wxInfoBar-close');
        if (close) {
            close.style.display = 'none';
            container.insertBefore(btn, close);
        } else {
            container.appendChild(btn);
        }
    }, GetId(), (int)btnid, buf.data());

    m_buttons.push_back(btnid);
}

void wxInfoBar::RemoveButton(wxWindowID btnid)
{
    auto it = std::find(m_buttons.begin(), m_buttons.end(), btnid);
    wxCHECK_RET( it != m_buttons.end(), "button with this id not found" );

    m_buttons.erase(it);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var buttons = container.querySelectorAll('.wxButton');
        for (var i = 0; i < buttons.length; i++) {
            if (buttons[i].dataset.id === String($1)) {
                buttons[i].remove();
                break;
            }
        }

        // restore the default close button when no extra buttons are left
        if ($2 === 0) {
            var close = container.querySelector('.wxInfoBar-close');
            if (close) close.style.removeProperty('display');
        }
    }, GetId(), (int)btnid, (int)m_buttons.size());
}

size_t wxInfoBar::GetButtonCount() const
{
    return m_buttons.size();
}

wxWindowID wxInfoBar::GetButtonId(size_t idx) const
{
    wxCHECK_MSG( idx < m_buttons.size(), wxID_NONE,
                 "invalid infobar button position" );

    return m_buttons[idx];
}

bool wxInfoBar::HasButtonId(wxWindowID btnid) const
{
    return std::find(m_buttons.begin(), m_buttons.end(), btnid)
               != m_buttons.end();
}

wxSize wxInfoBar::DoGetBestSize() const
{
    return wxSize(100, 30);
}

void wxInfoBar::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id != m_windowId )
        return;

    if ( event.eventType == "dismiss" )
    {
        Dismiss();

        wxCommandEvent evt(wxEVT_INFOBAR_DISMISSED, m_windowId);
        evt.SetEventObject(this);
        HandleWindowEvent(evt);
    }
    else if ( event.eventType == "button" )
    {
        wxCommandEvent evt(wxEVT_BUTTON, event.x);
        evt.SetEventObject(this);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

#endif // wxUSE_INFOBAR
