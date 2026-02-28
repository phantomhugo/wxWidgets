/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menu.cpp
// Author:      Peter Most, Mariano Reingart, Hugo Castellanos
// Copyright:   (c) 2010 wxWidgets dev team, 2024 updates
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menu.h"
#include "wx/frame.h"
#include "wx/wasm/cssstyles.h"
#include "wx/menuitem.h"
#include <emscripten.h>

// Helper para crear elementos DOM
static void CreateDOMElement(int id, const char* tag, const char* className, 
                              int parentId)
{
    EM_ASM_({
        var elem = document.createElement(UTF8ToString($1));
        elem.id = $0;
        elem.className = UTF8ToString($2);
        
        var parent = document.getElementById($3);
        if (parent) {
            parent.appendChild(elem);
        }
    }, id, tag, className, parentId);
}

//##############################################################################
// wxMenu
//##############################################################################

wxMenu::wxMenu(long style)
    : wxMenuBase(style)
{
}

wxMenu::wxMenu(const wxString& title, long style)
    : wxMenuBase(title, style)
{
}

wxMenuItem *wxMenu::DoAppend(wxMenuItem *item)
{
    if (wxMenuBase::DoAppend(item) == nullptr)
        return nullptr;
    
    // Crear elemento DOM para el item
    item->CreateDOM(this);
    
    return item;
}

wxMenuItem *wxMenu::DoInsert(size_t insertPosition, wxMenuItem *item)
{
    if (wxMenuBase::DoInsert(insertPosition, item) == nullptr)
        return nullptr;

    item->CreateDOM(this);
    return item;
}

wxMenuItem *wxMenu::DoRemove(wxMenuItem *item)
{
    if (wxMenuBase::DoRemove(item) == nullptr)
        return nullptr;

    // Remover del DOM
    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem) elem.remove();
    }, item->GetId());

    return item;
}

void *wxMenu::GetHandle() const
{
    return nullptr;
}

//##############################################################################
// wxMenuBar
//##############################################################################

wxMenuBar::wxMenuBar(long style)
{
    wxWasmCreateMenuBar(style);
    PostCreation(false);
}

void wxMenuBar::wxWasmCreateMenuBar(long style)
{
    if (!CreateBase(nullptr, -1, wxDefaultPosition, wxDefaultSize, style, 
                    wxDefaultValidator, wxT("menubar")))
    {
        wxFAIL_MSG(wxT("wxMenuBar creation failed"));
        return;
    }

    // Crear el elemento DOM del MenuBar
    EM_ASM_({
        var menuBar = document.createElement("div");
        menuBar.id = $0;
        menuBar.className = 'wxMenuBar';
        
        // Insertar en el div de parentless inicialmente
        var parentlessDiv = document.getElementById("wxParentlessTags");
        if (parentlessDiv) {
            parentlessDiv.appendChild(menuBar);
        }
    }, GetId());
}

