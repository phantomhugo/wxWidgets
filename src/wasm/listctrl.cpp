/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/listctrl.cpp
// Purpose:     wxListCtrl implementation using HTML <table>
// Author:      Mariano Reingart, Peter Most, Hugo Castellanos
// Copyright:   (c) 2010-2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/app.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"
#include "wx/recguard.h"
#include <emscripten.h>

//-----------------------------------------------------------------------------
// wxListCtrl
//-----------------------------------------------------------------------------

wxListCtrl::wxListCtrl()
{
    Init();
}

wxListCtrl::wxListCtrl(wxWindow *parent,
           wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Init();
    Create( parent, id, pos, size, style, validator, name );
}


bool wxListCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    int domId = GetId();
    bool isReport = HasFlag(wxLC_REPORT);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        if ($1) {
            // REPORT mode: use <table>
            var table = document.createElement('table');
            table.className = 'wxListCtrl-table';

            var thead = document.createElement('thead');
            thead.className = 'wxListCtrl-head';
            var headRow = document.createElement('tr');
            headRow.id = 'wxListCtrl_headrow_' + $0;
            thead.appendChild(headRow);

            var tbody = document.createElement('tbody');
            tbody.className = 'wxListCtrl-body';
            tbody.id = 'wxListCtrl_body_' + $0;

            // Click handler for row selection
            tbody.onclick = function(e) {
                var tr = e.target.closest('tr');
                if (!tr) return;
                var itemIdx = parseInt(tr.dataset.itemIdx);
                if (isNaN(itemIdx)) return;

                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'list_select', itemIdx, 0]);
                }
            };

            table.appendChild(thead);
            table.appendChild(tbody);
            container.appendChild(table);
        } else {
            // ICON / SMALL_ICON / LIST mode: use <div> grid
            var grid = document.createElement('div');
            grid.className = 'wxListCtrl-grid';
            grid.id = 'wxListCtrl_grid_' + $0;

            // Click handler for item selection
            grid.onclick = function(e) {
                var item = e.target.closest('.wxListCtrl-item');
                if (!item) return;
                var itemIdx = parseInt(item.dataset.itemIdx);
                if (isNaN(itemIdx)) return;

                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'list_select', itemIdx, 0]);
                }
            };

            container.appendChild(grid);
        }
    }, domId, isReport ? 1 : 0);

    SetWindowStyleFlag(style);
    return true;
}

void wxListCtrl::Init()
{
    m_hasCheckBoxes = false;
    m_editItem = -1;
}

wxListCtrl::~wxListCtrl()
{
}

bool wxListCtrl::SetForegroundColour(const wxColour& col)
{
    return wxListCtrlBase::SetForegroundColour(col);
}

bool wxListCtrl::SetBackgroundColour(const wxColour& col)
{
    return wxListCtrlBase::SetBackgroundColour(col);
}

//-----------------------------------------------------------------------------
// Columns
//-----------------------------------------------------------------------------

bool wxListCtrl::GetColumn(int col, wxListItem& info) const
{
    if (col < 0 || col >= (int)m_columns.size())
        return false;

    const ColumnInfo& c = m_columns[col];
    info.SetText(c.text);
    info.SetWidth(c.width);
    info.SetAlign(c.format);
    info.SetImage(c.image);
    return true;
}

bool wxListCtrl::SetColumn(int col, const wxListItem& info)
{
    if (col < 0 || col >= (int)m_columns.size())
        return false;

    ColumnInfo& c = m_columns[col];
    if (info.GetMask() & wxLIST_MASK_TEXT)
        c.text = info.GetText();
    if (info.GetMask() & wxLIST_MASK_WIDTH)
        c.width = info.GetWidth();
    if (info.GetMask() & wxLIST_MASK_FORMAT)
        c.format = info.GetAlign();
    if (info.GetMask() & wxLIST_MASK_IMAGE)
        c.image = info.GetImage();

    SyncColumnHeaders();
    return true;
}

int wxListCtrl::GetColumnWidth(int col) const
{
    if (col < 0 || col >= (int)m_columns.size())
        return 0;
    return m_columns[col].width;
}

bool wxListCtrl::SetColumnWidth(int col, int width)
{
    if (col < 0 || col >= (int)m_columns.size())
        return false;

    m_columns[col].width = width;

    EM_ASM_({
        var th = document.getElementById('wxListCtrl_col_' + $0 + '_' + $1);
        if (th) {
            if ($2 > 0) {
                th.style.width = $2 + 'px';
            } else {
                th.style.width = 'auto';
            }
        }
    }, GetId(), col, width);

    return true;
}

