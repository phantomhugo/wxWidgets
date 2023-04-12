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
    wxWindowWasm();

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

    void AddChild( wxWindowBase *child ) override;

    virtual bool Show( bool show = true ) override;

    virtual void SetLabel(const wxString& label) override;
    virtual wxString GetLabel() const override;

    virtual void DoEnable( bool enable ) override;
    virtual void SetFocus() override;

    // Parent/Child:
    virtual bool Reparent( wxWindowBase *newParent ) override;

    // Z-order
    virtual void Raise() override;
    virtual void Lower() override;

    // move the mouse to the specified position
    virtual void WarpPointer(int x, int y) override;

    virtual void Update() override;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL ) override;

    virtual bool SetCursor( const wxCursor &cursor ) override;
    virtual bool SetFont(const wxFont& font) override;

    // get the (average) character size for the current font
    virtual int GetCharHeight() const override;
    virtual int GetCharWidth() const override;

    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbvisible,
                               int range,
                               bool refresh = true ) override;
    virtual void SetScrollPos( int orient, int pos, bool refresh = true ) override;
    virtual int GetScrollPos( int orient ) const override;
    virtual int GetScrollThumb( int orient ) const override;
    virtual int GetScrollRange( int orient ) const override;

        // scroll window to the specified position
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = NULL ) override;

    // Styles
    virtual void SetWindowStyleFlag( long style ) override;
    virtual void SetExtraStyle( long exStyle ) override;

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) override;
    virtual bool IsTransparentBackgroundSupported(wxString* reason = NULL) const override;
    virtual bool SetTransparent(wxByte alpha) override;
    virtual bool CanSetTransparent() override { return true; }

    virtual bool SetBackgroundColour(const wxColour& colour) override;
    virtual bool SetForegroundColour(const wxColour& colour) override;

    WXWidget GetHandle() const override;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) override;
#endif

#if wxUSE_ACCEL
    // accelerators
    // ------------
    virtual void SetAcceleratorTable( const wxAcceleratorTable& accel ) override;

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
                                 const wxFont *font = NULL) const override;

    // coordinates translation
    virtual void DoClientToScreen( int *x, int *y ) const override;
    virtual void DoScreenToClient( int *x, int *y ) const override;

    // capture/release the mouse, used by Capture/ReleaseMouse()
    virtual void DoCaptureMouse() override;
    virtual void DoReleaseMouse() override;

    // retrieve the position/size of the window
    virtual void DoGetPosition(int *x, int *y) const override;

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) override;
    virtual void DoGetSize(int *width, int *height) const override;

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height) override;
    virtual void DoGetClientSize(int *width, int *height) const override;

    virtual void DoMoveWindow(int x, int y, int width, int height) override;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) override;
#endif // wxUSE_TOOLTIPS

#if wxUSE_MENUS
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y) override;
#endif // wxUSE_MENUS

    virtual bool EnableTouchEvents(int eventsMask) override;

private:
    void Init();

    bool m_mouseInside;

#if wxUSE_ACCEL
    bool m_processingShortcut;
#endif // wxUSE_ACCEL

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxWindowWasm );
};

#endif // _WX_WINDOW_H_
