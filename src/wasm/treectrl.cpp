/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/treectrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/settings.h"
#include "wx/sharedptr.h"
#include "wx/withimages.h"

#include <emscripten.h>

wxTreeCtrl::wxTreeCtrl()
{
    m_nextId = 1;
}

wxTreeCtrl::wxTreeCtrl(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Create(parent, id, pos, size, style, validator, name);
}

bool wxTreeCtrl::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_nextId = 1;
    m_rootItem = wxTreeItemId();
    m_selectedItem = wxTreeItemId();
    m_focusedItem = wxTreeItemId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var ul = document.createElement('ul');
        ul.className = 'wxTreeCtrl';
        ul.style.listStyle = 'none';
        ul.style.paddingLeft = '0';
        ul.style.overflow = 'auto';
        ul.style.width = '100%';
        ul.style.height = '100%';
        ul.style.margin = '0';
        ul.style.boxSizing = 'border-box';

        container.appendChild(ul);

        ul.addEventListener('click', function(e) {
            var toggle = e.target.closest('.wxTreeToggle');
            var li = e.target.closest('li[data-item-id]');
            if (!li) return;

            var itemId = parseInt(li.dataset.itemId);
            if (isNaN(itemId)) return;

            if (toggle) {
                e.stopPropagation();
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'tree_toggle', itemId, 0]);
                }
            } else {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'tree_click', itemId, 0]);
                }
            }
        });
    }, GetId());

    return true;
}

wxTreeCtrl::~wxTreeCtrl()
{
    DeleteAllItems();
}

unsigned int wxTreeCtrl::GetCount() const
{
    return m_items.size();
}

unsigned int wxTreeCtrl::GetIndent() const
{
    return 20;
}

void wxTreeCtrl::SetIndent(unsigned int indent)
{
    wxUnusedVar(indent);
}

wxString wxTreeCtrl::GetItemText(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxString(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        return it->second.text;

    return wxString();
}

int wxTreeCtrl::GetItemImage(const wxTreeItemId& item, wxTreeItemIcon which ) const
{
    wxUnusedVar(item);
    wxUnusedVar(which);
    return -1;
}

void wxTreeCtrl::DoUpdateIconsSize(wxImageList *imageList)
{
    int width, height;
    imageList->GetSize(0, width, height);
}

void wxTreeCtrl::OnImagesChanged()
{
    if ( HasImages() )
    {
        DoUpdateIconsSize(GetUpdatedImageListFor(this));
    }
}

void wxTreeCtrl::SetImageList(wxImageList *imageList)
{
    m_imagesState.SetImageList(imageList);
}

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imagesState.SetImageList(imageList);
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        return it->second.data;

    return nullptr;
}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxUnusedVar(item);
    return wxNullColour;
}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxUnusedVar(item);
    return wxNullColour;
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxUnusedVar(item);
    return wxNullFont;
}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        it->second.text = text;

    wxCharBuffer buf = text.ToUTF8();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return;
        var label = li.querySelector('.wxTreeLabel');
        if (label) label.textContent = UTF8ToString($2);
    }, GetId(), (int)(wxIntPtr)item.GetID(), buf.data());
}

void wxTreeCtrl::SetItemImage(
    const wxTreeItemId& item,
    int image,
    wxTreeItemIcon which
)
{
    wxUnusedVar(item);
    wxUnusedVar(image);
    wxUnusedVar(which);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
    {
        if (it->second.data)
            delete it->second.data;
        it->second.data = data;
    }

    if (data)
        data->SetId(item);
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if (has)
    {
        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var li = container.querySelector('li[data-item-id="' + $1 + '"]');
            if (!li) return;

            var childUl = li.querySelector(':scope > ul');
            if (!childUl) {
                childUl = document.createElement('ul');
                childUl.style.listStyle = 'none';
                childUl.style.paddingLeft = '20px';
                childUl.style.margin = '0';
                li.appendChild(childUl);
            }

            if (!li.querySelector(':scope > .wxTreeToggle')) {
                var toggle = document.createElement('span');
                toggle.className = 'wxTreeToggle';
                toggle.textContent = '\u25BC'; // ▼
                toggle.style.cursor = 'pointer';
                toggle.style.marginRight = '4px';
                toggle.style.userSelect = 'none';
                li.insertBefore(toggle, li.firstChild);
            }
        }, GetId(), (int)(wxIntPtr)item.GetID());
    }
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxUnusedVar(item);
    wxUnusedVar(bold);
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item, bool highlight)
{
    wxUnusedVar(item);
    wxUnusedVar(highlight);
}

