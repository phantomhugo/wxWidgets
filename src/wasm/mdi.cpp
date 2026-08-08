/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/mdi.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MDI

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/mdi.h"
#include "wx/stockitem.h"
#include "wx/vector.h"

#include <emscripten.h>

// ----------------------------------------------------------------------------
// local helpers
// ----------------------------------------------------------------------------

namespace
{

// Collect the MDI children of the parent frame client window in creation
// order (the MDI child frames are direct children of the client window).
wxVector<wxMDIChildFrame *> GetMDIChildren(wxMDIParentFrame *frame)
{
    wxVector<wxMDIChildFrame *> children;

    wxMDIClientWindowBase * const client = frame->GetClientWindow();
    if ( client )
    {
        for ( wxWindowList::const_iterator i = client->GetChildren().begin();
              i != client->GetChildren().end(); ++i )
        {
            wxMDIChildFrame * const child = wxDynamicCast(*i, wxMDIChildFrame);
            if ( child )
                children.push_back(child);
        }
    }

    return children;
}

// Activate the child following (or preceding) the currently active one,
// wrapping around at the end (or beginning) of the children list.
void AdvanceActiveChild(wxMDIParentFrame *frame, bool forward)
{
    const wxVector<wxMDIChildFrame *> children = GetMDIChildren(frame);
    if ( children.empty() )
        return;

    size_t next = 0;
    for ( size_t n = 0; n < children.size(); n++ )
    {
        if ( children[n] == frame->GetActiveChild() )
        {
            next = forward ? (n + 1) % children.size()
                           : (n + children.size() - 1) % children.size();
            break;
        }
    }

    children[next]->Activate();
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxMDIParentFrame
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame);

wxMDIParentFrame::wxMDIParentFrame()
{
}

wxMDIParentFrame::wxMDIParentFrame(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
{
    (void)Create(parent, id, title, pos, size, style, name);
}

bool wxMDIParentFrame::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    // The scrolling styles apply to the client window, not to the frame.
    style &= ~(wxHSCROLL | wxVSCROLL);

    if ( !wxFrame::Create(parent, id, title, pos, size, style, name) )
        return false;

    wxMDIClientWindow * const client = OnCreateClient();
    if ( !client->CreateClient(this) )
    {
        delete client;
        return false;
    }

    m_clientWindow = client;

#if wxUSE_MENUS
    // Unless told otherwise, create the standard "Window" menu with the
    // same items as the native MSW implementation; it is inserted in the
    // menu bar by SetMenuBar() (which may only be called later).
    if ( !(style & wxFRAME_NO_WINDOW_MENU) )
    {
        m_windowMenu = new wxMenu;

        m_windowMenu->Append(wxID_MDI_WINDOW_CASCADE, _("&Cascade"));
        m_windowMenu->Append(wxID_MDI_WINDOW_TILE_HORZ, _("Tile &Horizontally"));
        m_windowMenu->Append(wxID_MDI_WINDOW_TILE_VERT, _("Tile &Vertically"));
        m_windowMenu->AppendSeparator();
        m_windowMenu->Append(wxID_MDI_WINDOW_ARRANGE_ICONS, _("&Arrange Icons"));
        m_windowMenu->Append(wxID_MDI_WINDOW_NEXT, _("&Next"));
        m_windowMenu->Append(wxID_MDI_WINDOW_PREV, _("&Previous"));

        Bind(wxEVT_MENU, [this](wxCommandEvent&) { Cascade(); },
             wxID_MDI_WINDOW_CASCADE);
        Bind(wxEVT_MENU, [this](wxCommandEvent&) { Tile(wxHORIZONTAL); },
             wxID_MDI_WINDOW_TILE_HORZ);
        Bind(wxEVT_MENU, [this](wxCommandEvent&) { Tile(wxVERTICAL); },
             wxID_MDI_WINDOW_TILE_VERT);
        Bind(wxEVT_MENU, [this](wxCommandEvent&) { ArrangeIcons(); },
             wxID_MDI_WINDOW_ARRANGE_ICONS);
        Bind(wxEVT_MENU, [this](wxCommandEvent&) { ActivateNext(); },
             wxID_MDI_WINDOW_NEXT);
        Bind(wxEVT_MENU, [this](wxCommandEvent&) { ActivatePrevious(); },
             wxID_MDI_WINDOW_PREV);
    }
#endif // wxUSE_MENUS

    return true;
}

void wxMDIParentFrame::Cascade()
{
    const wxVector<wxMDIChildFrame *> children = GetMDIChildren(this);
    if ( children.empty() )
        return;

    // Give all the children the same size: two thirds of the client area,
    // falling back to a sensible default if it is not realized yet.
    wxSize size = GetClientWindow()->GetClientSize();
    size.x = 2 * size.x / 3;
    size.y = 2 * size.y / 3;
    if ( size.x <= 0 )
        size.x = 400;
    if ( size.y <= 0 )
        size.y = 300;

    int offset = 0;
    for ( wxMDIChildFrame * const child : children )
    {
        child->SetSize(offset, offset, size.x, size.y);
        offset += 24;
    }
}

void wxMDIParentFrame::Tile(wxOrientation orient)
{
    // Only the visible children take part in the tiling.
    wxVector<wxMDIChildFrame *> children;
    for ( wxMDIChildFrame * const child : GetMDIChildren(this) )
    {
        if ( child->IsShown() )
            children.push_back(child);
    }

    const size_t count = children.size();
    if ( !count )
        return;

    const wxSize clientSize = GetClientWindow()->GetClientSize();
    if ( clientSize.x <= 0 || clientSize.y <= 0 )
        return;

    // Lay the children out in a simple grid of same-sized cells filling the
    // client area, with ceil(sqrt(n)) columns (computed without <cmath>).
    size_t cols = 1;
    while ( cols * cols < count )
        cols++;
    const size_t rows = (count + cols - 1) / cols;

    const int cellW = clientSize.x / static_cast<int>(cols);
    const int cellH = clientSize.y / static_cast<int>(rows);

    for ( size_t n = 0; n < count; n++ )
    {
        // Fill row by row for horizontal tiling, column by column for
        // vertical tiling.
        const size_t col = orient == wxVERTICAL ? n / rows : n % cols;
        const size_t row = orient == wxVERTICAL ? n % rows : n / cols;
        children[n]->SetSize(static_cast<int>(col) * cellW,
                             static_cast<int>(row) * cellH,
                             cellW, cellH);
    }
}

void wxMDIParentFrame::ArrangeIcons()
{
    // No-op: the children are never really iconized in this port (there are
    // no minimized icons in the client area), so there is nothing to do.
}

#if wxUSE_MENUS
void wxMDIParentFrame::SetWindowMenu(wxMenu *menu)
{
    // Do nothing if the application passes the current window menu back to
    // us (the mdi sample does exactly this after customizing it in place):
    // deleting it and re-adding the same pointer would use a dangling menu.
    if ( menu == m_windowMenu )
        return;

    // Replace the window menu in the currently used menu bar.
    wxMenuBar * const menuBar = GetMenuBar();

    if ( m_windowMenu )
    {
        RemoveWindowMenu(menuBar);

        wxDELETE(m_windowMenu);
    }

    if ( menu )
    {
        m_windowMenu = menu;

        AddWindowMenu(menuBar);
    }
}

void wxMDIParentFrame::SetMenuBar(wxMenuBar *menuBar)
{
    // Remove the Window menu from the old menu bar and add it to the new
    // one, as the generic MDI implementation does.
    RemoveWindowMenu(GetMenuBar());
    AddWindowMenu(menuBar);

    wxFrame::SetMenuBar(menuBar);
}

void wxMDIParentFrame::AddWindowMenu(wxMenuBar *menuBar)
{
    if ( menuBar && m_windowMenu )
    {
        const int pos = menuBar->FindMenu(wxGetStockLabel(wxID_HELP, false));
        if ( pos == wxNOT_FOUND )
        {
            menuBar->Append(m_windowMenu, _("&Window"));
        }
        else
        {
            menuBar->Insert(pos, m_windowMenu, _("&Window"));
        }
    }
}

void wxMDIParentFrame::RemoveWindowMenu(wxMenuBar *menuBar)
{
    if ( menuBar && m_windowMenu )
    {
        // Remove old window menu.
        const int pos = menuBar->FindMenu(_("&Window"));
        if ( pos != wxNOT_FOUND )
        {
            wxASSERT(m_windowMenu == menuBar->GetMenu(pos));
            menuBar->Remove(pos);
        }
    }
}
#endif // wxUSE_MENUS

void wxMDIParentFrame::ActivateNext()
{
    AdvanceActiveChild(this, true);
}

void wxMDIParentFrame::ActivatePrevious()
{
    AdvanceActiveChild(this, false);
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxMDIChildFrameBase)

wxMDIChildFrame::wxMDIChildFrame()
{
}

wxMDIChildFrame::wxMDIChildFrame(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create(parent, id, title, pos, size, style, name);
}

wxMDIChildFrame::~wxMDIChildFrame()
{
    // Don't leave a dangling pointer to us in the parent frame.
    if ( m_mdiParent && m_mdiParent->GetActiveChild() == this )
        m_mdiParent->SetActiveChild(nullptr);
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    wxCHECK_MSG( parent, false, "MDI child frame must have a parent" );

    m_mdiParent = parent;

    wxMDIClientWindowBase * const client = parent->GetClientWindow();
    wxCHECK_MSG( client, false, "MDI parent frame must be created first" );

    // Create the frame inside the client window: its <div> is appended to
    // the client area element by wxWindowWasm::PostCreation().
    if ( !wxFrame::Create(client, id, title, pos, size, style, name) )
        return false;

    // Mark the element as an MDI child.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem)
            elem.classList.add('wxMDIChild');
    }, GetId());

    // Give the child a sensible geometry: wxTopLevelWindowWasm::Create()
    // forces an hardcoded size and a default position would leave the
    // element without any CSS geometry at all.
    wxSize childSize = size;
    if ( childSize == wxDefaultSize )
        childSize = wxSize(400, 300);

    wxPoint childPos = pos;
    if ( childPos == wxDefaultPosition )
    {
        // Cascade the children: our own window is already in the list, so
        // the first child gets the offset 0.
        const size_t count = GetMDIChildren(parent).size();
        const int offset = 24 * ((count - 1) % 10);
        childPos = wxPoint(offset, offset);
    }

    SetSize(childPos.x, childPos.y, childSize.x, childSize.y);

    // The newly created child becomes the active one.
    Activate();

    return true;
}

