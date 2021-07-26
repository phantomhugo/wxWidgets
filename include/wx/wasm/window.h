/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/window.h
// Purpose:     wxWindow class
// Author:      Hugo Castellanos
// Created:     24.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#include "wx/region.h"

// ----------------------------------------------------------------------------
// wxWindow class for Motif - see also wxWindowBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowWasm : public wxWindowBase
{
    friend class WXDLLIMPEXP_FWD_CORE wxDC;
    friend class WXDLLIMPEXP_FWD_CORE wxWindowDC;

public:
    wxWindowWasm() { Init(); }

    wxWindowWasm(wxWindowWasm *parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxPanelNameStr))
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowWasm();

    bool Create(wxWindowWasm *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxPanelNameStr));

    // Used by all window classes in the widget creation process.
    void PostCreation( bool generic = true );

    void AddChild( wxWindowBase *child ) wxOVERRIDE;

    virtual bool Show( bool show = true ) wxOVERRIDE;

    virtual void SetLabel(const wxString& label) wxOVERRIDE;
    virtual wxString GetLabel() const wxOVERRIDE;

    virtual void DoEnable( bool enable ) wxOVERRIDE;
    virtual void SetFocus() wxOVERRIDE;

    // Parent/Child:
    virtual bool Reparent( wxWindowBase *newParent ) wxOVERRIDE;

    // Z-order
    virtual void Raise() wxOVERRIDE;
    virtual void Lower() wxOVERRIDE;

    // move the mouse to the specified position
    virtual void WarpPointer(int x, int y) wxOVERRIDE;

    virtual void Update() wxOVERRIDE;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL ) wxOVERRIDE;

    virtual bool SetCursor( const wxCursor &cursor ) wxOVERRIDE;
    virtual bool SetFont(const wxFont& font) wxOVERRIDE;

    // get the (average) character size for the current font
    virtual int GetCharHeight() const wxOVERRIDE;
    virtual int GetCharWidth() const wxOVERRIDE;

    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbvisible,
                               int range,
                               bool refresh = true ) wxOVERRIDE;
    virtual void SetScrollPos( int orient, int pos, bool refresh = true ) wxOVERRIDE;
    virtual int GetScrollPos( int orient ) const wxOVERRIDE;
    virtual int GetScrollThumb( int orient ) const wxOVERRIDE;
    virtual int GetScrollRange( int orient ) const wxOVERRIDE;

        // scroll window to the specified position
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = NULL ) wxOVERRIDE;

    // Styles
    virtual void SetWindowStyleFlag( long style ) wxOVERRIDE;
    virtual void SetExtraStyle( long exStyle ) wxOVERRIDE;

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) wxOVERRIDE;
    virtual bool IsTransparentBackgroundSupported(wxString* reason = NULL) const wxOVERRIDE;
    virtual bool SetTransparent(wxByte alpha) wxOVERRIDE;
    virtual bool CanSetTransparent() wxOVERRIDE { return true; }

    virtual bool SetBackgroundColour(const wxColour& colour) wxOVERRIDE;
    virtual bool SetForegroundColour(const wxColour& colour) wxOVERRIDE;

    WXWidget GetHandle() const wxOVERRIDE;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) wxOVERRIDE;
#endif

#if wxUSE_ACCEL
    // accelerators
    // ------------
    virtual void SetAcceleratorTable( const wxAcceleratorTable& accel ) wxOVERRIDE;

#endif // wxUSE_ACCEL

#if wxUSE_TOOLTIPS
    // applies tooltip to the widget.
    virtual void QtApplyToolTip(const wxString& text);
#endif // wxUSE_TOOLTIPS

protected:
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = NULL,
                                 int *externalLeading = NULL,
                                 const wxFont *font = NULL) const wxOVERRIDE;

    // coordinates translation
    virtual void DoClientToScreen( int *x, int *y ) const wxOVERRIDE;
    virtual void DoScreenToClient( int *x, int *y ) const wxOVERRIDE;

    // capture/release the mouse, used by Capture/ReleaseMouse()
    virtual void DoCaptureMouse() wxOVERRIDE;
    virtual void DoReleaseMouse() wxOVERRIDE;

    // retrieve the position/size of the window
    virtual void DoGetPosition(int *x, int *y) const wxOVERRIDE;

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;
    virtual void DoGetClientSize(int *width, int *height) const wxOVERRIDE;

    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) wxOVERRIDE;
#endif // wxUSE_TOOLTIPS

#if wxUSE_MENUS
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y) wxOVERRIDE;
#endif // wxUSE_MENUS

    virtual bool EnableTouchEvents(int eventsMask) wxOVERRIDE;

private:
    void Init();

    bool m_mouseInside;

#if wxUSE_ACCEL
    bool m_processingShortcut;
#endif // wxUSE_ACCEL

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxWindowWasm );
};

#endif // _WX_WINDOW_H_