int wxListCtrl::GetColumnOrder(int col) const
{
    if (m_colOrder.IsEmpty())
        return col;
    for (size_t i = 0; i < m_colOrder.size(); ++i)
    {
        if (m_colOrder[i] == col)
            return (int)i;
    }
    return col;
}

int wxListCtrl::GetColumnIndexFromOrder(int order) const
{
    if (m_colOrder.IsEmpty())
        return order;
    if (order >= 0 && order < (int)m_colOrder.size())
        return m_colOrder[order];
    return order;
}

wxArrayInt wxListCtrl::GetColumnsOrder() const
{
    if (!m_colOrder.IsEmpty())
        return m_colOrder;
    wxArrayInt orders;
    for (size_t i = 0; i < m_columns.size(); ++i)
        orders.Add((int)i);
    return orders;
}

bool wxListCtrl::SetColumnsOrder(const wxArrayInt& orders)
{
    if ((size_t)orders.GetCount() != m_columns.size())
        return false;

    m_colOrder = orders;

    // Rebuild headers
    SyncColumnHeaders();

    // Rebuild all rows to match new column order
    if (IsReportView())
    {
        for (size_t i = 0; i < m_items.size(); ++i)
        {
            RemoveItemRow((long)i);
            CreateItemDOM((long)i);
            SyncItemRow((long)i);
            UpdateItemStateClass((long)i);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Items
//-----------------------------------------------------------------------------

bool wxListCtrl::GetItem(wxListItem& info) const
{
    long item = info.GetId();
    int col = info.GetColumn();

    if (item < 0 || item >= (long)m_items.size())
        return false;

    const ItemInfo& it = m_items[item];

    if (info.GetMask() & wxLIST_MASK_TEXT)
    {
        if (col >= 0 && col < (int)it.texts.size())
            info.SetText(it.texts[col]);
        else if (!it.texts.empty())
            info.SetText(it.texts[0]);
    }
    if (info.GetMask() & wxLIST_MASK_DATA)
        info.SetData((long)it.data);
    if (info.GetMask() & wxLIST_MASK_STATE)
        info.SetState(it.state);
    if (info.GetMask() & wxLIST_MASK_IMAGE)
        info.SetImage(it.image);

    return true;
}

bool wxListCtrl::SetItem(wxListItem& info)
{
    long item = info.GetId();
    int col = info.GetColumn();

    if (item < 0 || item >= (long)m_items.size())
        return false;

    ItemInfo& it = m_items[item];

    if (info.GetMask() & wxLIST_MASK_TEXT)
    {
        if (col < 0) col = 0;
        if (col >= (int)it.texts.size())
            it.texts.resize(col + 1);
        it.texts[col] = info.GetText();
        SyncItemRow(item);
    }
    if (info.GetMask() & wxLIST_MASK_DATA)
        it.data = info.GetData();
    if (info.GetMask() & wxLIST_MASK_STATE)
    {
        it.state = info.GetState();
        UpdateItemStateClass(item);
    }
    if (info.GetMask() & wxLIST_MASK_IMAGE)
        it.image = info.GetImage();

    return true;
}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{
    if (index < 0 || index >= (long)m_items.size())
        return -1;

    ItemInfo& it = m_items[index];
    if (col >= (int)it.texts.size())
        it.texts.resize(col + 1);
    it.texts[col] = label;
    if (imageId != -1)
        it.image = imageId;

    SyncItemRow(index);
    return index;
}

int wxListCtrl::GetItemState(long item, long stateMask) const
{
    if (item < 0 || item >= (long)m_items.size())
        return 0;
    return m_items[item].state & stateMask;
}

bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{
    if (item < 0 || item >= (long)m_items.size())
        return false;

    long oldState = m_items[item].state;
    m_items[item].state = (oldState & ~stateMask) | (state & stateMask);
    UpdateItemStateClass(item);

    // If selecting and single-select mode, deselect others
    if ((stateMask & wxLIST_STATE_SELECTED) && (state & wxLIST_STATE_SELECTED)
        && HasFlag(wxLC_SINGLE_SEL))
    {
        for (size_t i = 0; i < m_items.size(); ++i)
        {
            if ((long)i != item && (m_items[i].state & wxLIST_STATE_SELECTED))
            {
                m_items[i].state &= ~wxLIST_STATE_SELECTED;
                UpdateItemStateClass(i);
            }
        }
    }

    return true;
}

bool wxListCtrl::SetItemImage(long item, int image, int WXUNUSED(selImage))
{
    return SetItemColumnImage(item, 0, image);
}

bool wxListCtrl::SetItemColumnImage(long item, long column, int image)
{
    if (item < 0 || item >= (long)m_items.size())
        return false;
    m_items[item].image = image;
    wxUnusedVar(column);
    return true;
}

wxString wxListCtrl::GetItemText(long item, int col) const
{
    if (item < 0 || item >= (long)m_items.size())
        return wxString();
    const ItemInfo& it = m_items[item];
    if (col >= 0 && col < (int)it.texts.size())
        return it.texts[col];
    if (!it.texts.empty())
        return it.texts[0];
    return wxString();
}

void wxListCtrl::SetItemText(long item, const wxString& str)
{
    if (item < 0 || item >= (long)m_items.size())
        return;
    if (m_items[item].texts.empty())
        m_items[item].texts.resize(1);
    m_items[item].texts[0] = str;
    SyncItemRow(item);
}

wxUIntPtr wxListCtrl::GetItemData(long item) const
{
    if (item < 0 || item >= (long)m_items.size())
        return 0;
    return m_items[item].data;
}

bool wxListCtrl::SetItemPtrData(long item, wxUIntPtr data)
{
    if (item < 0 || item >= (long)m_items.size())
        return false;
    m_items[item].data = data;
    return true;
}

bool wxListCtrl::SetItemData(long item, long data)
{
    return SetItemPtrData(item, static_cast<wxUIntPtr>(data));
}

bool wxListCtrl::GetItemRect(long item, wxRect& rect, int WXUNUSED(code)) const
{
    wxCHECK_MSG(item >= 0 && (item < GetItemCount()), false,
                 "invalid item in GetItemRect");
    rect = wxRect();
    return true;
}

bool wxListCtrl::GetSubItemRect(long item,
                                long subItem,
                                wxRect& rect,
                                int WXUNUSED(code)) const
{
    wxCHECK_MSG(item >= 0 && item < GetItemCount(),
        false, "invalid row index in GetSubItemRect");
    wxCHECK_MSG(subItem >= 0 && subItem < GetColumnCount(),
        false, "invalid column index in GetSubItemRect");
    rect = wxRect();
    return true;
}

bool wxListCtrl::GetItemPosition(long item, wxPoint& pos) const
{
    wxRect rect;
    if ( !GetItemRect(item, rect) )
        return false;
    pos.x = rect.x;
    pos.y = rect.y;
    return true;
}

bool wxListCtrl::SetItemPosition(long item, const wxPoint& pos)
{
    if (item < 0 || item >= (long)m_items.size())
        return false;
    if (IsReportView())
        return false;

    // In flexbox grid mode, we can't set absolute pixel positions.
    // As a best-effort approximation, we move the item to the target
    // visual index computed from pos (treating x as column hint).
    int targetIndex = pos.x;
    if (targetIndex < 0) targetIndex = 0;
    if (targetIndex >= (int)m_items.size()) targetIndex = (int)m_items.size() - 1;

    if (targetIndex == item)
        return true;

    ItemInfo it = m_items[item];
    m_items.erase(m_items.begin() + item);
    m_items.insert(m_items.begin() + targetIndex, it);

    // Rebuild DOM
    EM_ASM_({
        var grid = document.getElementById('wxListCtrl_grid_' + $0);
        if (grid) grid.innerHTML = '';
    }, GetId());

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        CreateItemDOM((long)i);
        SyncItemRow((long)i);
        UpdateItemStateClass((long)i);
    }

    return true;
}

int wxListCtrl::GetItemCount() const
{
    return (int)m_items.size();
}

int wxListCtrl::GetColumnCount() const
{
    return (int)m_columns.size();
}

wxSize wxListCtrl::GetItemSpacing() const
{
    return wxSize();
}

void wxListCtrl::SetItemTextColour(long item, const wxColour& col)
{
    wxUnusedVar(item);
    wxUnusedVar(col);
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
    wxUnusedVar(item);
    return wxColour();
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col)
{
    wxUnusedVar(item);
    wxUnusedVar(col);
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
    wxUnusedVar(item);
    return wxColour();
}

void wxListCtrl::SetItemFont( long item, const wxFont &f)
{
    wxUnusedVar(item);
    wxUnusedVar(f);
}

wxFont wxListCtrl::GetItemFont( long item ) const
{
    wxUnusedVar(item);
    return wxFont();
}

int wxListCtrl::GetSelectedItemCount() const
{
    int count = 0;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (m_items[i].state & wxLIST_STATE_SELECTED)
            ++count;
    }
    return count;
}

wxColour wxListCtrl::GetTextColour() const
{
    return wxColour();
}

void wxListCtrl::SetTextColour(const wxColour& col)
{
    wxUnusedVar(col);
}

long wxListCtrl::GetTopItem() const
{
    return 0;
}

bool wxListCtrl::HasCheckBoxes() const
{
    return m_hasCheckBoxes;
}

bool wxListCtrl::EnableCheckBoxes(bool enable)
{
    m_hasCheckBoxes = enable;
    return true;
}

bool wxListCtrl::IsItemChecked(long item) const
{
    if (item < 0 || item >= (long)m_items.size())
        return false;
    return m_items[item].checked;
}

void wxListCtrl::CheckItem(long item, bool check)
{
    if (item < 0 || item >= (long)m_items.size())
        return;
    m_items[item].checked = check;

    if (IsReportView())
    {
        EM_ASM_({
            var cb = document.getElementById('wxListCtrl_checkbox_' + $0 + '_' + $1);
            if (cb) cb.checked = $2 ? true : false;
        }, GetId(), (int)item, check ? 1 : 0);
    }
    else
    {
        EM_ASM_({
            var cb = document.getElementById('wxListCtrl_checkbox_' + $0 + '_' + $1);
            if (cb) cb.checked = $2 ? true : false;
        }, GetId(), (int)item, check ? 1 : 0);
    }
}

void wxListCtrl::SetSingleStyle(long WXUNUSED(style), bool WXUNUSED(add))
{
}

void wxListCtrl::SetWindowStyleFlag(long style)
{
    wxListCtrlBase::SetWindowStyleFlag(style);
}

long wxListCtrl::GetNextItem(long item, int WXUNUSED(geometry), int state) const
{
    for (long i = item + 1; i < (long)m_items.size(); ++i)
    {
        if ((m_items[i].state & state) == state)
            return i;
    }
    return -1;
}

void wxListCtrl::DoUpdateImages(int WXUNUSED(which))
{
}

void wxListCtrl::RefreshItem(long item)
{
    RefreshItems(item, item);
}

void wxListCtrl::RefreshItems(long WXUNUSED(itemFrom), long WXUNUSED(itemTo))
{
}

bool wxListCtrl::Arrange(int WXUNUSED(flag))
{
    if (IsReportView())
        return false;

    // Sort items alphabetically by label and rebuild DOM
    wxVector<std::pair<wxString, size_t>> sorted;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        wxString label = m_items[i].texts.empty() ? wxString() : m_items[i].texts[0];
        sorted.push_back(std::make_pair(label, i));
    }
    std::sort(sorted.begin(), sorted.end(),
        [](const std::pair<wxString, size_t>& a, const std::pair<wxString, size_t>& b)
        {
            return a.first.CmpNoCase(b.first) < 0;
        });

    wxVector<ItemInfo> newItems;
    for (size_t i = 0; i < sorted.size(); ++i)
        newItems.push_back(m_items[sorted[i].second]);
    m_items = newItems;

    // Rebuild DOM
    EM_ASM_({
        var grid = document.getElementById('wxListCtrl_grid_' + $0);
        if (grid) grid.innerHTML = '';
    }, GetId());

    for (size_t i = 0; i < m_items.size(); ++i)
    {
        CreateItemDOM((long)i);
        SyncItemRow((long)i);
        UpdateItemStateClass((long)i);
    }

    return true;
}

