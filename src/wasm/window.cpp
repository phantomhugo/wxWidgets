/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/window.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/event.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/window.h"
#include "wx/cursor.h"
#include "wx/dnd.h"
#include "wx/tooltip.h"
#include <emscripten.h>

#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

// Currently captured window, or nullptr. See DoCaptureMouse() below for the
// emulated capture mechanism; defined here because the destructor releases
// a dangling capture.
static wxWindowWasm* s_capturedWindow = nullptr;

// Text measurement helpers defined in src/wasm/dc.cpp: plain C++ functions
// using EM_ASM_DOUBLE (never EM_JS: EM_JS symbols are not resolved across
// static-library translation units by Emscripten's JS linker).
extern double wxWasmMeasureTextWidth(const char* fontSpec, const char* text);
extern double wxWasmMeasureCharHeight(const char* fontSpec);
extern double wxWasmMeasureDescent(const char* fontSpec);

// DOM event entry point defined in src/wasm/evtloop.cpp: queues a
// wxWasmEvent in the event sink later dispatched to WasmNotifyEvent().
extern "C" void addEvent(int id, const std::string& eventType, int x, int y);

// Map a KeyboardEvent.key value to a wx keycode, defined in
// src/wasm/utils.cpp.
extern int wxWasmKeyToWxKeyCode(const wxString& key);

// Returns the CSS font-family value for a wxFont: the quoted face name when
// there is an explicit one, the generic CSS family matching the wxFont
// family otherwise.
static wxString wxWasmFontFamilyCSS(const wxFont& font)
{
    wxString face = font.GetFaceName();
    if ( !face.empty() )
        return "\"" + face + "\"";

    switch ( font.GetFamily() )
    {
        case wxFONTFAMILY_ROMAN:
            return "serif";
        case wxFONTFAMILY_SCRIPT:
            return "cursive";
        case wxFONTFAMILY_DECORATIVE:
            return "fantasy";
        case wxFONTFAMILY_MODERN:
        case wxFONTFAMILY_TELETYPE:
            return "monospace";
        case wxFONTFAMILY_SWISS:
        default:
            return "sans-serif";
    }
}

// Builds a minimal CSS font spec ("<size>px <family>") for the measurement
// helpers: 1pt == 96/72 px, generic CSS family from the wxFont family when
// there is no explicit face name.
static wxString wxWasmWindowFontSpec(const wxFont& font)
{
    if ( !font.IsOk() )
        return wxString("12px sans-serif");

    double size = font.GetFractionalPointSize();
    if ( size <= 0 )
        size = 12;

    return wxString::Format("%.1fpx %s", size * 96.0 / 72.0,
                            wxWasmFontFamilyCSS(font));
}

//##############################################################################

void wxWindowWasm::Init()
{
    m_mouseInside = false;
    m_refreshPending = false;
}

wxWindowWasm::wxWindowWasm()
{
    Init();
}


wxWindowWasm::wxWindowWasm(wxWindowWasm *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    Init();

    Create( parent, id, pos, size, style, name );
}


wxWindowWasm::~wxWindowWasm()
{
#if wxUSE_DRAG_AND_DROP
    SetDropTarget(nullptr);
#endif

    // Don't leave a dangling pointer if the window dies while captured.
    if ( s_capturedWindow == this )
        DoReleaseMouse();

    // Remove the window's DOM element (with its contents: the listeners die
    // with it, and the wx children are destroyed separately by the base
    // class) and the persistent drawing canvas, if any. The removal is
    // idempotent: some dialogs (dialog.cpp, msgdlg.cpp) already remove
    // their element when closed.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem) elem.remove();
        var canvas = document.getElementById('wx_canvas_' + $0);
        if (canvas) canvas.remove();
    }, GetId());
}


bool wxWindowWasm::Create( wxWindowWasm * parent, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style, const wxString &name )
{
    if ( !wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
        return false;

    if (m_parent)
        parent->AddChild( this );

    PostCreation();

    return true;
}

void wxWindowWasm::PostCreation(bool generic)
{
    // Create the DOM element for this window if it doesn't exist yet.
    // Top-level windows and child controls all need a container div.
    EM_ASM_({
        var id = $0;
        var parentId = $1;
        var hasParent = $2 !== 0;
        var elem = document.getElementById(id);
        if (!elem) {
            elem = document.createElement('div');
            elem.id = id;
            elem.className = 'wxWindow';
            elem.style.position = 'absolute';
            elem.style.boxSizing = 'border-box';
            elem.style.overflow = 'hidden';
            // Use the explicit "has parent" flag: auto-assigned ids (wxID_ANY)
            // are negative, so the parent's id alone cannot be trusted.
            if (hasParent) {
                var parentElem = document.getElementById(parentId);
                // Non top-level children of a frame live in its content
                // container, so that their coordinates are relative to the
                // client area (below the menubar, above the statusbar).
                if (!$3) {
                    var content = document.getElementById('wxFrame_content_' + parentId);
                    if (content) parentElem = content;
                }
                if (parentElem) {
                    parentElem.appendChild(elem);
                } else {
                    document.body.appendChild(elem);
                }
            } else {
                document.body.appendChild(elem);
            }
        }
    }, GetId(), m_parent ? m_parent->GetId() : -1, m_parent ? 1 : 0,
       IsTopLevel() ? 1 : 0);

    wxWindowCreateEvent event(this);
    HandleWindowEvent(event);
}

void wxWindowWasm::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the inner scroll area widget (if any):

    wxWindowBase::AddChild( child );
}

bool wxWindowWasm::Show( bool show )
{

    // Show can be called before the underlying window is created:
    bool result=false;
    if(show!=IsShown())
    {
        if(show)
        {
            result=EM_ASM_INT(
                {
                    var elem = document.getElementById($0);
                    if (!elem) return 0;
                    // Frames stack menubar/content/statusbar with flexbox.
                    elem.style.display =
                        elem.classList.contains('wxFrame') ? 'flex' : 'block';
                    return 1;
                },
                GetId()
            );
        }
        else
        {
            result=EM_ASM_INT(
                {
                    var elem = document.getElementById($0);
                    if (!elem) return 0;
                    elem.style.display="none";
                    return 1;
                },
                GetId()
            );
        }

    }
    if ( !wxWindowBase::Show( show ))
        return false;

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    // Newly shown windows need to be painted.
    if ( show )
        Refresh();

    return result;
}


