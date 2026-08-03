/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/cssstyles.cpp
// Purpose:     CSS Style management for WASM port
// Author:      Hugo Armando Castellanos Morales
// Created:     2024
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/wasm/cssstyles.h"
#include <emscripten.h>

bool wxWasmCSSManager::ms_defaultStylesInjected = false;
bool wxWasmCSSManager::ms_hasCustomCSS = false;

// Default GTK3-like CSS - modern light theme
static const char* g_defaultGTK3CSS = R"CSS(
/* ========================================
   wxWidgets WASM - GTK3-like Default Theme
   ======================================== */

/* wxMenuBar - main menu bar */
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

/* Container of each menu in the bar */
.wxMenuBar-menu {
    position: relative;
}

/* Menu label (File, Edit, View, etc.) */
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

/* State when the menu is open */
.wxMenuBar-menu.open .wxMenuBar-label {
    background-color: #3584e4;
    color: white;
    border-color: #1c6bc4;
}

/* Disabled menu */
.wxMenuBar-menu.disabled .wxMenuBar-label {
    color: #9c9c9c;
    cursor: not-allowed;
}

.wxMenuBar-menu.disabled .wxMenuBar-label:hover {
    background-color: transparent;
    color: #9c9c9c;
}

/* ========================================
   Menu Popup (wxMenu)
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
   Menu Items (wxMenuItem)
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

/* Disabled item */
.wxMenuItem.disabled {
    color: #9c9c9c;
    cursor: not-allowed;
}

.wxMenuItem.disabled:hover {
    background-color: transparent;
    color: #9c9c9c;
}

/* Checked item (checkmark) */
.wxMenuItem.checked::before {
    content: "✓";
    margin-right: 8px;
    font-weight: bold;
    width: 16px;
    text-align: center;
}

/* Item icon */
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

/* Item label */
.wxMenuItem-label {
    flex-grow: 1;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

/* Keyboard shortcut */
.wxMenuItem-shortcut {
    color: #6c6c6c;
    font-size: 12px;
    margin-left: 16px;
    flex-shrink: 0;
}

.wxMenuItem:hover .wxMenuItem-shortcut {
    color: rgba(255, 255, 255, 0.8);
}

/* Separator */
.wxMenuItem-separator {
    height: 1px;
    background: #e0e0e0;
    margin: 6px 16px;
    pointer-events: none;
}

/* ========================================
   Sub-menu (wxMenu with sub-items)
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

/* Sub-menu popup */
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

/* Frame content container */
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

/* Fields container */
.wxStatusBar-fields {
    display: flex;
    flex: 1;
    width: 100%;
    gap: 2px;
}

/* Individual field */
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

/* Field with size grip */
.wxStatusBar-field-with-grip {
    justify-content: space-between;
    padding-right: 4px;
}

/* Field text */
.wxStatusBar-field-text {
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

/* Size grip (resizable corner) */
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

/* Style variants */

/* StatusBar with flat border (wxSTB_FLAT) */
.wxStatusBar-flat {
    background: #f0f0f0;
    border-top: 1px solid #cccccc;
}

.wxStatusBar-flat .wxStatusBar-field {
    border: none;
    padding: 2px 6px;
}

/* StatusBar with sunken border (default) */
.wxStatusBar-raised .wxStatusBar-field {
    border: 1px solid #bfbfbf;
    border-top-color: #ffffff;
    border-left-color: #ffffff;
    border-right-color: #808080;
    border-bottom-color: #808080;
    background: #e8e8e8;
}

/* StatusBar with visible grip */
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

/* Default button (OK, Save, etc.) */
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

/* Disabled button */
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
   wxMessageDialog
   ======================================== */

/* Modal dialog backdrop */
.wxMessageDialog::backdrop {
    background: rgba(0, 0, 0, 0.5);
    backdrop-filter: blur(2px);
}

/* The dialog itself */
.wxMessageDialog {
    border: none;
    border-radius: 8px;
    padding: 0;
    box-shadow: 0 16px 48px rgba(0, 0, 0, 0.3);
    max-width: 500px;
    min-width: 300px;
    background: #ffffff;
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
}

/* Dialog content */
.wxMessageDialog-content {
    display: flex;
    flex-direction: column;
}

/* Header with title */
.wxMessageDialog-header {
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 12px 16px;
    font-size: 16px;
    font-weight: 600;
    color: #1c1c1c;
    border-radius: 8px 8px 0 0;
}

/* Message area */
.wxMessageDialog-message-area {
    display: flex;
    gap: 16px;
    padding: 24px;
    align-items: flex-start;
}

/* Icon */
.wxMessageDialog-icon {
    font-size: 32px;
    flex-shrink: 0;
    width: 48px;
    height: 48px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 50%;
    background: #f0f0f0;
}

/* Specific classes for icons */
.wxMessageDialog-icon-error {
    background: #ffebee;
}

.wxMessageDialog-icon-warning {
    background: #fff3e0;
}

.wxMessageDialog-icon-question {
    background: #e3f2fd;
}

.wxMessageDialog-icon-info {
    background: #e8f5e9;
}

/* Text container */
.wxMessageDialog-text {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: 8px;
}

/* Main message */
.wxMessageDialog-main-message {
    font-size: 14px;
    line-height: 1.5;
    color: #1c1c1c;
}

/* Extended message */
.wxMessageDialog-extended-message {
    font-size: 13px;
    line-height: 1.4;
    color: #5c5c5c;
}

/* Button area */
.wxMessageDialog-buttons {
    display: flex;
    justify-content: flex-end;
    gap: 8px;
    padding: 16px 24px;
    border-top: 1px solid #e0e0e0;
    background: #fafafa;
    border-radius: 0 0 8px 8px;
}

/* Dialog buttons */
.wxMessageDialog-button {
    padding: 8px 20px;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    color: #1c1c1c;
    font-size: 14px;
    cursor: pointer;
    transition: all 0.15s ease;
    min-width: 80px;
}

.wxMessageDialog-button:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

.wxMessageDialog-button:active {
    background: #e0e0e0;
    box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.1);
}

/* Default button */
.wxMessageDialog-button-default {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    color: white;
    border-color: #1c6bc4;
}

.wxMessageDialog-button-default:hover {
    background: linear-gradient(to bottom, #2a76d4, #165cad);
}

.wxMessageDialog-button-default:active {
    background: #1c6bc4;
}

/* Help button (goes to the left) */
.wxMessageDialog-button-help {
    margin-right: auto;
    background: transparent;
    border: none;
    color: #3584e4;
    min-width: auto;
}

.wxMessageDialog-button-help:hover {
    background: rgba(53, 132, 228, 0.1);
}

/* ========================================
   wxPanel
   ======================================== */

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

/* Overlay for modal dialogs */
.wxDialog-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.5);
    z-index: 1999;
}

