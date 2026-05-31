/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menuitem.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menuitem.h"
#include "wx/menu.h"
#include "wx/bitmap.h"
#include "wx/frame.h"
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

    // Actualizar DOM si existe
    wxCharBuffer buffer = label.ToUTF8();
    EM_ASM_({
        var item = document.getElementById($0);
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
    // TODO: Implementar bitmaps en items de menú
}

void *wxMenuItem::GetHandle() const
{
    return nullptr;
}

void wxMenuItem::SetFont(const wxFont& font)
{
    // TODO: Implementar fuentes personalizadas
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
        var parent = document.getElementById($2);
        if (!parent) return;

        // If parent is a sub-menu item, insert into its popup instead
        var popup = parent.querySelector('.wxSubMenu-popup');
        if (popup) {
            parent = popup;
        }

        var elem = document.createElement("div");
        elem.id = $0;
        elem.className = UTF8ToString($1);

        var className = UTF8ToString($1);
        var isSeparator = className === 'wxMenuItem-separator';
        var isSubMenu = className === 'wxSubMenu';
        var itemText = UTF8ToString($3);

        if (isSeparator) {
            // Separador: solo el div con la clase
        } else if (isSubMenu) {
            // Sub-menú: label + popup
            var label = document.createElement("div");
            label.className = 'wxSubMenu-label';
            label.textContent = itemText;
            elem.appendChild(label);

            // Crear popup para submenú
            var popup = document.createElement("div");
            popup.className = 'wxSubMenu-popup';
            popup.id = $0 + '_popup';
            elem.appendChild(popup);

            // Create an alias div so child items can find the container by menu ID
            var alias = document.createElement("div");
            alias.id = $4;  // subMenu->GetId()
            alias.style.display = 'none';
            popup.appendChild(alias);
        } else {
            // Item normal
            var icon = document.createElement("span");
            icon.className = 'wxMenuItem-icon';
            elem.appendChild(icon);

            var label = document.createElement("span");
            label.className = 'wxMenuItem-label';
            label.textContent = itemText;
            elem.appendChild(label);

            var shortcut = document.createElement("span");
            shortcut.className = 'wxMenuItem-shortcut';
            // Parsear aceleradores si existen (ej: "&Open\tCtrl+O")
            var tabIndex = itemText.indexOf('\t');
            if (tabIndex >= 0) {
                shortcut.textContent = itemText.substring(tabIndex + 1);
                label.textContent = itemText.substring(0, tabIndex).replace('&', '');
            }
            elem.appendChild(shortcut);

            // Evento click
            elem.onclick = function(e) {
                e.stopPropagation();
                if (!elem.classList.contains('disabled')) {
                    // Llamar a C++ para procesar el evento
                    if (typeof Module !== 'undefined' && Module.ccall) {
                        Module.ccall('ProcessMenuEvent', null, ['number'], [$0]);
                    }
                    // Cerrar el menú
                    document.querySelectorAll('.wxMenuBar-menu.open').forEach(function(m) {
                        m.classList.remove('open');
                    });
                    // Cerrar popups flotantes
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

    // Actualizar estado inicial
    UpdateDOMState();
}

void wxMenuItem::UpdateDOMState()
{
    EM_ASM_({
        var item = document.getElementById($0);
        if (item) {
            // Actualizar estado disabled
            if ($1) {
                item.classList.remove('disabled');
            } else {
                item.classList.add('disabled');
            }

            // Actualizar estado checked
            if ($2) {
                item.classList.add('checked');
            } else {
                item.classList.remove('checked');
            }
        }
    }, GetId(), m_isEnabled ? 1 : 0, m_isChecked ? 1 : 0);
}
