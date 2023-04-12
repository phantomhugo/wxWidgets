/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/display.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
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