/* ========================================
   wxDialog - Native modal dialog
   ======================================== */
.wxDialog {
    border: none;
    border-radius: 8px;
    padding: 0;
    background: #ffffff;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
    min-width: 300px;
}

.wxDialog::backdrop {
    background: rgba(0, 0, 0, 0.4);
    backdrop-filter: blur(2px);
}

/* ========================================
   wxGauge (progress bar)
   ======================================== */

.wxGauge {
    -webkit-appearance: none;
    appearance: none;
    border: none;
    border-radius: 4px;
    background: #e0e0e0;
    overflow: hidden;
}

.wxGauge-horizontal {
    width: 100%;
    height: 16px;
}

.wxGauge-vertical {
    width: 16px;
    height: 100px;
    transform: rotate(-90deg);
    transform-origin: left bottom;
}

/* WebKit */
.wxGauge::-webkit-progress-bar {
    background: #e0e0e0;
    border-radius: 4px;
}

.wxGauge::-webkit-progress-value {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    border-radius: 4px;
    transition: width 0.15s ease;
}

/* Firefox */
.wxGauge::-moz-progress-bar {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    border-radius: 4px;
}

/* ========================================
   wxListCtrl (HTML table)
   ======================================== */

.wxListCtrl-table {
    width: 100%;
    border-collapse: collapse;
    font-family: inherit;
    font-size: 13px;
    color: #1c1c1c;
}

.wxListCtrl-head th {
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 6px 10px;
    text-align: left;
    font-weight: 600;
    white-space: nowrap;
}

.wxListCtrl-body tr {
    border-bottom: 1px solid #e8e7e6;
    transition: background-color 0.1s ease;
}

.wxListCtrl-body tr:hover {
    background-color: #e8f4fd;
}

.wxListCtrl-body tr.selected {
    background-color: #3584e4;
    color: white;
}

.wxListCtrl-body tr.focused {
    outline: 1px solid #1c6bc4;
    outline-offset: -1px;
}

