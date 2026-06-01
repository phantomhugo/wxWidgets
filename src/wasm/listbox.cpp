/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/listbox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listbox.h"
#include <emscripten.h>

wxListBox::wxListBox(): wxScrollHelper(this)
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name): wxScrollHelper(this)
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name) : wxScrollHelper(this)
{
    Create( parent, id, pos, size, choices, style, validator, name );
}

wxListBox::~wxListBox()
{
    Clear();
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if (!wxListBoxBase::Create(parent, id, pos, size, style, validator, name))
        return false;

    bool isMultiple = (style & wxLB_MULTIPLE) || (style & wxLB_EXTENDED);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var select = document.createElement('select');
        select.className = 'wxListBox';
        select.style.width = '100%';
        select.style.height = '100%';
        select.style.boxSizing = 'border-box';
        select.style.fontFamily = 'inherit';
        select.style.fontSize = 'inherit';
        select.style.margin = '0';
        select.style.padding = '0';

        if ($1) {
            select.multiple = true;
        }

        select.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(select);
    }, GetId(), isMultiple ? 1 : 0);

    for (int i = 0; i < n; ++i)
    {
        m_items.push_back(choices[i]);

        wxCharBuffer buf = choices[i].ToUTF8();
        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var select = container.querySelector('.wxListBox');
            if (!select) return;
            var option = document.createElement('option');
            option.textContent = UTF8ToString($1);
            select.appendChild(option);
        }, GetId(), buf.data());
    }

    return true;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    return Create(parent, id, pos, size, choices.size(), choices.size() ? &choices[0] : nullptr, style, validator, name);
}

void wxListBox::Init()
{
#if wxUSE_CHECKLISTBOX

#endif // wxUSE_CHECKLISTBOX
}

void wxListBox::EnsureVisible(int n)
{
    wxUnusedVar(n);
}

bool wxListBox::IsSelected(int n) const
{
    return m_selections.Index(n) != wxNOT_FOUND;
}

wxBorder wxListBox::GetDefaultBorder() const
{
    return wxBORDER_SUNKEN;
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections = m_selections;
    return (int)aSelections.size();
}

unsigned wxListBox::GetCount() const
{
    return (unsigned)m_items.size();
}

bool wxListBox::SetFont(const wxFont& font)
{
    return wxWindowBase::SetFont(font);
}

wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG(n < GetCount(), wxString(), "invalid index");
    return m_items[n];
}

void wxListBox::SetString(unsigned int n, const wxString& s)
{
    wxCHECK_RET(n < GetCount(), "invalid index");
    m_items[n] = s;

    wxCharBuffer buf = s.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxListBox');
        if (!select) return;
        if ($1 >= 0 && $1 < select.options.length) {
            select.options[$1].textContent = UTF8ToString($2);
        }
    }, GetId(), (int)n, buf.data());
}

int wxListBox::FindString(const wxString& s, bool bCase) const
{
    if (bCase)
    {
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i] == s)
                return (int)i;
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].CmpNoCase(s) == 0)
                return (int)i;
        }
    }
    return wxNOT_FOUND;
}

int wxListBox::GetSelection() const
{
    if (HasMultipleSelection())
        return wxNOT_FOUND;

    if (m_selections.IsEmpty())
        return wxNOT_FOUND;

    return m_selections[0];
}

void wxListBox::DoDrawRange(wxControlRenderer *renderer, int itemFirst, int itemLast)
{
    wxUnusedVar(renderer);
    wxUnusedVar(itemFirst);
    wxUnusedVar(itemLast);
}

void wxListBox::DoEnsureVisible(int n)
{
    wxUnusedVar(n);
}