bool wxListCtrl::DeleteItem(long item)
{
    if (item < 0 || item >= (long)m_items.size())
        return false;

    RemoveItemRow(item);
    m_items.erase(m_items.begin() + item);

    // Re-sync indices for remaining rows
    for (size_t i = item; i < m_items.size(); ++i)
    {
        EM_ASM_({
            var tr = document.getElementById('wxListCtrl_row_' + $0 + '_' + $1);
            if (tr) tr.dataset.itemIdx = $1;
        }, GetId(), (int)i);
    }

    return true;
}

bool wxListCtrl::DeleteAllItems()
{
    if (IsReportView())
    {
        EM_ASM_({
            var tbody = document.getElementById('wxListCtrl_body_' + $0);
            if (tbody) tbody.innerHTML = '';
        }, GetId());
    }
    else
    {
        EM_ASM_({
            var grid = document.getElementById('wxListCtrl_grid_' + $0);
            if (grid) grid.innerHTML = '';
        }, GetId());
    }

    m_items.clear();
    return true;
}

bool wxListCtrl::DeleteColumn(int col)
{
    if (col < 0 || col >= (int)m_columns.size())
        return false;

    m_columns.erase(m_columns.begin() + col);
    m_colOrder.Clear(); // invalidate custom column order
    SyncColumnHeaders();

    // Remove corresponding cell from each row
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        if (col < (int)m_items[i].texts.size())
            m_items[i].texts.erase(m_items[i].texts.begin() + col);
        SyncItemRow(i);
    }

    return true;
}