void wxWindowWasm::SetLabel(const wxString& label)
{
    m_label = label;

    wxCharBuffer buf = label.ToUTF8();
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        // Keep the label around in the DOM: top-level windows use it as
        // their title, other controls may show it themselves.
        elem.dataset.label = UTF8ToString($1);
    }, GetId(), buf.data());
}


wxString wxWindowWasm::GetLabel() const
{
    return m_label;
}

void wxWindowWasm::DoEnable(bool enable)
{
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        var enabled = $1 !== 0;
        var fields = elem.querySelectorAll('input, button, select, textarea');
        fields.forEach(function(f) { f.disabled = !enabled; });
        elem.classList.toggle('wx-disabled', !enabled);
    }, GetId(), enable ? 1 : 0);
}

void wxWindowWasm::SetFocus()
{
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        var target = elem.querySelector('input, button, select, textarea, a[href], [tabindex]');
        if (!target) {
            if (!elem.hasAttribute('tabindex')) elem.tabIndex = -1;
            target = elem;
        }
        target.focus();
    }, GetId());
}

bool wxWindowWasm::Reparent( wxWindowBase *parent )
{
    if ( !wxWindowBase::Reparent( parent ))
        return false;

    // Move the container div under the new parent's div, or to the document
    // body when the new parent is null (top-level window). As in
    // PostCreation(), the children of a frame go to its content container.
    // The parent id alone cannot be trusted: auto-assigned ids are negative.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        var newParent = $2 ? document.getElementById($1) : null;
        if (newParent && !$3) {
            var content = document.getElementById('wxFrame_content_' + $1);
            if (content) newParent = content;
        }
        (newParent || document.body).appendChild(elem);
    }, GetId(), parent ? parent->GetId() : -1, parent ? 1 : 0,
       IsTopLevel() ? 1 : 0);

    return true;
}


void wxWindowWasm::Raise()
{
    // Bring the container div to the front by giving it a z-index higher
    // than any of its siblings (same pattern as wxMDIChildFrame::Activate).
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
}

void wxWindowWasm::Lower()
{
    // Send the container div to the back by giving it a z-index lower than
    // any of its siblings.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem || !elem.parentElement) return;
        var minZ = 0;
        var siblings = elem.parentElement.children;
        for (var i = 0; i < siblings.length; i++) {
            if (siblings[i] === elem) continue;
            var z = parseInt(siblings[i].style.zIndex || '0', 10);
            if (z < minZ) minZ = z;
        }
        elem.style.zIndex = (minZ - 1).toString();
    }, GetId());
}

void wxWindowWasm::WarpPointer(int x, int y)
{
    // The pointer cannot be warped from a browser sandbox; just translate
    // the coordinates to global screen coordinates as documented.

    ClientToScreen( &x, &y );
}

void wxWindowWasm::Update()
{
    // Repaint immediately if a refresh is pending. The queued "paint"
    // pseudo-event (if any) is ignored when dispatched because the flag is
    // already cleared by then.
    if ( m_refreshPending )
    {
        m_refreshPending = false;
        SendPaintEvent();
    }
}

void wxWindowWasm::Refresh( bool WXUNUSED( eraseBackground ), const wxRect *WXUNUSED( rect ) )
{
    // There is no OS paint cycle in the browser, so queue a "paint"
    // pseudo-event in the regular DOM event sink: it will be dispatched
    // asynchronously to WasmNotifyEvent(), which sends the wxPaintEvent.
    // The flag coalesces multiple Refresh() calls into a single repaint.
    // The rect (and background erasing) is not honoured yet: the whole
    // window is always repainted.
    if ( IsShown() && !m_refreshPending )
    {
        m_refreshPending = true;
        addEvent(GetId(), "paint", 0, 0);
    }
}

void wxWindowWasm::SendPaintEvent()
{
    // There is no OS paint cycle in the browser and Refresh() repaints the
    // whole window, so the update region must cover it entirely: controls
    // that only redraw the exposed area (e.g. the generic wxGrid, which
    // computes the exposed cells/labels from GetUpdateRegion()) would
    // otherwise paint nothing at all.
    const wxSize clientSize = GetClientSize();
    GetUpdateRegion() = wxRegion(0, 0, clientSize.x, clientSize.y);

    wxPaintEvent event(this);
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

bool wxWindowWasm::SetCursor( const wxCursor &cursor )
{
    // An invalid cursor restores the default one.
    const char *cssCursor = cursor.IsOk()
                                ? wxWasmCursorToCSS(cursor.GetStockCursorId())
                                : "default";

    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        elem.style.cursor = UTF8ToString($1);
    }, GetId(), cssCursor);

    return wxWindowBase::SetCursor(cursor);
}

