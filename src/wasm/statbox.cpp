/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statbox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbox.h"
#include "wx/window.h"
#include <emscripten.h>


wxStaticBox::wxStaticBox()
{
}

wxStaticBox::wxStaticBox(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBox::Create(wxWindow *parent, wxWindowID id,
            const wxString& label,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    int domId = GetId();
    wxCharBuffer labelBuffer = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var fieldset = document.createElement('fieldset');
        fieldset.className = 'wxStaticBox';

        var legend = document.createElement('legend');
        legend.textContent = UTF8ToString($1);

        fieldset.appendChild(legend);
        container.appendChild(fieldset);
    }, domId, labelBuffer.data());

    return true;
}

void wxStaticBox::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);

    wxCharBuffer buffer = label.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (container) {
            var legend = container.querySelector('.wxStaticBox legend');
            if (legend) {
                legend.textContent = UTF8ToString($1);
            }
        }
    }, GetId(), buffer.data());
}

wxString wxStaticBox::GetLabel() const
{
    return wxControl::GetLabel();
}

void wxStaticBox::GetBordersForSizer(int *borderTop, int *borderOther) const
{
    wxStaticBoxBase::GetBordersForSizer(borderTop, borderOther);

    // need extra space for the label:
    *borderTop += GetCharHeight();
}

wxSize wxStaticBox::DoGetBestSize() const
{
    return wxSize(50, 30);
}

WXWidget wxStaticBox::GetHandle() const
{
    return nullptr;
}

void wxStaticBox::WasmNotifyEvent(const wxWasmEvent& WXUNUSED(event))
{
}