void wxTreeCtrl::SetItemTextColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxUnusedVar(item);
    wxUnusedVar(col);
}

void wxTreeCtrl::SetItemBackgroundColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxUnusedVar(item);
    wxUnusedVar(col);
}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxUnusedVar(item);
    wxUnusedVar(font);
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    wxTreeItemId parent = GetItemParent(item);
    while (parent.IsOk())
    {
        if (!IsExpanded(parent))
            return false;
        parent = GetItemParent(parent);
    }

    return true;
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return 0;
        var ul = li.querySelector(':scope > ul');
        if (!ul) return 0;
        return ul.children.length > 0 ? 1 : 0;
    }, GetId(), (int)(wxIntPtr)item.GetID()) != 0;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        return it->second.expanded;

    return false;
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    return m_selectedItem == item;
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxUnusedVar(item);
    return false;
}

size_t wxTreeCtrl::GetChildrenCount(
    const wxTreeItemId& item,
    bool recursively
) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");

    wxTreeItemIdValue cookie = nullptr;
    wxTreeItemId child = GetFirstChild(item, cookie);
    size_t count = 0;

    while (child.IsOk())
    {
        count++;
        if (recursively)
            count += GetChildrenCount(child, true);
        child = GetNextChild(item, cookie);
    }

    return count;
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    return m_rootItem;
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    return m_selectedItem;
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    selections.Clear();
    if (m_selectedItem.IsOk())
        selections.Add(m_selectedItem);
    return selections.size();
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    m_focusedItem = item;
}

void wxTreeCtrl::ClearFocusedItem()
{
    m_focusedItem = wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return m_focusedItem;
}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        return it->second.parent;

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetFirstChild(
    const wxTreeItemId& item,
    wxTreeItemIdValue& cookie
) const
{
    cookie = nullptr;
    return GetNextChild(item, cookie);
}

wxTreeItemId wxTreeCtrl::GetNextChild(
    const wxTreeItemId& item,
    wxTreeItemIdValue& cookie
) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    int currentIndex = static_cast<int>(reinterpret_cast<wxIntPtr>(cookie));

    int childId = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return 0;
        var ul = li.querySelector(':scope > ul');
        if (!ul) return 0;

        var idx = $2;
        var liCount = 0;
        for (var i = 0; i < ul.children.length; i++) {
            var child = ul.children[i];
            if (child.tagName === 'LI' && child.dataset.itemId) {
                if (liCount === idx) {
                    return parseInt(child.dataset.itemId);
                }
                liCount++;
            }
        }
        return 0;
    }, GetId(), (int)(wxIntPtr)item.GetID(), currentIndex);

    if (childId == 0)
        return wxTreeItemId();

    cookie = reinterpret_cast<wxTreeItemIdValue>(static_cast<wxIntPtr>(currentIndex + 1));
    return wxTreeItemId(IntToItemIdValue(childId));
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    int childId = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return 0;
        var ul = li.querySelector(':scope > ul');
        if (!ul) return 0;

        for (var i = ul.children.length - 1; i >= 0; i--) {
            var child = ul.children[i];
            if (child.tagName === 'LI' && child.dataset.itemId) {
                return parseInt(child.dataset.itemId);
            }
        }
        return 0;
    }, GetId(), (int)(wxIntPtr)item.GetID());

    if (childId == 0)
        return wxTreeItemId();

    return wxTreeItemId(IntToItemIdValue(childId));
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    int siblingId = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li || !li.parentNode) return 0;

        var next = li.nextElementSibling;
        while (next) {
            if (next.tagName === 'LI' && next.dataset.itemId) {
                return parseInt(next.dataset.itemId);
            }
            next = next.nextElementSibling;
        }
        return 0;
    }, GetId(), (int)(wxIntPtr)item.GetID());

    if (siblingId == 0)
        return wxTreeItemId();

    return wxTreeItemId(IntToItemIdValue(siblingId));
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    int siblingId = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li || !li.parentNode) return 0;

        var prev = li.previousElementSibling;
        while (prev) {
            if (prev.tagName === 'LI' && prev.dataset.itemId) {
                return parseInt(prev.dataset.itemId);
            }
            prev = prev.previousElementSibling;
        }
        return 0;
    }, GetId(), (int)(wxIntPtr)item.GetID());

    if (siblingId == 0)
        return wxTreeItemId();

    return wxTreeItemId(IntToItemIdValue(siblingId));
}

