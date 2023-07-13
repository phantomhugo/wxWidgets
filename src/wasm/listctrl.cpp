/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/listctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"



#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/app.h"
#include "wx/listctrl.h"
#include "wx/imaglist.h"
#include "wx/recguard.h"

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

    SetWindowStyleFlag(style);
    return true;
}

void wxListCtrl::Init()
{

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

bool wxListCtrl::GetColumn(int col, wxListItem& info) const
{

}

bool wxListCtrl::SetColumn(int col, const wxListItem& info)
{


    return true;
}

int wxListCtrl::GetColumnWidth(int col) const
{

}

bool wxListCtrl::SetColumnWidth(int col, int width)
{

    return true;
}

int wxListCtrl::GetColumnOrder(int col) const
{
    return col;
}

int wxListCtrl::GetColumnIndexFromOrder(int order) const
{
    return order;
}

wxArrayInt wxListCtrl::GetColumnsOrder() const
{
    return wxArrayInt();
}

bool wxListCtrl::SetColumnsOrder(const wxArrayInt& WXUNUSED(orders))
{
    return false;
}

int wxListCtrl::GetCountPerPage() const
{
    return 0;
}

wxRect wxListCtrl::GetViewRect() const
{

}

wxTextCtrl* wxListCtrl::GetEditControl() const
{

}

bool wxListCtrl::GetItem(wxListItem& info) const
{

}

bool wxListCtrl::SetItem(wxListItem& info)
{

}

long wxListCtrl::SetItem(long index, int col, const wxString& label, int imageId)
{

}

int wxListCtrl::GetItemState(long item, long stateMask) const
{

}

bool wxListCtrl::SetItemState(long item, long state, long stateMask)
{

}

bool wxListCtrl::SetItemImage(long item, int image, int WXUNUSED(selImage))
{
    return SetItemColumnImage(item, 0, image);
}

bool wxListCtrl::SetItemColumnImage(long item, long column, int image)
{

}

wxString wxListCtrl::GetItemText(long item, int col) const
{

}

void wxListCtrl::SetItemText(long item, const wxString& str)
{

}

wxUIntPtr wxListCtrl::GetItemData(long item) const
{

}

bool wxListCtrl::SetItemPtrData(long item, wxUIntPtr data)
{

}

bool wxListCtrl::SetItemData(long item, long data)
{
    return SetItemPtrData(item, static_cast<wxUIntPtr>(data));
}

bool wxListCtrl::GetItemRect(long item, wxRect& rect, int WXUNUSED(code)) const
{
    wxCHECK_MSG(item >= 0 && (item < GetItemCount()), false,
                 "invalid item in GetSubItemRect");


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

bool wxListCtrl::SetItemPosition(long WXUNUSED(item),
                                 const wxPoint& WXUNUSED(pos))
{
    return false;
}

int wxListCtrl::GetItemCount() const
{
}

int wxListCtrl::GetColumnCount() const
{
}

wxSize wxListCtrl::GetItemSpacing() const
{
    return wxSize();
}

void wxListCtrl::SetItemTextColour(long item, const wxColour& col)
{
}

wxColour wxListCtrl::GetItemTextColour( long item ) const
{
}

void wxListCtrl::SetItemBackgroundColour( long item, const wxColour &col)
{
}

wxColour wxListCtrl::GetItemBackgroundColour( long item ) const
{
}

void wxListCtrl::SetItemFont( long item, const wxFont &f)
{
}

wxFont wxListCtrl::GetItemFont( long item ) const
{

}

int wxListCtrl::GetSelectedItemCount() const
{
}

wxColour wxListCtrl::GetTextColour() const
{
}

void wxListCtrl::SetTextColour(const wxColour& col)
{
}

long wxListCtrl::GetTopItem() const
{
    return 0;
}

bool wxListCtrl::HasCheckBoxes() const
{
}

bool wxListCtrl::EnableCheckBoxes(bool enable /*= true*/)
{
    return true;
}

bool wxListCtrl::IsItemChecked(long item) const
{
}

void wxListCtrl::CheckItem(long item, bool check)
{
}

void wxListCtrl::SetSingleStyle(long WXUNUSED(style), bool WXUNUSED(add))
{
}

void wxListCtrl::SetWindowStyleFlag(long style)
{

}

long wxListCtrl::GetNextItem(long item, int WXUNUSED(geometry), int state) const
{
    return -1;
}

void wxListCtrl::DoUpdateImages(int WXUNUSED(which))
{
    // TODO: Ensure the icons are actually updated.
}

void wxListCtrl::RefreshItem(long item)
{
    RefreshItems(item, item);
}

void wxListCtrl::RefreshItems(long itemFrom, long itemTo)
{
}

bool wxListCtrl::Arrange(int WXUNUSED(flag))
{
    return false;
}

bool wxListCtrl::DeleteItem(long item)
{
    return true;
}

bool wxListCtrl::DeleteAllItems()
{
    if ( GetItemCount() == 0 )
        return true;

    return true;
}

bool wxListCtrl::DeleteColumn(int col)
{
    return true;
}

bool wxListCtrl::DeleteAllColumns()
{
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
}

bool wxListCtrl::EndEditLabel(bool WXUNUSED(cancel))
{
}

bool wxListCtrl::EnsureVisible(long item)
{
}

long wxListCtrl::FindItem(long start, const wxString& str, bool partial)
{

}

long wxListCtrl::FindItem(long start, wxUIntPtr data)
{
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
}

long wxListCtrl::InsertItem(const wxListItem& info)
{
}

long wxListCtrl::InsertItem(long index, const wxString& label)
{
}

long wxListCtrl::InsertItem(long index, int imageIndex)
{
}

long wxListCtrl::InsertItem(long index, const wxString& label, int imageIndex)
{
}

long wxListCtrl::DoInsertColumn(long col, const wxListItem& info)
{
}

void wxListCtrl::SetItemCount(long count)
{
}

bool wxListCtrl::ScrollList(int dx, int dy)
{
    return true;
}

bool wxListCtrl::SortItems(wxListCtrlCompare fn, wxIntPtr data)
{
    return true;
}

void *wxListCtrl::GetHandle() const
{
}
