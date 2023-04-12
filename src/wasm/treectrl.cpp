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

wxTreeCtrl::wxTreeCtrl() :
    m_qtTreeWidget(nullptr)
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

void wxTreeCtrl::SetStateImageList(wxImageList *imageList)
{
    m_imagesState.SetImageList(imageList);
    m_qtTreeWidget->update();
}

wxTreeItemData *wxTreeCtrl::GetItemData(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");

}

wxColour wxTreeCtrl::GetItemTextColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->foreground(0).color());
}

wxColour wxTreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullColour, "invalid tree item");

    const QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertColour(qTreeItem->background(0).color());
}

wxFont wxTreeCtrl::GetItemFont(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxNullFont, "invalid tree item");

}

void wxTreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem* qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setText(0, wxQtConvertString(text));
}

void wxTreeCtrl::SetItemImage(
    const wxTreeItemId& item,
    int image,
    wxTreeItemIcon which
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    m_qtTreeWidget->SetItemImage(wxQtConvertTreeItem(item), image, which);
}

void wxTreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData *data)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( data != nullptr )
        data->SetId(item);

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const TreeItemDataQt treeItemData(data);
    qTreeItem->setData(0, Qt::UserRole, QVariant::fromValue(treeItemData));
}

void wxTreeCtrl::SetItemHasChildren(const wxTreeItemId& item, bool has)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setChildIndicatorPolicy(has
        ? QTreeWidgetItem::ShowIndicator
        : QTreeWidgetItem::DontShowIndicatorWhenChildless);
}

void wxTreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QFont font = qTreeItem->font(0);
    font.setBold(bold);
    qTreeItem->setFont(0, font);
}

void wxTreeCtrl::SetItemDropHighlight(const wxTreeItemId& item, bool highlight)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    wxColour fg, bg;

    if ( highlight )
    {
        bg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        fg = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
    }
    else
    {
        bg = GetBackgroundColour();
        fg = GetForegroundColour();
    }

    qTreeItem->setBackground(0, wxQtConvertColour(bg));
    qTreeItem->setForeground(0, wxQtConvertColour(fg));
}

void wxTreeCtrl::SetItemTextColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setForeground(0, wxQtConvertColour(col));
}

void wxTreeCtrl::SetItemBackgroundColour(
    const wxTreeItemId& item,
    const wxColour& col
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setBackground(0, wxQtConvertColour(col));
}

void wxTreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setFont(0, font.GetHandle());
}

bool wxTreeCtrl::IsVisible(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QRect itemRect = m_qtTreeWidget->visualItemRect(qTreeItem);
    const QRect clientRect = m_qtTreeWidget->rect();
    return itemRect.isValid() && clientRect.contains(itemRect);
}

bool wxTreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->childCount() > 0;
}

bool wxTreeCtrl::IsExpanded(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isExpanded();
}

bool wxTreeCtrl::IsSelected(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return qTreeItem->isSelected();
}

bool wxTreeCtrl::IsBold(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QFont font = qTreeItem->font(0);
    return font.bold();
}

size_t wxTreeCtrl::GetChildrenCount(
    const wxTreeItemId& item,
    bool recursively
) const
{
    wxCHECK_MSG(item.IsOk(), 0, "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( recursively )
        return CountChildren(qTreeItem);

    return qTreeItem->childCount();
}

wxTreeItemId wxTreeCtrl::GetRootItem() const
{
    const QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    return wxQtConvertTreeItem(root->child(0));
}

wxTreeItemId wxTreeCtrl::GetSelection() const
{
    QList<QTreeWidgetItem*> selections = m_qtTreeWidget->selectedItems();
    return selections.isEmpty()
        ? wxTreeItemId()
        : wxQtConvertTreeItem(selections[0]);
}

size_t wxTreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
{
    QList<QTreeWidgetItem*> qtSelections = m_qtTreeWidget->selectedItems();

    const size_t numberOfSelections = qtSelections.size();
    selections.reserve(numberOfSelections);
    for ( size_t i = 0; i < numberOfSelections; ++i )
    {
        QTreeWidgetItem *item = qtSelections[i];
        selections.push_back(wxQtConvertTreeItem(item));
    }

    return numberOfSelections;
}

void wxTreeCtrl::SetFocusedItem(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    m_qtTreeWidget->setCurrentItem(wxQtConvertTreeItem(item), 0);
}

void wxTreeCtrl::ClearFocusedItem()
{
    m_qtTreeWidget->setCurrentItem(nullptr);
}

wxTreeItemId wxTreeCtrl::GetFocusedItem() const
{
    return wxQtConvertTreeItem(m_qtTreeWidget->currentItem());
}

wxTreeItemId wxTreeCtrl::GetItemParent(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    return wxQtConvertTreeItem(qTreeItem->parent());
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

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    wxTreeItemId childItem;
    if ( currentIndex < qTreeItem->childCount() )
    {
        childItem = wxQtConvertTreeItem(qTreeItem->child(currentIndex++));
        cookie = reinterpret_cast<wxTreeItemIdValue>(currentIndex);
    }

    return childItem;
}

wxTreeItemId wxTreeCtrl::GetLastChild(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const int childCount = qTreeItem->childCount();
    return childCount == 0
        ? wxTreeItemId()
        : wxQtConvertTreeItem(qTreeItem->child(childCount - 1));
}

wxTreeItemId wxTreeCtrl::GetNextSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != nullptr )
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        ++index;
        return index < parent->childCount()
            ? wxQtConvertTreeItem(parent->child(index))
            : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    ++index;
    return index < m_qtTreeWidget->topLevelItemCount()
        ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index))
        : wxTreeItemId();
}