wxTreeItemId wxTreeCtrl::GetFirstVisibleItem() const
{
   wxTreeItemId itemid = GetRootItem();
    if ( !itemid.IsOk() )
        return itemid;

    do
    {
        if ( IsVisible(itemid) )
            return itemid;
        itemid = GetNext(itemid);
    } while ( itemid.IsOk() );

    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetNextVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
    wxASSERT_MSG(IsVisible(item), "this item itself should be visible");

    wxTreeItemId id = item;
    if ( id.IsOk() )
    {
        while ( id = GetNext(id), id.IsOk() )
        {
            if ( IsVisible(id) )
                return id;
        }
    }
    return wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
    wxASSERT_MSG(IsVisible(item), "this item itself should be visible");

    // find out the starting point
    wxTreeItemId prevItem = GetPrevSibling(item);
    if ( !prevItem.IsOk() )
    {
        prevItem = GetItemParent(item);
    }

    // find the first visible item after it
    while ( prevItem.IsOk() && !IsVisible(prevItem) )
    {
        prevItem = GetNext(prevItem);
        if ( !prevItem.IsOk() || prevItem == item )
        {
            // there are no visible items before item
            return wxTreeItemId();
        }
    }

    // from there we must be able to navigate until this item
    while ( prevItem.IsOk() )
    {
        const wxTreeItemId nextItem = GetNextVisible(prevItem);
        if ( !nextItem.IsOk() || nextItem == item )
            break;

        prevItem = nextItem;
    }

    return prevItem;
}

wxTreeItemId wxTreeCtrl::AddRoot(const wxString& text,
                             int image, int selImage,
                             wxTreeItemData *data)
{
    if (m_rootItem.IsOk())
        DeleteAllItems();

    wxTreeItemIdValue idValue = MakeItemIdValue();
    wxTreeItemId itemId(idValue);

    wxTreeItemDataInternal itemData;
    itemData.id = itemId;
    itemData.text = text;
    itemData.parent = wxTreeItemId();
    itemData.data = data;
    itemData.expanded = true;

    m_items[idValue] = itemData;
    m_rootItem = itemId;

    if (data)
        data->SetId(itemId);

    wxCharBuffer buf = text.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var ul = container.querySelector('.wxTreeCtrl');
        if (!ul) return;

        var li = document.createElement('li');
        li.dataset.itemId = $1;
        li.style.cursor = 'default';

        var label = document.createElement('span');
        label.className = 'wxTreeLabel';
        label.textContent = UTF8ToString($2);
        label.style.userSelect = 'none';

        li.appendChild(label);
        ul.appendChild(li);
    }, GetId(), (int)(wxIntPtr)idValue, buf.data());

    return itemId;
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if (item == m_rootItem)
        m_rootItem = wxTreeItemId();

    if (item == m_selectedItem)
        m_selectedItem = wxTreeItemId();

    if (item == m_focusedItem)
        m_focusedItem = wxTreeItemId();

    SendDeleteEvent(item);
    DeleteItemInternal(item);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (li && li.parentNode) {
            li.parentNode.removeChild(li);
        }
    }, GetId(), (int)(wxIntPtr)item.GetID());
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    wxTreeItemIdValue cookie = nullptr;
    wxTreeItemId child = GetFirstChild(item, cookie);
    while (child.IsOk())
    {
        wxTreeItemId next = GetNextChild(item, cookie);
        Delete(child);
        child = next;
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return;
        var toggle = li.querySelector(':scope > .wxTreeToggle');
        if (toggle) li.removeChild(toggle);
        var ul = li.querySelector(':scope > ul');
        if (ul) li.removeChild(ul);
    }, GetId(), (int)(wxIntPtr)item.GetID());
}

