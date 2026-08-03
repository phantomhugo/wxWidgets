/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/cssstyles.h
// Purpose:     CSS Style management for WASM port
// Author:      Hugo Armando Castellanos Morales
// Created:     2024
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CSSSTYLES_H_
#define _WX_WASM_CSSSTYLES_H_

#include "wx/string.h"
#include "wx/defs.h"

// Class to manage CSS styles
class WXDLLIMPEXP_CORE wxWasmCSSManager
{
public:
    // Injects default CSS if there is no custom CSS
    static void InjectDefaultStyles();
    
    // Allows the user to load custom CSS
    static void LoadCustomCSS(const wxString& cssContent);
    
    // Checks whether custom CSS exists
    static bool HasCustomCSS();
    
private:
    static bool ms_defaultStylesInjected;
    static bool ms_hasCustomCSS;
};

#endif // _WX_WASM_CSSSTYLES_H_