bool wxWindowWasm::SetFont( const wxFont &font )
{
    if ( !wxWindowBase::SetFont(font) )
        return false;

    // Apply the font to the container div: the GTK3 theme styles the native
    // elements inside the controls with "font: inherit", so setting it on
    // the container is enough for it to propagate to them.
    if ( font.IsOk() )
    {
        wxString family = wxWasmFontFamilyCSS(font);

        double size;
        if ( font.IsUsingSizeInPixels() )
        {
            size = font.GetPixelSize().GetHeight();
            if ( size <= 0 )
                size = 16;
        }
        else
        {
            size = font.GetFractionalPointSize();
            if ( size <= 0 )
                size = 12;
            size *= 96.0 / 72.0;
        }

        const char *style;
        switch ( font.GetStyle() )
        {
            case wxFONTSTYLE_ITALIC:
                style = "italic";
                break;
            case wxFONTSTYLE_SLANT:
                style = "oblique";
                break;
            default:
                style = "normal";
                break;
        }

        wxString decoration;
        if ( font.GetUnderlined() && font.GetStrikethrough() )
            decoration = "underline line-through";
        else if ( font.GetUnderlined() )
            decoration = "underline";
        else if ( font.GetStrikethrough() )
            decoration = "line-through";

        wxCharBuffer familyBuf = family.ToUTF8();
        wxCharBuffer decorationBuf = decoration.ToUTF8();
        EM_ASM_({
            var elem = document.getElementById($0);
            if (!elem) return;
            elem.style.fontFamily = UTF8ToString($1);
            elem.style.fontSize = $2 + 'px';
            elem.style.fontStyle = UTF8ToString($3);
            elem.style.fontWeight = String($4);
            elem.style.textDecoration = UTF8ToString($5);
        }, GetId(), familyBuf.data(), size, style, font.GetNumericWeight(),
           decorationBuf.data());

        Refresh();
    }

    return true;
}


int wxWindowWasm::GetCharHeight() const
{
    // Approximate from the current font size: 1pt == 96/72 px.
    const wxFont& font = GetFont();
    if ( font.IsOk() )
        return (int)(font.GetPointSize() * 96.0 / 72.0 + 0.5);

    return 16;
}


int wxWindowWasm::GetCharWidth() const
{
    return GetCharHeight() / 2 + 1;
}

void wxWindowWasm::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
        int *externalLeading, const wxFont *font ) const
{
    wxFont theFont = (font && font->IsOk()) ? *font : GetFont();
    wxString fontSpec = wxWasmWindowFontSpec(theFont);
    wxCharBuffer fontCStr = fontSpec.ToUTF8();

    // The descent and external leading are returned even for an empty string.
    if ( descent )
        *descent = (int)(wxWasmMeasureDescent(fontCStr.data()) + 0.5);
    if ( externalLeading )
        *externalLeading = 0;

    if ( x )
    {
        if ( string.empty() )
            *x = 0;
        else
        {
            wxCharBuffer textCStr = string.ToUTF8();
            *x = (int)(wxWasmMeasureTextWidth(fontCStr.data(), textCStr.data()) + 0.5);
        }
    }
    if ( y )
        *y = (int)(wxWasmMeasureCharHeight(fontCStr.data()) + 0.5);
}

// Id of the window whose native scroll event is being dispatched, or 0.
// While set, ScrollWindow() must not move the viewport of that window:
// the browser already moved it natively, and the SetScrollPos()/
// ScrollWindow() calls wxScrollHelperBase::HandleOnScroll() does in
// response to the event would scroll it a second time. The dispatch in
// WasmNotifyEvent() is synchronous, so a plain global is enough (no
// threads in this port); it is saved/restored around the dispatch so a
// reentrant native scroll of another window still works.
static int s_nativeScrollWindowId = 0;

// Pixels per scroll unit for the given window/orientation. The native
// scroll offset (scrollLeft/scrollTop) is in PIXELS, but the wx scroll
// API works in UNITS: the generic wxScrollHelper calls SetScrollbar()
// with range/thumb in units (see wxScrollHelper::DoAdjustScrollbar in
// src/generic/scrlwing.cpp) and expects the positions of
// wxScrollWinEvent/GetScrollPos() in units too. The ratio is derived
// from the values cached by SetScrollbar(): thumb units always represent
// the visible client area, so clientSize/thumb is exactly the number of
// pixels per unit, with no need to look at the real overflow size (which
// the spacer div created by SetScrollbar() makes equal to range *
// pixelsPerUnit anyway). Falls back to 1 (pixels == units, the old
// behaviour) when no usable range/thumb was cached, e.g. for controls
// that scroll natively without SetScrollbar().
static double wxWasmScrollPixelsPerUnit(int id, int orientation)
{
    return EM_ASM_DOUBLE({
        var elem = document.getElementById($0);
        if (!elem) return 1.0;
        var horz = ($1 & 0x0004) !== 0; // wxHORIZONTAL == 0x0004
        var prop = horz ? 'X' : 'Y';
        var range = parseInt(elem.dataset['scrollRange' + prop], 10);
        var thumb = parseInt(elem.dataset['scrollThumb' + prop], 10);
        if (isNaN(range) || range <= 0 || isNaN(thumb) || thumb <= 0) return 1.0;
        var clientPx = horz ? elem.clientWidth : elem.clientHeight;
        if (clientPx <= 0) return 1.0;
        return clientPx / thumb;
    }, id, orientation);
}