void wxTreeCtrl::DeleteAllItems()
{
    for (auto& it : m_items)
    {
        if (it.second.data)
            delete it.second.data;
    }
    m_items.clear();
    m_rootItem = wxTreeItemId();
    m_selectedItem = wxTreeItemId();
    m_focusedItem = wxTreeItemId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var ul = container.querySelector('.wxTreeCtrl');
        if (ul) {
            while (ul.firstChild) {
                ul.removeChild(ul.firstChild);
            }
        }
    }, GetId());
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        it->second.expanded = true;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return;
        var ul = li.querySelector(':scope > ul');
        if (ul) ul.style.display = 'block';
        var toggle = li.querySelector(':scope > .wxTreeToggle');
        if (toggle) toggle.textContent = '\u25BC'; // ▼
    }, GetId(), (int)(wxIntPtr)item.GetID());

    wxTreeEvent event(wxEVT_TREE_ITEM_EXPANDED, m_windowId);
    event.SetItem(item);
    HandleWindowEvent(event);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
        it->second.expanded = false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var li = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!li) return;
        var ul = li.querySelector(':scope > ul');
        if (ul) ul.style.display = 'none';
        var toggle = li.querySelector(':scope > .wxTreeToggle');
        if (toggle) toggle.textContent = '\u25B6'; // ▶
    }, GetId(), (int)(wxIntPtr)item.GetID());

    wxTreeEvent event(wxEVT_TREE_ITEM_COLLAPSED, m_windowId);
    event.SetItem(item);
    HandleWindowEvent(event);
}

void wxTreeCtrl::CollapseAndReset(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    Collapse(item);
    DeleteChildren(item);
}

void wxTreeCtrl::Toggle(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if (IsExpanded(item))
        Collapse(item);
    else
        Expand(item);
}

void wxTreeCtrl::Unselect()
{
    if (m_selectedItem.IsOk())
    {
        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var li = container.querySelector('li[data-item-id="' + $1 + '"]');
            if (li) li.classList.remove('selected');
        }, GetId(), (int)(wxIntPtr)m_selectedItem.GetID());
        m_selectedItem = wxTreeItemId();
    }
}