wxTreeItemId wxTreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    if ( parent != nullptr )
    {
        int index = parent->indexOfChild(qTreeItem);
        wxASSERT(index != -1);

        --index;
        return index >= 0
            ? wxQtConvertTreeItem(parent->child(index))
            : wxTreeItemId();
    }

    int index = m_qtTreeWidget->indexOfTopLevelItem(qTreeItem);
    wxASSERT(index != -1);

    --index;
    return index >= 0
        ? wxQtConvertTreeItem(m_qtTreeWidget->topLevelItem(index))
        : wxTreeItemId();
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
    QTreeWidgetItem *root = m_qtTreeWidget->invisibleRootItem();
    wxTreeItemId newItem = DoInsertItem(
        wxQtConvertTreeItem(root),
        0,
        text,
        image,
        selImage,
        data
    );

    m_qtTreeWidget->setCurrentItem(nullptr);

    if ( (GetWindowStyleFlag() & wxTR_HIDE_ROOT) != 0 )
        m_qtTreeWidget->setRootIndex(m_qtTreeWidget->model()->index(0, 0));
    else
        m_qtTreeWidget->setRootIndex(QModelIndex());

    return newItem;
}

void wxTreeCtrl::Delete(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    DeleteChildren(qTreeItem);

    if ( parent != nullptr )
    {
        parent->removeChild(qTreeItem);
    }
    else
    {
        m_qtTreeWidget->removeItemWidget(qTreeItem, 0);
    }

    SendDeleteEvent(item);

    delete qTreeItem;
}

void wxTreeCtrl::DeleteChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    wxQtEnsureSignalsBlocked ensureSignalsBlock(m_qtTreeWidget);
    while ( qTreeItem->childCount() > 0 )
    {
        QTreeWidgetItem *child = qTreeItem->child(0);
        DeleteChildren(wxQtConvertTreeItem(child));
        qTreeItem->removeChild(child);

        SendDeleteEvent(wxQtConvertTreeItem(child));

        delete child;
    }
}

void wxTreeCtrl::DeleteAllItems()
{
    DeleteChildren(wxQtConvertTreeItem(m_qtTreeWidget->invisibleRootItem()));
}

void wxTreeCtrl::Expand(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setExpanded(true);
}

void wxTreeCtrl::Collapse(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setExpanded(false);
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

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->setExpanded(!qTreeItem->isExpanded());
}

void wxTreeCtrl::Unselect()
{
    QTreeWidgetItem *current = m_qtTreeWidget->currentItem();
    if ( current != nullptr )
        m_qtTreeWidget->select(current, QItemSelectionModel::Deselect);
}

void wxTreeCtrl::UnselectAll()
{
    m_qtTreeWidget->selectionModel()->clearSelection();
}

void wxTreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    if ( !HasFlag(wxTR_MULTIPLE) )
    {
        m_qtTreeWidget->clearSelection();
    }

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( qTreeItem )
    {
        m_qtTreeWidget->select(qTreeItem, select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
        if ( select && m_qtTreeWidget->selectionMode() == QTreeWidget::SingleSelection )
        {
            m_qtTreeWidget->setCurrentItem(qTreeItem);
        }
    }
}

void wxTreeCtrl::SelectChildren(const wxTreeItemId& parent)
{
    wxCHECK_RET(parent.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int childCount = qTreeItem->childCount();

    for ( int i = 0; i < childCount; ++i )
    {
        m_qtTreeWidget->select(qTreeItem->child(i), QItemSelectionModel::Select);
    }
}

void wxTreeCtrl::EnsureVisible(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    QTreeWidgetItem *parent = qTreeItem->parent();

    while ( parent != nullptr )
    {
        parent->setExpanded(true);
        parent = parent->parent();
    }

    ScrollTo(item);
}

void wxTreeCtrl::ScrollTo(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    m_qtTreeWidget->scrollToItem(qTreeItem);
}

wxTextCtrl *wxTreeCtrl::EditLabel(
    const wxTreeItemId& item,
    wxClassInfo* WXUNUSED(textCtrlClass)
)
{
    wxCHECK_MSG(item.IsOk(), nullptr, "invalid tree item");
    m_qtTreeWidget->editItem(wxQtConvertTreeItem(item));
    return m_qtTreeWidget->GetEditControl();
}

wxTextCtrl *wxTreeCtrl::GetEditControl() const
{
    return m_qtTreeWidget->GetEditControl();
}

void wxTreeCtrl::EndEditLabel(
    const wxTreeItemId& item,
    bool WXUNUSED(discardChanges)
)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    m_qtTreeWidget->closePersistentEditor(qTreeItem);
}