bool wxListCtrl::DeleteAllColumns()
{
    m_columns.clear();
    SyncColumnHeaders();
    return true;
}

void wxListCtrl::ClearAll()
{
    DeleteAllColumns();
    DeleteAllItems();
}

wxTextCtrl* wxListCtrl::EditLabel(long item,
                                  wxClassInfo* WXUNUSED(textControlClass))
{
    if (item < 0 || item >= (long)m_items.size())
        return nullptr;

    if (m_editItem != -1)
        EndEditLabel(true);

    m_editItem = item;

    wxString text = GetItemText(item);
    wxCharBuffer buf = text.ToUTF8();
    int id = GetId();

    if (IsReportView())
    {
        EM_ASM_({
            var td = document.getElementById('wxListCtrl_cell_' + $0 + '_' + $1 + '_0');
            if (!td) return;
            var input = document.createElement('input');
            input.type = 'text';
            input.value = UTF8ToString($2);
            input.style.width = '100%';
            input.style.boxSizing = 'border-box';
            input.id = 'wxListCtrl_edit_' + $0 + '_' + $1;
            input.onkeydown = function(e) {
                if (e.key === 'Enter') {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_edit_end', $1, 0]);
                } else if (e.key === 'Escape') {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_edit_end', $1, 1]);
                }
            };
            input.onblur = function(e) {
                Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                    [$0, 'list_edit_end', $1, 0]);
            };
            td.innerHTML = '';
            td.appendChild(input);
            input.focus();
            input.select();
        }, id, (int)item, buf.data());
    }
    else
    {
        EM_ASM_({
            var label = document.getElementById('wxListCtrl_label_' + $0 + '_' + $1);
            if (!label) return;
            var input = document.createElement('input');
            input.type = 'text';
            input.value = UTF8ToString($2);
            input.style.width = '80px';
            input.style.fontSize = '12px';
            input.style.textAlign = 'center';
            input.id = 'wxListCtrl_edit_' + $0 + '_' + $1;
            input.onkeydown = function(e) {
                if (e.key === 'Enter') {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_edit_end', $1, 0]);
                } else if (e.key === 'Escape') {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_edit_end', $1, 1]);
                }
            };
            input.onblur = function(e) {
                Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                    [$0, 'list_edit_end', $1, 0]);
            };
            label.innerHTML = '';
            label.appendChild(input);
            input.focus();
            input.select();
        }, id, (int)item, buf.data());
    }

    wxListEvent le(wxEVT_LIST_BEGIN_LABEL_EDIT, m_windowId);
    le.SetIndex(item);
    if (!HandleWindowEvent(le))
    {
        // Vetoed
        EndEditLabel(true);
        return nullptr;
    }

    return nullptr;
}