void wxWindowWasm::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool WXUNUSED(refresh) )
{
    // The container div scrolls natively once its overflow is enabled.
    // pos/thumbvisible/range are in scroll UNITS (the generic
    // wxScrollHelper calls this with unit values, see
    // wxScrollHelper::DoAdjustScrollbar in src/generic/scrlwing.cpp) and
    // are cached in the DOM for the getters and for the pixels<->units
    // conversion of wxWasmScrollPixelsPerUnit(). The position is NOT
    // applied to the native scroll offset here: the viewport is only
    // moved by ScrollWindow() or by the native scroll itself, see
    // SetScrollPos() (consequence: a non-zero initial position passed to
    // wxScrolledWindow::SetScrollbars() is currently ignored).
    //
    // Native scrolling also needs real overflow, but a wxScrolledWindow
    // that only paints (no children, e.g. MyCanvas in samples/scroll)
    // has nothing overflowing its div, so it would get no scrollbar and
    // no wheel scrolling. To materialize the virtual size, a hidden
    // "spacer" child of range*pixelsPerUnit pixels per axis is kept
    // inside the container while a range is active. The pixel size is
    // computed without circularity: thumb units always represent the
    // client area, so pixelsPerUnit = clientSize/thumb and
    // virtualPx = range*pixelsPerUnit (if thumb is 0 the last known
    // ratio, or 1, is reused).
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        var horz = ($1 & 0x0004) !== 0; // wxHORIZONTAL == 0x0004
        var prop = horz ? 'X' : 'Y';
        var spacerId = 'wx_scrollspacer_' + $0;

        if ($4 > 0) {
            elem.dataset['scrollPos' + prop] = String($2);
            elem.dataset['scrollThumb' + prop] = String($3);
            elem.dataset['scrollRange' + prop] = String($4);
            elem.style['overflow' + prop] = 'auto';

            var clientPx = horz ? elem.clientWidth : elem.clientHeight;
            var ppu = 1;
            if ($3 > 0 && clientPx > 0) {
                ppu = clientPx / $3;
            } else {
                var oldPpu = parseFloat(elem.dataset['scrollPpu' + prop]);
                if (!isNaN(oldPpu) && oldPpu > 0) ppu = oldPpu;
            }
            elem.dataset['scrollPpu' + prop] = String(ppu);

            // Spacer materializing the virtual size (see above). It is
            // absolutely positioned so it does not disturb the layout of
            // the real children; the container divs are themselves
            // positioned, so it enlarges exactly their scrollable
            // overflow. Hidden, it takes part in layout but draws
            // nothing.
            var spacer = document.getElementById(spacerId);
            if (!spacer) {
                spacer = document.createElement('div');
                spacer.id = spacerId;
                spacer.style.position = 'absolute';
                spacer.style.left = '0px';
                spacer.style.top = '0px';
                spacer.style.pointerEvents = 'none';
                spacer.style.visibility = 'hidden';
                elem.appendChild(spacer);
            }
            spacer.style[horz ? 'width' : 'height'] = Math.ceil($4 * ppu) + 'px';
        } else {
            elem.dataset['scrollRange' + prop] = '0';
            elem.style['overflow' + prop] = 'hidden';

            // The spacer is only removed when neither axis scrolls any
            // more; otherwise only the dimension of this axis is reset.
            var spacer = document.getElementById(spacerId);
            if (spacer) {
                var otherProp = horz ? 'Y' : 'X';
                var otherRange = parseInt(elem.dataset['scrollRange' + otherProp], 10);
                if (isNaN(otherRange) || otherRange <= 0) {
                    elem.removeChild(spacer);
                } else {
                    spacer.style[horz ? 'width' : 'height'] = '0px';
                }
            }
        }
    }, GetId(), orientation, pos, thumbvisible, range);
}

void wxWindowWasm::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{
    // Only record the position (in scroll units): moving the native
    // scroll offset here would conflict with ScrollWindow(), which the
    // generic scroll helper always calls right after SetScrollPos() with
    // the same displacement in pixels (wxScrollHelperBase::HandleOnScroll
    // and wxScrollHelper::DoScroll in src/generic/scrlwing.cpp), and with
    // the native scroll itself when the change comes from the user
    // dragging the scrollbar. This matches the documented wx semantics of
    // SetScrollPos(), which changes the scrollbar position without
    // scrolling the window contents; with native scrolling the thumb IS
    // the viewport, so the position only becomes observable through
    // ScrollWindow() or the native scroll.
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        var horz = ($1 & 0x0004) !== 0; // wxHORIZONTAL == 0x0004
        elem.dataset['scrollPos' + (horz ? 'X' : 'Y')] = String($2);
    }, GetId(), orientation, pos);
}

int wxWindowWasm::GetScrollPos( int orientation ) const
{
    // The native scroll offset is in pixels; convert to scroll units (see
    // wxWasmScrollPixelsPerUnit). Pixels == units when the window never
    // got a SetScrollbar() range.
    const int px = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        return ($1 & 0x0004) !== 0 ? elem.scrollLeft : elem.scrollTop;
    }, GetId(), orientation);
    return (int)(px / wxWasmScrollPixelsPerUnit(GetId(), orientation));
}

int wxWindowWasm::GetScrollThumb( int orientation ) const
{
    return EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        var v = parseInt(($1 & 0x0004) !== 0 ? elem.dataset.scrollThumbX
                                             : elem.dataset.scrollThumbY, 10);
        return isNaN(v) ? 0 : v;
    }, GetId(), orientation);
}

int wxWindowWasm::GetScrollRange( int orientation ) const
{
    return EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        var v = parseInt(($1 & 0x0004) !== 0 ? elem.dataset.scrollRangeX
                                             : elem.dataset.scrollRangeY, 10);
        return isNaN(v) ? 0 : v;
    }, GetId(), orientation);
}

// scroll window to the specified position
void wxWindowWasm::ScrollWindow( int dx, int dy, const wxRect *WXUNUSED(rect) )
{
    // While a native scroll event of this window is being dispatched the
    // browser has already moved the viewport; the ScrollWindow() call
    // that wxScrollHelperBase::HandleOnScroll() does in response to that
    // event must be ignored or the window would scroll twice.
    if ( GetId() == s_nativeScrollWindowId )
        return;

    // Scrolling the window contents by (dx, dy) means moving the viewport
    // in the opposite direction. The deltas are in PIXELS (the generic
    // scroll helper multiplies its unit deltas by the scroll rate before
    // calling, see src/generic/scrlwing.cpp).
    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;
        elem.scrollLeft -= $1;
        elem.scrollTop -= $2;
    }, GetId(), dx, dy);
}


#if wxUSE_DRAG_AND_DROP
void wxWindowWasm::SetDropTarget( wxDropTarget *dropTarget )
{

}
#endif

void wxWindowWasm::SetWindowStyleFlag( long style )
{
    wxWindowBase::SetWindowStyleFlag( style );
}

void wxWindowWasm::SetExtraStyle( long exStyle )
{

}



void wxWindowWasm::DoClientToScreen( int *x, int *y ) const
{
    if ( x )
    {
        *x += EM_ASM_INT({
            var elem = document.getElementById($0);
            return elem ? elem.getBoundingClientRect().left : 0;
        }, GetId());
    }
    if ( y )
    {
        *y += EM_ASM_INT({
            var elem = document.getElementById($0);
            return elem ? elem.getBoundingClientRect().top : 0;
        }, GetId());
    }
}


