/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/app.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/wasm/cssstyles.h"

#if wxUSE_ACCEL
    #include "wx/accel.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/menuitem.h"
#endif

#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxApp::wxApp()
{
    WXAppConstructed();
}


wxApp::~wxApp()
{

}

bool wxApp::Initialize( int &argc, wxChar **argv )
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

    //We create a hidden div containing all temprarly parentless controls (line wxMenuBar) otherwise the browser is going to delete it
    EM_ASM_INT(
            const newTopLevelWindow=document.createElement("div");
            newTopLevelWindow.id= "wxParentlessTags";
            newTopLevelWindow.style.display="none";
            document.body.append(newTopLevelWindow);
            return 1;
    );

    // Inject default CSS styles (GTK3-like)
    wxWasmCSSManager::InjectDefaultStyles();

    // Register global keydown listener for menu accelerators
    EM_ASM_({
        if (window._wxWasmAccelListener) return;
        window._wxWasmAccelListener = true;

        document.addEventListener('keydown', function(e) {
            var key = e.key;
            var ctrl = e.ctrlKey ? 1 : 0;
            var alt = e.altKey ? 1 : 0;
            var shift = e.shiftKey ? 1 : 0;

            if (typeof Module !== 'undefined' && Module.ccall) {
                var len = lengthBytesUTF8(key) + 1;
                var buf = Module._malloc(len);
                stringToUTF8(key, buf, len);
                var handled = Module.ccall('ProcessAcceleratorKey', 'number',
                    ['number', 'number', 'number', 'number'],
                    [buf, ctrl, alt, shift]);
                Module._free(buf);
                // The accelerator triggered a menu item: don't let the
                // browser (or the focused control) handle the key.
                // stopImmediatePropagation also blocks the input keydown
                // listener below (registered on the same target), so no
                // wxEVT_KEY_DOWN is generated for the consumed key.
                if (handled) {
                    e.preventDefault();
                    e.stopImmediatePropagation();
                }
            }
        });
    });

    // Register the global input listeners that feed wxMouseEvent/wxKeyEvent.
    // They run in the bubble phase on document and never call
    // preventDefault()/stopPropagation(), so the per-control listeners and
    // the native behaviour of the HTML elements are unaffected.
    //
    // Target resolution: walk up from the event target (or the active
    // element for keys) to the first element whose id is a numeric
    // wxWindowID, the same pattern as wxWindowBase::DoFindFocus().
    // Menu DOM ids are namespaced ('wxMenuItem_<id>', 'wxMenuPopup_<id>',
    // 'menubar_menu_<id>'), so parseInt() yields NaN for them and they are
    // skipped: menu clicks are handled by the items' own onclick handlers,
    // not by this pipeline. Coordinates are relative to the resolved
    // window; the modifier bits are 1=shift, 2=ctrl, 4=alt, 8=meta (same
    // as window.wxWasmMods); bit 16 is wheel-only and marks native
    // scrolling (see the wheel listener below).
    EM_ASM_({
        if (window._wxWasmInputListeners) return;
        window._wxWasmInputListeners = true;

        // Last wx window div under the pointer, for enter/leave tracking.
        window._wxWasmHoverElem = null;

        var modsOf = function(e) {
            return (e.shiftKey ? 1 : 0) | (e.ctrlKey ? 2 : 0) |
                   (e.altKey ? 4 : 0) | (e.metaKey ? 8 : 0);
        };

        var targetWin = function(node) {
            var elem = node;
            while (elem && elem !== document.documentElement) {
                if (elem.id) {
                    var id = parseInt(elem.id, 10);
                    if (!isNaN(id)) return elem;
                }
                elem = elem.parentElement;
            }
            return null;
        };

        var sendMouse = function(e, type, win) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var r = win.getBoundingClientRect();
            var button = e.button > 0 ? e.button : 0;
            Module.ccall('addInputEvent', null,
                ['number', 'string', 'number', 'number', 'number', 'number', 'number', 'number'],
                [parseInt(win.id, 10), type,
                 Math.round(e.clientX - r.left), Math.round(e.clientY - r.top),
                 button, e.buttons | 0, 0, modsOf(e)]);
        };

        document.addEventListener('pointerdown', function(e) {
            var win = targetWin(e.target);
            if (win) sendMouse(e, 'mousedown', win);
        });
        document.addEventListener('pointerup', function(e) {
            var win = targetWin(e.target);
            if (win) sendMouse(e, 'mouseup', win);
        });
        document.addEventListener('dblclick', function(e) {
            var win = targetWin(e.target);
            if (win) sendMouse(e, 'dblclick', win);
        });

        document.addEventListener('pointermove', function(e) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var win = targetWin(e.target);
            // The remembered hover element may have been removed from the
            // DOM (the window dtor deletes its div): drop it, sending a
            // leave to a detached element is useless.
            if (window._wxWasmHoverElem && !window._wxWasmHoverElem.isConnected)
                window._wxWasmHoverElem = null;
            // While the mouse is captured (document._wxWasmCaptureActive,
            // see wxWindowWasm::DoCaptureMouse) hover tracking is
            // suppressed: no enter/leave events. The mousemove itself
            // keeps flowing through the capture re-dispatch.
            if (!document._wxWasmCaptureActive && win !== window._wxWasmHoverElem) {
                var old = window._wxWasmHoverElem;
                window._wxWasmHoverElem = win;
                if (old) sendMouse(e, 'mouseleave', old);
                if (win) sendMouse(e, 'mouseenter', win);
            }
            if (win) sendMouse(e, 'mousemove', win);
        });

        // Passive: native scrolling (overflow) must keep working, we only
        // observe. wx wheel delta convention: 120 per notch, positive up.
        // When the window's own div scrolls natively in the wheel axis
        // (SetScrollbar() enabled its overflow and the content actually
        // overflows), the browser has already moved the viewport; bit 16
        // of the modifier mask marks the event so the C++ side suppresses
        // the wxEVT_MOUSEWHEEL: the generic wxScrollHelperBase would
        // scroll the window a second time otherwise (see WasmNotifyEvent
        // in src/wasm/window.cpp). The scroll position still reaches the
        // helper through the native "scroll" event below.
        document.addEventListener('wheel', function(e) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var win = targetWin(e.target);
            if (!win || e.deltaY === 0) return;
            var r = win.getBoundingClientRect();
            var rotation = e.deltaY < 0 ? 120 : -120;
            var nativeScroll = (win.style.overflowY === 'auto' ||
                                win.style.overflowY === 'scroll') &&
                               win.scrollHeight > win.clientHeight;
            var mods = modsOf(e);
            if (nativeScroll) mods = mods | 16;
            Module.ccall('addInputEvent', null,
                ['number', 'string', 'number', 'number', 'number', 'number', 'number', 'number'],
                [parseInt(win.id, 10), 'wheel',
                 Math.round(e.clientX - r.left), Math.round(e.clientY - r.top),
                 0, e.buttons | 0, rotation, mods]);
        }, { passive: true });

        // Scroll: the window divs scroll natively once SetScrollbar()
        // enables their overflow. "scroll" does not bubble, so listen in
        // the capture phase. Only the window's own scrolling is reported
        // (e.target is the scrolled element); scrolling of a nested
        // non-wx element is ignored. x/y carry scrollLeft/scrollTop and
        // the button slot carries the orientation (wxVERTICAL=8,
        // wxHORIZONTAL=4). One event is sent per axis that changed,
        // tracked against the last reported position kept on the
        // element: a single native scroll can move both axes, and the
        // queue and the wx dispatch handle one orientation per event,
        // so merging them would silently drop one axis.
        document.addEventListener('scroll', function(e) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var win = targetWin(e.target);
            if (!win || win !== e.target) return;
            var left = win.scrollLeft;
            var top = win.scrollTop;
            var prevLeft = win._wxWasmScrollLeft;
            var prevTop = win._wxWasmScrollTop;
            if (prevLeft === undefined) prevLeft = 0;
            if (prevTop === undefined) prevTop = 0;
            win._wxWasmScrollLeft = left;
            win._wxWasmScrollTop = top;
            if (top !== prevTop) {
                Module.ccall('addInputEvent', null,
                    ['number', 'string', 'number', 'number', 'number', 'number', 'number', 'number'],
                    [parseInt(win.id, 10), 'scroll', left, top, 8, 0, 0, 0]);
            }
            if (left !== prevLeft) {
                Module.ccall('addInputEvent', null,
                    ['number', 'string', 'number', 'number', 'number', 'number', 'number', 'number'],
                    [parseInt(win.id, 10), 'scroll', left, top, 4, 0, 0, 0]);
            }
        }, true);

        // Keyboard: dispatched to the wx window owning the focused element.
        // Never prevented here: text input must flow, and menu accelerators
        // are already consumed by the listener above (preventDefault there
        // does not stop other document listeners, only default behaviour).
        var sendKey = function(e, type) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var win = targetWin(document.activeElement);
            if (!win) return;
            Module.ccall('addKeyEvent', null,
                ['number', 'string', 'string', 'number'],
                [parseInt(win.id, 10), type, e.key, modsOf(e)]);
        };
        document.addEventListener('keydown', function(e) { sendKey(e, 'keydown'); });
        document.addEventListener('keyup', function(e) { sendKey(e, 'keyup'); });

        // Focus: dispatched to the wx window gaining/losing the DOM focus.
        var sendFocus = function(e, type) {
            if (typeof Module === 'undefined' || !Module.ccall) return;
            var win = targetWin(e.target);
            if (!win) return;
            Module.ccall('addInputEvent', null,
                ['number', 'string', 'number', 'number', 'number', 'number', 'number', 'number'],
                [parseInt(win.id, 10), type, 0, 0, 0, 0, 0, 0]);
        };
        document.addEventListener('focusin', function(e) { sendFocus(e, 'setfocus'); });
        document.addEventListener('focusout', function(e) { sendFocus(e, 'killfocus'); });
    });

    WakeUpIdle();

    return true;
}

