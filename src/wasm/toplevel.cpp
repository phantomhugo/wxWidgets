/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/toplevel.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/toplevel.h"
#endif

#include "wx/wasm/toplevel.h"

#include <emscripten.h>

// ----------------------------------------------------------------------------
// Browser-level notifications for top-level windows
// ----------------------------------------------------------------------------

// C functions exposed to JavaScript (see RegisterTopLevelBrowserListeners).

// Forward the real CSS size of a top-level window as a wxSizeEvent.
// The event is queued (not handled synchronously from the DOM listener) to
// keep the flat dispatch discipline of the event loop.
extern "C" EMSCRIPTEN_KEEPALIVE void wxWasmTopLevelResized(int id, int w, int h)
{
    wxWindow *win = wxWindow::FindWindowById(id);
    if ( win )
    {
        wxSizeEvent *event = new wxSizeEvent(wxSize(w, h), id);
        event->SetEventObject(win);
        win->GetEventHandler()->QueueEvent(event);
    }
}

// Generate a wxActivateEvent when the focus enters/leaves a top-level window.
extern "C" EMSCRIPTEN_KEEPALIVE void wxWasmTopLevelActivated(int id, int active)
{
    wxWindow *win = wxWindow::FindWindowById(id);
    if ( win )
    {
        wxActivateEvent *event = new wxActivateEvent(wxEVT_ACTIVATE,
                                                     active != 0, id);
        event->SetEventObject(win);
        win->GetEventHandler()->QueueEvent(event);
    }
}

// Register once (from the first Create()) the global browser listeners:
// a window "resize" listener that forwards the real size of every shown
// parentless top-level window (they typically use 100vw/100vh and get no
// other notification) and focusin/blur listeners that report focus changes
// between top-level windows as wxActivateEvent.
static void RegisterTopLevelBrowserListeners()
{
    static bool s_listenersRegistered = false;
    if ( s_listenersRegistered )
        return;
    s_listenersRegistered = true;

    EM_ASM({
        window.addEventListener('resize', function() {
            var elems = document.querySelectorAll('.wxTopLevelWindow');
            for (var i = 0; i < elems.length; i++) {
                var el = elems[i];
                // Only shown parentless windows (appended to <body>) track
                // the viewport; MDI children and hidden windows are skipped.
                if (el.parentElement !== document.body || el.style.display === 'none')
                    continue;
                Module.ccall('wxWasmTopLevelResized', null,
                             ['number', 'number', 'number'],
                             [parseInt(el.id, 10), el.offsetWidth, el.offsetHeight]);
            }
        });

        // Capture phase, so the focus change is seen before the target.
        var lastActiveId = 0;
        function topLevelOf(node) {
            while (node && node !== document) {
                if (node.classList && node.classList.contains('wxTopLevelWindow'))
                    return node;
                node = node.parentNode;
            }
            return null;
        }
        document.addEventListener('focusin', function(e) {
            var tlw = topLevelOf(e.target);
            var id = tlw ? parseInt(tlw.id, 10) : 0;
            if (id === lastActiveId)
                return;
            if (lastActiveId) {
                Module.ccall('wxWasmTopLevelActivated', null,
                             ['number', 'number'], [lastActiveId, 0]);
            }
            lastActiveId = id;
            if (id) {
                Module.ccall('wxWasmTopLevelActivated', null,
                             ['number', 'number'], [id, 1]);
            }
        }, true);
        // The page itself losing focus deactivates the current window.
        window.addEventListener('blur', function() {
            if (lastActiveId) {
                Module.ccall('wxWasmTopLevelActivated', null,
                             ['number', 'number'], [lastActiveId, 0]);
                lastActiveId = 0;
            }
        });
    });
}

wxTopLevelWindowWasm::wxTopLevelWindowWasm()
    : m_maximized(false),
      m_iconized(false),
      m_fullscreen(false)
{
}

wxTopLevelWindowWasm::wxTopLevelWindowWasm(wxWindow *parent,
            wxWindowID winid,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
    : m_maximized(false),
      m_iconized(false),
      m_fullscreen(false)
{
    Create(parent,winid,title,pos,size,style,name);
}

bool wxTopLevelWindowWasm::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    // wxWindow::Create already creates a <div> with id = GetId().
    // We must NOT create a second div; instead we style the existing one.
    bool result = wxWindow::Create(parent,id,pos,size,style,name);

    RegisterTopLevelBrowserListeners();

    SetTitle(title);

    EM_ASM_INT(
        {
            const tlw = document.getElementById($0);
            if (tlw) {
                tlw.className = "wxTopLevelWindow";
                tlw.style.display = "none";
                tlw.style.position = "absolute";
            }
            return 1;
        },
        GetId()
    );

    if(parent == nullptr)
    {
        EM_ASM_INT(
        {
            document.body.style.height="100%";
            document.body.style.width="100%";
            const currentWindow=document.getElementById($0);
            if (currentWindow) {
                currentWindow.style.height="100%";
                currentWindow.style.width="100%";
            }
            return 1;
        },
        GetId()
    );
    }

    // Honour an explicit pos/size from the caller instead of forcing every
    // top-level window to 800x600 at (0,0): this broke wxMDIChildFrame and
    // dialogs created with their own size. Derived classes (wxFrame,
    // wxDialog) may still call SetSize() afterwards with their own defaults.
    wxPoint actualPos = pos;
    wxSize actualSize = size;
    if ( actualPos == wxDefaultPosition )
        actualPos = wxPoint(0, 0);
    if ( actualSize == wxDefaultSize )
        actualSize = wxSize(800, 600);
    SetSize(actualPos.x, actualPos.y, actualSize.x, actualSize.y);
    return result;
}