bool wxListCtrl::EndEditLabel(bool cancel)
{
    if (m_editItem < 0 || m_editItem >= (long)m_items.size())
    {
        m_editItem = -1;
        return false;
    }

    long item = m_editItem;
    wxString newLabel;

    if (!cancel)
    {
        // Get new label from DOM input
        EM_ASM_({
            var input = document.getElementById('wxListCtrl_edit_' + $0 + '_' + $1);
            if (input) {
                var len = lengthBytesUTF8(input.value) + 1;
                var buffer = _malloc(len);
                stringToUTF8(input.value, buffer, len);
                Module._lastEditLabel = buffer;
            } else {
                Module._lastEditLabel = 0;
            }
        }, GetId(), (int)item);

        char* buf = (char*)EM_ASM_INT({ return Module._lastEditLabel || 0; });
        if (buf)
        {
            newLabel = wxString::FromUTF8(buf);
            free(buf);
        }
    }

    wxListEvent le(wxEVT_LIST_END_LABEL_EDIT, m_windowId);
    le.SetIndex(item);
    wxListItem li;
    li.SetText(newLabel);
    le.SetItem(li);
    le.SetEditCanceled(cancel);
    if (!HandleWindowEvent(le))
    {
        // Vetoed - restore original text
        cancel = true;
    }
    else if (!cancel)
    {
        SetItemText(item, newLabel);
    }

    // Restore DOM
    if (IsReportView())
    {
        EM_ASM_({
            var td = document.getElementById('wxListCtrl_cell_' + $0 + '_' + $1 + '_0');
            if (td) td.innerHTML = '';
        }, GetId(), (int)item);
    }
    else
    {
        EM_ASM_({
            var label = document.getElementById('wxListCtrl_label_' + $0 + '_' + $1);
            if (label) label.innerHTML = '';
        }, GetId(), (int)item);
    }

    SyncItemRow(item);
    m_editItem = -1;
    return true;
}