void wxTreeCtrl::UnselectAll()
{
    Unselect();
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if (!HasFlag(wxTR_MULTIPLE))
    {
        if (m_selectedItem.IsOk() && m_selectedItem != item)
        {
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var li = container.querySelector('li[data-item-id="' + $1 + '"]');
                if (li) li.classList.remove('selected');
            }, GetId(), (int)(wxIntPtr)m_selectedItem.GetID());
        }

        if (select)
        {
            m_selectedItem = item;
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var li = container.querySelector('li[data-item-id="' + $1 + '"]');
                if (li) li.classList.add('selected');
            }, GetId(), (int)(wxIntPtr)item.GetID());
        }
        else if (m_selectedItem == item)
        {
            m_selectedItem = wxTreeItemId();
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var li = container.querySelector('li[data-item-id="' + $1 + '"]');
                if (li) li.classList.remove('selected');
            }, GetId(), (int)(wxIntPtr)item.GetID());
        }
    }
    else
    {
        if (select)
        {
            m_selectedItem = item;
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var li = container.querySelector('li[data-item-id="' + $1 + '"]');
                if (li) li.classList.add('selected');
            }, GetId(), (int)(wxIntPtr)item.GetID());
        }
        else
        {
            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var li = container.querySelector('li[data-item-id="' + $1 + '"]');
                if (li) li.classList.remove('selected');
            }, GetId(), (int)(wxIntPtr)item.GetID());
            if (m_selectedItem == item)
                m_selectedItem = wxTreeItemId();
        }
    }
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxUnusedVar(parent);
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    wxTreeItemId parent = GetItemParent(item);
    while (parent.IsOk())
    {
        Expand(parent);
        parent = GetItemParent(parent);
    }
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    EnsureVisible(item);
}

wxTextCtrl *wxTreeCtrl::EditLabel(
    const wxTreeItemId& item,
    wxClassInfo* WXUNUSED(textCtrlClass)
)
{
    wxUnusedVar(item);
    return nullptr;
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return nullptr;
}

void wxTreeCtrl::EndEditLabel(
    const wxTreeItemId& item,
    bool WXUNUSED(discardChanges)
)
{
    wxUnusedVar(item);
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxUnusedVar(item);
}

bool wxTreeCtrl::GetBoundingRect(
    const wxTreeItemId& item,
    wxRect& rect,
    bool WXUNUSED(textOnly)
) const
{
    wxUnusedVar(item);
    rect = wxRect();
    return false;
}

void wxTreeCtrl::SetWindowStyleFlag(long styles)
{
    wxControl::SetWindowStyleFlag(styles);
}

void *wxTreeCtrl::GetHandle() const
{
    return nullptr;
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxUnusedVar(item);
    return wxTREE_ITEMSTATE_NONE;
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxUnusedVar(item);
    wxUnusedVar(state);
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image, int selImage,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");

    wxTreeItemIdValue idValue = MakeItemIdValue();
    wxTreeItemId itemId(idValue);

    wxTreeItemDataInternal itemData;
    itemData.id = itemId;
    itemData.text = text;
    itemData.parent = parent;
    itemData.data = data;
    itemData.expanded = true;

    m_items[idValue] = itemData;

    if (data)
        data->SetId(itemId);

    wxCharBuffer buf = text.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var parentLi = container.querySelector('li[data-item-id="' + $1 + '"]');
        if (!parentLi) return;

        var childUl = parentLi.querySelector(':scope > ul');
        if (!childUl) {
            childUl = document.createElement('ul');
            childUl.style.listStyle = 'none';
            childUl.style.paddingLeft = '20px';
            childUl.style.margin = '0';
            parentLi.appendChild(childUl);

            if (!parentLi.querySelector(':scope > .wxTreeToggle')) {
                var toggle = document.createElement('span');
                toggle.className = 'wxTreeToggle';
                toggle.textContent = '\u25BC'; // ▼
                toggle.style.cursor = 'pointer';
                toggle.style.marginRight = '4px';
                toggle.style.userSelect = 'none';
                parentLi.insertBefore(toggle, parentLi.firstChild);
            }
        }

        var li = document.createElement('li');
        li.dataset.itemId = $2;
        li.style.cursor = 'default';

        var label = document.createElement('span');
        label.className = 'wxTreeLabel';
        label.textContent = UTF8ToString($3);
        label.style.userSelect = 'none';

        li.appendChild(label);

        if ($4 >= 0 && $4 < childUl.children.length) {
            childUl.insertBefore(li, childUl.children[$4]);
        } else {
            childUl.appendChild(li);
        }
    }, GetId(), (int)(wxIntPtr)parent.GetID(), (int)(wxIntPtr)idValue, buf.data(), (int)pos);

    return itemId;
}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& idPrevious,
                                   const wxString& text,
                                   int image, int selImage,
                                   wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");
    wxCHECK_MSG(idPrevious.IsOk(), wxTreeItemId(), "invalid tree item");

    wxTreeItemIdValue cookie = nullptr;
    wxTreeItemId child = GetFirstChild(parent, cookie);
    size_t idx = 0;
    while (child.IsOk())
    {
        if (child == idPrevious)
            return DoInsertItem(parent, idx + 1, text, image, selImage, data);
        child = GetNextChild(parent, cookie);
        idx++;
    }

    return DoInsertItem(parent, (size_t)-1, text, image, selImage, data);
}

