/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dnd.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"
#include "wx/scopedarray.h"
#include "wx/window.h"

#include <vector>

#include <emscripten.h>

// Drag and drop is emulated in-process: HTML5 drag & drop cannot be started
// programmatically (a native drag must begin on a draggable element with a
// real user gesture, before the application gets to call DoDragDrop()), so
// this port runs its own modal drag loop instead. While the loop runs, the
// pointer is tracked with capture-phase DOM listeners (which also suppress
// the normal wx mouse events), the window under the pointer is resolved
// through the DOM element id map and its wxDropTarget receives the usual
// OnEnter/OnDragOver/OnLeave/OnDrop/OnData calls. The data travels between
// the wxDataObjects directly, so any format supported by wxDataObject works,
// including custom ones. Limitation: drags cannot leave or enter the browser
// tab (no OS interop).

// Defined in src/wasm/window.cpp: resolves a DOM element id to its window.
extern wxWindowWasm* wxWasmFindWindowByDomId(int domId);

namespace
{

struct wxWasmDragState
{
    wxDataObject*  data = nullptr;      // the source's data object
    wxDropSource*  source = nullptr;
    int            flags = 0;
    wxWindow*      overWin = nullptr;   // window under the pointer (has target)
    wxDragResult   overResult = wxDragNone; // last OnDragOver/OnEnter result
    wxDragResult   result = wxDragNone;     // final result of the drag
    bool           done = false;
};

wxWasmDragState s_dnd;

// Copies all the formats supported by both data objects from the source to
// the destination (the target's data object). Returns true if at least one
// format was transferred.
bool wxWasmDndCopyData(wxDataObject* dst, wxDataObject* src)
{
    if ( !dst || !src )
        return false;

    bool any = false;
    const size_t count = dst->GetFormatCount(wxDataObject::Set);
    if ( count )
    {
        wxScopedArray<wxDataFormat> formats(count);
        dst->GetAllFormats(formats.get(), wxDataObject::Set);
        for ( size_t i = 0; i < count; ++i )
        {
            const wxDataFormat& fmt = formats[i];
            if ( !src->IsSupported(fmt) )
                continue;
            const size_t size = src->GetDataSize(fmt);
            if ( !size )
                continue;
            std::vector<char> buf(size);
            if ( src->GetDataHere(fmt, buf.data()) &&
                 dst->SetData(fmt, size, buf.data()) )
            {
                any = true;
            }
        }
    }
    return any;
}

// Resolves the window with a drop target under the given client position
// (walking up from the hit window through the parent chain) and, when found,
// fills pt with the position in its client coordinates.
wxWindow* wxWasmDndTargetAt(int clientX, int clientY, wxPoint* pt)
{
    // NB: wx auto-assigned ids are negative, so a separate sentinel (not
    // the sign) marks "no window under the pointer".
    const int domId = EM_ASM_INT({
        var el = document.elementFromPoint($0, $1);
        while (el && el !== document.documentElement) {
            if (el.id) {
                var id = parseInt(el.id, 10);
                if (!isNaN(id)) return id;
            }
            el = el.parentElement;
        }
        return 0x7FFFFFFF;
    }, clientX, clientY);

    wxWindow* win = domId != 0x7FFFFFFF ? wxWasmFindWindowByDomId(domId)
                                        : nullptr;
    while ( win && !win->GetDropTarget() )
        win = win->GetParent();

    if ( win && pt )
    {
        const wxWindowID winDomId =
            static_cast<wxWindowWasm*>(win)->GetDomWindowId();
        int32_t rect[2] = { 0, 0 };
        EM_ASM_({
            var el = document.getElementById($0);
            if (el) {
                var r = el.getBoundingClientRect();
                HEAP32[$1 >> 2] = Math.round(r.left);
                HEAP32[($1 >> 2) + 1] = Math.round(r.top);
            }
        }, winDomId, rect);
        pt->x = clientX - rect[0];
        pt->y = clientY - rect[1];
    }
    return win;
}

void wxWasmDndSetCursor(wxDragResult res)
{
    // The application may give its own feedback (e.g. change the cursor or
    // paint a drop marker); only apply the default one when it doesn't.
    if ( s_dnd.source && s_dnd.source->GiveFeedback(res) )
        return;

    // NB: no array literal here: inside EM_ASM code only parentheses
    // protect commas from the C preprocessor, brackets/braces don't.
    EM_ASM_({
        var cursor = '';
        if ($0 === 0) cursor = 'no-drop';
        else if ($0 === 1) cursor = 'copy';
        else if ($0 === 2) cursor = 'move';
        else if ($0 === 3) cursor = 'alias';
        document.body.style.cursor = cursor;
    }, res == wxDragNone ? 0 :
       res == wxDragCopy ? 1 :
       res == wxDragMove ? 2 :
       res == wxDragLink ? 3 : -1);
}

// The default action, as on the other platforms: moving requires
// wxDrag_AllowMove and is the default with wxDrag_DefaultMove, Ctrl forces
// copying; it is always wxDragNone when the carried data is not in a format
// the target understands.
wxDragResult wxWasmDndDefaultAction(wxDropTarget* target, int mods)
{
    wxDragResult def = wxDragNone;
    if ( s_dnd.data && target &&
         target->GetMatchingPair().GetType() != wxDF_INVALID )
    {
        const bool ctrl = (mods & 2) != 0; // wxMOD_CONTROL style bit
        if ( (s_dnd.flags & wxDrag_AllowMove) && !ctrl )
            def = wxDragMove;
        else
            def = wxDragCopy;
    }
    return def;
}

} // anonymous namespace

