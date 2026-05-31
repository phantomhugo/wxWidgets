/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dnf.h
// Purpose:     wxDrop classes
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DND_H_
#define _WX_WASM_DND_H_

#define wxDROP_ICON(name)   wxCursor(name##_xpm)

class WXDLLIMPEXP_CORE wxDropTarget : public wxDropTargetBase
{
public:
    wxDropTarget(wxDataObject *dataObject = NULL);
    virtual ~wxDropTarget();

    virtual bool OnDrop(wxCoord x, wxCoord y) override;
    virtual wxDragResult OnData(wxCoord x,
                                wxCoord y,
                                wxDragResult def) override;
    virtual bool GetData() override;

    wxDataFormat GetMatchingPair();

    void Disconnect();

private:
    class Impl;
    Impl* m_pImpl;
};

class WXDLLIMPEXP_CORE wxDropSource: public wxDropSourceBase
{
public:
    wxDropSource(wxWindow *win = NULL,
                 const wxCursor &copy = wxNullCursor,
                 const wxCursor &move = wxNullCursor,
                 const wxCursor &none = wxNullCursor);

    wxDropSource(wxDataObject& data,
                 wxWindow *win,
                 const wxCursor &copy = wxNullCursor,
                 const wxCursor &move = wxNullCursor,
                 const wxCursor &none = wxNullCursor);

    virtual wxDragResult DoDragDrop(int flags = wxDrag_CopyOnly) override;

private:
    wxWindow* m_parentWindow;
};
#endif // _WX_WASM_DND_H_
