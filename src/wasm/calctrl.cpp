/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/calctrl.cpp
// Purpose:     wxCalendarCtrl control implementation for wxWasm
// Author:      Hugo Armando Castellanos Morales
// Created:     2022-11-14
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_CALENDARCTRL

#include "wx/calctrl.h"
#include <emscripten.h>

void wxCalendarCtrl::Init()
{
    for ( size_t n = 0; n < WXSIZEOF(m_attrs); n++ )
        m_attrs[n] = nullptr;
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
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var input = document.createElement('input');
        input.type = 'date';
        input.className = 'wxCalendarCtrl';
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';

        input.addEventListener('change', function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(input);
    }, GetId());

    if ( date.IsValid() )
        SetDate(date);

    return true;
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
    wxCHECK_MSG( date.IsValid(), false, "invalid date" );

    wxString iso = date.FormatISODate();
    wxCharBuffer buf = iso.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxCalendarCtrl');
        if (input) input.value = UTF8ToString($1);
    }, GetId(), buf.data());

    return true;
}

wxDateTime wxCalendarCtrl::GetDate() const
{
    char* val = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxCalendarCtrl');
        if (!input) return 0;
        var str = input.value;
        if (!str) return 0;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    if ( !val )
        return wxDateTime();

    wxDateTime dt;
    dt.ParseISODate(wxString::FromUTF8(val));
    free(val);
    return dt;
}

bool wxCalendarCtrl::SetDateRange(const wxDateTime& lowerdate,
                                  const wxDateTime& upperdate)
{
    wxString minStr, maxStr;
    wxCharBuffer minBuf, maxBuf;
    const char* minPtr = nullptr;
    const char* maxPtr = nullptr;

    if ( lowerdate.IsValid() )
    {
        minStr = lowerdate.FormatISODate();
        minBuf = minStr.ToUTF8();
        minPtr = minBuf.data();
    }

    if ( upperdate.IsValid() )
    {
        maxStr = upperdate.FormatISODate();
        maxBuf = maxStr.ToUTF8();
        maxPtr = maxBuf.data();
    }

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var input = container.querySelector('.wxCalendarCtrl');
        if (!input) return;
        if ($1) input.min = UTF8ToString($1);
        else input.removeAttribute('min');
        if ($2) input.max = UTF8ToString($2);
        else input.removeAttribute('max');
    }, GetId(), minPtr, maxPtr);

    return true;
}

bool wxCalendarCtrl::GetDateRange(wxDateTime *lowerdate,
                                  wxDateTime *upperdate) const
{
    if ( lowerdate )
        *lowerdate = wxDefaultDateTime;
    if ( upperdate )
        *upperdate = wxDefaultDateTime;

    char* minVal = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxCalendarCtrl');
        if (!input || !input.min) return 0;
        var str = input.min;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    char* maxVal = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var input = container.querySelector('.wxCalendarCtrl');
        if (!input || !input.max) return 0;
        var str = input.max;
        var len = lengthBytesUTF8(str) + 1;
        var buf = Module._malloc(len);
        stringToUTF8(str, buf, len);
        return buf;
    }, GetId());

    bool hasRange = false;

    if ( minVal )
    {
        if ( lowerdate )
            lowerdate->ParseISODate(wxString::FromUTF8(minVal));
        free(minVal);
        hasRange = true;
    }

    if ( maxVal )
    {
        if ( upperdate )
            upperdate->ParseISODate(wxString::FromUTF8(maxVal));
        free(maxVal);
        hasRange = true;
    }

    return hasRange;
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
    wxUnusedVar(mark);
}

void wxCalendarCtrl::SetHeaderColours(const wxColour& colFg, const wxColour& colBg)
{
    m_colHeaderFg = colFg;
    m_colHeaderBg = colBg;
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
    wxCHECK_RET( day > 0 && day < 32, "invalid day" );

    delete m_attrs[day - 1];
    m_attrs[day - 1] = attr;
}

void *wxCalendarCtrl::GetHandle() const
{
    return nullptr;
}

wxSize wxCalendarCtrl::DoGetBestSize() const
{
    return wxSize(150, 24);
}

void wxCalendarCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "change" )
    {
        GenerateEvent(wxEVT_CALENDAR_SEL_CHANGED);
    }
}

//=============================================================================

#endif // wxUSE_CALENDARCTRL