void wxWindowWasm::DoScreenToClient( int *x, int *y ) const
{
    if ( x )
    {
        *x -= EM_ASM_INT({
            var elem = document.getElementById($0);
            return elem ? elem.getBoundingClientRect().left : 0;
        }, GetId());
    }
    if ( y )
    {
        *y -= EM_ASM_INT({
            var elem = document.getElementById($0);
            return elem ? elem.getBoundingClientRect().top : 0;
        }, GetId());
    }
}


// Emulated mouse capture: the DOM has no global grab and
// Element.setPointerCapture() needs the pointerId of the originating event,
// which is not available when wx decides to capture. Instead, while a
// window is captured (s_capturedWindow, defined at the top of this file),
// temporary capture-phase listeners on document re-dispatch
// pointermove/pointerup targeted at the captured window's element
// (swallowing the original event, as a real grab would), so the element's
// own listeners keep receiving the drag with the real coordinates even when
// the pointer leaves the window. The capture ends on pointerup
// (auto-release, as in the desktop ports) or DoReleaseMouse().

// Called from JS when the capture ends on pointerup (see DoCaptureMouse).
extern "C" EMSCRIPTEN_KEEPALIVE void wxWasmMouseCaptureReleased()
{
    s_capturedWindow = nullptr;
}

void wxWindowWasm::DoCaptureMouse()
{
    s_capturedWindow = this;

    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        // Suppress hover (enter/leave) tracking while captured: the global
        // pointermove listener in wxApp::Initialize checks this flag.
        document._wxWasmCaptureActive = true;

        // Drop any previous capture first: only one window can hold it.
        if (document._wxMouseCapture) {
            document.removeEventListener('pointermove', document._wxMouseCapture.move, true);
            document.removeEventListener('pointerup', document._wxMouseCapture.up, true);
            document._wxMouseCapture = null;
        }

        var redispatch = function(e) {
            // Events already inside the captured element flow normally.
            if (elem.contains(e.target)) return;
            e.stopPropagation();
            e.preventDefault();
            elem.dispatchEvent(new PointerEvent(e.type, e));
        };
        var move = function(e) { redispatch(e); };
        var up = function(e) {
            redispatch(e);
            document.removeEventListener('pointermove', move, true);
            document.removeEventListener('pointerup', up, true);
            document._wxMouseCapture = null;
            document._wxWasmCaptureActive = false;
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('wxWasmMouseCaptureReleased', null, [], []);
            }
        };

        // NOTE: no object literal here: top-level commas inside the EM_ASM
        // block would be parsed as macro argument separators.
        document._wxMouseCapture = {};
        document._wxMouseCapture.move = move;
        document._wxMouseCapture.up = up;
        document.addEventListener('pointermove', move, true);
        document.addEventListener('pointerup', up, true);
    }, GetId());
}


void wxWindowWasm::DoReleaseMouse()
{
    s_capturedWindow = nullptr;

    EM_ASM_({
        if (document._wxMouseCapture) {
            document.removeEventListener('pointermove', document._wxMouseCapture.move, true);
            document.removeEventListener('pointerup', document._wxMouseCapture.up, true);
            document._wxMouseCapture = null;
        }
        document._wxWasmCaptureActive = false;
    });
}

wxWindowWasm *wxWindowBase::GetCapture()
{
    return s_capturedWindow;
}


void wxWindowWasm::DoGetPosition(int *x, int *y) const
{
    wxCHECK_RET( x && y, "invalid pointer" );

    *x = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        // Only trust the inline style when it is a pure pixel value:
        // parseInt('100%') would return 100.
        var l = elem.style.left;
        if (l && l.slice(-2) === 'px') return parseInt(l, 10);
        return elem.offsetLeft;
    }, GetId());

    *y = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        var t = elem.style.top;
        if (t && t.slice(-2) === 'px') return parseInt(t, 10);
        return elem.offsetTop;
    }, GetId());
}


void wxWindowWasm::DoGetSize(int *width, int *height) const
{
    wxCHECK_RET( width && height, "invalid pointer" );

    // Read the inline style first: unlike offsetWidth it stays valid
    // even while the element is hidden (display:none). But only trust it
    // when it is a pure pixel value: parseInt('100vw') would return 100
    // (toplevel.cpp fixes 100vw/100vh/100% after Maximize).
    *width = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        var w = elem.style.width;
        if (w && w.slice(-2) === 'px') return parseInt(w, 10);
        return elem.offsetWidth;
    }, GetId());

    *height = EM_ASM_INT({
        var elem = document.getElementById($0);
        if (!elem) return 0;
        var h = elem.style.height;
        if (h && h.slice(-2) === 'px') return parseInt(h, 10);
        return elem.offsetHeight;
    }, GetId());
}



void wxWindowWasm::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    int currentX, currentY;
    GetPosition( &currentX, &currentY );
    if ( x == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        x = currentX;
    if ( y == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        y = currentY;

    // Should we use the best size:

    if (( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH )) ||
        ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT )))
    {
        const wxSize BEST_SIZE = GetBestSize();
        if ( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH ))
            width = BEST_SIZE.x;
        if ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT ))
            height = BEST_SIZE.y;
    }

    int w, h;
    GetSize(&w, &h);
    if (width == -1)
        width = w;
    if (height == -1)
        height = h;

    DoMoveWindow( x, y, width, height );

    // Schedule a repaint after a real resize: the drawing canvas buffer is
    // cleared when re-synchronized with the new size (see wxWindowDCImpl).
    if ( IsShown() && (width != w || height != h) )
        Refresh();

    // Notify the application about geometry changes.
    if ( width != w || height != h )
    {
        wxSizeEvent sizeEvent(wxSize(width, height), GetId());
        sizeEvent.SetEventObject(this);
        HandleWindowEvent(sizeEvent);
    }

    if ( x != currentX || y != currentY )
    {
        wxMoveEvent moveEvent(wxPoint(x, y), GetId());
        moveEvent.SetEventObject(this);
        HandleWindowEvent(moveEvent);
    }
}


