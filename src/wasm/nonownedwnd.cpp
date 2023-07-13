/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/nonownedwnd.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/region.h"
    #include "wx/region.h"
#endif // WX_PRECOMP

#include "wx/nonownedwnd.h"

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

wxNonOwnedWindow::wxNonOwnedWindow()
{
}

bool wxNonOwnedWindow::DoClearShape()
{
    return true;
}

bool wxNonOwnedWindow::DoSetRegionShape(const wxRegion& region)
{

    return true;
}

#if wxUSE_GRAPHICS_CONTEXT
bool wxNonOwnedWindow::DoSetPathShape(const wxGraphicsPath& WXUNUSED(path))
{
    return true;
}
#endif