bool wxListCtrl::EnsureVisible(long WXUNUSED(item))
{
    return true;
}

long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{
    for (long i = start + 1; i < (long)m_items.size(); ++i)
    {
        wxString text = GetItemText(i);
        if (partial ? text.StartsWith(str) : text == str)
            return i;
    }
    return -1;
}

long wxListCtrl::FindItem(long start, wxUIntPtr data)
{
    for (long i = start + 1; i < (long)m_items.size(); ++i)
    {
        if (m_items[i].data == data)
            return i;
    }
    return -1;
}

long wxListCtrl::FindItem(
    long WXUNUSED(start),
    const wxPoint& WXUNUSED(pt),
    int WXUNUSED(direction)
)
{
    return -1;
}

long wxListCtrl::HitTest(
    const wxPoint& point,
    int &flags,
    long* ptrSubItem
) const
{
    wxUnusedVar(point);
    flags = 0;
    if (ptrSubItem)
        *ptrSubItem = 0;
    return -1;
}

long wxListCtrl::InsertItem(const wxListItem& info)
{
    long index = info.GetId();
    if (index < 0)
        index = (long)m_items.size();

    if (index > (long)m_items.size())
        index = (long)m_items.size();

    ItemInfo it;
    it.data = info.GetData();
    it.state = info.GetState();
    it.image = info.GetImage();
    it.checked = false;

    if (info.GetMask() & wxLIST_MASK_TEXT)
    {
        int col = info.GetColumn();
        if (col < 0) col = 0;
        if (col >= (int)it.texts.size())
            it.texts.resize(col + 1);
        it.texts[col] = info.GetText();
    }
    else
    {
        it.texts.resize(wxMax(1, GetColumnCount()));
    }

    m_items.insert(m_items.begin() + index, it);

    CreateItemDOM(index);
    SyncItemRow(index);
    UpdateItemStateClass(index);

    // Re-sync indices for rows/items after insertion
    for (size_t i = index + 1; i < m_items.size(); ++i)
    {
        if (IsReportView())
        {
            EM_ASM_({
                var tr = document.getElementById('wxListCtrl_row_' + $0 + '_' + $1);
                if (tr) tr.dataset.itemIdx = $1;
            }, GetId(), (int)i);
        }
        else
        {
            EM_ASM_({
                var item = document.getElementById('wxListCtrl_item_' + $0 + '_' + $1);
                if (item) item.dataset.itemIdx = $1;
            }, GetId(), (int)i);
        }
    }

    return index;
}

void wxListCtrl::CreateItemDOM(long index)
{
    if (index < 0 || index >= (long)m_items.size())
        return;

    if (IsReportView())
    {
        int hasCheck = m_hasCheckBoxes ? 1 : 0;
        int id = GetId();
        EM_ASM_({
            var tbody = document.getElementById('wxListCtrl_body_' + $0);
            if (!tbody) return;

            var tr = document.createElement('tr');
            tr.id = 'wxListCtrl_row_' + $0 + '_' + $1;
            tr.dataset.itemIdx = $1;

            if ($3) {
                var td0 = document.createElement('td');
                td0.style.width = '24px';
                td0.style.textAlign = 'center';
                var cb = document.createElement('input');
                cb.type = 'checkbox';
                cb.id = 'wxListCtrl_checkbox_' + $0 + '_' + $1;
                cb.onclick = function(e) { e.stopPropagation(); };
                cb.onchange = function(e) {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_check', $1, cb.checked ? 1 : 0]);
                };
                td0.appendChild(cb);
                tr.appendChild(td0);
            }

            var numCols = $2;
            for (var c = 0; c < numCols; c++) {
                var td = document.createElement('td');
                td.id = 'wxListCtrl_cell_' + $0 + '_' + $1 + '_' + c;
                td.textContent = '';
                tr.appendChild(td);
            }

            if ($1 < tbody.children.length) {
                tbody.insertBefore(tr, tbody.children[$1]);
            } else {
                tbody.appendChild(tr);
            }
        }, id, (int)index, GetColumnCount(), hasCheck);
    }
    else
    {
        bool isIcon = HasFlag(wxLC_ICON);
        int hasCheck = m_hasCheckBoxes ? 1 : 0;
        int id = GetId();
        EM_ASM_({
            var grid = document.getElementById('wxListCtrl_grid_' + $0);
            if (!grid) return;

            var item = document.createElement('div');
            item.className = 'wxListCtrl-item';
            if ($2) item.classList.add('wxListCtrl-item-icon-large');
            item.id = 'wxListCtrl_item_' + $0 + '_' + $1;
            item.dataset.itemIdx = $1;

            if ($3) {
                var cb = document.createElement('input');
                cb.type = 'checkbox';
                cb.id = 'wxListCtrl_checkbox_' + $0 + '_' + $1;
                cb.style.marginBottom = '4px';
                cb.onclick = function(e) { e.stopPropagation(); };
                cb.onchange = function(e) {
                    Module.ccall('addEvent', 'number', ['number', 'string', 'number', 'number'],
                        [$0, 'list_check', $1, cb.checked ? 1 : 0]);
                };
                item.appendChild(cb);
            }

            var icon = document.createElement('div');
            icon.className = 'wxListCtrl-item-icon';
            icon.textContent = $2 ? '📄' : '📃';
            item.appendChild(icon);

            var label = document.createElement('div');
            label.className = 'wxListCtrl-item-label';
            label.id = 'wxListCtrl_label_' + $0 + '_' + $1;
            item.appendChild(label);

            if ($1 < grid.children.length) {
                grid.insertBefore(item, grid.children[$1]);
            } else {
                grid.appendChild(item);
            }
        }, id, (int)index, isIcon ? 1 : 0, hasCheck);
    }
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
    wxListItem info;
    info.SetId(index);
    info.SetMask(wxLIST_MASK_TEXT);
    info.SetText(label);
    return InsertItem(info);
}

