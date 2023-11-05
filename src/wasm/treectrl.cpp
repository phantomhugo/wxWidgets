/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/treectrl.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/treectrl.h"
#include "wx/imaglist.h"
#include "wx/settings.h"
#include "wx/sharedptr.h"
#include "wx/withimages.h"

wxTreeCtrl::wxTreeCtrl()
{
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

    SetWindowStyleFlag(style);
}

wxTreeCtrl::~wxTreeCtrl()
{
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

}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullFont, "invalid tree item");

}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemImage(
    const wxTreeItemId& item,
    int image,
    wxTreeItemIcon which
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item, bool highlight)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemTextColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemBackgroundColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

}

size_t wxTreeCtrl::GetChildrenCount(
    const wxTreeItemId& item,
    bool recursively
) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");

}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

void wxTreeCtrl::ClearFocusedItem()
{
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{

}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
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

    wxIntPtr currentIndex = reinterpret_cast<wxIntPtr>(cookie);


    wxTreeItemId childItem;

    return childItem;
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");
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
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::DeleteAllItems()
{

}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

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

}

void wxTreeCtrl::Unselect()
{
}

void wxTreeCtrl::UnselectAll()
{
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( !HasFlag(wxTR_MULTIPLE) )
    {

    }

}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxCHECK_RET(parent.IsOk(), "invalid tree item");

}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

wxTextCtrl *wxTreeCtrl::EditLabel(
    const wxTreeItemId& item,
    wxClassInfo* WXUNUSED(textCtrlClass)
)
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
}

void wxTreeCtrl::EndEditLabel(
    const wxTreeItemId& item,
    bool WXUNUSED(discardChanges)
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

}

bool wxTreeCtrl::GetBoundingRect(
    const wxTreeItemId& item,
    wxRect& rect,
    bool WXUNUSED(textOnly)
) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    return true;
}

void wxTreeCtrl::SetWindowStyleFlag(long styles)
{
    wxControl::SetWindowStyleFlag(styles);

}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTREE_ITEMSTATE_NONE, "invalid tree item");
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image, int selImage,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");

}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& idPrevious,
                                   const wxString& text,
                                   int image, int selImage,
                                   wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");
    wxCHECK_MSG(idPrevious.IsOk(), wxTreeItemId(), "invalid tree item");

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


}
