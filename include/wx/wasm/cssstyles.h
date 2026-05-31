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

// Clase para gestionar estilos CSS
class WXDLLIMPEXP_CORE wxWasmCSSManager
{
public:
    // Inyecta CSS por defecto si no existe CSS personalizado
    static void InjectDefaultStyles();
    
    // Permite al usuario cargar CSS personalizado
    static void LoadCustomCSS(const wxString& cssContent);
    
    // Obtiene el namespace CSS para una clase wxWidgets
    static wxString GetCSSClass(const wxString& wxClassName);
    
    // Verifica si existe CSS personalizado
    static bool HasCustomCSS();
    
private:
    static bool ms_defaultStylesInjected;
    static bool ms_hasCustomCSS;
};

// Macro helper para aplicar clases CSS a elementos
#define WX_WASM_CSS_CLASS(className) \
    wxWasmCSSManager::GetCSSClass(wxT(#className))

#endif // _WX_WASM_CSSSTYLES_H_
