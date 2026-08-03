/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/utils.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/cursor.h"
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/utils.h"

#include <emscripten.h>

void wxBell()
{
    // Short oscillator beep via Web Audio. Browsers may refuse to create or
    // start an AudioContext before the first user gesture; in that case the
    // bell is silently skipped.
    EM_ASM_({
        try {
            if (!window.wxWasmAudioCtx) {
                var AudioCtx = window.AudioContext || window.webkitAudioContext;
                window.wxWasmAudioCtx = new AudioCtx();
            }
            var ctx = window.wxWasmAudioCtx;
            var osc = ctx.createOscillator();
            var gain = ctx.createGain();
            osc.type = 'sine';
            osc.frequency.value = 880;
            gain.gain.setValueAtTime(0.1, ctx.currentTime);
            gain.gain.exponentialRampToValueAtTime(0.0001,
                ctx.currentTime + 0.15);
            osc.connect(gain);
            gain.connect(ctx.destination);
            osc.start();
            osc.stop(ctx.currentTime + 0.15);
        } catch (e) {
            // Audio not available or not allowed yet: ignore.
        }
    });
}

#if wxUSE_GUI

// Map a KeyboardEvent.key value to a wx keycode (WXK_* or ASCII). Shared by
// the accelerator matching (src/wasm/app.cpp) and the wxKeyEvent translation
// (src/wasm/window.cpp); declared extern at both places.
int wxWasmKeyToWxKeyCode(const wxString& key)
{
    // Single characters (letters, digits, symbols, space) map to their code
    if (key.length() == 1)
        return key[0U];

    static const struct KeyNameMapping
    {
        const char* jsName;
        int wxCode;
    } keyNameMap[] =
    {
        { "Enter",     WXK_RETURN   },
        { "Escape",    WXK_ESCAPE   },
        { "Tab",       WXK_TAB      },
        { "Backspace", WXK_BACK     },
        { "Delete",    WXK_DELETE   },
        { "Insert",    WXK_INSERT   },
        { "Home",      WXK_HOME     },
        { "End",       WXK_END      },
        { "PageUp",    WXK_PAGEUP   },
        { "PageDown",  WXK_PAGEDOWN },
        { "ArrowLeft", WXK_LEFT     },
        { "ArrowRight",WXK_RIGHT    },
        { "ArrowUp",   WXK_UP       },
        { "ArrowDown", WXK_DOWN     },
        { "Spacebar",  WXK_SPACE    }, // legacy name for ' '
    };

    for (size_t i = 0; i < WXSIZEOF(keyNameMap); i++)
    {
        if (key.CmpNoCase(keyNameMap[i].jsName) == 0)
            return keyNameMap[i].wxCode;
    }

    // Function keys "F1".."F24"
    if (key.length() > 1 && (key[0U] == 'F' || key[0U] == 'f'))
    {
        unsigned long num;
        if (key.Mid(1).ToULong(&num) && num >= 1 && num <= 24)
            return WXK_F1 + num - 1;
    }

    return 0;
}

