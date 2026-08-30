/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/menu.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/menu.h"
#include "wx/frame.h"
#include "wx/wasm/cssstyles.h"
#include "wx/menuitem.h"
#include <emscripten.h>

// Recreates the DOM of all items of a menu (recursively into sub-menus),
// defined in src/wasm/window.cpp.
void wxWasmEnsureMenuItemsDOM(wxMenu* menu);

//##############################################################################
// wxMenu
//##############################################################################

int wxMenu::GetNextMenuId()
{
    static int s_nextId = 10000;  // Start well above standard wxID values
    return s_nextId++;
}

wxMenu::wxMenu(long style)
    : wxMenuBase(style), m_id(GetNextMenuId())
{
}

wxMenu::wxMenu(const wxString& title, long style)
    : wxMenuBase(title, style), m_id(GetNextMenuId())
{
}

wxMenuItem *wxMenu::DoAppend(wxMenuItem *item)
{
    if (wxMenuBase::DoAppend(item) == nullptr)
        return nullptr;

    // Create DOM element for the item
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

    // Remove from the DOM (item element ids are namespaced, see
    // wxMenuItem::CreateDOM in src/wasm/menuitem.cpp)
    EM_ASM_({
        var elem = document.getElementById('wxMenuItem_' + $0);
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

    // Create the MenuBar DOM element
    EM_ASM_({
        var menuBar = document.createElement("div");
        menuBar.id = $0;
        menuBar.className = 'wxMenuBar';
        
        // Insert into the parentless div initially
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

wxMenuBar::~wxMenuBar()
{
    // Remove the document click listeners registered per menu container
    // (see Append/Insert), otherwise they would leak on document.
    EM_ASM_({
        var menuBar = document.getElementById($0);
        if (!menuBar) return;
        menuBar.querySelectorAll('.wxMenuBar-menu').forEach(function(c) {
            if (c._wxDocClickHandler) {
                document.removeEventListener('click', c._wxDocClickHandler);
                c._wxDocClickHandler = null;
            }
        });
    }, GetId());
}

bool wxMenuBar::Append(wxMenu *menu, const wxString& title)
{
    if (!wxMenuBarBase::Append(menu, title))
        return false;

    // Store the title in the menu itself, as the other ports do: the base
    // class only keeps the menus list and GetMenuLabel()/FindMenu() read
    // the title back from the menu.
    menu->SetTitle(title);

    // Create the menu container in the DOM
    int menuId = menu->GetId();
    // The DOM label shows the title without mnemonic markers (GTK strips
    // them too, showing the accelerator letter underlined instead — we
    // just strip).
    wxCharBuffer titleBuffer = wxControl::GetLabelText(title).ToUTF8();
    
    EM_ASM_({
        var menuBar = document.getElementById($0);
        if (!menuBar) return;
        
        // Create the menu container
        var menuContainer = document.createElement("div");
        menuContainer.id = 'menubar_menu_' + $1;
        menuContainer.className = 'wxMenuBar-menu';
        
        // Create label
        var label = document.createElement("span");
        label.className = 'wxMenuBar-label';
        label.textContent = UTF8ToString($2);
        menuContainer.appendChild(label);
        
        // Create the menu popup (item container). The id uses the
        // 'wxMenuPopup_' namespace shared with DoPopupMenu (window.cpp)
        // and the sub-menu aliases (menuitem.cpp) so that menu ids never
        // collide with the numeric wxWindowID element ids; items find this
        // container by that id in wxMenuItem::CreateDOM.
        var popup = document.createElement("div");
        popup.id = 'wxMenuPopup_' + $1;
        popup.className = 'wxMenu-popup';
        menuContainer.appendChild(popup);
        
        // Events to open/close the menu
        var isOpen = false;
        
        label.onclick = function(e) {
            e.stopPropagation();
            isOpen = !isOpen;
            if (isOpen) {
                menuContainer.classList.add('open');
                // Close other menus
                document.querySelectorAll('.wxMenuBar-menu.open').forEach(function(m) {
                    if (m !== menuContainer) m.classList.remove('open');
                });
            } else {
                menuContainer.classList.remove('open');
            }
        };
        
        // Close when clicking outside. The handler is stored as a named
        // reference on the container so it can be removed later (on
        // Insert/Remove/destruction) instead of accumulating anonymous
        // listeners on document, cf. the popup cleanup in window.cpp.
        var docClickHandler = function(e) {
            if (isOpen && !menuContainer.contains(e.target)) {
                isOpen = false;
                menuContainer.classList.remove('open');
            }
        };
        menuContainer._wxDocClickHandler = docClickHandler;
        document.addEventListener('click', docClickHandler);
        
        menuBar.appendChild(menuContainer);
    }, GetId(), menuId, titleBuffer.data());

    // Create the DOM of the items already added to this menu (items
    // appended before the menu was attached to the bar found no popup
    // container and were silently skipped by CreateDOM); recurses into
    // sub-menus.
    wxWasmEnsureMenuItemsDOM(menu);

    return true;
}

bool wxMenuBar::Insert(size_t pos, wxMenu *menu, const wxString& title)
{
    if (!wxMenuBarBase::Insert(pos, menu, title))
        return false;

    // Same as in Append(): keep the title in the menu (see there).
    menu->SetTitle(title);

    // Re-append all menus in correct DOM order
    int menuBarId = GetId();
    EM_ASM_({
        var menuBar = document.getElementById($0);
        if (!menuBar) return;
        // Detach all existing menu containers, removing their document
        // click listeners first (they close over the container and would
        // otherwise leak and reference detached elements).
        var containers = Array.from(menuBar.querySelectorAll('.wxMenuBar-menu'));
        containers.forEach(function(c) {
            if (c._wxDocClickHandler) {
                document.removeEventListener('click', c._wxDocClickHandler);
                c._wxDocClickHandler = null;
            }
            menuBar.removeChild(c);
        });
    }, menuBarId);

    const size_t count = GetMenuCount();
    for (size_t i = 0; i < count; ++i)
    {
        wxMenu* m = GetMenu(i);
        wxString lbl = GetMenuLabel(i);
        int menuId = m->GetId();
        wxCharBuffer titleBuffer = wxControl::GetLabelText(lbl).ToUTF8();
        EM_ASM_({
            var menuBar = document.getElementById($0);
            if (!menuBar) return;
            var menuContainer = document.createElement("div");
            menuContainer.id = 'menubar_menu_' + $1;
            menuContainer.className = 'wxMenuBar-menu';
            var label = document.createElement("span");
            label.className = 'wxMenuBar-label';
            label.textContent = UTF8ToString($2);
            menuContainer.appendChild(label);
            var popup = document.createElement("div");
            popup.id = 'wxMenuPopup_' + $1;
            popup.className = 'wxMenu-popup';
            menuContainer.appendChild(popup);
            var isOpen = false;
            label.onclick = function(e) {
                e.stopPropagation();
                isOpen = !isOpen;
                if (isOpen) {
                    menuContainer.classList.add('open');
                    document.querySelectorAll('.wxMenuBar-menu.open').forEach(function(m) {
                        if (m !== menuContainer) m.classList.remove('open');
                    });
                } else {
                    menuContainer.classList.remove('open');
                }
            };
            var docClickHandler = function(e) {
                if (isOpen && !menuContainer.contains(e.target)) {
                    isOpen = false;
                    menuContainer.classList.remove('open');
                }
            };
            menuContainer._wxDocClickHandler = docClickHandler;
            document.addEventListener('click', docClickHandler);
            menuBar.appendChild(menuContainer);
        }, menuBarId, menuId, titleBuffer.data());

        // Recreate DOM items for this menu (they reference the popup
        // container); recurses into sub-menus.
        wxWasmEnsureMenuItemsDOM(m);
    }

    return true;
}

wxMenu *wxMenuBar::Remove(size_t pos)
{
    wxMenu *menu = wxMenuBarBase::Remove(pos);
    if (!menu)
        return nullptr;

    // Remove from the DOM, dropping its document click listener first
    EM_ASM_({
        var elem = document.getElementById('menubar_menu_' + $0);
        if (elem) {
            if (elem._wxDocClickHandler) {
                document.removeEventListener('click', elem._wxDocClickHandler);
                elem._wxDocClickHandler = null;
            }
            elem.remove();
        }
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
        wxCharBuffer labelBuffer = wxControl::GetLabelText(label).ToUTF8();
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
    wxMenuList::compatibility_iterator node = m_menus.Item(pos);
    wxCHECK_MSG(node, wxT("invalid"), wxT("menu not found"));
    wxMenu* menu = node->GetData();
    return menu->GetTitle();
}

void wxMenuBar::Attach(wxFrame *frame)
{
    wxMenuBarBase::Attach(frame);

    // Move the menu bar to the frame
    EM_ASM_({
        var menuBar = document.getElementById($0);
        var frame = document.getElementById($1);
        
        if (menuBar && frame) {
            // Insert as the first child of the frame
            frame.insertBefore(menuBar, frame.firstChild);
            menuBar.style.display = 'flex';
        }
    }, GetId(), frame->GetId());
}

void wxMenuBar::Detach()
{
    // Move back to parentless
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