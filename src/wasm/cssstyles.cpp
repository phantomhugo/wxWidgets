/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/cssstyles.cpp
// Purpose:     CSS Style management for WASM port
// Author:      wxWidgets dev team
// Created:     2024
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/wasm/cssstyles.h"
#include <emscripten.h>

bool wxWasmCSSManager::ms_defaultStylesInjected = false;
bool wxWasmCSSManager::ms_hasCustomCSS = false;

// CSS por defecto tipo GTK3 - Tema claro moderno
static const char* g_defaultGTK3CSS = R"CSS(
/* ========================================
   wxWidgets WASM - GTK3-like Default Theme
   ======================================== */

/* wxMenuBar - Barra de menú principal */
.wxMenuBar {
    display: flex;
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 0;
    margin: 0;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    font-size: 14px;
    user-select: none;
    box-sizing: border-box;
}

/* Contenedor de cada menú en la barra */
.wxMenuBar-menu {
    position: relative;
}

/* Label del menú (File, Edit, View, etc.) */
.wxMenuBar-label {
    display: block;
    padding: 8px 16px;
    color: #1c1c1c;
    cursor: pointer;
    border: 1px solid transparent;
    border-radius: 4px 4px 0 0;
    transition: background-color 0.15s ease, color 0.15s ease;
}

.wxMenuBar-label:hover {
    background-color: #3584e4;
    color: white;
}

/* Estado cuando el menú está abierto */
.wxMenuBar-menu.open .wxMenuBar-label {
    background-color: #3584e4;
    color: white;
    border-color: #1c6bc4;
}

/* Menú deshabilitado */
.wxMenuBar-menu.disabled .wxMenuBar-label {
    color: #9c9c9c;
    cursor: not-allowed;
}

.wxMenuBar-menu.disabled .wxMenuBar-label:hover {
    background-color: transparent;
    color: #9c9c9c;
}

/* ========================================
   Popup de Menú (wxMenu)
   ======================================== */

.wxMenu-popup {
    position: absolute;
    top: 100%;
    left: 0;
    min-width: 220px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 0 4px 4px 4px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
    padding: 6px 0;
    z-index: 1000;
    display: none;
}

.wxMenuBar-menu.open .wxMenu-popup {
    display: block;
}

/* ========================================
   Items del Menú (wxMenuItem)
   ======================================== */

.wxMenuItem {
    display: flex;
    align-items: center;
    padding: 6px 16px;
    color: #1c1c1c;
    cursor: pointer;
    transition: background-color 0.1s ease, color 0.1s ease;
    min-height: 24px;
}

.wxMenuItem:hover {
    background-color: #3584e4;
    color: white;
}

/* Item deshabilitado */
.wxMenuItem.disabled {
    color: #9c9c9c;
    cursor: not-allowed;
}

.wxMenuItem.disabled:hover {
    background-color: transparent;
    color: #9c9c9c;
}

/* Item chequeado (checkmark) */
.wxMenuItem.checked::before {
    content: "✓";
    margin-right: 8px;
    font-weight: bold;
    width: 16px;
    text-align: center;
}

/* Icono del item */
.wxMenuItem-icon {
    width: 16px;
    height: 16px;
    margin-right: 8px;
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: center;
}

.wxMenuItem-icon:empty::before {
    content: "";
    width: 16px;
}

