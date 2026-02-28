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
#include <emscripten.h>

// Función externa para procesar eventos de menú desde JavaScript
extern "C" EMSCRIPTEN_KEEPALIVE void ProcessMenuEvent(int menuItemId)
{
    // TODO: Enviar evento a wxWidgets
    // Esto requiere implementar el sistema de eventos completo
}

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
        
    // Determinar el tipo de elemento a crear
    bool isSeparator = m_text == wxT("---") || m_text == wxT("-");
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
            var tabIndex = itemText.indexOf('\\t');
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
                }
            };
        }
        
        parent.appendChild(elem);
    }, itemId, cssClass.ToUTF8().data(), parentId, textBuffer.data());
    
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