void wxListBox::DoSetFirstItem(int n)
{
    wxUnusedVar(n);
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if (n < 0 || n >= (int)GetCount())
        return;

    if (select)
    {
        if (!HasMultipleSelection())
        {
            for (size_t i = 0; i < m_selections.GetCount(); ++i)
            {
                int oldSel = m_selections[i];
                EM_ASM_({
                    var container = document.getElementById($0);
                    if (!container) return;
                    var select = container.querySelector('.wxListBox');
                    if (!select) return;
                    if ($1 >= 0 && $1 < select.options.length) {
                        select.options[$1].selected = false;
                    }
                }, GetId(), oldSel);
            }
            m_selections.Clear();
        }

        if (m_selections.Index(n) == wxNOT_FOUND)
            m_selections.Add(n);
    }
    else
    {
        int idx = m_selections.Index(n);
        if (idx != wxNOT_FOUND)
            m_selections.RemoveAt(idx);
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxListBox');
        if (!select) return;
        if ($1 >= 0 && $1 < select.options.length) {
            select.options[$1].selected = $2;
        }
    }, GetId(), n, select ? 1 : 0);
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                          unsigned int pos,
                          void **clientData,
                          wxClientDataType type)
{
    wxUnusedVar(clientData);
    wxUnusedVar(type);

    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    UpdateOldSelections();
    return n;
}

int wxListBox::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    if (pos > m_items.size())
        pos = (unsigned)m_items.size();

    m_items.insert(m_items.begin() + pos, item);

    wxCharBuffer buf = item.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxListBox');
        if (!select) return;
        var option = document.createElement('option');
        option.textContent = UTF8ToString($1);
        if ($2 >= 0 && $2 < select.options.length) {
            select.insertBefore(option, select.options[$2]);
        } else {
            select.appendChild(option);
        }
    }, GetId(), buf.data(), (int)pos);

    for (size_t i = 0; i < m_selections.GetCount(); ++i)
    {
        if (m_selections[i] >= (int)pos)
            m_selections[i]++;
    }

    return (int)pos;
}

int wxListBox::DoListHitTest(const wxPoint& point) const
{
    wxUnusedVar(point);
    return wxNOT_FOUND;
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    wxUnusedVar(n);
    wxUnusedVar(clientData);
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    wxUnusedVar(n);
    return nullptr;
}

wxSize wxListBox::DoGetBestClientSize() const
{
    return wxSize(100, GetCount() * 18);
}

void wxListBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);
}

void wxListBox::DoClear()
{
    m_items.clear();
    m_selections.Clear();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxListBox');
        if (!select) return;
        while (select.options.length > 0) {
            select.remove(0);
        }
    }, GetId());
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
    if (pos >= m_items.size())
        return;

    m_items.erase(m_items.begin() + pos);

    wxArrayInt newSelections;
    for (size_t i = 0; i < m_selections.GetCount(); ++i)
    {
        if (m_selections[i] == (int)pos)
            continue;
        if (m_selections[i] > (int)pos)
            newSelections.Add(m_selections[i] - 1);
        else
            newSelections.Add(m_selections[i]);
    }
    m_selections = newSelections;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var select = container.querySelector('.wxListBox');
        if (!select) return;
        if ($1 >= 0 && $1 < select.options.length) {
            select.remove($1);
        }
    }, GetId(), (int)pos);
}

void wxListBox::OnInternalIdle()
{
}

void wxListBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        m_selections.Clear();

        int count = EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 0;
            var select = container.querySelector('.wxListBox');
            if (!select) return 0;
            return select.options.length;
        }, GetId());

        for (int i = 0; i < count; ++i)
        {
            bool selected = EM_ASM_INT({
                var container = document.getElementById($0);
                if (!container) return 0;
                var select = container.querySelector('.wxListBox');
                if (!select) return 0;
                if ($1 >= 0 && $1 < select.options.length) {
                    return select.options[$1].selected ? 1 : 0;
                }
                return 0;
            }, GetId(), i);

            if (selected)
                m_selections.Add(i);
        }

        int sel = GetSelection();
        wxCommandEvent evt(wxEVT_LISTBOX, m_windowId);
        evt.SetInt(sel);
        HandleWindowEvent(evt);
    }
}
