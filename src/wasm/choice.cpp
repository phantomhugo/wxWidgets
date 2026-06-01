/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/choice.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/choice.h"

#include <emscripten.h>

wxChoice::wxChoice()
{
}

wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxChoice::wxChoice( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    return Create( parent, id, pos, size, choices.size(), choices.size() ? &choices[ 0 ] : nullptr, style,
        validator, name );
}

bool wxChoice::Create( wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name )
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    int domId = GetId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var select = document.createElement('select');
        select.className = 'wxChoice';
        select.style.width = '100%';
        select.style.height = '100%';
        select.style.boxSizing = 'border-box';
        select.style.fontFamily = 'inherit';
        select.style.fontSize = 'inherit';

        select.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(select);
    }, domId);

    for (int i = 0; i < n; ++i)
    {
        DoInsertOneItem(choices[i], m_choices.GetCount());
    }

    return true;
}

wxSize wxChoice::DoGetBestSize() const
{
    return wxSize(100, 24);
}

unsigned wxChoice::GetCount() const
{
    return m_choices.GetCount();
}

wxString wxChoice::GetString(unsigned int n) const
{
    wxCHECK_MSG(n < GetCount(), wxString(), "invalid index");

    return m_choices[n];
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET(n < GetCount(), "invalid index");

    m_choices[n] = s;

    wxCharBuffer buf = s.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxChoice');
        if (select && $1 < select.options.length) {
            select.options[$1].textContent = UTF8ToString($2);
        }
    }, GetId(), (int)n, buf.data());
}

void wxChoice::SetSelection(int n)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxChoice');
        if (select) select.selectedIndex = $1;
    }, GetId(), n);
}

int wxChoice::GetSelection() const
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return -1;
        var select = container.querySelector('.wxChoice');
        return select ? select.selectedIndex : -1;
    }, GetId());
}

int wxChoice::DoInsertItems(const wxArrayStringsAdapter & items,
                  unsigned int pos,
                  void **clientData,
                  wxClientDataType type)
{
    InvalidateBestSize();

    // Hack: to avoid resorting the model many times in DoInsertOneItem(),
    // which will be called for each item from DoInsertItemsInLoop(), reset the
    // wxCB_SORT style if we have it temporarily and only sort once at the end.
    bool wasSorted = false;
    if ( IsSorted() )
    {
        wasSorted = true;
        ToggleWindowStyle(wxCB_SORT);
    }

    int n = DoInsertItemsInLoop(items, pos, clientData, type);

    if ( wasSorted )
    {
        // Restore the flag turned off above.
        ToggleWindowStyle(wxCB_SORT);

    }

    return n;
}

int wxChoice::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    wxCHECK_MSG(IsValidInsert(pos), wxNOT_FOUND, "invalid index");

    wxCharBuffer buf = item.ToUTF8();

    m_choices.Insert(item, pos);
    m_clientData.insert(m_clientData.begin() + pos, nullptr);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxChoice');
        if (!select) return;

        var option = document.createElement('option');
        option.textContent = UTF8ToString($1);

        if ($2 >= select.options.length) {
            select.appendChild(option);
        } else {
            select.insertBefore(option, select.options[$2]);
        }
    }, GetId(), buf.data(), (int)pos);

    return pos;
}

void wxChoice::DoSetItemClientData(unsigned int n, void *clientData)
{
    wxCHECK_RET(n < GetCount(), "invalid index");

    if (n >= m_clientData.size())
        m_clientData.resize(n + 1, nullptr);

    m_clientData[n] = clientData;
}

void *wxChoice::DoGetItemClientData(unsigned int n) const
{
    wxCHECK_MSG(n < GetCount(), nullptr, "invalid index");

    if (n < m_clientData.size())
        return m_clientData[n];

    return nullptr;
}

void wxChoice::DoClear()
{
    m_choices.Clear();
    m_clientData.clear();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxChoice');
        if (select) {
            while (select.options.length > 0) {
                select.remove(0);
            }
        }
    }, GetId());
}

void wxChoice::DoDeleteOneItem(unsigned int pos)
{
    wxCHECK_RET(pos < GetCount(), "invalid index");

    m_choices.RemoveAt(pos);
    m_clientData.erase(m_clientData.begin() + pos);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxChoice');
        if (select && $1 < select.options.length) {
            select.remove($1);
        }
    }, GetId(), (int)pos);
}

void *wxChoice::GetHandle() const
{
    return nullptr;
}

void wxChoice::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        wxCommandEvent evt(wxEVT_CHOICE, m_windowId);
        HandleWindowEvent(evt);
    }
}