long wxListCtrl::InsertItem(long index, int imageIndex)
{
    wxListItem info;
    info.SetId(index);
    info.SetMask(wxLIST_MASK_IMAGE);
    info.SetImage(imageIndex);
    return InsertItem(info);
}

long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
    wxListItem info;
    info.SetId(index);
    info.SetMask(wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE);
    info.SetText(label);
    info.SetImage(imageIndex);
    return InsertItem(info);
}

long wxListCtrl::DoInsertColumn(long col, const wxListItem& info)
{
    if (col < 0 || col > (long)m_columns.size())
        col = (long)m_columns.size();

    ColumnInfo c;
    c.text = info.GetText();
    c.width = info.GetWidth();
    c.format = info.GetAlign();
    c.image = info.GetImage();

    m_columns.insert(m_columns.begin() + col, c);
    m_colOrder.Clear(); // invalidate custom column order

    if (IsReportView())
    {
        // Add empty cell to each existing row
        for (size_t i = 0; i < m_items.size(); ++i)
        {
            if (col >= (int)m_items[i].texts.size())
                m_items[i].texts.resize(col + 1);
            else
                m_items[i].texts.insert(m_items[i].texts.begin() + col, wxString());

            EM_ASM_({
                var tr = document.getElementById('wxListCtrl_row_' + $0 + '_' + $1);
                if (!tr) return;
                var td = document.createElement('td');
                td.id = 'wxListCtrl_cell_' + $0 + '_' + $1 + '_' + $2;
                td.textContent = '';
                if ($2 < tr.children.length) {
                    tr.insertBefore(td, tr.children[$2]);
                } else {
                    tr.appendChild(td);
                }
            }, GetId(), (int)i, (int)col);
        }

        SyncColumnHeaders();
    }

    return col;
}

void wxListCtrl::SetItemCount(long count)
{
    long oldCount = (long)m_items.size();
    if (count > oldCount)
    {
        for (long i = oldCount; i < count; ++i)
            InsertItem(i, wxString());
    }
    else if (count < oldCount)
    {
        for (long i = oldCount - 1; i >= count; --i)
            DeleteItem(i);
    }
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    wxUnusedVar(dx);
    wxUnusedVar(dy);
    return true;
}

bool wxListCtrl::SortItems(wxListCtrlCompare WXUNUSED(fn), wxIntPtr WXUNUSED(data))
{
    return true;
}

void *wxListCtrl::GetHandle() const
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Event handling
//-----------------------------------------------------------------------------

void wxListCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id != m_windowId)
        return;

    if (event.eventType == "list_select")
    {
        long item = event.x;
        if (item >= 0 && item < (long)m_items.size())
        {
            SetItemState(item, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
                         wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);

            wxListEvent le(wxEVT_LIST_ITEM_SELECTED, m_windowId);
            le.SetIndex(item);
            HandleWindowEvent(le);
        }
    }
    else if (event.eventType == "list_check")
    {
        long item = event.x;
        bool checked = event.y != 0;
        if (item >= 0 && item < (long)m_items.size())
        {
            m_items[item].checked = checked;

            wxListEvent le(wxEVT_LIST_ITEM_CHECKED, m_windowId);
            le.SetIndex(item);
            HandleWindowEvent(le);
        }
    }
    else if (event.eventType == "list_edit_end")
    {
        long item = event.x;
        bool cancel = event.y != 0;
        if (item == m_editItem)
        {
            EndEditLabel(cancel);
        }
    }
}

