/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/stattext.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/stattext.h"
#include <emscripten.h>

wxStaticText::wxStaticText()
{
}

wxStaticText::wxStaticText(wxWindow *parent,
             wxWindowID id,
             const wxString &label,
             const wxPoint &pos,
             const wxSize &size,
             long style,
             const wxString &name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticText::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &label,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxString &name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    int domId = GetId();
    wxCharBuffer labelBuffer = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var span = document.createElement('span');
        span.className = 'wxStaticText';
        span.textContent = UTF8ToString($1);
        container.appendChild(span);
    }, domId, labelBuffer.data());

    SetLabel(label);

    return true;
}

void wxStaticText::SetLabel(const wxString& label)
{
    wxControl::SetLabel(label);
    WXSetVisibleLabel(label);
}

void wxStaticText::WXSetVisibleLabel(const wxString& label)
{
    wxCharBuffer buffer = label.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (container) {
            var span = container.querySelector('.wxStaticText');
            if (span) {
                span.textContent = UTF8ToString($1);
            }
        }
    }, GetId(), buffer.data());
}

wxString wxStaticText::WXGetVisibleLabel() const
{
    return GetLabel();
}

WXWidget wxStaticText::GetHandle() const
{
    return nullptr;
}
