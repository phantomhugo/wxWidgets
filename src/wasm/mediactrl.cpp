/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/mediactrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MEDIACTRL

#include "wx/mediactrl.h"

// Force link into main library so this backend can be loaded
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends)

#endif //wxUSE_MEDIACTRL