void wxWindowWasm::DoGetClientSize(int *width, int *height) const
{
    // No window decorations in the DOM container: client size == size.
    DoGetSize(width, height);
}


void wxWindowWasm::DoSetClientSize(int width, int height)
{
    DoSetSize(wxDefaultCoord, wxDefaultCoord, width, height, wxSIZE_AUTO);
}

void wxWindowWasm::DoMoveWindow(int x, int y, int width, int height)
{
    EM_ASM_INT(
        {
            const currentWindow=document.getElementById($0);
            currentWindow.style.position="absolute";
            currentWindow.style.width=$3.toString()+"px";
            currentWindow.style.height=$4.toString()+"px";
            currentWindow.style.top=$2.toString()+"px";
            currentWindow.style.left=$1.toString()+"px";
            return 1;
        },
        GetId(),
        x,y,width,height
    );
}

#if wxUSE_TOOLTIPS


void wxWindowWasm::DoSetToolTip( wxToolTip *tip )
{
    wxWindowBase::DoSetToolTip(tip);

    const wxString text = tip ? tip->GetTip() : wxString();
    wxCharBuffer buf = text.ToUTF8();

    EM_ASM_({
        var elem = document.getElementById($0);
        if (!elem) return;

        var tip = UTF8ToString($1);
        if (tip.length > 0)
            elem.title = tip;
        else
            elem.removeAttribute('title');
    }, GetId(), buf.data());
}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
// The DOM element of a menu item is only created if the popup container
// of its menu already exists (wxMenuItem::CreateDOM fails silently
// otherwise), which is not the case for a menu filled before its first
// popup. Recreate the DOM of every item once the container exists;
// CreateDOM skips items whose element already exists. Sub-menus are
// handled recursively: their container (the 'wxMenuPopup_' alias div) is
// only created together with the DOM of the sub-menu item itself.
// Non-static because wxMenuBar (src/wasm/menu.cpp) needs it too.
void wxWasmEnsureMenuItemsDOM(wxMenu* menu)
{
    const wxMenuItemList& items = menu->GetMenuItems();
    for ( wxMenuItemList::const_iterator it = items.begin(); it != items.end(); ++it )
    {
        wxMenuItem* item = *it;
        item->CreateDOM(menu);
        if ( wxMenu* subMenu = item->GetSubMenu() )
            wxWasmEnsureMenuItemsDOM(subMenu);
    }
}

bool wxWindowWasm::DoPopupMenu(wxMenu *menu, int x, int y)
{
    if (!menu)
        return false;

    menu->UpdateUI();

    wxPoint screenPos = ClientToScreen(wxPoint(x, y));
    int menuId = menu->GetId();

    // Find or create the floating popup container for this menu. Menu
    // DOM ids are namespaced ('wxMenuPopup_' prefix) so they never
    // collide with the numeric wxWindowID element ids.
    EM_ASM_({
        var popup = document.getElementById('wxMenuPopup_' + $0);
        if (!popup) {
            popup = document.createElement('div');
            popup.id = 'wxMenuPopup_' + $0;
            popup.className = 'wxMenu-popup wxMenu-popup-floating';
            document.body.appendChild(popup);
        }
    }, menuId);

    // The container may not have existed when the items were appended, in
    // which case they have no DOM yet (see wxWasmEnsureMenuItemsDOM).
    wxWasmEnsureMenuItemsDOM(menu);

    EM_ASM_({
        var popup = document.getElementById('wxMenuPopup_' + $0);
        if (!popup) return;
        var x = $1;
        var y = $2;

        // Position the popup at screen coordinates
        popup.style.position = 'fixed';
        popup.style.left = x + 'px';
        popup.style.top = y + 'px';
        popup.style.display = 'block';
        popup.style.zIndex = '2000';

        // Close on click outside; also clean up when an item is chosen
        // (both listeners remove each other to avoid accumulating them).
        function closePopup(e) {
            if (!popup.contains(e.target)) {
                popup.style.display = 'none';
                document.removeEventListener('click', closePopup);
                popup.removeEventListener('click', closePopupOnItem);
            }
        }
        function closePopupOnItem(e) {
            popup.style.display = 'none';
            document.removeEventListener('click', closePopup);
            popup.removeEventListener('click', closePopupOnItem);
        }
        // Delay to avoid immediate close from the triggering click
        setTimeout(function() {
            document.addEventListener('click', closePopup);
            popup.addEventListener('click', closePopupOnItem);
        }, 100);
    }, menuId, screenPos.x, screenPos.y);

    return true;
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindowWasm::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxWindowBase::SetAcceleratorTable( accel );

}
#endif // wxUSE_ACCEL

bool wxWindowWasm::SetBackgroundStyle(wxBackgroundStyle style)
{
    if (!wxWindowBase::SetBackgroundStyle(style))
        return false;

    return true;
}


bool wxWindowWasm::IsTransparentBackgroundSupported(wxString* WXUNUSED(reason)) const
{
    return true;
}

bool wxWindowWasm::SetTransparent(wxByte WXUNUSED(alpha))
{
    // Per-window opacity is not implemented; report success.
    return true;
}


bool wxWindowWasm::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindowBase::SetBackgroundColour(colour) )
        return false;

    // Apply the colour to the container div and, for controls with a native
    // element inside, to that element too: the GTK3 theme fixes some native
    // backgrounds (e.g. white for wxTextCtrl's input) which would otherwise
    // hide the colour set on the container.
    if ( colour.IsOk() )
    {
        wxString css = colour.GetAsString(wxC2S_HTML_SYNTAX);
        wxCharBuffer buf = css.ToUTF8();
        EM_ASM_({
            var elem = document.getElementById($0);
            if (!elem) return;
            var css = UTF8ToString($1);
            elem.style.backgroundColor = css;
            var native = elem.querySelector('input, button, select, textarea');
            if (native) native.style.backgroundColor = css;
        }, GetId(), buf.data());

        Refresh();
    }

    return true;
}

