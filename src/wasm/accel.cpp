/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/accel.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/accel.h"
#include "wx/list.h"

// ----------------------------------------------------------------------------
// wxAccelList: a list of wxAcceleratorEntries
// ----------------------------------------------------------------------------

WX_DECLARE_LIST(wxAcceleratorEntry, wxAccelList);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxAccelList)

// ----------------------------------------------------------------------------
// wxAccelRefData: the data used by wxAcceleratorTable
// ----------------------------------------------------------------------------

class wxAccelRefData : public wxObjectRefData
{
    public:
        wxAccelRefData()
        {
        }

        wxAccelRefData(const wxAccelRefData& data)
        : wxObjectRefData()
            , m_accels(data.m_accels)
        {
        }

        virtual ~wxAccelRefData()
        {
            WX_CLEAR_LIST(wxAccelList, m_accels);
        }

        wxAccelList m_accels;
};

// macro which can be used to access wxAccelRefData from wxAcceleratorTable
#define M_ACCELDATA ((wxAccelRefData *)m_refData)


// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS( wxAcceleratorTable, wxObject );

wxAcceleratorTable::wxAcceleratorTable()
{
}

wxAcceleratorTable::wxAcceleratorTable(int n, const wxAcceleratorEntry entries[])
{
    m_refData = new wxAccelRefData;

    for ( int i = 0; i < n; i++ )
    {
        M_ACCELDATA->m_accels.Append( new wxAcceleratorEntry( entries[i] ) );
    }
}

wxVector<QShortcut*> wxAcceleratorTable::ConvertShortcutTable( QWidget *parent ) const
{
    wxVector<QShortcut*> shortcuts;

    for ( wxAccelList::compatibility_iterator node = M_ACCELDATA->m_accels.GetFirst();
          node; node = node->GetNext() )
    {
        shortcuts.push_back(ConvertAccelerator(node->GetData(), parent));
    }

    return shortcuts;
}

wxObjectRefData *wxAcceleratorTable::CreateRefData() const
{
    return new wxAccelRefData;
}

wxObjectRefData *wxAcceleratorTable::CloneRefData(const wxObjectRefData *data) const
{
    return new wxAccelRefData(*(wxAccelRefData *)data);
}

bool wxAcceleratorTable::IsOk() const
{
    return (m_refData != nullptr);
}