// The browser doesn't allow querying the mouse position or the key states
// outside event handlers, so global listeners registered lazily on first use
// keep track of them:
//
//   window.wxWasmMouse = { x, y, buttons } in client (viewport) coordinates
//   window.wxWasmKeys  = set of pressed KeyboardEvent.code values
//   window.wxWasmMods  = modifier flags of the last keyboard event
//
// Limitation: only input seen while the page had focus is known; before the
// first mouse move the position is (0,0).
static void wxWasmEnsureInputListeners()
{
    static bool s_listenersRegistered = false;
    if ( s_listenersRegistered )
        return;
    s_listenersRegistered = true;

    // Note: EM_ASM code is a variadic macro argument, so commas must be
    // inside parentheses (braces/brackets don't protect them) -- hence the
    // field-by-field initialization instead of object literals.
    EM_ASM_({
        window.wxWasmMouse = {};
        window.wxWasmMouse.x = 0;
        window.wxWasmMouse.y = 0;
        window.wxWasmMouse.buttons = 0;
        window.wxWasmKeys = {};
        window.wxWasmMods = {};
        window.wxWasmMods.shift = false;
        window.wxWasmMods.ctrl = false;
        window.wxWasmMods.alt = false;

        var updateMouse = function(e) {
            window.wxWasmMouse.x = e.clientX;
            window.wxWasmMouse.y = e.clientY;
            window.wxWasmMouse.buttons = e.buttons;
        };
        document.addEventListener('pointermove', updateMouse);
        document.addEventListener('pointerdown', updateMouse);
        document.addEventListener('pointerup', updateMouse);

        document.addEventListener('keydown', function(e) {
            window.wxWasmKeys[e.code] = true;
            window.wxWasmMods.shift = e.shiftKey;
            window.wxWasmMods.ctrl = e.ctrlKey;
            window.wxWasmMods.alt = e.altKey;
        });
        document.addEventListener('keyup', function(e) {
            delete window.wxWasmKeys[e.code];
            window.wxWasmMods.shift = e.shiftKey;
            window.wxWasmMods.ctrl = e.ctrlKey;
            window.wxWasmMods.alt = e.altKey;
        });

        // Losing focus means no more keyup/pointerup events: reset.
        window.addEventListener('blur', function() {
            window.wxWasmKeys = {};
            window.wxWasmMouse.buttons = 0;
        });
    });
}

static bool wxWasmIsKeyPressed(const char *code)
{
    wxWasmEnsureInputListeners();
    return EM_ASM_INT({
        var code = UTF8ToString($0);
        return window.wxWasmKeys[code] ? 1 : 0;
    }, code) != 0;
}

static int wxWasmMouseButtons()
{
    wxWasmEnsureInputListeners();
    return EM_ASM_INT({
        return window.wxWasmMouse.buttons;
    });
}

void wxGetMousePosition( int *x, int *y )
{
    wxWasmEnsureInputListeners();

    // Last known position in client (viewport) coordinates: the page
    // viewport is the closest notion of "screen" available to a browser app.
    if ( x )
        *x = EM_ASM_INT({ return window.wxWasmMouse.x; });
    if ( y )
        *y = EM_ASM_INT({ return window.wxWasmMouse.y; });
}

// Map a wxKeyCode to the DOM KeyboardEvent.code string, or nullptr if there
// is no direct mapping. Modifiers and mouse buttons are handled separately
// in wxGetKeyState() below.
static const char *wxWasmDOMCodeFromKeyCode(wxKeyCode key)
{
    if ( key >= 'A' && key <= 'Z' )
    {
        static char code[] = "KeyA";
        code[3] = static_cast<char>(key);
        return code;
    }

    if ( key >= '0' && key <= '9' )
    {
        static char code[] = "Digit0";
        code[5] = static_cast<char>(key);
        return code;
    }

    if ( key >= WXK_F1 && key <= WXK_F12 )
    {
        static char code[] = "F12";
        const int n = key - WXK_F1 + 1;
        code[1] = static_cast<char>('0' + (n / 10 ? n / 10 : n));
        code[2] = n / 10 ? static_cast<char>('0' + n % 10) : '\0';
        return code;
    }

    switch ( key )
    {
        case WXK_ESCAPE:    return "Escape";
        case WXK_SPACE:     return "Space";
        case WXK_RETURN:    return "Enter";
        case WXK_TAB:       return "Tab";
        case WXK_BACK:      return "Backspace";
        case WXK_DELETE:    return "Delete";
        case WXK_INSERT:    return "Insert";
        case WXK_HOME:      return "Home";
        case WXK_END:       return "End";
        case WXK_PAGEUP:    return "PageUp";
        case WXK_PAGEDOWN:  return "PageDown";
        case WXK_LEFT:      return "ArrowLeft";
        case WXK_RIGHT:     return "ArrowRight";
        case WXK_UP:        return "ArrowUp";
        case WXK_DOWN:      return "ArrowDown";
        case WXK_NUMPAD_ENTER: return "NumpadEnter";
        case WXK_CAPITAL:   return "CapsLock";
        case WXK_NUMLOCK:   return "NumLock";
        case WXK_SCROLL:    return "ScrollLock";
        case WXK_PAUSE:     return "Pause";
        case WXK_PRINT:     return "PrintScreen";
        case WXK_MENU:      return "ContextMenu";
        case WXK_WINDOWS_LEFT:  return "MetaLeft";
        case WXK_WINDOWS_RIGHT: return "MetaRight";
        default:            return nullptr;
    }
}

