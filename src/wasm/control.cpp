/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/control.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/control.h"
#include "wx/window.h"

#include <emscripten.h>

namespace
{

// return the index of the accel char (the one immediately after '&') in the
// label or -1 if there is none, and store the label without '&' in labelOnly
int FindAccelIndex(const wxString& label, wxString *labelOnly)
{
    labelOnly->clear();

    int indexAccel = -1;

    const size_t len = label.length();
    for ( size_t n = 0; n < len; n++ )
    {
        const wxChar ch = label[n];
        if ( ch == wxT('&') )
        {
            if ( n + 1 < len && label[n + 1] == wxT('&') )
            {
                // literal '&': keep one and skip the other
                labelOnly->append(ch);
                n++;
            }
            else if ( indexAccel == -1 )
            {
                indexAccel = (int)labelOnly->length();
            }
        }
        else
        {
            labelOnly->append(ch);
        }
    }

    return indexAccel;
}

} // anonymous namespace

wxIMPLEMENT_DYNAMIC_CLASS(wxControl, wxWindow);

void wxControl::Init()
{
    m_indexAccel = -1;
}

wxControl::wxControl()
{
    Init();
}

wxControl::wxControl(wxWindow *parent, wxWindowID id,
         const wxPoint& pos,
         const wxSize& size, long style,
         const wxValidator& validator,
         const wxString& name )
{
    Create( parent, id, pos, size, style, validator, name );
}


bool wxControl::Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size, long style,
        const wxValidator& validator,
        const wxString& name )
{
    bool isCreated = wxWindow::Create(parent, id, pos, size, style, name);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif

    return isCreated;
}

void wxControl::SetLabel(const wxString& label)
{
    m_indexAccel = FindAccelIndex(label, &m_label);

    // cache the label in the base (without the accel markers)
    wxWindow::SetLabel(m_label);

    // The label typically contributes to the best size (and the common
    // wxWindowBase::SetLabel() does not invalidate it).
    InvalidateBestSize();

    // Update the DOM element when it is a simple text-bearing widget.
    wxCharBuffer buf = m_label.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var text = UTF8ToString($1);
        var child = container.firstElementChild;
        if (!child) return;

        switch (child.tagName) {
            case 'BUTTON':
            case 'A':
            case 'SPAN':
            case 'LEGEND':
                child.textContent = text;
                break;
            case 'LABEL': {
                // wxCheckBox-like structure: <label><input><span>text</span></label>
                var span = child.querySelector('span');
                if (span) span.textContent = text;
                break;
            }
        }
    }, GetId(), buf.data());
}
