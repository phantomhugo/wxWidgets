/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/settings.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/settings.h"

#include <emscripten.h>

wxColour wxSystemSettingsNative::GetColour(wxSystemColour index)
{
    // Match the GTK3-like palette used by the default theme of this port
    // (see g_defaultGTK3CSS in src/wasm/cssstyles.cpp).
    switch (index)
    {
        case wxSYS_COLOUR_WINDOW:
        case wxSYS_COLOUR_LISTBOX:
            return wxColour(0xff, 0xff, 0xff);

        case wxSYS_COLOUR_WINDOWFRAME:
        case wxSYS_COLOUR_ACTIVEBORDER:
        case wxSYS_COLOUR_INACTIVEBORDER:
            return wxColour(0xbf, 0xb8, 0xb1);

        case wxSYS_COLOUR_BTNFACE:
        case wxSYS_COLOUR_MENUBAR:
        case wxSYS_COLOUR_MENU:
        case wxSYS_COLOUR_SCROLLBAR:
        case wxSYS_COLOUR_BACKGROUND:
        case wxSYS_COLOUR_APPWORKSPACE:
        case wxSYS_COLOUR_INACTIVECAPTION:
            return wxColour(0xf6, 0xf5, 0xf4);

        case wxSYS_COLOUR_BTNTEXT:
        case wxSYS_COLOUR_WINDOWTEXT:
        case wxSYS_COLOUR_MENUTEXT:
        case wxSYS_COLOUR_CAPTIONTEXT:
        case wxSYS_COLOUR_INACTIVECAPTIONTEXT:
        case wxSYS_COLOUR_LISTBOXTEXT:
        case wxSYS_COLOUR_INFOTEXT:
            return wxColour(0x1c, 0x1c, 0x1c);

        case wxSYS_COLOUR_HIGHLIGHT:
        case wxSYS_COLOUR_ACTIVECAPTION:
        case wxSYS_COLOUR_MENUHILIGHT:
        case wxSYS_COLOUR_GRADIENTACTIVECAPTION:
            return wxColour(0x35, 0x84, 0xe4);

        case wxSYS_COLOUR_HIGHLIGHTTEXT:
        case wxSYS_COLOUR_LISTBOXHIGHLIGHTTEXT:
            return wxColour(0xff, 0xff, 0xff);

        case wxSYS_COLOUR_GRAYTEXT:
        case wxSYS_COLOUR_BTNSHADOW:
            return wxColour(0x9b, 0x99, 0x97);

        case wxSYS_COLOUR_BTNHIGHLIGHT:
        case wxSYS_COLOUR_3DLIGHT:
            return wxColour(0xff, 0xff, 0xff);

        case wxSYS_COLOUR_3DDKSHADOW:
            return wxColour(0x5e, 0x5c, 0x64);

        case wxSYS_COLOUR_INFOBK:
            return wxColour(0xfc, 0xf3, 0xe3);

        case wxSYS_COLOUR_HOTLIGHT:
            return wxColour(0x2a, 0x76, 0xc6);

        case wxSYS_COLOUR_GRADIENTINACTIVECAPTION:
            return wxColour(0xe8, 0xe7, 0xe6);

        default:
            return wxColour(0xf6, 0xf5, 0xf4);
    }
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
            // Default to a sans serif font, as in GTK.
            font.SetFamily(wxFONTFAMILY_SWISS);
            break;
    }

    wxASSERT( font.IsOk() );

    return font;

}

int wxSystemSettingsNative::GetMetric(wxSystemMetric index, const wxWindow* WXUNUSED(win))
{
    // Typical GTK3 desktop values (in pixels).
    switch (index)
    {
        case wxSYS_SCREEN_X:
            return EM_ASM_INT({ return window.screen.width; });

        case wxSYS_SCREEN_Y:
            return EM_ASM_INT({ return window.screen.height; });

        case wxSYS_CURSOR_X:
        case wxSYS_CURSOR_Y:
            return 24;

        case wxSYS_DCLICK_X:
        case wxSYS_DCLICK_Y:
        case wxSYS_DRAG_X:
        case wxSYS_DRAG_Y:
            return 4;

        case wxSYS_EDGE_X:
        case wxSYS_EDGE_Y:
            return 2;

        case wxSYS_BORDER_X:
        case wxSYS_BORDER_Y:
            return 1;

        case wxSYS_FRAMESIZE_X:
        case wxSYS_FRAMESIZE_Y:
            return 4;

        case wxSYS_CAPTION_Y:
            return 28;

        case wxSYS_MENU_Y:
            return 24;

        case wxSYS_HSCROLL_Y:
        case wxSYS_VSCROLL_X:
            return 15;

        case wxSYS_HSCROLL_ARROW_X:
        case wxSYS_HSCROLL_ARROW_Y:
        case wxSYS_VSCROLL_ARROW_X:
        case wxSYS_VSCROLL_ARROW_Y:
            return 15;

        case wxSYS_HTHUMB_X:
        case wxSYS_VTHUMB_Y:
            return 15;

        case wxSYS_ICON_X:
        case wxSYS_ICON_Y:
            return 32;

        case wxSYS_SMALLICON_X:
        case wxSYS_SMALLICON_Y:
            return 16;

        case wxSYS_MOUSE_BUTTONS:
            return 3;

        case wxSYS_SWAP_BUTTONS:
        case wxSYS_SHOW_SOUNDS:
        case wxSYS_PENWINDOWS_PRESENT:
        case wxSYS_NETWORK_PRESENT:
            return 0;

        default:
            return 0;
    }
}

bool wxSystemSettingsNative::HasFeature(wxSystemFeature WXUNUSED(index))
{
    return false;
}