extern "C"
{

EMSCRIPTEN_KEEPALIVE void wxWasmDndMove(int clientX, int clientY, int mods)
{
    if ( s_dnd.done )
        return;

    wxPoint pt;
    wxWindow* win = wxWasmDndTargetAt(clientX, clientY, &pt);

    if ( win != s_dnd.overWin )
    {
        if ( s_dnd.overWin )
            s_dnd.overWin->GetDropTarget()->OnLeave();
        s_dnd.overWin = win;
        s_dnd.overResult = wxDragNone;
        if ( win )
        {
            wxDropTarget* target = win->GetDropTarget();
            s_dnd.overResult = target->OnEnter(pt.x, pt.y,
                wxWasmDndDefaultAction(target, mods));
        }
    }
    else if ( win )
    {
        wxDropTarget* target = win->GetDropTarget();
        s_dnd.overResult = target->OnDragOver(pt.x, pt.y,
            wxWasmDndDefaultAction(target, mods));
    }

    wxWasmDndSetCursor(s_dnd.overWin ? s_dnd.overResult : wxDragNone);
}

EMSCRIPTEN_KEEPALIVE void wxWasmDndUp(int clientX, int clientY, int mods)
{
    if ( s_dnd.done )
        return;

    // Make sure the position under the pointer is up to date: the last
    // pointermove may not have been processed yet.
    wxWasmDndMove(clientX, clientY, mods);

    if ( s_dnd.overWin && s_dnd.overResult != wxDragNone )
    {
        wxPoint pt;
        wxWindow* win = wxWasmDndTargetAt(clientX, clientY, &pt);
        if ( win )
        {
            wxDropTarget* target = win->GetDropTarget();
            if ( target->OnDrop(pt.x, pt.y) )
                s_dnd.result = target->OnData(pt.x, pt.y, s_dnd.overResult);
        }
    }
    s_dnd.done = true;
}

EMSCRIPTEN_KEEPALIVE void wxWasmDndCancel()
{
    s_dnd.result = wxDragCancel;
    s_dnd.done = true;
}

} // extern "C"

// ----------------------------------------------------------------------------

wxDropTarget::wxDropTarget(wxDataObject *dataObject)
    : wxDropTargetBase(dataObject)
{
}

wxDropTarget::~wxDropTarget()
{
}

bool wxDropTarget::OnDrop(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y))
{
    return true;
}

wxDragResult wxDropTarget::OnData(wxCoord WXUNUSED(x),
                                  wxCoord WXUNUSED(y),
                                  wxDragResult def)
{
    return GetData() ? def : wxDragNone;
}

