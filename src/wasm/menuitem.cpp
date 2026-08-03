/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menuitem.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/bitmap.h"
#include "wx/frame.h"

#if wxUSE_IMAGE
    #include "wx/image.h"
#endif

#include <emscripten.h>

//##############################################################################
// ProcessMenuEvent - called from JavaScript when a menu item is clicked
//##############################################################################

extern "C" EMSCRIPTEN_KEEPALIVE void ProcessMenuEvent(int menuItemId)
{
    wxMenu* parentMenu = nullptr;
    wxMenuItem* item = nullptr;

    // Search in all top-level windows for the menu item
    for (wxWindowList::const_iterator it = wxTopLevelWindows.begin();
         it != wxTopLevelWindows.end(); ++it)
    {
        wxFrame* frame = wxDynamicCast(*it, wxFrame);
        if (!frame)
            continue;

        wxMenuBar* menubar = frame->GetMenuBar();
        if (!menubar)
            continue;

        item = menubar->FindItem(menuItemId, &parentMenu);
        if (item)
            break;
    }

    if (!item || !parentMenu)
        return;

    // Send wxEVT_MENU
    wxCommandEvent event(wxEVT_MENU, menuItemId);
    event.SetEventObject(parentMenu);

    if (item->IsCheckable())
    {
        item->Toggle();
        event.SetInt(item->IsChecked() ? 1 : 0);
    }

    parentMenu->ProcessEvent(event);
}

extern "C" EMSCRIPTEN_KEEPALIVE void ProcessMenuHighlight(int menuItemId, int highlight)
{
    wxMenu* parentMenu = nullptr;
    wxMenuItem* item = nullptr;

    for (wxWindowList::const_iterator it = wxTopLevelWindows.begin();
         it != wxTopLevelWindows.end(); ++it)
    {
        wxFrame* frame = wxDynamicCast(*it, wxFrame);
        if (!frame) continue;

        wxMenuBar* menubar = frame->GetMenuBar();
        if (!menubar) continue;

        item = menubar->FindItem(menuItemId, &parentMenu);
        if (item) break;
    }

    if (!item || !parentMenu)
        return;

    wxEventType evtType = highlight ? wxEVT_MENU_HIGHLIGHT : wxEVT_MENU_CLOSE;
    wxMenuEvent event(evtType, menuItemId, parentMenu);
    event.SetEventObject(parentMenu);
    parentMenu->ProcessEvent(event);
}

//##############################################################################
// wxMenuItem
//##############################################################################

wxMenuItem *wxMenuItemBase::New(wxMenu *parentMenu, int id, const wxString& name,
    const wxString& help, wxItemKind kind, wxMenu *subMenu)
{
    return new wxMenuItem(parentMenu, id, name, help, kind, subMenu);
}

wxMenuItem::wxMenuItem(wxMenu *parentMenu, int id, const wxString& text,
        const wxString& help, wxItemKind kind, wxMenu *subMenu)
    : wxMenuItemBase(parentMenu, id, text, help, kind, subMenu)
{
}

void wxMenuItem::SetItemLabel(const wxString& label)
{
    wxMenuItemBase::SetItemLabel(label);

    // Update DOM if it exists
    wxCharBuffer buffer = label.ToUTF8();
    EM_ASM_({
        var item = document.getElementById('wxMenuItem_' + $0);
        if (item) {
            var labelElem = item.querySelector('.wxMenuItem-label');
            if (labelElem) {
                labelElem.textContent = UTF8ToString($1);
            }
        }
    }, GetId(), buffer.data());
}

void wxMenuItem::SetCheckable(bool checkable)
{
    wxMenuItemBase::SetCheckable(checkable);
    UpdateDOMState();
}

void wxMenuItem::Enable(bool enable)
{
    wxMenuItemBase::Enable(enable);
    UpdateDOMState();
}

bool wxMenuItem::IsEnabled() const
{
    return m_isEnabled;
}

void wxMenuItem::Check(bool checked)
{
    wxMenuItemBase::Check(checked);
    UpdateDOMState();
}

bool wxMenuItem::IsChecked() const
{
    return m_isChecked;
}

void wxMenuItem::SetBitmap(const wxBitmapBundle& bitmap)
{
    m_bitmap = bitmap;
    UpdateDOMBitmap();
}