#if wxUSE_ACCEL

// Map a KeyboardEvent.key value to a wx keycode, defined in
// src/wasm/utils.cpp (shared with the wxKeyEvent translation in
// src/wasm/window.cpp).
extern int wxWasmKeyToWxKeyCode(const wxString& key);

namespace
{

// wxAcceleratorEntry::FromString() uppercases letters combined with
// modifiers ("Ctrl+o" -> 'O') while KeyboardEvent.key reflects the Shift
// state, so compare letters case-insensitively
bool WasmKeyCodeMatches(int accelKeyCode, int keyCode)
{
    if (accelKeyCode == keyCode)
        return true;

    if (accelKeyCode < 128 && keyCode < 128 &&
        wxIsalpha(accelKeyCode) && wxIsalpha(keyCode))
    {
        return wxToupper(accelKeyCode) == wxToupper(keyCode);
    }

    return false;
}

// Depth-first search for an enabled item whose accelerator matches
wxMenuItem* WasmFindAccelItem(wxMenu* menu, int flags, int keyCode)
{
    const wxMenuItemList& items = menu->GetMenuItems();
    for (wxMenuItemList::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        wxMenuItem* item = *it;

        if (wxMenu* subMenu = item->GetSubMenu())
        {
            if (wxMenuItem* found = WasmFindAccelItem(subMenu, flags, keyCode))
                return found;
            continue;
        }

        if (item->IsSeparator() || !item->IsEnabled())
            continue;

        wxAcceleratorEntry* accel = item->GetAccel();
        if (accel)
        {
            const int accelFlags = accel->GetFlags() &
                (wxACCEL_ALT | wxACCEL_CTRL | wxACCEL_SHIFT);
            const bool match = accelFlags == flags &&
                WasmKeyCodeMatches(accel->GetKeyCode(), keyCode);
            delete accel;

            if (match)
                return item;
        }
    }

    return nullptr;
}

} // anonymous namespace