.wxListCtrl-body td {
    padding: 5px 10px;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

/* ========================================
   wxListCtrl - Grid modes (ICON, SMALL_ICON, LIST)
   ======================================== */

.wxListCtrl-grid {
    display: flex;
    flex-wrap: wrap;
    gap: 8px;
    padding: 8px;
    overflow: auto;
    align-content: flex-start;
}

.wxListCtrl-item {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: 6px 8px;
    border-radius: 6px;
    cursor: pointer;
    transition: background-color 0.1s ease;
    min-width: 60px;
    max-width: 120px;
    text-align: center;
}

.wxListCtrl-item:hover {
    background-color: #e8f4fd;
}

.wxListCtrl-item.selected {
    background-color: #3584e4;
    color: white;
}

.wxListCtrl-item.focused {
    outline: 1px solid #1c6bc4;
    outline-offset: -1px;
}

.wxListCtrl-item-icon {
    font-size: 20px;
    margin-bottom: 4px;
    line-height: 1;
}

.wxListCtrl-item-icon-large .wxListCtrl-item-icon {
    font-size: 32px;
    margin-bottom: 6px;
}

.wxListCtrl-item-label {
    font-size: 12px;
    line-height: 1.3;
    word-break: break-word;
    overflow: hidden;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
}

/* Floating popup (context menu) */
.wxMenu-popup-floating {
    position: fixed;
    top: 0;
    left: 0;
    min-width: 220px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
    padding: 6px 0;
    z-index: 2000;
    display: none;
}

/* Disabled state set by wxWindowDisabler */
.wx-disabled {
    pointer-events: none !important;
    opacity: 0.5;
}

/* ========================================
   wxComboBox (editable: input + datalist)
   ======================================== */

.wxComboBox {
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

.wxComboBox:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 1px #3584e4;
}

.wxComboBox:disabled {
    background: #f6f5f4;
    color: #9b9997;
}

/* ========================================
   wxStaticBitmap / wxBitmapButton
   ======================================== */

.wxStaticBitmap {
    display: block;
}

.wxButton.wxBitmapButton {
    padding: 4px;
}

.wxButton.wxBitmapButton img {
    display: block;
    margin: 0 auto;
    max-width: 100%;
    max-height: 100%;
}

/* ========================================
   wxSpinButton
   ======================================== */

.wxSpinButton {
    display: flex;
    overflow: hidden;
}

.wxSpinButton-vertical {
    flex-direction: column;
}

.wxSpinButton-horizontal {
    flex-direction: row;
}

.wxSpinButton button {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    background: linear-gradient(to bottom, #fafafa, #ececec);
    border: 1px solid #bfb8b1;
    color: #1c1c1c;
    font-size: 8px;
    line-height: 1;
    padding: 0;
    margin: 0;
    cursor: pointer;
    min-width: 0;
    min-height: 0;
}

.wxSpinButton button:hover {
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
}

.wxSpinButton button:active {
    background: #d9d7d4;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.1);
}

.wxSpinButton-vertical .wxSpinButton-up {
    border-radius: 4px 4px 0 0;
    border-bottom: none;
}

.wxSpinButton-vertical .wxSpinButton-down {
    border-radius: 0 0 4px 4px;
}

.wxSpinButton-horizontal .wxSpinButton-up {
    border-radius: 4px 0 0 4px;
    border-right: none;
}

.wxSpinButton-horizontal .wxSpinButton-down {
    border-radius: 0 4px 4px 0;
}

.wxSpinButton button:disabled {
    color: #9b9997;
    background: #f6f5f4;
}

/* ========================================
   wxSearchCtrl
   ======================================== */

.wxSearchCtrl {
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

.wxSearchCtrl:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 1px #3584e4;
}

/* ========================================
   wxHyperlinkCtrl
   ======================================== */

.wxHyperlinkCtrl {
    color: #2a76c6;
    text-decoration: none;
    cursor: pointer;
    font-family: inherit;
}

.wxHyperlinkCtrl:hover {
    text-decoration: underline;
}

.wxHyperlinkCtrl:visited {
    color: #9141ac;
}

/* ========================================
   wxInfoBar
   ======================================== */

.wxInfoBar {
    display: flex;
    align-items: center;
    gap: 8px;
    padding: 6px 10px;
    border-bottom: 1px solid #bfb8b1;
    font-size: 13px;
    color: #1c1c1c;
    box-sizing: border-box;
}

.wxInfoBar-info {
    background: #e8f4fd;
    border-bottom-color: #3584e4;
}

.wxInfoBar-warning {
    background: #fcf3e3;
    border-bottom-color: #e5a50a;
}

.wxInfoBar-question {
    background: #e8f4fd;
    border-bottom-color: #3584e4;
}

.wxInfoBar-error {
    background: #fbe9e8;
    border-bottom-color: #e01b24;
}

.wxInfoBar-none {
    background: #f6f5f4;
}

.wxInfoBar-message {
    flex: 1;
}

.wxInfoBar-close {
    background: none;
    border: none;
    border-radius: 4px;
    color: inherit;
    cursor: pointer;
    font-size: 14px;
    padding: 2px 6px;
}

.wxInfoBar-close:hover {
    background: rgba(0, 0, 0, 0.08);
}

/* ========================================
   wxCollapsiblePane
   ======================================== */

.wxCollapsiblePane {
    display: flex;
    flex-direction: column;
    box-sizing: border-box;
}

.wxCollapsiblePane-button {
    display: flex;
    align-items: center;
    gap: 6px;
    background: none;
    border: none;
    padding: 4px 2px;
    font: inherit;
    font-weight: 600;
    color: #1c1c1c;
    cursor: pointer;
    text-align: left;
}

.wxCollapsiblePane-button:focus {
    outline: 1px solid #3584e4;
    outline-offset: -1px;
}

.wxCollapsiblePane-arrow {
    font-size: 10px;
    transition: transform 0.15s ease;
}

.wxCollapsiblePane-pane {
    overflow: hidden;
}

/* ========================================
   wxActivityIndicator
   ======================================== */

.wxActivityIndicator {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
}

.wxActivityIndicator-spinner {
    display: none;
    width: 24px;
    height: 24px;
    border: 3px solid #d5d3cf;
    border-top-color: #3584e4;
    border-radius: 50%;
    animation: wxActivityIndicator-spin 0.8s linear infinite;
}

.wxActivityIndicator.running .wxActivityIndicator-spinner {
    display: block;
}

@keyframes wxActivityIndicator-spin {
    to { transform: rotate(360deg); }
}

/* ========================================
   Base - Inheritable font and color
   ======================================== */

/* Every wx window is a <div class="wxWindow">; the font is inherited
   by the native elements that use font-family: inherit */
.wxWindow {
    font-family: Cantarell, -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, sans-serif;
    font-size: 14px;
    color: #1c1c1c;
}

/* ========================================
   Native controls - GTK3 accent
   ======================================== */

input[type="checkbox"],
input[type="radio"] {
    accent-color: #3584e4;
    width: 16px;
    height: 16px;
    margin: 0;
}

/* ========================================
   wxStaticText / wxStaticLine / wxStaticBox
   ======================================== */

.wxStaticText {
    display: inline-block;
    color: #1c1c1c;
}

/* wxStaticLine is a div; its thickness comes from the window size */
.wxStaticLine {
    background: #bfb8b1;
}

.wxStaticBox {
    border: 1px solid #bfb8b1;
    border-radius: 6px;
    padding: 12px;
    margin: 0;
    box-sizing: border-box;
}

.wxStaticBox legend {
    padding: 0 6px;
    font-weight: 600;
    color: #1c1c1c;
}

/* ========================================
   wxCheckBox / wxRadioButton / wxRadioBox
   ======================================== */

.wxCheckBox,
.wxRadioButton {
    display: flex;
    align-items: center;
    gap: 6px;
    cursor: pointer;
    user-select: none;
    color: #1c1c1c;
}

.wxRadioBox {
    border: 1px solid #bfb8b1;
    border-radius: 6px;
    padding: 8px 12px;
    margin: 0;
    box-sizing: border-box;
}

.wxRadioBox legend {
    padding: 0 6px;
    font-weight: 600;
    color: #1c1c1c;
}

.wxRadioBox label {
    display: flex;
    align-items: center;
    gap: 6px;
    padding: 2px 4px;
    cursor: pointer;
    user-select: none;
}

/* ========================================
   wxCheckListBox
   ======================================== */

.wxCheckListBox {
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 4px 0;
    overflow: auto;
    box-sizing: border-box;
}

.wxCheckListBox label {
    display: flex;
    align-items: center;
    gap: 6px;
    padding: 3px 10px;
    cursor: pointer;
    user-select: none;
    transition: background-color 0.1s ease;
}

.wxCheckListBox label:hover {
    background-color: #e8f4fd;
}

/* ========================================
   wxChoice / wxListBox (native select)
   ======================================== */

.wxChoice {
    display: block;
    width: 100%;
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
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

.wxChoice:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxChoice:disabled {
    background: #f6f5f4;
    color: #9b9997;
}

.wxListBox {
    display: block;
    width: 100%;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 4px 0;
    font-size: 14px;
    color: #1c1c1c;
    outline: none;
    font-family: inherit;
    box-sizing: border-box;
}

.wxListBox:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxListBox option {
    padding: 4px 10px;
}

.wxListBox option:checked {
    background: #3584e4;
    color: #ffffff;
}

/* ========================================
   wxSlider / wxScrollBar (input range)
   ======================================== */

.wxSlider,
.wxScrollBar {
    accent-color: #3584e4;
    cursor: pointer;
    background: transparent;
}

.wxSlider:disabled,
.wxScrollBar:disabled {
    cursor: not-allowed;
    opacity: 0.5;
}

/* ========================================
   wxSpinCtrl (input number)
   ======================================== */

.wxSpinCtrl {
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

.wxSpinCtrl:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxSpinCtrl:disabled {
    background: #f6f5f4;
    color: #9b9997;
}

/* ========================================
   Pickers (colour/file/dir/date/time/calendar)
   ======================================== */

.wxColourPickerWidget {
    min-width: 48px;
    min-height: 32px;
    padding: 2px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    cursor: pointer;
    box-sizing: border-box;
}

.wxDatePickerCtrl,
.wxTimePickerCtrl,
.wxCalendarCtrl {
    display: block;
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

.wxDatePickerCtrl:focus,
.wxTimePickerCtrl:focus,
.wxCalendarCtrl:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxFilePickerWidget,
.wxDirPickerWidget {
    font-family: inherit;
    font-size: 13px;
    color: #1c1c1c;
}

.wxFilePickerWidget::file-selector-button,
.wxDirPickerWidget::file-selector-button {
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 6px 16px;
    margin-right: 8px;
    font-family: inherit;
    font-size: 13px;
    color: #1c1c1c;
    cursor: pointer;
    transition: all 0.15s ease;
}

.wxFilePickerWidget::file-selector-button:hover,
.wxDirPickerWidget::file-selector-button:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

/* ========================================
   wxFontPickerWidget
   ======================================== */

.wxFontPickerWidget {
    display: flex;
    align-items: center;
    gap: 6px;
}

.wxFontPickerWidget select,
.wxFontDialog select {
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 6px 8px;
    font-size: 13px;
    color: #1c1c1c;
    outline: none;
    font-family: inherit;
}

.wxFontPickerWidget select:focus,
.wxFontDialog select:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

.wxFontPickerWidget input,
.wxFontDialog input[type="number"] {
    width: 70px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    padding: 6px 8px;
    font-size: 13px;
    color: #1c1c1c;
    outline: none;
    font-family: inherit;
    box-sizing: border-box;
}

.wxFontPickerWidget input:focus,
.wxFontDialog input[type="number"]:focus {
    border-color: #3584e4;
    box-shadow: 0 0 0 2px rgba(53, 132, 228, 0.2);
}

/* ========================================
   wxToggleButton
   ======================================== */

.wxToggleButton {
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

.wxToggleButton:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

/* Pressed state */
.wxToggleButton.pressed {
    background: #d9d7d4;
    border-color: #a0a0a0;
    box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.15);
}

/* ========================================
   wxNotebook
   ======================================== */

.wxNotebook-tabs {
    display: flex;
    align-items: flex-end;
    gap: 2px;
    background: #f6f5f4;
    border-bottom: 1px solid #bfb8b1;
    padding: 4px 4px 0;
    box-sizing: border-box;
}

.wxNotebook-tab {
    background: transparent;
    border: 1px solid transparent;
    border-bottom: none;
    border-radius: 6px 6px 0 0;
    padding: 6px 14px;
    margin-bottom: -1px;
    font-family: inherit;
    font-size: 13px;
    color: #5c5c5c;
    cursor: pointer;
    transition: background-color 0.1s ease, color 0.1s ease;
}

.wxNotebook-tab:hover {
    background: #e8e7e6;
    color: #1c1c1c;
}

.wxNotebook-tab.active {
    background: #ffffff;
    border-color: #bfb8b1;
    border-bottom-color: #ffffff;
    color: #1c1c1c;
}

.wxNotebook-pages {
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-top: none;
    flex: 1;
    overflow: auto;
    box-sizing: border-box;
}

/* ========================================
   wxToolBar
   ======================================== */

.wxToolBar {
    display: flex;
    align-items: center;
    gap: 2px;
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 4px 6px;
    box-sizing: border-box;
}

.wxToolBar-tool {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    background: transparent;
    border: 1px solid transparent;
    border-radius: 4px;
    padding: 5px;
    font-family: inherit;
    font-size: 13px;
    color: #1c1c1c;
    cursor: pointer;
    transition: background-color 0.1s ease;
}

.wxToolBar-tool:hover {
    background: #e8e7e6;
    border-color: #bfb8b1;
}

/* Pressed toggle tool */
.wxToolBar-tool.active {
    background: #d9d7d4;
    border-color: #bfb8b1;
    box-shadow: inset 0 1px 2px rgba(0, 0, 0, 0.1);
}

.wxToolBar-tool:disabled {
    opacity: 0.5;
    cursor: not-allowed;
}

.wxToolBar-separator {
    width: 1px;
    align-self: stretch;
    background: #bfb8b1;
    margin: 2px 4px;
}

/* ========================================
   wxTreeCtrl
   ======================================== */

.wxTreeCtrl {
    list-style: none;
    margin: 0;
    padding: 4px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    overflow: auto;
    font-size: 14px;
    color: #1c1c1c;
    user-select: none;
    box-sizing: border-box;
}

/* Nested <ul> elements have no class (20px inline indentation) */
.wxTreeCtrl ul {
    list-style: none;
    margin: 0;
}

.wxTreeToggle {
    display: inline-block;
    width: 16px;
    text-align: center;
    cursor: pointer;
    color: #9b9997;
}

.wxTreeToggle:hover {
    color: #5c5c5c;
}

.wxTreeLabel {
    display: inline-block;
    padding: 2px 6px;
    border-radius: 4px;
    transition: background-color 0.1s ease;
}

.wxTreeLabel:hover {
    background: #e8f4fd;
}

/* The selected class is applied to the <li>, not to the label */
.wxTreeCtrl li.selected > .wxTreeLabel {
    background: #3584e4;
    color: #ffffff;
}

/* ========================================
   wxMDI (client area and child windows)
   ======================================== */

.wxMDIClient {
    background: #e8e7e6;
    position: relative;
    overflow: hidden;
}

.wxMDIChild {
    box-shadow: 0 6px 24px rgba(0, 0, 0, 0.35);
}

/* ========================================
   wxPopupWindow
   ======================================== */

.wxPopupWindow {
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 6px;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
    box-sizing: border-box;
}

/* ========================================
   wxColourDialog (custom dialog)
   ======================================== */

.wxColourDialog {
    border: none;
    border-radius: 8px;
    padding: 0;
    background: #ffffff;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
    min-width: 280px;
}

.wxColourDialog::backdrop {
    background: rgba(0, 0, 0, 0.4);
    backdrop-filter: blur(2px);
}

.wxColourDialog-content {
    display: flex;
    flex-direction: column;
}

.wxColourDialog-header {
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 12px 16px;
    font-size: 16px;
    font-weight: 600;
    color: #1c1c1c;
    border-radius: 8px 8px 0 0;
}

.wxColourDialog-picker {
    display: flex;
    justify-content: center;
    padding: 24px;
}

.wxColourDialog-input {
    width: 120px;
    height: 48px;
    padding: 2px;
    background: #ffffff;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    cursor: pointer;
}

.wxColourDialog-buttons {
    display: flex;
    justify-content: flex-end;
    gap: 8px;
    padding: 16px 24px;
    border-top: 1px solid #e0e0e0;
    background: #fafafa;
    border-radius: 0 0 8px 8px;
}

.wxColourDialog-button {
    padding: 8px 20px;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    color: #1c1c1c;
    font-size: 14px;
    cursor: pointer;
    transition: all 0.15s ease;
    min-width: 80px;
    font-family: inherit;
}

.wxColourDialog-button:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

.wxColourDialog-button-default {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    color: #ffffff;
    border-color: #1c6bc4;
}

.wxColourDialog-button-default:hover {
    background: linear-gradient(to bottom, #2a76d4, #165cad);
}

/* ========================================
   wxFontDialog (custom dialog)
   ======================================== */

.wxFontDialog {
    border: none;
    border-radius: 8px;
    padding: 0;
    background: #ffffff;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.25);
    min-width: 380px;
}

.wxFontDialog::backdrop {
    background: rgba(0, 0, 0, 0.4);
    backdrop-filter: blur(2px);
}

.wxFontDialog-content {
    display: flex;
    flex-direction: column;
}

.wxFontDialog-header {
    background: linear-gradient(to bottom, #f6f5f4, #e8e7e6);
    border-bottom: 1px solid #bfb8b1;
    padding: 12px 16px;
    font-size: 16px;
    font-weight: 600;
    color: #1c1c1c;
    border-radius: 8px 8px 0 0;
}

.wxFontDialog-row {
    display: flex;
    align-items: center;
    gap: 8px;
    padding: 6px 24px;
}

.wxFontDialog-row:first-of-type {
    padding-top: 20px;
}

.wxFontDialog-label {
    width: 90px;
    flex-shrink: 0;
    font-size: 13px;
    color: #5c5c5c;
}

.wxFontDialog-row select {
    flex: 1;
}

.wxFontDialog-buttons {
    display: flex;
    justify-content: flex-end;
    gap: 8px;
    padding: 16px 24px;
    margin-top: 12px;
    border-top: 1px solid #e0e0e0;
    background: #fafafa;
    border-radius: 0 0 8px 8px;
}

.wxFontDialog-button {
    padding: 8px 20px;
    border: 1px solid #bfb8b1;
    border-radius: 4px;
    background: linear-gradient(to bottom, #ffffff, #f0f0f0);
    color: #1c1c1c;
    font-size: 14px;
    cursor: pointer;
    transition: all 0.15s ease;
    min-width: 80px;
    font-family: inherit;
}

.wxFontDialog-button:hover {
    background: linear-gradient(to bottom, #f5f5f5, #e8e8e8);
    border-color: #a0a0a0;
}

.wxFontDialog-button-default {
    background: linear-gradient(to bottom, #3584e4, #1c6bc4);
    color: #ffffff;
    border-color: #1c6bc4;
}

.wxFontDialog-button-default:hover {
    background: linear-gradient(to bottom, #2a76d4, #165cad);
}

/* ========================================
   wxMediaCtrl (<video> HTML5)
   ======================================== */

.wxMediaCtrl {
    background: #000000;
    object-fit: contain;
    width: 100%;
    height: 100%;
}

/* ========================================
   Miscellaneous (bitmap combo, menu, infobar, collpane)
   ======================================== */

/* Bitmap next to the selected item of the wxBitmapComboBox */
.wxBitmapComboBox-bitmap {
    display: inline-block;
    vertical-align: middle;
    margin-right: 6px;
    object-fit: contain;
}

/* Bitmap of a wxMenuItem (img inside the flex item) */
.wxMenuItem-bitmap {
    width: 16px;
    height: 16px;
    margin-right: 8px;
    object-fit: contain;
    flex-shrink: 0;
}

/* "Do not show again" checkbox of the wxInfoBar */
.wxInfoBar-checkbox {
    display: inline-flex;
    align-items: center;
    gap: 4px;
    font-size: 12px;
    cursor: pointer;
    user-select: none;
}

/* Label inside the wxCollapsiblePane button */
.wxCollapsiblePane-label {
    flex: 1;
}
)CSS";

void wxWasmCSSManager::InjectDefaultStyles()
{
    if (ms_defaultStylesInjected || ms_hasCustomCSS)
        return;
    
    EM_ASM_({
        // Check whether the stylesheet already exists
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
        // Remove default styles if they exist
        var defaultStyle = document.getElementById('wxwidgets-default-styles');
        if (defaultStyle) {
            defaultStyle.remove();
        }
        
        // Remove previous custom CSS if it exists
        var customStyle = document.getElementById('wxwidgets-custom-styles');
        if (customStyle) {
            customStyle.remove();
        }
        
        // Create new style element
        var style = document.createElement('style');
        style.id = 'wxwidgets-custom-styles';
        style.textContent = UTF8ToString($0);
        document.head.appendChild(style);
    }, buffer.data());
    
    ms_hasCustomCSS = true;
}

bool wxWasmCSSManager::HasCustomCSS()
{
    return ms_hasCustomCSS;
}