bool wxWindowWasm::SetForegroundColour(const wxColour& colour)
{
    if (!wxWindowBase::SetForegroundColour(colour))
        return false;

    // Same as SetBackgroundColour: the colour is set on the container div
    // and also on the first native element inside it, if any.
    if ( colour.IsOk() )
    {
        wxString css = colour.GetAsString(wxC2S_HTML_SYNTAX);
        wxCharBuffer buf = css.ToUTF8();
        EM_ASM_({
            var elem = document.getElementById($0);
            if (!elem) return;
            var css = UTF8ToString($1);
            elem.style.color = css;
            var native = elem.querySelector('input, button, select, textarea');
            if (native) native.style.color = css;
        }, GetId(), buf.data());

        Refresh();
    }

    return true;
}

WXWidget wxWindowWasm::GetHandle() const
{
    // There is no native handle: the window id doubles as the DOM element id.
    return nullptr;
}

bool wxWindowWasm::EnableTouchEvents(int eventsMask)
{
    if ( eventsMask == wxTOUCH_NONE )
    {

        return true;
    }

    if ( eventsMask & wxTOUCH_PRESS_GESTURES )
    {
    }
    if ( eventsMask & wxTOUCH_PAN_GESTURES )
    {
    }
    if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
    {
    }

    return true;
}

wxWindow *wxWindowBase::DoFindFocus()
{
    // Walk up from the active element until a DOM element whose id is a
    // (numeric) wxWindowID is found.
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

    return FindWindowById(id);
}

wxWindow *wxWindowWasm::DoFindFocus()
{
    return wxWindowBase::DoFindFocus();
}




namespace
{

// Send wxEVT_CHAR_HOOK for the given key event and return true only if it
// was processed (and not skipped), meaning the key must not be processed
// any further. Mirrors SendCharHookEvent() in src/gtk/window.cpp; the
// event propagates up the window chain (wxKeyEvent gives it
// wxEVENT_PROPAGATE_MAX), which is how a wxDialog catches ESC pressed in
// any of its children (wxDialogBase::OnCharHook).
bool WasmSendCharHookEvent(const wxKeyEvent& event, wxWindow* win)
{
    // Same as wxGTK: with the mouse captured the keyboard is considered
    // captured too, so no char hook is sent.
    if ( wxWindow::GetCapture() )
        return false;

    wxKeyEvent hookEvent(wxEVT_CHAR_HOOK, event);
    return win->HandleWindowEvent(hookEvent) && !hookEvent.IsNextEventAllowed();
}

} // anonymous namespace

