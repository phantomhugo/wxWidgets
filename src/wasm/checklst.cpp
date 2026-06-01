/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/checklst.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checklst.h"
#include <emscripten.h>

wxCheckListBox::wxCheckListBox()
{
    Init();
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int nStrings,
        const wxString *choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Init();
    Create( parent, id, pos, size, nStrings, choices, style, validator, name );
}

wxCheckListBox::wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Init();
    Create( parent, id, pos, size, choices, style, validator, name );
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              int n, const wxString choices[],
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var div = document.createElement('div');
        div.className = 'wxCheckListBox';
        div.style.width = '100%';
        div.style.height = '100%';
        div.style.overflow = 'auto';
        div.style.boxSizing = 'border-box';

        container.appendChild(div);
    }, GetId());

    for ( int i = 0; i < n; ++i )
    {
        Append(choices[i]);
    }

    return true;
}

bool wxCheckListBox::Create(wxWindow *parent, wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              const wxArrayString& choices,
              long style,
              const wxValidator& validator,
              const wxString& name )
{
    return Create( parent, id, pos, size, choices.size(), choices.size() ? &choices[0] : nullptr, style, validator, name );
}

void wxCheckListBox::Init()
{
}

bool wxCheckListBox::IsChecked(unsigned int n) const
{
    wxCHECK_MSG( n < GetCount(), false, "invalid index in wxCheckListBox::IsChecked" );

    int checked = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var div = container.querySelector('.wxCheckListBox');
        if (!div) return 0;
        if ($1 >= 0 && $1 < div.children.length) {
            var checkbox = div.children[$1].querySelector('input[type="checkbox"]');
            return checkbox ? checkbox.checked : 0;
        }
        return 0;
    }, GetId(), (int)n);

    return checked != 0;
}

void wxCheckListBox::Check(unsigned int n, bool check )
{
    wxCHECK_RET( n < GetCount(), "invalid index in wxCheckListBox::Check" );

    if ( n < m_checks.size() )
        m_checks[n] = check ? 1 : 0;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var div = container.querySelector('.wxCheckListBox');
        if (!div) return;
        if ($1 >= 0 && $1 < div.children.length) {
            var checkbox = div.children[$1].querySelector('input[type="checkbox"]');
            if (checkbox) checkbox.checked = $2;
        }
    }, GetId(), (int)n, check ? 1 : 0);
}

int wxCheckListBox::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    int n = wxListBox::DoInsertOneItem(item, pos);
    if ( n != wxNOT_FOUND )
        OnItemInserted(n);
    return n;
}

void wxCheckListBox::OnItemInserted(unsigned int pos)
{
    m_checks.Insert(0, pos);

    wxString text = GetString(pos);
    wxCharBuffer buf = text.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var div = container.querySelector('.wxCheckListBox');
        if (!div) return;

        var label = document.createElement('label');
        label.style.display = 'block';
        label.style.cursor = 'pointer';

        var checkbox = document.createElement('input');
        checkbox.type = 'checkbox';

        checkbox.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', $1, 0]);
            }
        });

        var span = document.createElement('span');
        span.textContent = UTF8ToString($2);

        label.appendChild(checkbox);
        label.appendChild(document.createTextNode(' '));
        label.appendChild(span);

        if ($1 >= 0 && $1 < div.children.length) {
            div.insertBefore(label, div.children[$1]);
        } else {
            div.appendChild(label);
        }
    }, GetId(), (int)pos, buf.data());
}

void wxCheckListBox::DoDeleteOneItem(unsigned int n)
{
    wxListBox::DoDeleteOneItem(n);

    if ( n < m_checks.size() )
        m_checks.RemoveAt(n);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var div = container.querySelector('.wxCheckListBox');
        if (!div) return;
        if ($1 >= 0 && $1 < div.children.length) {
            div.children[$1].remove();
        }
    }, GetId(), (int)n);
}

void wxCheckListBox::DoDrawRange(wxControlRenderer *renderer,int itemFirst, int itemLast)
{
    wxUnusedVar(renderer);
    wxUnusedVar(itemFirst);
    wxUnusedVar(itemLast);
}

wxSize wxCheckListBox::DoGetBestClientSize() const
{
    return wxSize(150, GetCount() * 22);
}

void wxCheckListBox::DoClear()
{
    wxListBox::DoClear();
    m_checks.Empty();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var div = container.querySelector('.wxCheckListBox');
        if (!div) return;
        while (div.firstChild) {
            div.removeChild(div.firstChild);
        }
    }, GetId());
}

void wxCheckListBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "change" )
    {
        int n = event.x;
        bool checked = IsChecked(n);
        if ( n >= 0 && n < (int)m_checks.size() )
            m_checks[n] = checked ? 1 : 0;

        wxCommandEvent evt(wxEVT_CHECKLISTBOX, m_windowId);
        evt.SetInt(n);
        HandleWindowEvent(evt);
    }
}
