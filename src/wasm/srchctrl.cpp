/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/srchctrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SEARCHCTRL

#include "wx/srchctrl.h"

#if wxUSE_MENUS
    #include "wx/menu.h"
#endif // wxUSE_MENUS

#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxSearchCtrl, wxSearchCtrlBase);

void wxSearchCtrl::Init()
{
#if wxUSE_MENUS
    m_menu = nullptr;
#endif // wxUSE_MENUS

    m_searchButtonVisible = true;
    m_cancelButtonVisible = false;
}

bool wxSearchCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxTextCtrl::Create(parent, id, value, pos, size, style, validator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxTextCtrl');
        if (!elem) return;

        // Turn the plain text input into the native search input of the
        // browser: it shows a search icon and a cancel button on its own.
        elem.type = 'search';
        elem.classList.add('wxSearchCtrl');

        // The DOM "search" event is fired when Enter is pressed or when
        // the native cancel button is clicked.
        elem.addEventListener('search', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'search', 0, 0]);
            }
        });
    }, GetId());

    return true;
}

#if wxUSE_MENUS
void wxSearchCtrl::SetMenu( wxMenu* menu )
{
    // Take ownership of the menu, as the other ports do.
    delete m_menu;
    m_menu = menu;
}
#endif // wxUSE_MENUS

void wxSearchCtrl::ShowSearchButton( bool show )
{
    // The icon of the native search input is drawn by the browser.
    m_searchButtonVisible = show;
}

void wxSearchCtrl::ShowCancelButton( bool show )
{
    // The cancel button of the native search input is drawn by the browser.
    m_cancelButtonVisible = show;
}

void wxSearchCtrl::SetDescriptiveText(const wxString& text)
{
    m_descriptiveText = text;

    wxCharBuffer buf = text.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var elem = container.querySelector('.wxSearchCtrl');
        if (elem) elem.placeholder = UTF8ToString($1);
    }, GetId(), buf.data());
}

void wxSearchCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "search" )
    {
        const wxString value = GetValue();

        if ( value.empty() )
        {
            // The native cancel button clears the input before the event.
            wxCommandEvent evt(wxEVT_SEARCHCTRL_CANCEL_BTN, m_windowId);
            HandleWindowEvent(evt);
        }
        else
        {
            wxCommandEvent evt(wxEVT_SEARCHCTRL_SEARCH_BTN, m_windowId);
            evt.SetString(value);
            HandleWindowEvent(evt);
        }

        return;
    }

    // Forward the text events to the base class.
    wxTextCtrl::WasmNotifyEvent(event);
}

#endif // wxUSE_SEARCHCTRL
