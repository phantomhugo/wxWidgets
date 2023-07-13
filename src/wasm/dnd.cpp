/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dnd.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"
#include "wx/scopedarray.h"
#include "wx/window.h"

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
    const wxDataFormat droppedFormat = GetMatchingPair();
}

wxDataFormat wxDropTarget::GetMatchingPair()
{

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
}

#endif // wxUSE_DRAG_AND_DROP
