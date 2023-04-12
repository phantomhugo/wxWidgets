/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dnd.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"
#include "wx/scopedarray.h"
#include "wx/window.h"

wxDropTarget::wxDropTarget(wxDataObject *dataObject)
    : wxDropTargetBase(dataObject),
      m_pImpl(new Impl(this))
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

    const wxString& mimeType = droppedFormat.GetMimeType();
    if ( mimeType.empty() )
        return false;

    const QString qMimeType = wxQtConvertString(mimeType);
    const QByteArray data = m_pImpl->GetMimeData()->data(qMimeType);

    return m_dataObject->SetData(droppedFormat, data.size(), data.data());
}

wxDataFormat wxDropTarget::GetMatchingPair()
{
    const QMimeData* mimeData = m_pImpl->GetMimeData();
    if ( mimeData == nullptr || m_dataObject == nullptr )
        return wxFormatInvalid;

    const QStringList formats = mimeData->formats();
    for ( int i = 0; i < formats.count(); ++i )
    {
        const wxDataFormat format(wxQtConvertString(formats[i]));

        if ( m_dataObject->IsSupportedFormat(format) )
        {
            return format;
        }
    }
    return wxFormatInvalid;
}

void wxDropTarget::ConnectTo(QWidget* widget)
{
    m_pImpl->ConnectTo(widget);
}

void wxDropTarget::Disconnect()
{
    m_pImpl->Disconnect();
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
    wxCHECK_MSG(m_data != nullptr, wxDragNone,
                wxT("No data in wxDropSource!"));

    wxCHECK_MSG(m_parentWindow != nullptr, wxDragNone,
                wxT("null parent window in wxDropSource!"));

    QDrag drag(m_parentWindow->GetHandle());
    drag.setMimeData(CreateMimeData(m_data));

    SetDragCursor(drag, m_cursorCopy, Qt::CopyAction);
    SetDragCursor(drag, m_cursorMove, Qt::MoveAction);
    SetDragCursor(drag, m_cursorStop, Qt::IgnoreAction);

    const Qt::DropActions actions =
        flags == wxDrag_CopyOnly
            ? Qt::CopyAction
            : Qt::CopyAction | Qt::MoveAction;

    const Qt::DropAction defaultAction =
        flags == wxDrag_DefaultMove
            ? Qt::MoveAction
            : Qt::CopyAction;

    return DropActionToDragResult(drag.exec(actions, defaultAction));
}

#endif // wxUSE_DRAG_AND_DROP