void wxTreeCtrl::SortChildren(const wxTreeItemId& item)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    qTreeItem->sortChildren(0, Qt::AscendingOrder);
}

bool wxTreeCtrl::GetBoundingRect(
    const wxTreeItemId& item,
    wxRect& rect,
    bool WXUNUSED(textOnly)
) const
{
    wxCHECK_MSG(item.IsOk(), false, "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);
    const QRect visualRect = m_qtTreeWidget->visualItemRect(qTreeItem);
    if ( !visualRect.isValid() )
        return false;

    rect = wxQtConvertRect(visualRect);
    return true;
}

void wxTreeCtrl::SetWindowStyleFlag(long styles)
{
    wxControl::SetWindowStyleFlag(styles);

    m_qtTreeWidget->setSelectionMode(
        styles & wxTR_MULTIPLE
            ? QTreeWidget::ExtendedSelection
            : QTreeWidget::SingleSelection
    );
}

int wxTreeCtrl::DoGetItemState(const wxTreeItemId& item) const
{
    wxCHECK_MSG(item.IsOk(), wxTREE_ITEMSTATE_NONE, "invalid tree item");
    return m_qtTreeWidget->GetItemState(wxQtConvertTreeItem(item));
}

void wxTreeCtrl::DoSetItemState(const wxTreeItemId& item, int state)
{
    wxCHECK_RET(item.IsOk(), "invalid tree item");
    m_qtTreeWidget->SetItemState(wxQtConvertTreeItem(item), state);
}

wxTreeItemId wxTreeCtrl::DoInsertItem(const wxTreeItemId& parent,
                                  size_t pos,
                                  const wxString& text,
                                  int image, int selImage,
                                  wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);

    QTreeWidgetItem *newItem = new QTreeWidgetItem;
    newItem->setText(0, wxQtConvertString(text));
    newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

    TreeItemDataQt treeItemData(data);
    newItem->setData(0, Qt::UserRole, QVariant::fromValue(treeItemData));

    m_qtTreeWidget->SetItemImage(newItem, image, wxTreeItemIcon_Normal);
    m_qtTreeWidget->SetItemImage(newItem, selImage, wxTreeItemIcon_Selected);

    newItem->setIcon(0, m_qtTreeWidget->GetPlaceHolderImage());

    wxTreeItemId wxItem = wxQtConvertTreeItem(newItem);

    if ( data != nullptr )
        data->SetId(wxItem);

    if ( pos == static_cast<size_t>(-1) )
    {
        qTreeItem->addChild(newItem);
    }
    else
    {
        qTreeItem->insertChild(pos, newItem);
    }

    return wxItem;
}

wxTreeItemId wxTreeCtrl::DoInsertAfter(const wxTreeItemId& parent,
                                   const wxTreeItemId& idPrevious,
                                   const wxString& text,
                                   int image, int selImage,
                                   wxTreeItemData *data)
{
    wxCHECK_MSG(parent.IsOk(), wxTreeItemId(), "invalid tree item");
    wxCHECK_MSG(idPrevious.IsOk(), wxTreeItemId(), "invalid tree item");

    const QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(parent);
    const int index = qTreeItem->indexOfChild(wxQtConvertTreeItem(idPrevious));
    return DoInsertItem(parent, index + 1, text, image, selImage, data);
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

    QTreeWidgetItem *hitItem = m_qtTreeWidget->itemAt(wxQtConvertPoint(point));
    flags = hitItem == nullptr ? wxTREE_HITTEST_NOWHERE : wxTREE_HITTEST_ONITEM;
    return wxQtConvertTreeItem(hitItem);
}

QWidget *wxTreeCtrl::GetHandle() const
{
    return m_qtTreeWidget;
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

    QTreeWidgetItem *qTreeItem = wxQtConvertTreeItem(item);

    if ( qTreeItem->childCount() > 0 )
    {
        return qTreeItem->child(0);
    }

    // Try a sibling of this or ancestor instead
    wxTreeItemId p = item;
    wxTreeItemId toFind;
    do
    {
        toFind = GetNextSibling(p);
        p = GetItemParent(p);
    } while ( p.IsOk() && !toFind.IsOk() );
    return toFind;
}
