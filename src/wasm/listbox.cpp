/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/listbox.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/listbox.h"
wxListBox::wxListBox()
{
    Init();
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        int n, const wxString choices[],
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, pos, size, n, choices, style, validator, name );
}

wxListBox::wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style,
        const wxValidator& validator,
        const wxString& name)
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
    DoCreate(parent, style);

    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    DoCreate(parent, style);


    return wxListBoxBase::Create( parent, id, pos, size, style, validator, name );
}

void wxListBox::DoCreate(wxWindow* parent, long style)
{
    Init();


}

void wxListBox::Init()
{
#if wxUSE_CHECKLISTBOX
    m_hasCheckBoxes = false;
#endif // wxUSE_CHECKLISTBOX
}

bool wxListBox::IsSelected(int n) const
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    return item->isSelected();
}

int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.clear();

    return aSelections.size();
}

unsigned wxListBox::GetCount() const
{
    return m_qtListWidget->count();
}

wxString wxListBox::GetString(unsigned int n) const
{
    wxCHECK_MSG(item != nullptr, wxString(), wxT("wrong listbox index") );

}

void wxListBox::SetString(unsigned int n, const wxString& s)
{

}

int wxListBox::GetSelection() const
{
    if ( m_qtListWidget->selectedItems().empty() )
    {
        return wxNOT_FOUND;
    }


    QListWidgetItem* item = m_qtListWidget->selectedItems().first();

    return m_qtListWidget->row(item);
}

void wxListBox::DoSetFirstItem(int n)
{
    m_qtListWidget->scrollToItem(m_qtListWidget->item(n), QAbstractItemView::PositionAtTop);
}

void wxListBox::DoSetSelection(int n, bool select)
{
    if ( n == wxNOT_FOUND )
    {
        UnSelectAll();
        return;
    }

    m_qtListWidget->item(n)->setSelected(select);
}

int wxListBox::DoInsertItems(const wxArrayStringsAdapter & items,
                          unsigned int pos,
                          void **clientData,
                          wxClientDataType type)
{
    InvalidateBestSize();
    int n = DoInsertItemsInLoop(items, pos, clientData, type);
    UpdateOldSelections();
    return n;
}

int wxListBox::DoInsertOneItem(const wxString& text, unsigned int pos)
{
    QListWidgetItem* item = new QListWidgetItem();
    item->setText(wxQtConvertString( text ));
    if ( m_hasCheckBoxes )
    {
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(Qt::Unchecked);
    }
    m_qtListWidget->insertItem(pos, item);
    return pos;
}

void wxListBox::DoSetItemClientData(unsigned int n, void *clientData)
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    QVariant variant = QVariant::fromValue(clientData);
    item->setData(Qt::UserRole, variant);
}

void *wxListBox::DoGetItemClientData(unsigned int n) const
{
    QListWidgetItem* item = m_qtListWidget->item(n);
    QVariant variant = item->data(Qt::UserRole);
    return variant.value<void *>();
}

void wxListBox::DoClear()
{
    m_qtListWidget->clear();
}

void wxListBox::DoDeleteOneItem(unsigned int pos)
{
    QListWidgetItem* item = m_qtListWidget->item(pos);
    delete item;
}

QWidget *wxListBox::GetHandle() const
{
    return m_qtListWidget;
}

void wxListBox::QtSendEvent(wxEventType evtType, int rowIndex, bool selected)
{
    SendEvent(evtType, rowIndex, selected);
}

QScrollArea *wxListBox::QtGetScrollBarsContainer() const
{
    return (QScrollArea *) m_qtListWidget;
}

void wxListBox::UnSelectAll()
{
    Q_FOREACH(QListWidgetItem* l, m_qtListWidget->selectedItems())
    {
        l->setSelected(false);
    }
}
