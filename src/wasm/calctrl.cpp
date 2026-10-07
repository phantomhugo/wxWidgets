/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/calctrl.cpp
// Purpose:     wxCalendarCtrl control implementation for wxWasm
// Author:      Hugo Armando Castellanos Morales
// Created:     2022-11-14
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"

void wxCalendarCtrl::Init()
{

}

wxCalendarCtrl::~wxCalendarCtrl()
{
    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
    {
        delete m_attrs[n];
    }
}

bool wxCalendarCtrl::Create(wxWindow *parent, wxWindowID id, const wxDateTime& date,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{

}

void wxCalendarCtrl::UpdateStyle()
{


    RefreshHolidays();
}

void wxCalendarCtrl::SetWindowStyleFlag(long style)
{
    const long styleOld = GetWindowStyleFlag();

    wxCalendarCtrlBase::SetWindowStyleFlag(style);

    if ( styleOld != GetWindowStyleFlag() )
        UpdateStyle();
}

bool wxCalendarCtrl::SetDate(const wxDateTime& date)
{

}

wxDateTime wxCalendarCtrl::GetDate() const
{

}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& lowerdate,
                                  const wxDateTime& upperdate)
{

    return true;
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *lowerdate,
                                  wxDateTime *upperdate) const
{

    bool status = false;

    return status;
}

// Copied from wxMSW
bool wxCalendarCtrl::EnableMonthChange(bool enable)
{
    if ( !wxCalendarCtrlBase::EnableMonthChange(enable) )
        return false;

    wxDateTime dtStart, dtEnd;
    if ( !enable )
    {
        dtStart = GetDate();
        dtStart.SetDay(1);

        dtEnd = dtStart.GetLastMonthDay();
    }
    //else: leave them invalid to remove the restriction

    SetDateRange(dtStart, dtEnd);

    return true;
}

void wxCalendarCtrl::Mark(size_t day, bool mark)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

}

void wxCalendarCtrl::SetHoliday(size_t day)
{
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    if ( !(m_windowStyle & wxCAL_SHOW_HOLIDAYS) )
        return;

}

void wxCalendarCtrl::SetHolidayColours(const wxColour& colFg, const wxColour& colBg)
{
    m_colHolidayFg = colFg;
    m_colHolidayBg = colBg;
    RefreshHolidays();
}

void wxCalendarCtrl::RefreshHolidays()
{

}

wxCalendarDateAttr *wxCalendarCtrl::GetAttr(size_t day) const
{
    wxCHECK_MSG( day > 0 && day < 32, nullptr, wxT("invalid day") );

    return m_attrs[day - 1];
}

void wxCalendarCtrl::SetAttr(size_t day, wxCalendarDateAttr *attr)
{

}

//=============================================================================



#endif // wxUSE_CALENDARCTRL
