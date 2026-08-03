/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/uiaction.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/private/uiaction.h"

#include <emscripten.h>

// The simulator works by dispatching synthetic DOM events. They are not
// trusted (isTrusted == false), but the listeners installed by the wasm
// controls fire all the same, so the events flow through the normal
// wxWasmEventSink pipeline.
class wxUIActionSimulatorWasmImpl : public wxUIActionSimulatorImpl
{
public:
    // Returns a pointer to the global simulator object: a single instance is
    // enough as the object only keeps the last mouse position.
    static wxUIActionSimulatorWasmImpl* Get()
    {
        static wxUIActionSimulatorWasmImpl s_impl;
        return &s_impl;
    }

    virtual bool MouseMove(long x, long y) override;
    virtual bool MouseDown(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseUp(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool MouseClick(int button = wxMOUSE_BTN_LEFT) override;
    virtual bool MouseDblClick(int button = wxMOUSE_BTN_LEFT) override;

    virtual bool DoKey(int keycode, int modifiers, bool isDown) override;

private:
    // This class has no public ctors, use Get() instead.
    wxUIActionSimulatorWasmImpl() = default;

    // Dispatches a synthetic PointerEvent of the given type to the element
    // under the given position, in screen (i.e. viewport client) coordinates.
    // PointerEvent is used even for "click"/"dblclick": it derives from
    // MouseEvent, so the listeners of the native controls fire all the same,
    // while the global input listeners of the port (src/wasm/app.cpp) only
    // listen to pointerdown/pointerup/pointermove.
    bool DispatchMouseEvent(const char* type, int button, int buttons,
                            long x, long y)
    {
        return EM_ASM_INT({
            var elem = document.elementFromPoint($1, $2);
            if (!elem) return 0;
            // Initialized property by property: the commas of an object
            // literal would split the EM_ASM macro arguments.
            var init = {};
            init.clientX = $1;
            init.clientY = $2;
            init.button = $3;
            init.buttons = $4;
            init.bubbles = true;
            init.cancelable = true;
            elem.dispatchEvent(new PointerEvent(UTF8ToString($0), init));
            return 1;
        }, type, x, y, button, buttons) != 0;
    }

    // Convert a wx mouse button constant to the DOM MouseEvent.button value.
    static int ToDOMMouseButton(int button)
    {
        switch ( button )
        {
            case wxMOUSE_BTN_LEFT:
                return 0;

            case wxMOUSE_BTN_MIDDLE:
                return 1;

            case wxMOUSE_BTN_RIGHT:
                return 2;

            default:
                wxFAIL_MSG( "Unsupported mouse button" );
                return 0;
        }
    }

    // Convert a wx mouse button constant to the DOM MouseEvent.buttons mask
    // with only that button pressed (the state during the down gesture).
    static int ToDOMMouseButtons(int button)
    {
        switch ( button )
        {
            case wxMOUSE_BTN_LEFT:
                return 1;

            case wxMOUSE_BTN_MIDDLE:
                return 4;

            case wxMOUSE_BTN_RIGHT:
                return 2;

            default:
                wxFAIL_MSG( "Unsupported mouse button" );
                return 0;
        }
    }

    // Last position passed to MouseMove(), used by the button methods which
    // don't take coordinates.
    long m_mouseX = 0;
    long m_mouseY = 0;

    wxDECLARE_NO_COPY_CLASS(wxUIActionSimulatorWasmImpl);
};

bool wxUIActionSimulatorWasmImpl::MouseMove(long x, long y)
{
    m_mouseX = x;
    m_mouseY = y;

    return DispatchMouseEvent("pointermove", 0, 0, x, y);
}

bool wxUIActionSimulatorWasmImpl::MouseDown(int button)
{
    return DispatchMouseEvent("pointerdown", ToDOMMouseButton(button),
                              ToDOMMouseButtons(button), m_mouseX, m_mouseY);
}

bool wxUIActionSimulatorWasmImpl::MouseUp(int button)
{
    // After the release no button remains pressed, hence buttons == 0.
    return DispatchMouseEvent("pointerup", ToDOMMouseButton(button), 0,
                              m_mouseX, m_mouseY);
}

bool wxUIActionSimulatorWasmImpl::MouseClick(int button)
{
    // A synthetic pointerdown/pointerup pair doesn't make the browser
    // generate a "click" event on its own, so dispatch it explicitly.
    MouseDown(button);
    MouseUp(button);

    return DispatchMouseEvent("click", ToDOMMouseButton(button), 0,
                              m_mouseX, m_mouseY);
}

bool wxUIActionSimulatorWasmImpl::MouseDblClick(int button)
{
    MouseClick(button);
    MouseClick(button);

    return DispatchMouseEvent("dblclick", ToDOMMouseButton(button), 0,
                              m_mouseX, m_mouseY);
}

bool wxUIActionSimulatorWasmImpl::DoKey(int keycode, int modifiers, bool isDown)
{
    // Resolve the DOM KeyboardEvent.key value and the legacy keyCode for the
    // wx key code. Only the most common keys are mapped; printable ASCII
    // characters map to themselves.
    const char* key = nullptr;
    char asciiKey[2] = { 0, 0 };
    int domKeyCode = keycode;

    switch ( keycode )
    {
        case WXK_RETURN:  key = "Enter";      domKeyCode = 13; break;
        case WXK_ESCAPE:  key = "Escape";     domKeyCode = 27; break;
        case WXK_TAB:     key = "Tab";        domKeyCode = 9;  break;
        case WXK_BACK:    key = "Backspace";  domKeyCode = 8;  break;
        case WXK_DELETE:  key = "Delete";     domKeyCode = 46; break;
        case WXK_LEFT:    key = "ArrowLeft";  domKeyCode = 37; break;
        case WXK_RIGHT:   key = "ArrowRight"; domKeyCode = 39; break;
        case WXK_UP:      key = "ArrowUp";    domKeyCode = 38; break;
        case WXK_DOWN:    key = "ArrowDown";  domKeyCode = 40; break;
        case WXK_HOME:    key = "Home";       domKeyCode = 36; break;
        case WXK_END:     key = "End";        domKeyCode = 35; break;
        case WXK_SHIFT:   key = "Shift";      domKeyCode = 16; break;
        case WXK_ALT:     key = "Alt";        domKeyCode = 18; break;
        case WXK_CONTROL: key = "Control";    domKeyCode = 17; break;
        case WXK_SPACE:   key = " ";          domKeyCode = 32; break;

        default:
            if ( keycode >= 32 && keycode < 127 )
            {
                asciiKey[0] = static_cast<char>(keycode);
                key = asciiKey;
            }
            break;
    }

    wxCHECK_MSG( key, false, "Unsupported key code" );

    return EM_ASM_INT({
        var target = document.activeElement || document.body;
        // Parenthesized so the commas don't split the EM_ASM macro argument.
        var init = ({
            key: UTF8ToString($0),
            keyCode: $1,
            which: $1,
            altKey: ($2 & 1) !== 0,   // wxMOD_ALT
            ctrlKey: ($2 & 2) !== 0,  // wxMOD_CONTROL
            shiftKey: ($2 & 4) !== 0, // wxMOD_SHIFT
            bubbles: true,
            cancelable: true
        });
        target.dispatchEvent(new KeyboardEvent($3 ? 'keydown' : 'keyup', init));
        // A real key press also generates "keypress" for printable keys.
        if ($3 && init.key.length === 1)
            target.dispatchEvent(new KeyboardEvent('keypress', init));
        return 1;
    }, key, domKeyCode, modifiers, isDown ? 1 : 0) != 0;
}

wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(wxUIActionSimulatorWasmImpl::Get())
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    // We use a static wxUIActionSimulatorWasmImpl object, so no need to
    // delete it here (the base class doesn't own m_impl either).
}

#endif // wxUSE_UIACTIONSIMULATOR
