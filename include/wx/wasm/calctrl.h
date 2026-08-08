/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/calctrl.h
// Purpose:     wxCalendarCtrl control implementation for wxWasm
// Author:      Hugo Armando Castellanos Morales
// Created:     2022-11-14
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CALCTRL_H_
#define _WX_WASM_CALCTRL_H_

#include "wx/generic/calctrlg.h"

// ----------------------------------------------------------------------------
// wxCalendarCtrl
// ----------------------------------------------------------------------------

// NB: derives from the generic custom-drawn control (src/generic/calctrlg.cpp
// is part of the common sources), as the GTK implementation does: an
// <input type="date"> cannot render a persistent month grid and does not
// support day attributes, marked days or holidays. It must be a distinct
// class (not an alias) because src/common/calctrlcmn.cpp implements the
// wxCalendarCtrl class info itself.
class WXDLLIMPEXP_ADV wxCalendarCtrl : public wxGenericCalendarCtrl
{
public:
    wxCalendarCtrl() { }

    wxCalendarCtrl(wxWindow *parent,
                   wxWindowID id,
                   const wxDateTime& date = wxDefaultDateTime,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCAL_SHOW_HOLIDAYS,
                   const wxString& name = wxASCII_STR(wxCalendarNameStr))
        : wxGenericCalendarCtrl(parent, id, date, pos, size, style, name) { }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCAL_SHOW_HOLIDAYS,
                const wxString& name = wxASCII_STR(wxCalendarNameStr))
    {
        return wxGenericCalendarCtrl::Create(parent, id, date, pos, size,
                                             style, name);
    }

private:
    // The class info is implemented by wxIMPLEMENT_DYNAMIC_CLASS_XTI in
    // src/common/calctrlcmn.cpp.
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxCalendarCtrl);
};

#endif // _WX_WASM_CALCTRL_H_