/* Label del item */
.wxMenuItem-label {
    flex-grow: 1;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

/* Atajo de teclado */
.wxMenuItem-shortcut {
    color: #6c6c6c;
    font-size: 12px;
    margin-left: 16px;
    flex-shrink: 0;
}

.wxMenuItem:hover .wxMenuItem-shortcut {
    color: rgba(255, 255, 255, 0.8);
}

/* Separador */
.wxMenuItem-separator {
    height: 1px;
    background: #e0e0e0;
    margin: 6px 16px;
    pointer-events: none;
}

/* ========================================
   Sub-menú (wxMenu con sub-items)
   ======================================== */

.wxSubMenu {
    position: relative;
}

.wxSubMenu-label {
    display: flex;
    align-items: center;
    padding: 6px 16px;
    color: #1c1c1c;
    cursor: pointer;
    transition: background-color 0.1s ease;
}

.wxSubMenu-label::after {
    content: "▶";
    margin-left: auto;
    font-size: 10px;
    color: #6c6c6c;
}

.wxSubMenu:hover .wxSubMenu-label {
    background-color: #3584e4;
    color: white;
}

.wxSubMenu:hover .wxSubMenu-label::after {
    color: rgba(255, 255, 255, 0.8);
}

/* Popup del sub-menú */
.wxSubMenu-popup {
    position: absolute;
    top: -6px;
    left: 100%;
    min-width: 220px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
    padding: 6px 0;
    display: none;
    z-index: 1001;
}

.wxSubMenu:hover .wxSubMenu-popup {
    display: block;
}

/* ========================================
   wxFrame
   ======================================== */

.wxFrame {
    background: #f6f5f4;
    border: 1px solid #bfb8b1;
    border-radius: 8px 8px 0 0;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
    overflow: hidden;
    display: flex;
    flex-direction: column;
    position: absolute;
}

/* Contenedor de contenido del frame */
.wxFrame_content {
    flex: 1;
    position: relative;
    overflow: auto;
    background: #ffffff;
}

/* ========================================
   wxStatusBar
   ======================================== */

.wxStatusBar {
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-top: 1px solid #bfb8b1;
    padding: 4px 8px;
    font-size: 12px;
    color: #4c4c4c;
    display: flex;
    align-items: center;
    min-height: 22px;
    box-sizing: border-box;
}

/* Contenedor de fields */
.wxStatusBar-fields {
    display: flex;
    flex: 1;
    width: 100%;
    gap: 2px;
}

/* Field individual */
.wxStatusBar-field {
    display: flex;
    align-items: center;
    padding: 2px 8px;
    border-right: 1px solid #d0d0d0;
    min-height: 18px;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.wxStatusBar-field:last-child {
    border-right: none;
}

/* Field con size grip */
.wxStatusBar-field-with-grip {
    justify-content: space-between;
    padding-right: 4px;
}

/* Texto del field */
.wxStatusBar-field-text {
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

/* Size grip (esquina redimensionable) */
.wxStatusBar-sizegrip {
    color: #8c8c8c;
    font-size: 10px;
    line-height: 1;
    margin-left: 4px;
    cursor: se-resize;
    user-select: none;
}

.wxStatusBar-sizegrip:hover {
    color: #5c5c5c;
}

/* Variantes de estilo */

/* StatusBar con borde plano (wxSTB_FLAT) */
.wxStatusBar-flat {
    background: #f0f0f0;
    border-top: 1px solid #cccccc;
}

.wxStatusBar-flat .wxStatusBar-field {
    border: none;
    padding: 2px 6px;
}

/* StatusBar con borde hundido (por defecto) */
.wxStatusBar-raised .wxStatusBar-field {
    border: 1px solid #bfbfbf;
    border-top-color: #ffffff;
    border-left-color: #ffffff;
    border-right-color: #808080;
    border-bottom-color: #808080;
    background: #e8e8e8;
}

/* StatusBar con grip visible */
.wxStatusBar-with-grip {
    padding-right: 4px;
}

/* ========================================
   wxButton
   ======================================== */

.wxButton {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 6px 16px;
    font-size: 14px;
    color: #1c1c1c;
    cursor: pointer;
    transition: all 0.15s ease;
    font-family: inherit;
}

.wxButton:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

.wxButton:active {
    background: #e0e0e0;
    box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.1);
}

/* Botón por defecto (OK, Save, etc.) */
.wxButton.default {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    color: white;
    border-color: #1c6bc4;
}

.wxButton.default:hover {
    background: linear-gradient(to bottom, #2a76d4, #165cad);
}

.wxButton.default:active {
    background: #1c6bc4;
}

/* Botón deshabilitado */
.wxButton.disabled {
    opacity: 0.5;
    cursor: not-allowed;
}

/* ========================================
   wxTextCtrl
   ======================================== */

.wxTextCtrl {
    display: block;
    width: 100%;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 6px 8px;
    font-size: 14px;
    color: #1c1c1c;
    outline: none;
    transition: border-color 0.15s ease, box-shadow 0.15s ease;
    font-family: inherit;
    box-sizing: border-box;
}

.wxTextCtrl:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxTextCtrl.disabled {
    background: #f5f5f5;
    color: #9c9c9c;
    cursor: not-allowed;
}

/* ========================================
   wxPanel
   ======================================== */

.wxPanel {
    background: transparent;
}

/* ========================================
   wxDialog
   ======================================== */

.wxDialog {
    background: #f6f5f4;
    border: 1px solid #bfb8b1;
    border-radius: 8px;
    box-shadow: 0 12px 48px rgba(0, 0, 0, 0.3);
}

.wxDialog.modal {
    position: fixed;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    z-index: 2000;
}

/* Overlay para diálogos modales */
.wxDialog-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.5);
    z-index: 1999;
}
)CSS";

void wxWasmCSSManager::InjectDefaultStyles()
{
    if (ms_defaultStylesInjected || ms_hasCustomCSS)
        return;
    
    EM_ASM_({
        // Verificar si ya existe el stylesheet
        if (!document.getElementById('wxwidgets-default-styles')) {
            var style = document.createElement('style');
            style.id = 'wxwidgets-default-styles';
            style.textContent = UTF8ToString($0);
            document.head.appendChild(style);
        }
    }, g_defaultGTK3CSS);
    
    ms_defaultStylesInjected = true;
}

void wxWasmCSSManager::LoadCustomCSS(const wxString& cssContent)
{
    wxCharBuffer buffer = cssContent.ToUTF8();
    
    EM_ASM_({
        // Remover estilos por defecto si existen
        var defaultStyle = document.getElementById('wxwidgets-default-styles');
        if (defaultStyle) {
            defaultStyle.remove();
        }
        
        // Remover CSS personalizado anterior si existe
        var customStyle = document.getElementById('wxwidgets-custom-styles');
        if (customStyle) {
            customStyle.remove();
        }
        
        // Crear nuevo elemento de estilo
        var style = document.createElement('style');
        style.id = 'wxwidgets-custom-styles';
        style.textContent = UTF8ToString($0);
        document.head.appendChild(style);
    }, buffer.data());
    
    ms_hasCustomCSS = true;
}

wxString wxWasmCSSManager::GetCSSClass(const wxString& wxClassName)
{
    // Remover "wx" del prefijo para las clases CSS
    if (wxClassName.StartsWith(wxT("wx"))) {
        return wxClassName.Mid(2);
    }
    return wxClassName;
}

bool wxWasmCSSManager::HasCustomCSS()
{
    return ms_hasCustomCSS;
}