void wxMenuItem::UpdateDOMBitmap()
{
#if wxUSE_IMAGE
    if (!m_bitmap.IsOk())
        return;

    wxImage image = m_bitmap.GetBitmap(m_bitmap.GetDefaultSize()).ConvertToImage();
    if (!image.IsOk())
        return;

    // Insert an <img> at the start of the item (before the text) if the
    // DOM element exists and doesn't have one yet
    EM_ASM_({
        var item = document.getElementById('wxMenuItem_' + $0);
        if (!item) return;
        if (!item.querySelector('img.wxMenuItem-bitmap')) {
            var img = document.createElement('img');
            img.className = 'wxMenuItem-bitmap';
            item.insertBefore(img, item.firstChild);
        }
    }, GetId());

    // Same pixels-to-data-URL logic as wxWasmSetImgFromPixels in
    // src/wasm/statbmp.cpp, inlined here because that helper looks the
    // container up by its numeric id, which menu items no longer use
    // (their DOM ids are namespaced as 'wxMenuItem_<id>').
    unsigned char* rgb = image.GetData();
    unsigned char* alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;
    EM_ASM_({
        var item = document.getElementById('wxMenuItem_' + $0);
        if (!item) return;
        var img = item.querySelector('img.wxMenuItem-bitmap');
        if (!img) return;

        var canvas = document.createElement('canvas');
        canvas.width = $3;
        canvas.height = $4;
        var ctx = canvas.getContext('2d');
        var imgData = ctx.createImageData($3, $4);
        var rgb = $1;
        var alpha = $2;
        var count = $3 * $4;
        for (var i = 0; i < count; i++) {
            imgData.data[i * 4 + 0] = HEAPU8[rgb + i * 3 + 0];
            imgData.data[i * 4 + 1] = HEAPU8[rgb + i * 3 + 1];
            imgData.data[i * 4 + 2] = HEAPU8[rgb + i * 3 + 2];
            imgData.data[i * 4 + 3] = alpha ? HEAPU8[alpha + i] : 255;
        }
        ctx.putImageData(imgData, 0, 0);
        img.src = canvas.toDataURL('image/png');
    }, GetId(), rgb, alpha, image.GetWidth(), image.GetHeight());
#endif // wxUSE_IMAGE
}

void *wxMenuItem::GetHandle() const
{
    return nullptr;
}

void wxMenuItem::SetFont(const wxFont& font)
{
    m_font = font;
    ApplyFontToDOM();
}

void wxMenuItem::ApplyFontToDOM()
{
    if (!m_font.IsOk())
        return;

    // CSS family: explicit face name or a generic family (same mapping as
    // the canvas font spec in src/wasm/dc.cpp)
    wxString family = m_font.GetFaceName();
    if (family.empty())
    {
        switch (m_font.GetFamily())
        {
            case wxFONTFAMILY_ROMAN:
                family = "serif";
                break;
            case wxFONTFAMILY_SCRIPT:
                family = "cursive";
                break;
            case wxFONTFAMILY_DECORATIVE:
                family = "fantasy";
                break;
            case wxFONTFAMILY_MODERN:
            case wxFONTFAMILY_TELETYPE:
                family = "monospace";
                break;
            case wxFONTFAMILY_SWISS:
            default:
                family = "sans-serif";
                break;
        }
    }

    const char* style = "normal";
    if (m_font.GetStyle() == wxFONTSTYLE_ITALIC)
        style = "italic";
    else if (m_font.GetStyle() == wxFONTSTYLE_SLANT)
        style = "oblique";

    double pointSize = m_font.GetFractionalPointSize();
    if (pointSize <= 0)
        pointSize = 12;
    int sizePx = (int)(pointSize * 96.0 / 72.0 + 0.5);

    wxCharBuffer familyBuffer = family.ToUTF8();
    EM_ASM_({
        var item = document.getElementById('wxMenuItem_' + $0);
        if (!item) return;
        item.style.fontFamily = UTF8ToString($1);
        item.style.fontSize = $2 + 'px';
        item.style.fontStyle = UTF8ToString($3);
        item.style.fontWeight = $4;
    }, GetId(), familyBuffer.data(), sizePx, style, m_font.GetNumericWeight());
}

