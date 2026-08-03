/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/activityindicator.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/activityindicator.h"

#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxActivityIndicator, wxControl);

wxActivityIndicator::wxActivityIndicator()
{
    m_running = false;
}

wxActivityIndicator::wxActivityIndicator(wxWindow* parent,
                                         wxWindowID winid,
                                         const wxPoint& pos,
                                         const wxSize& size,
                                         long style,
                                         const wxString& name)
{
    m_running = false;

    Create(parent, winid, pos, size, style, name);
}

bool wxActivityIndicator::Create(wxWindow* parent,
                                 wxWindowID winid,
                                 const wxPoint& pos,
                                 const wxSize& size,
                                 long style,
                                 const wxString& name)
{
    // The indicator needs a sensible default size if none was given.
    wxSize newSize(size);
    if ( newSize.x == wxDefaultCoord )
        newSize.x = 24;
    if ( newSize.y == wxDefaultCoord )
        newSize.y = 24;

    if ( !wxControl::Create(parent, winid, pos, newSize, style,
                            wxDefaultValidator, name) )
        return false;

    m_running = false;

    // The spinner itself is a plain <div> styled and animated by the
    // .wxActivityIndicator CSS rules; it is only shown while the container
    // has the "running" class.
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        container.classList.add('wxActivityIndicator');

        var spinner = document.createElement('div');
        spinner.className = 'wxActivityIndicator-spinner';
        container.appendChild(spinner);
    }, GetId());

    return true;
}

void wxActivityIndicator::Start()
{
    m_running = true;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        container.classList.add('running');
    }, GetId());
}

void wxActivityIndicator::Stop()
{
    m_running = false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        container.classList.remove('running');
    }, GetId());
}

bool wxActivityIndicator::IsRunning() const
{
    return m_running;
}

wxSize wxActivityIndicator::DoGetBestSize() const
{
    return wxSize(24, 24);
}