bool wxDropTarget::GetData()
{
    // May only be called from inside OnData(), while the drag state is live.
    if ( s_dnd.done || !s_dnd.data )
        return false;

    return wxWasmDndCopyData(m_dataObject, s_dnd.data);
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    if ( !m_dataObject || !s_dnd.data )
        return wxDataFormat();

    const size_t count = m_dataObject->GetFormatCount(wxDataObject::Set);
    wxScopedArray<wxDataFormat> formats(count ? count : 1);
    if ( count )
        m_dataObject->GetAllFormats(formats.get(), wxDataObject::Set);
    for ( size_t i = 0; i < count; ++i )
    {
        if ( s_dnd.data->IsSupported(formats[i]) )
            return formats[i];
    }
    return wxDataFormat();
}

void wxDropTarget::Disconnect()
{
}

//###########################################################################

wxDropSource::wxDropSource(wxWindow *win,
              const wxCursor &copy,
              const wxCursor &move,
              const wxCursor &none)
    : wxDropSourceBase(copy, move, none),
      m_parentWindow(win)
{
}

wxDropSource::wxDropSource(wxDataObject& data,
              wxWindow *win,
              const wxCursor &copy,
              const wxCursor &move,
              const wxCursor &none)
    : wxDropSourceBase(copy, move, none),
      m_parentWindow(win)
{
    SetData(data);
}

wxDragResult wxDropSource::DoDragDrop(int flags /*=wxDrag_CopyOnly*/)
{
    if ( !m_data )
        return wxDragError;

    s_dnd.data = m_data;
    s_dnd.source = this;
    s_dnd.flags = flags;
    s_dnd.overWin = nullptr;
    s_dnd.overResult = wxDragNone;
    s_dnd.result = wxDragNone;
    s_dnd.done = false;

    // Capture-phase listeners: they run before the normal input listeners
    // (bubble phase, see wxApp::Initialize in src/wasm/app.cpp) and
    // stopPropagation() keeps the regular wx mouse events from being
    // generated while the drag is in progress.
    EM_ASM_({
        var modsOf = function(e) {
            return (e.shiftKey ? 1 : 0) | (e.ctrlKey ? 2 : 0) |
                   (e.altKey ? 4 : 0) | (e.metaKey ? 8 : 0);
        };
        var move = function(e) {
            e.stopPropagation();
            Module.ccall('wxWasmDndMove', null,
                ['number', 'number', 'number'],
                [Math.round(e.clientX), Math.round(e.clientY), modsOf(e)]);
        };
        var up = function(e) {
            e.stopPropagation();
            Module.ccall('wxWasmDndUp', null,
                ['number', 'number', 'number'],
                [Math.round(e.clientX), Math.round(e.clientY), modsOf(e)]);
        };
        var key = function(e) {
            if (e.key === 'Escape') {
                e.stopPropagation();
                Module.ccall('wxWasmDndCancel', null, [], []);
            }
        };
        document.addEventListener('pointermove', move, true);
        document.addEventListener('pointerup', up, true);
        document.addEventListener('keydown', key, true);
        window._wxWasmDndMove = move;
        window._wxWasmDndUp = up;
        window._wxWasmDndKey = key;
        document.body.style.cursor = 'grabbing';
    });

    // Modal drag loop: ends from wxWasmDndUp()/wxWasmDndCancel() above.
    while ( !s_dnd.done )
        emscripten_sleep(10);

    if ( s_dnd.overWin )
        s_dnd.overWin->GetDropTarget()->OnLeave();

    EM_ASM_({
        document.removeEventListener('pointermove', window._wxWasmDndMove, true);
        document.removeEventListener('pointerup', window._wxWasmDndUp, true);
        document.removeEventListener('keydown', window._wxWasmDndKey, true);
        window._wxWasmDndMove = null;
        window._wxWasmDndUp = null;
        window._wxWasmDndKey = null;
        document.body.style.cursor = '';
    });

    s_dnd.data = nullptr;
    s_dnd.source = nullptr;

    return s_dnd.result;
}

#endif // wxUSE_DRAG_AND_DROP
