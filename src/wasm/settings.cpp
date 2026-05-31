/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/settings.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{

}

wxFont wxSystemSettingsNative::GetFont(wxSystemFont index)
{
    wxFont font;
    switch (index)
    {
        case wxSYS_OEM_FIXED_FONT:
        case wxSYS_ANSI_FIXED_FONT:
        case wxSYS_SYSTEM_FIXED_FONT:
            font.SetFamily(wxFONTFAMILY_TELETYPE);
            break;

        case wxSYS_ANSI_VAR_FONT:
        case wxSYS_SYSTEM_FONT:
        case wxSYS_DEVICE_DEFAULT_FONT:
        case wxSYS_DEFAULT_GUI_FONT:
        default:
            // Let Qt select any sans serif font.
            font.SetFamily(wxFONTFAMILY_SWISS);
            break;
    }

    wxASSERT( font.IsOk() );

    return font;

}

int wxSystemSettingsNative::GetMetric(wxSystemMetric index, const wxWindow* WXUNUSED(win))
{

}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature WXUNUSED(index))
{
    return false;
}