wxTreeItemId wxTreeCtrl::DoTreeHitTest(const wxPoint& point, int& flags) const
{
    int w, h;
    GetSize(&w, &h);
    flags = 0;
    if ( point.x < 0 )
        flags |= wxTREE_HITTEST_TOLEFT;
    else if ( point.x > w )
        flags |= wxTREE_HITTEST_TORIGHT;

    if ( point.y < 0 )
        flags |= wxTREE_HITTEST_ABOVE;
    else if ( point.y > h )
        flags |= wxTREE_HITTEST_BELOW;

    if ( flags != 0 )
        return wxTreeItemId();

    return wxTreeItemId();
}

void wxTreeCtrl::SendDeleteEvent(const wxTreeItemId &item)
{
    wxTreeEvent event(wxEVT_TREE_DELETE_ITEM, GetId());
    event.SetItem(item);
    HandleWindowEvent(event);
}

wxTreeItemId wxTreeCtrl::GetNext(const wxTreeItemId &item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    wxTreeItemIdValue cookie = nullptr;
    wxTreeItemId child = GetFirstChild(item, cookie);
    if (child.IsOk())
        return child;

    wxTreeItemId next = GetNextSibling(item);
    if (next.IsOk())
        return next;

    wxTreeItemId parent = GetItemParent(item);
    while (parent.IsOk())
    {
        next = GetNextSibling(parent);
        if (next.IsOk())
            return next;
        parent = GetItemParent(parent);
    }

    return wxTreeItemId();
}

void wxTreeCtrl::DeleteItemInternal(const wxTreeItemId &item)
{
    wxTreeItemIdValue cookie = nullptr;
    wxTreeItemId child = GetFirstChild(item, cookie);
    while (child.IsOk())
    {
        wxTreeItemId next = GetNextChild(item, cookie);
        DeleteItemInternal(child);
        child = next;
    }

    auto it = m_items.find(item.GetID());
    if (it != m_items.end())
    {
        if (it->second.data)
            delete it->second.data;
        m_items.erase(it);
    }
}

wxTreeItemIdValue wxTreeCtrl::MakeItemIdValue()
{
    return reinterpret_cast<wxTreeItemIdValue>(static_cast<wxIntPtr>(m_nextId++));
}

wxTreeItemIdValue wxTreeCtrl::IntToItemIdValue(int id) const
{
    return reinterpret_cast<wxTreeItemIdValue>(static_cast<wxIntPtr>(id));
}

void wxTreeCtrl::SetStateImages(const wxVector<wxBitmapBundle>& images)
{
    wxUnusedVar(images);
}

void wxTreeCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id != m_windowId)
        return;

    if (event.eventType == "tree_click")
    {
        wxTreeItemId item(IntToItemIdValue(event.x));
        if (item.IsOk() && m_items.count(item.GetID()))
        {
            SelectItem(item);
            wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED, m_windowId);
            evt.SetItem(item);
            HandleWindowEvent(evt);
        }
    }
    else if (event.eventType == "tree_toggle")
    {
        wxTreeItemId item(IntToItemIdValue(event.x));
        if (item.IsOk() && m_items.count(item.GetID()))
        {
            Toggle(item);
        }
    }
}