void wxMDIChildFrame::Activate()
{
    wxMDIParentFrame * const parent = GetMDIParent();
    wxCHECK_RET( parent, "can't activate MDI child without parent" );

    if ( parent->GetActiveChild() == this )
        return; // already active

    // Notify the previously active child that it has been deactivated.
    if ( wxMDIChildFrame * const oldActive = parent->GetActiveChild() )
    {
        wxActivateEvent event(wxEVT_ACTIVATE, false, oldActive->GetId());
        event.SetEventObject(oldActive);
        oldActive->HandleWindowEvent(event);
    }

    // Bring our div to the front of the client area by giving it a z-index
    // higher than any of its siblings.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem || !elem.parentElement) return;
        var maxZ = 0;
        var siblings = elem.parentElement.children;
        for (var i = 0; i < siblings.length; i++) {
            if (siblings[i] === elem) continue;
            var z = parseInt(siblings[i].style.zIndex || '0', 10);
            if (z > maxZ) maxZ = z;
        }
        elem.style.zIndex = (maxZ + 1).toString();
    }, GetId());

    parent->SetActiveChild(this);

    SetFocus();

    // Notify that we have been activated.
    wxActivateEvent event(wxEVT_ACTIVATE, true, GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow,wxMDIClientWindowBase)

wxMDIClientWindow::wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    // Create the window itself: this generates the container <div> for the
    // MDI client area inside the parent frame element.
    if ( !wxWindow::Create(parent, wxID_ANY,
                           wxDefaultPosition, wxDefaultSize, style) )
        return false;

    // Move the client div into the frame content area (children are normally
    // appended to the frame div itself) and make it fill it: the MDI child
    // frames are positioned absolutely inside it.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        elem.className = 'wxMDIClient';
        elem.style.position = 'absolute';
        elem.style.left = '0px';
        elem.style.top = '0px';
        elem.style.width = '100%';
        elem.style.height = '100%';
        var content = document.getElementById('wxFrame_content_' + $1);
        if (content)
            content.appendChild(elem);
    }, GetId(), parent->GetId());

    return true;
}

#endif // wxUSE_MDI