void wxWindowWasm::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id != m_windowId )
        return;

    if ( event.eventType == "paint" )
    {
        // Deferred repaint queued by Refresh(): send the wxPaintEvent
        // now, unless Update() already repainted synchronously.
        if ( m_refreshPending )
        {
            m_refreshPending = false;
            SendPaintEvent();
        }
        return;
    }

    // ------------------------------------------------------------------------
    // Input events queued by the global DOM listeners registered in
    // wxApp::Initialize (src/wasm/app.cpp) via addInputEvent()/addKeyEvent().
    // Modifier bits: 1=shift, 2=ctrl, 4=alt, 8=meta; bit 16 is wheel-only
    // and marks native scrolling (see the "wheel" branch below). Button
    // bitmask: 1=left, 2=right, 4=middle, 8=aux1, 16=aux2 (DOM
    // MouseEvent.buttons order).
    // ------------------------------------------------------------------------

    if ( event.eventType == "scroll" )
    {
        // Native scrolling of the window's div: x/y carry the new
        // scrollLeft/scrollTop in PIXELS and the button slot the
        // orientation (wxHORIZONTAL or wxVERTICAL, see the listener in
        // app.cpp), but wxScrollWinEvent positions are in scroll UNITS
        // (wxScrollHelperBase uses the event position as the new absolute
        // thumb position in units, see wxScrollHelperBase::CalcScrollInc
        // in src/generic/scrlwing.cpp). The sub-unit remainder is passed
        // as the pixel offset so smooth (sub-unit) native scrolling is
        // preserved in wxScrollHelperBase::HandleOnScroll.
        const int orient = event.button == wxVERTICAL ? wxVERTICAL
                                                      : wxHORIZONTAL;
        const int px = orient == wxVERTICAL ? event.y : event.x;
        const double ppu = wxWasmScrollPixelsPerUnit(m_windowId, orient);
        const int pos = (int)(px / ppu);
        wxScrollWinEvent scrollEvent(wxEVT_SCROLLWIN_THUMBTRACK, pos, orient);
        scrollEvent.SetPixelOffset(px - (int)(pos * ppu));
        scrollEvent.SetId(m_windowId);
        scrollEvent.SetEventObject(this);

        // The browser already moved the viewport natively: the
        // ScrollWindow() call wxScrollHelperBase::HandleOnScroll() will
        // do in response to this event must not move it again (see
        // ScrollWindow and s_nativeScrollWindowId above).
        const int previousNativeScrollWindowId = s_nativeScrollWindowId;
        s_nativeScrollWindowId = m_windowId;
        HandleWindowEvent(scrollEvent);
        s_nativeScrollWindowId = previousNativeScrollWindowId;
        return;
    }

    // Resolve the mouse event type from the DOM event type and button
    // (DOM MouseEvent.button: 0=left, 1=middle, 2=right, 3=aux1, 4=aux2).
    wxEventType mouseType = wxEVT_NULL;
    const std::string& type = event.eventType;

    if ( type == "mousedown" || type == "mouseup" || type == "dblclick" )
    {
        const bool down = type == "mousedown";
        const bool up = type == "mouseup";
        switch ( event.button )
        {
            case 0:
                mouseType = down ? wxEVT_LEFT_DOWN
                          : up   ? wxEVT_LEFT_UP : wxEVT_LEFT_DCLICK;
                break;
            case 1:
                mouseType = down ? wxEVT_MIDDLE_DOWN
                          : up   ? wxEVT_MIDDLE_UP : wxEVT_MIDDLE_DCLICK;
                break;
            case 2:
                mouseType = down ? wxEVT_RIGHT_DOWN
                          : up   ? wxEVT_RIGHT_UP : wxEVT_RIGHT_DCLICK;
                break;
            case 3:
                mouseType = down ? wxEVT_AUX1_DOWN
                          : up   ? wxEVT_AUX1_UP : wxEVT_AUX1_DCLICK;
                break;
            case 4:
                mouseType = down ? wxEVT_AUX2_DOWN
                          : up   ? wxEVT_AUX2_UP : wxEVT_AUX2_DCLICK;
                break;
        }
    }
    else if ( type == "mousemove" )
    {
        mouseType = wxEVT_MOTION;
    }
    else if ( type == "wheel" )
    {
        // Modifier bit 16 (set by the wheel listener in app.cpp) means the
        // window's div scrolls natively in the wheel axis: the browser
        // already moved the viewport and the new position reaches the
        // generic scroll helper through the native "scroll" event handled
        // above, so generating wxEVT_MOUSEWHEEL would make
        // wxScrollHelperBase::HandleOnMouseWheel() scroll the window a
        // second time. wxGTK skips the wheel in the helper for the same
        // reason (see wxScrollHelperBase::ProcessEvent in
        // src/generic/scrlwing.cpp). Consequence, as in wxGTK: handlers of
        // wxEVT_MOUSEWHEEL in a window with active native scrolling are
        // not called. A window whose scrollbars don't overflow (nothing
        // scrollable natively) gets the event normally.
        if ( !(event.mods & 16) )
            mouseType = wxEVT_MOUSEWHEEL;
    }
    else if ( type == "mouseenter" )
    {
        mouseType = wxEVT_ENTER_WINDOW;
        m_mouseInside = true;
    }
    else if ( type == "mouseleave" )
    {
        mouseType = wxEVT_LEAVE_WINDOW;
        m_mouseInside = false;
    }

    if ( mouseType != wxEVT_NULL )
    {
        // Note: the DOM dblclick listener fires after the second down/up
        // pair, so the wx sequence is DOWN UP DOWN UP DCLICK instead of the
        // desktop DOWN UP DOWN DCLICK UP. The down/up events themselves come
        // from the pointer listeners, only the DCLICK is added here.
        wxMouseEvent mouseEvent(mouseType);
        mouseEvent.SetX(event.x);
        mouseEvent.SetY(event.y);
        mouseEvent.SetLeftDown((event.buttons & 1) != 0);
        mouseEvent.SetRightDown((event.buttons & 2) != 0);
        mouseEvent.SetMiddleDown((event.buttons & 4) != 0);
        mouseEvent.SetAux1Down((event.buttons & 8) != 0);
        mouseEvent.SetAux2Down((event.buttons & 16) != 0);
        mouseEvent.SetShiftDown((event.mods & 1) != 0);
        mouseEvent.SetControlDown((event.mods & 2) != 0);
        mouseEvent.SetAltDown((event.mods & 4) != 0);
        mouseEvent.SetMetaDown((event.mods & 8) != 0);
        if ( mouseType == wxEVT_MOUSEWHEEL )
        {
            mouseEvent.m_wheelDelta = 120;
            mouseEvent.m_wheelRotation = event.wheelDelta;
        }
        mouseEvent.SetId(m_windowId);
        mouseEvent.SetEventObject(this);
        HandleWindowEvent(mouseEvent);
        return;
    }

    if ( type == "keydown" || type == "keyup" )
    {
        const bool down = type == "keydown";
        const wxString key = wxString::FromUTF8(event.key.c_str());
        int keyCode = wxWasmKeyToWxKeyCode(key);
        if ( !keyCode )
            return;

        // wx convention: letter key codes are uppercase; the character
        // actually typed (already Shift-translated by the browser) goes
        // to m_uniChar below.
        if ( keyCode >= 'a' && keyCode <= 'z' )
            keyCode = wxToupper(keyCode);

        wxKeyEvent keyEvent(down ? wxEVT_KEY_DOWN : wxEVT_KEY_UP);
        keyEvent.m_keyCode = keyCode;
        wxChar uniChar = 0;
        if ( key.length() == 1 )
            uniChar = key[0U];
        keyEvent.m_uniChar = uniChar;
        keyEvent.SetShiftDown((event.mods & 1) != 0);
        keyEvent.SetControlDown((event.mods & 2) != 0);
        keyEvent.SetAltDown((event.mods & 4) != 0);
        keyEvent.SetMetaDown((event.mods & 8) != 0);
        keyEvent.SetId(m_windowId);
        keyEvent.SetEventObject(this);

        // wxEVT_CHAR_HOOK goes first (on key down only, as in the other
        // ports): if it was processed without being skipped, the key is
        // fully handled and no KEY_DOWN/CHAR is generated.
        if ( down && WasmSendCharHookEvent(keyEvent, this) )
            return;

        HandleWindowEvent(keyEvent);

        // wxEVT_CHAR is only generated for text-producing keys: a single
        // printable character (already Shift-translated by the browser) or
        // Enter/Tab/Backspace, and never with Ctrl/Alt held (those combos
        // are commands, not text).
        const bool printable = (key.length() == 1) ||
            keyCode == WXK_RETURN || keyCode == WXK_TAB || keyCode == WXK_BACK;
        if ( down && printable && (event.mods & 6) == 0 )
        {
            wxKeyEvent charEvent(wxEVT_CHAR, keyEvent);
            HandleWindowEvent(charEvent);
        }
        return;
    }

    if ( type == "setfocus" || type == "killfocus" )
    {
        wxFocusEvent focusEvent(type == "setfocus" ? wxEVT_SET_FOCUS
                                                   : wxEVT_KILL_FOCUS,
                                m_windowId);
        focusEvent.SetEventObject(this);
        HandleWindowEvent(focusEvent);
        return;
    }

    // Any other event type is handled by the overrides in the controls
    // ("click", "input", "change", ...); the base class has no default.
}