#endif // wxUSE_ACCEL

extern "C" EMSCRIPTEN_KEEPALIVE int ProcessAcceleratorKey(const char* key, int ctrl, int alt, int shift)
{
#if wxUSE_ACCEL
    const int keyCode = wxWasmKeyToWxKeyCode(wxString::FromUTF8(key));
    if (!keyCode)
        return 0;

    const int flags = (ctrl  ? wxACCEL_CTRL  : 0) |
                      (alt   ? wxACCEL_ALT   : 0) |
                      (shift ? wxACCEL_SHIFT : 0);

    // First try the accelerator tables: walk up from the focused window to
    // the first window (itself included) whose table matches the key. A
    // match generates a wxEVT_MENU with the entry command id on that
    // window, as a menu command would (see wxAcceleratorTable docs).
    wxWindow* focus = wxWindow::FindFocus();
    for (wxWindow* win = focus; win; win = win->GetParent())
    {
        const wxAcceleratorTable* table = win->GetAcceleratorTable();
        if ( !table || !table->IsOk() )
            continue;

        const wxAcceleratorEntry* entry = table->FindEntry(flags, keyCode);
        if ( entry )
        {
            // Queue, don't dispatch synchronously: we run inside a DOM
            // listener and the handler could reenter code that triggered
            // it; the main loop processes pending events each cycle.
            wxCommandEvent* event = new wxCommandEvent(wxEVT_MENU, entry->GetCommand());
            event->SetEventObject(win);
            win->GetEventHandler()->QueueEvent(event);
            return 1;
        }
    }

    // Then the menubars: walk up from the focused window, falling back to
    // the first top-level frame with a menubar
    wxFrame* frame = nullptr;
    while (focus && !frame)
    {
        frame = wxDynamicCast(focus, wxFrame);
        focus = focus->GetParent();
    }

    if (!frame || !frame->GetMenuBar())
    {
        frame = nullptr;
        for (wxWindowList::const_iterator it = wxTopLevelWindows.begin();
             it != wxTopLevelWindows.end(); ++it)
        {
            wxFrame* candidate = wxDynamicCast(*it, wxFrame);
            if (candidate && candidate->GetMenuBar())
            {
                frame = candidate;
                break;
            }
        }
    }

    if (!frame)
        return 0;

    wxMenuBar* mb = frame->GetMenuBar();
    wxMenuItem* item = nullptr;
    const size_t menuCount = mb->GetMenuCount();
    for (size_t i = 0; i < menuCount && !item; ++i)
        item = WasmFindAccelItem(mb->GetMenu(i), flags, keyCode);

    if (!item)
        return 0;

    // Send the same event as a click on the item (see ProcessMenuEvent in
    // src/wasm/menuitem.cpp), queued for the same reason as above
    wxMenu* parentMenu = nullptr;
    mb->FindItem(item->GetId(), &parentMenu);
    if (!parentMenu)
        return 0;

    wxCommandEvent* event = new wxCommandEvent(wxEVT_MENU, item->GetId());
    event->SetEventObject(parentMenu);

    if (item->IsCheckable())
    {
        item->Toggle();
        event->SetInt(item->IsChecked() ? 1 : 0);
    }

    parentMenu->GetEventHandler()->QueueEvent(event);
    return 1;
#else
    wxUnused(key);
    wxUnused(ctrl);
    wxUnused(alt);
    wxUnused(shift);
    return 0;
#endif // wxUSE_ACCEL
}
