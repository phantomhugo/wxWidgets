/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/display.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/display.h"
#include "wx/private/display.h"

//##############################################################################

#if wxUSE_DISPLAY

//##############################################################################

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return nullptr;
}

#else // wxUSE_DISPLAY



#endif // wxUSE_DISPLAY/!wxUSE_DISPLAY
