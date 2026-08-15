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

    // The best size computed during Create() used an empty label (it is
    // stored only by the SetLabel() above), so recompute it now and apply
    // the initial size again: with a default size the window was left 0
    // pixels wide.
    InvalidateBestSize();
    SetInitialSize(size);

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

wxSize wxStaticText::DoGetBestSize() const
{
    // Measure the label line by line with the real text extents.
    wxSize best(0, 0);
    const wxString label = GetLabel();

    if ( label.empty() )
        return wxSize(0, GetCharHeight());

    wxString::const_iterator lineStart = label.begin();
    for ( wxString::const_iterator it = label.begin(); ; ++it )
    {
        if ( it == label.end() || *it == wxT('\n') )
        {
            int w = 0, h = 0;
            GetTextExtent(wxString(lineStart, it), &w, &h);
            if ( h <= 0 )
                h = GetCharHeight();
            best.x = wxMax(best.x, w);
            best.y += h;

            if ( it == label.end() )
                break;
            lineStart = it + 1;
        }
    }

    return best;
}

WXWidget wxStaticText::GetHandle() const
{
    return nullptr;
}