void wxTopLevelWindowWasm::Maximize(bool maximize)
{
    if ( maximize == m_maximized )
        return;

    m_maximized = maximize;

    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        if ($1) {
            // Save the current geometry so Restore() can bring it back.
            elem.dataset.wxPrevLeft = elem.style.left || "";
            elem.dataset.wxPrevTop = elem.style.top || "";
            elem.dataset.wxPrevWidth = elem.style.width || "";
            elem.dataset.wxPrevHeight = elem.style.height || "";

            elem.style.left = '0px';
            elem.style.top = '0px';
            // Parentless windows are appended to <body>: use the viewport.
            // Parented ones (e.g. MDI children) fill their container instead.
            var fillWidth = (elem.parentElement === document.body) ? '100vw' : '100%';
            var fillHeight = (elem.parentElement === document.body) ? '100vh' : '100%';
            elem.style.width = fillWidth;
            elem.style.height = fillHeight;
        } else if (elem.dataset.wxPrevLeft !== undefined) {
            elem.style.left = elem.dataset.wxPrevLeft;
            elem.style.top = elem.dataset.wxPrevTop;
            elem.style.width = elem.dataset.wxPrevWidth;
            elem.style.height = elem.dataset.wxPrevHeight;
            delete elem.dataset.wxPrevLeft;
            delete elem.dataset.wxPrevTop;
            delete elem.dataset.wxPrevWidth;
            delete elem.dataset.wxPrevHeight;
        }
    }, GetId(), maximize ? 1 : 0);
}

void wxTopLevelWindowWasm::Restore()
{
    if ( m_maximized )
        Maximize(false);

    if ( m_iconized )
        Iconize(false);

    if ( m_fullscreen )
        ShowFullScreen(false);
}

void wxTopLevelWindowWasm::Iconize(bool iconize)
{
    if ( iconize == m_iconized )
        return;

    m_iconized = iconize;

    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        if ($1) {
            // There is no taskbar in the browser: "minimizing" just hides
            // the window. Remember the previous display value to restore it.
            elem.dataset.wxPrevDisplay = elem.style.display || "";
            elem.style.display = 'none';
        } else {
            elem.style.display = elem.dataset.wxPrevDisplay || "";
            delete elem.dataset.wxPrevDisplay;
        }
    }, GetId(), iconize ? 1 : 0);
}

bool wxTopLevelWindowWasm::IsMaximized() const
{
    return m_maximized;
}

bool wxTopLevelWindowWasm::IsIconized() const
{
    return m_iconized;
}

bool wxTopLevelWindowWasm::ShowFullScreen(bool show, long WXUNUSED(style))
{
    if ( show == m_fullscreen )
        return true;

    // The Fullscreen API is asynchronous and can reject the request (e.g.
    // when not triggered by a user gesture); there is no way to wait for
    // the promise here, so the flag is updated optimistically.
    int ok = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;

        try {
            if ($1) {
                if (elem.requestFullscreen) {
                    var p = elem.requestFullscreen();
                    if (p && p.catch) p.catch(function() {});
                } else {
                    return 0;
                }
            } else {
                if (document.fullscreenElement && document.exitFullscreen) {
                    var p = document.exitFullscreen();
                    if (p && p.catch) p.catch(function() {});
                }
            }
            return 1;
        } catch (e) {
            return 0;
        }
    }, GetId(), show ? 1 : 0);

    if ( ok )
        m_fullscreen = show;

    return ok != 0;
}

bool wxTopLevelWindowWasm::IsFullScreen() const
{
    return m_fullscreen;
}

void wxTopLevelWindowWasm::SetTitle(const wxString& title)
{
    m_title = title;
    wxCharBuffer buffer = title.ToUTF8();

    // Only real top-level windows (frames/dialogs without a parent) own the
    // browser tab title; MDI children and owned dialogs must not touch it.
    if ( GetParent() == nullptr )
    {
        EM_ASM_({
            document.title = UTF8ToString($0);
        }, buffer.data());
    }

    // Update visual title inside the frame if the element exists
    EM_ASM_({
        var tlw = document.getElementById($0);
        if (!tlw) return;

        var titleBar = tlw.querySelector('.wxTopLevelWindow-title');
        if (titleBar) {
            titleBar.textContent = UTF8ToString($1);
        }
    }, GetId(), buffer.data());
}

wxString wxTopLevelWindowWasm::GetTitle() const
{
    return m_title;
}

void wxTopLevelWindowWasm::SetWindowStyleFlag( long style )
{
    // No window decorations to restyle in the DOM for now; just keep the
    // style up to date so GetWindowStyleFlag() works as expected.
    wxTopLevelWindowBase::SetWindowStyleFlag(style);
}

long wxTopLevelWindowWasm::GetWindowStyleFlag() const
{
    return m_windowStyle;
}