void wxMenuItem::CreateDOM(wxMenu* parentMenu)
{
    if (!parentMenu)
        return;

    // Use wxWidgets' IsSeparator() which handles the separator kind properly
    bool isSeparator = IsSeparator();
    bool isSubMenu = GetSubMenu() != nullptr;

    wxString cssClass;
    if (isSeparator) {
        cssClass = wxT("wxMenuItem-separator");
    } else if (isSubMenu) {
        cssClass = wxT("wxSubMenu");
    } else {
        cssClass = wxT("wxMenuItem");
    }

    int parentId = parentMenu->GetId();
    int itemId = GetId();
    wxCharBuffer textBuffer = m_text.ToUTF8();

    EM_ASM_({
        // Menu popup containers are namespaced ('wxMenuPopup_' + menu id):
        // the menubar popups (menu.cpp), the floating popup of
        // wxWindowWasm::DoPopupMenu (window.cpp) and the sub-menu alias
        // below all share that scheme, so menu ids never collide with the
        // numeric wxWindowID element ids.
        var parent = document.getElementById('wxMenuPopup_' + $2);
        if (!parent) return;

        // Skip if the element already exists: DoPopupMenu (window.cpp)
        // and wxMenuBar (menu.cpp) recreate the DOM of all items once the
        // popup container exists, but the items of an already-shown menu
        // must not be duplicated.
        if (document.getElementById('wxMenuItem_' + $0)) return;

        // If parent is a sub-menu item, insert into its popup instead
        var popup = parent.querySelector('.wxSubMenu-popup');
        if (popup) {
            parent = popup;
        }

        var elem = document.createElement("div");
        elem.id = 'wxMenuItem_' + $0;
        elem.className = UTF8ToString($1);

        var className = UTF8ToString($1);
        var isSeparator = className === 'wxMenuItem-separator';
        var isSubMenu = className === 'wxSubMenu';
        var itemText = UTF8ToString($3);

        if (isSeparator) {
            // Separator: only the div with the class
        } else if (isSubMenu) {
            // Sub-menu: label + popup
            var label = document.createElement("div");
            label.className = 'wxSubMenu-label';
            label.textContent = itemText;
            elem.appendChild(label);

            // Create popup for the sub-menu
            var popup = document.createElement("div");
            popup.className = 'wxSubMenu-popup';
            popup.id = 'wxMenuItem_' + $0 + '_popup';
            elem.appendChild(popup);

            // Create an alias div so child items can find the container by
            // the sub-menu id (same 'wxMenuPopup_' namespace as the other
            // menu popup containers)
            var alias = document.createElement("div");
            alias.id = 'wxMenuPopup_' + $4;  // subMenu->GetId()
            alias.style.display = 'none';
            popup.appendChild(alias);
        } else {
            // Normal item
            var icon = document.createElement("span");
            icon.className = 'wxMenuItem-icon';
            elem.appendChild(icon);

            var label = document.createElement("span");
            label.className = 'wxMenuItem-label';
            label.textContent = itemText;
            elem.appendChild(label);

            var shortcut = document.createElement("span");
            shortcut.className = 'wxMenuItem-shortcut';
            // Parse accelerators if any (e.g. "&Open\tCtrl+O")
            var tabIndex = itemText.indexOf('\t');
            if (tabIndex >= 0) {
                shortcut.textContent = itemText.substring(tabIndex + 1);
                label.textContent = itemText.substring(0, tabIndex).replace('&', "");
            }
            elem.appendChild(shortcut);

            // Click event
            elem.onclick = function(e) {
                e.stopPropagation();
                if (!elem.classList.contains('disabled')) {
                    // Call C++ to process the event
                    if (typeof Module !== 'undefined' && Module.ccall) {
                        Module.ccall('ProcessMenuEvent', null, ['number'], [$0]);
                    }
                    // Close the menu
                    document.querySelectorAll('.wxMenuBar-menu.open').forEach(function(m) {
                        m.classList.remove('open');
                    });
                    // Close floating popups
                    document.querySelectorAll('.wxMenu-popup-floating').forEach(function(p) {
                        p.style.display = 'none';
                    });
                }
            };

            // Highlight events (mouse enter/leave)
            elem.onmouseenter = function(e) {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('ProcessMenuHighlight', null, ['number', 'number'], [$0, 1]);
                }
            };
            elem.onmouseleave = function(e) {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('ProcessMenuHighlight', null, ['number', 'number'], [$0, 0]);
                }
            };
        }

        parent.appendChild(elem);
    }, itemId, cssClass.ToUTF8().data(), parentId, textBuffer.data(),
       isSubMenu ? GetSubMenu()->GetId() : 0);

    // Update initial state
    UpdateDOMState();

    // Apply bitmap/font set before the DOM element existed
    UpdateDOMBitmap();
    if (m_font.IsOk())
        ApplyFontToDOM();
}

void wxMenuItem::UpdateDOMState()
{
    EM_ASM_({
        var item = document.getElementById('wxMenuItem_' + $0);
        if (item) {
            // Update disabled state
            if ($1) {
                item.classList.remove('disabled');
            } else {
                item.classList.add('disabled');
            }

            // Update checked state
            if ($2) {
                item.classList.add('checked');
            } else {
                item.classList.remove('checked');
            }
        }
    }, GetId(), m_isEnabled ? 1 : 0, m_isChecked ? 1 : 0);
}