bool wxGetKeyState(wxKeyCode key)
{
    switch ( key )
    {
        case WXK_SHIFT:
            return wxWasmIsKeyPressed("ShiftLeft") ||
                   wxWasmIsKeyPressed("ShiftRight");
        case WXK_CONTROL:
            return wxWasmIsKeyPressed("ControlLeft") ||
                   wxWasmIsKeyPressed("ControlRight");
        case WXK_ALT:
            return wxWasmIsKeyPressed("AltLeft") ||
                   wxWasmIsKeyPressed("AltRight");

        // Mouse buttons come from the "buttons" bitmask of pointer events.
        case WXK_LBUTTON:
            return (wxWasmMouseButtons() & 1) != 0;
        case WXK_RBUTTON:
            return (wxWasmMouseButtons() & 2) != 0;
        case WXK_MBUTTON:
            return (wxWasmMouseButtons() & 4) != 0;

        default:
            break;
    }

    const char *code = wxWasmDOMCodeFromKeyCode(key);
    if ( !code )
    {
        // Only keys seen while the page had focus can be reported.
        return false;
    }

    return wxWasmIsKeyPressed(code);
}

wxMouseState wxGetMouseState()
{
    wxMouseState ms;

    int x, y;
    wxGetMousePosition( &x, &y );
    ms.SetX(x);
    ms.SetY(y);

    const int buttons = wxWasmMouseButtons();
    ms.SetLeftDown((buttons & 1) != 0);
    ms.SetRightDown((buttons & 2) != 0);
    ms.SetMiddleDown((buttons & 4) != 0);
    ms.SetAux1Down((buttons & 8) != 0);
    ms.SetAux2Down((buttons & 16) != 0);

    const int mods = EM_ASM_INT({
        return (window.wxWasmMods.shift ? 1 : 0) |
               (window.wxWasmMods.ctrl ? 2 : 0) |
               (window.wxWasmMods.alt ? 4 : 0);
    });
    ms.SetShiftDown((mods & 1) != 0);
    ms.SetControlDown((mods & 2) != 0);
    ms.SetAltDown((mods & 4) != 0);

    return ms;
}

#endif // wxUSE_GUI

wxWindow *wxFindWindowAtPoint(const wxPoint& pt)
{
    return wxGenericFindWindowAtPoint( pt );
}

wxWindow *wxGetActiveWindow()
{
    // Same DOM walk as wxWindowBase::DoFindFocus(): find the first ancestor
    // of document.activeElement whose id is a numeric wxWindowID.
    int id = EM_ASM_INT({
        var elem = document.activeElement;
        while (elem) {
            if (elem.id) {
                var id = parseInt(elem.id, 10);
                if (!isNaN(id)) return id;
            }
            elem = elem.parentElement;
        }
        return -1;
    });

    if ( id == -1 )
        return nullptr;

    wxWindow *win = wxWindow::FindWindowById(id);
    if ( !win )
        return nullptr;

    // The "active window" is the top level window owning the focused one.
    return wxGetTopLevelParent(win);
}

bool wxLaunchDefaultApplication(const wxString& path, int WXUNUSED( flags ) )
{
    if ( path.empty() )
        return false;

    // The browser blocks window.open() outside user-gesture handlers; in
    // that case it returns null and we report failure.
    const wxCharBuffer buf = path.ToUTF8();
    return EM_ASM_INT({
        var url = UTF8ToString($0);
        return window.open(url, '_blank') ? 1 : 0;
    }, buf.data()) != 0;
}