wxMenuBar::wxMenuBar(size_t count, wxMenu *menus[], const wxString titles[], long style)
{
    wxWasmCreateMenuBar(style);

    for (size_t i = 0; i < count; ++i)
        Append(menus[i], titles[i]);

    PostCreation(false);
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    if (!wxMenuBarBase::Append(menu, title))
        return false;

    // Crear el contenedor del menú en el DOM
    int menuId = menu->GetId();
    wxCharBuffer titleBuffer = title.ToUTF8();
    
    EM_ASM_({
        var menuBar = document.getElementById($0);
        if (!menuBar) return;
        
        // Crear contenedor del menú
        var menuContainer = document.createElement("div");
        menuContainer.id = 'menubar_menu_' + $1;
        menuContainer.className = 'wxMenuBar-menu';
        
        // Crear label
        var label = document.createElement("span");
        label.className = 'wxMenuBar-label';
        label.textContent = UTF8ToString($2);
        menuContainer.appendChild(label);
        
        // Crear popup del menú (contenedor de items)
        var popup = document.createElement("div");
        popup.id = $1;  // Usar ID del menú para que los items se agreguen aquí
        popup.className = 'wxMenu-popup';
        menuContainer.appendChild(popup);
        
        // Eventos para abrir/cerrar el menú
        var isOpen = false;
        
        label.onclick = function(e) {
            e.stopPropagation();
            isOpen = !isOpen;
            if (isOpen) {
                menuContainer.classList.add('open');
                // Cerrar otros menús
                document.querySelectorAll('.wxMenuBar-menu.open').forEach(function(m) {
                    if (m !== menuContainer) m.classList.remove('open');
                });
            } else {
                menuContainer.classList.remove('open');
            }
        };
        
        // Cerrar al hacer click fuera
        document.addEventListener('click', function(e) {
            if (isOpen && !menuContainer.contains(e.target)) {
                isOpen = false;
                menuContainer.classList.remove('open');
            }
        });
        
        menuBar.appendChild(menuContainer);
    }, GetId(), menuId, titleBuffer.data());

    return true;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if (!wxMenuBarBase::Insert(pos, menu, title))
        return false;

    // TODO: Manejar inserción en posición específica en el DOM
    // Por ahora, simplemente re-append todos los menús
    
    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if (!menu)
        return nullptr;

    // Remover del DOM
    EM_ASM_({
        var elem = document.getElementById('menubar_menu_' + $0);
        if (elem) elem.remove();
    }, menu->GetId());

    return menu;
}

void wxMenuBar::EnableTop(size_t pos, bool enable)
{
    wxMenu* menu = GetMenu(pos);
    if (menu) {
        EM_ASM_({
            var elem = document.getElementById('menubar_menu_' + $0);
            if (elem) {
                if ($1) {
                    elem.classList.remove('disabled');
                } else {
                    elem.classList.add('disabled');
                }
            }
        }, menu->GetId(), enable ? 1 : 0);
    }
}

bool wxMenuBar::IsEnabledTop(size_t pos) const
{
    wxMenu* menu = GetMenu(pos);
    if (menu) {
        return EM_ASM_INT({
            var elem = document.getElementById('menubar_menu_' + $0);
            return elem && !elem.classList.contains('disabled');
        }, menu->GetId());
    }
    return false;
}

void wxMenuBar::SetMenuLabel(size_t pos, const wxString& label)
{
    wxMenu* menu = GetMenu(pos);
    if (menu) {
        wxCharBuffer labelBuffer = label.ToUTF8();
        EM_ASM_({
            var container = document.getElementById('menubar_menu_' + $0);
            if (container) {
                var labelElem = container.querySelector('.wxMenuBar-label');
                if (labelElem) {
                    labelElem.textContent = UTF8ToString($1);
                }
            }
        }, menu->GetId(), labelBuffer.data());
    }
}

wxString wxMenuBar::GetMenuLabel(size_t pos) const
{
    return wxMenuBarBase::GetMenuLabel(pos);
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach(frame);

    // Mover el menú al frame
    EM_ASM_({
        var menuBar = document.getElementById($0);
        var frame = document.getElementById($1);
        
        if (menuBar && frame) {
            // Insertar como primer hijo del frame
            frame.insertBefore(menuBar, frame.firstChild);
            menuBar.style.display = 'flex';
        }
    }, GetId(), frame->GetId());
}

void wxMenuBar::Detach()
{
    // Mover de vuelta a parentless
    EM_ASM_({
        var menuBar = document.getElementById($0);
        var parentless = document.getElementById("wxParentlessTags");
        
        if (menuBar && parentless) {
            parentless.appendChild(menuBar);
            menuBar.style.display = 'none';
        }
    }, GetId());

    wxMenuBarBase::Detach();
}

WXWidget wxMenuBar::GetHandle() const
{
    return nullptr;
}