//-----------------------------------------------------------------------------
// DOM synchronization helpers
//-----------------------------------------------------------------------------

void wxListCtrl::SyncColumnHeaders()
{
    if (!IsReportView())
        return;

    EM_ASM_({
        var row = document.getElementById('wxListCtrl_headrow_' + $0);
        if (!row) return;
        row.innerHTML = '';
    }, GetId());

    for (size_t vis = 0; vis < m_columns.size(); ++vis)
    {
        int logical = m_colOrder.IsEmpty() ? (int)vis : m_colOrder[vis];
        if (logical < 0 || logical >= (int)m_columns.size())
            logical = (int)vis;
        wxCharBuffer buf = m_columns[logical].text.ToUTF8();
        EM_ASM_({
            var row = document.getElementById('wxListCtrl_headrow_' + $0);
            if (!row) return;
            var th = document.createElement('th');
            th.id = 'wxListCtrl_col_' + $0 + '_' + $1;
            th.textContent = UTF8ToString($2);
            if ($3 > 0) {
                th.style.width = $3 + 'px';
            }
            row.appendChild(th);
        }, GetId(), logical, buf.data(), m_columns[logical].width);
    }
}

void wxListCtrl::SyncItemRow(long index)
{
    if (index < 0 || index >= (long)m_items.size())
        return;

    const ItemInfo& it = m_items[index];

    if (IsReportView())
    {
        int numCols = wxMax((int)m_columns.size(), 1);
        for (int vis = 0; vis < numCols; ++vis)
        {
            int logical = m_colOrder.IsEmpty() ? vis : m_colOrder[vis];
            if (logical < 0 || logical >= numCols)
                logical = vis;

            wxString text;
            if (logical < (int)it.texts.size())
                text = it.texts[logical];

            wxCharBuffer buf = text.ToUTF8();
            EM_ASM_({
                var td = document.getElementById('wxListCtrl_cell_' + $0 + '_' + $1 + '_' + $2);
                if (td) td.textContent = UTF8ToString($3);
            }, GetId(), (int)index, vis, buf.data());
        }
    }
    else
    {
        wxString text = it.texts.empty() ? wxString() : it.texts[0];
        wxCharBuffer buf = text.ToUTF8();
        EM_ASM_({
            var label = document.getElementById('wxListCtrl_label_' + $0 + '_' + $1);
            if (label) label.textContent = UTF8ToString($2);
        }, GetId(), (int)index, buf.data());
    }
}

void wxListCtrl::RemoveItemRow(long index)
{
    if (IsReportView())
    {
        EM_ASM_({
            var tr = document.getElementById('wxListCtrl_row_' + $0 + '_' + $1);
            if (tr) tr.remove();
        }, GetId(), (int)index);
    }
    else
    {
        EM_ASM_({
            var item = document.getElementById('wxListCtrl_item_' + $0 + '_' + $1);
            if (item) item.remove();
        }, GetId(), (int)index);
    }
}

void wxListCtrl::UpdateItemStateClass(long index)
{
    if (index < 0 || index >= (long)m_items.size())
        return;

    bool selected = (m_items[index].state & wxLIST_STATE_SELECTED) != 0;
    bool focused = (m_items[index].state & wxLIST_STATE_FOCUSED) != 0;

    if (IsReportView())
    {
        EM_ASM_({
            var tr = document.getElementById('wxListCtrl_row_' + $0 + '_' + $1);
            if (!tr) return;
            if ($2) {
                tr.classList.add('selected');
            } else {
                tr.classList.remove('selected');
            }
            if ($3) {
                tr.classList.add('focused');
            } else {
                tr.classList.remove('focused');
            }
        }, GetId(), (int)index, selected ? 1 : 0, focused ? 1 : 0);
    }
    else
    {
        EM_ASM_({
            var item = document.getElementById('wxListCtrl_item_' + $0 + '_' + $1);
            if (!item) return;
            if ($2) {
                item.classList.add('selected');
            } else {
                item.classList.remove('selected');
            }
            if ($3) {
                item.classList.add('focused');
            } else {
                item.classList.remove('focused');
            }
        }, GetId(), (int)index, selected ? 1 : 0, focused ? 1 : 0);
    }
}

int wxListCtrl::GetCountPerPage() const
{
    return GetItemCount();
}

wxRect wxListCtrl::GetViewRect() const
{
    return wxRect();
}

wxTextCtrl* wxListCtrl::GetEditControl() const
{
    return nullptr;
}
