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

    wxWindowWasm(wxWindow *parent,
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

    bool Create(wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxASCII_STR(wxPanelNameStr));

    virtual void Raise() wxOVERRIDE;
    virtual void Lower() wxOVERRIDE;

    // SetLabel(), which does nothing in wxWindow
    virtual void SetLabel(const wxString& label) wxOVERRIDE { m_Label = label; }
    virtual wxString GetLabel() const wxOVERRIDE            { return m_Label; }

    virtual bool Show( bool show = true ) wxOVERRIDE;
    virtual bool Enable( bool enable = true ) wxOVERRIDE;

    virtual void SetFocus() wxOVERRIDE;

    virtual void WarpPointer(int x, int y) wxOVERRIDE;

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL ) wxOVERRIDE;
    virtual void Update() wxOVERRIDE;

    virtual bool SetBackgroundColour( const wxColour &colour ) wxOVERRIDE;
    virtual bool SetForegroundColour( const wxColour &colour ) wxOVERRIDE;

    virtual bool SetCursor( const wxCursor &cursor ) wxOVERRIDE;
    virtual bool SetFont( const wxFont &font ) wxOVERRIDE;

    virtual int GetCharHeight() const wxOVERRIDE;
    virtual int GetCharWidth() const wxOVERRIDE;

    virtual void ScrollWindow( int dx, int dy, const wxRect* rect = NULL ) wxOVERRIDE;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) wxOVERRIDE;
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept) wxOVERRIDE;

    // Get the unique identifier of a window
    virtual WXWindow GetHandle() const wxOVERRIDE;

    // implementation from now on
    // --------------------------

    // accessors
    // ---------

    // Get main X11 window
    virtual WXWindow X11GetMainWindow() const;

    // Get X11 window representing the client area
    virtual WXWindow GetClientAreaWindow() const;

    void SetLastClick(int button, long timestamp)
        { m_lastButton = button; m_lastTS = timestamp; }

    int GetLastClickedButton() const { return m_lastButton; }
    long GetLastClickTime() const { return m_lastTS; }

    // Gives window a chance to do something in response to a size message, e.g.
    // arrange status bar, toolbar etc.
    virtual bool PreResize();

    // Generates paint events from m_updateRegion
    void SendPaintEvents();

    // Generates paint events from flag
    void SendNcPaintEvents();

    // Generates erase events from m_clearRegion
    void SendEraseEvents();

    // Clip to paint region?
    bool GetClipPaintRegion() { return m_clipPaintRegion; }

    // Return clear region
    wxRegion &GetClearRegion() { return m_clearRegion; }

    void NeedUpdateNcAreaInIdle( bool update = true ) { m_updateNcArea = update; }

    // Inserting into main window instead of client
    // window. This is mostly for a wxWindow's own
    // scrollbars.
    void SetInsertIntoMain( bool insert = true ) { m_insertIntoMain = insert; }
    bool GetInsertIntoMain() { return m_insertIntoMain; }

    // sets the fore/background colour for the given widget
    static void DoChangeForegroundColour(WXWindow widget, wxColour& foregroundColour);
    static void DoChangeBackgroundColour(WXWindow widget, wxColour& backgroundColour, bool changeArmColour = false);

    // I don't want users to override what's done in idle so everything that
    // has to be done in idle time in order for wxX11 to work is done in
    // OnInternalIdle
    virtual void OnInternalIdle() wxOVERRIDE;

protected:
    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // For double-click detection
    long   m_lastTS;         // last timestamp
    int    m_lastButton;     // last pressed button

protected:
    WXWindow              m_mainWindow;
    WXWindow              m_clientWindow;
    bool                  m_insertIntoMain;

    bool                  m_winCaptured;
    wxRegion              m_clearRegion;
    bool                  m_clipPaintRegion;
    bool                  m_updateNcArea;
    bool                  m_needsInputFocus; // Input focus set in OnIdle

    // implement the base class pure virtuals
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = NULL,
                                 int *externalLeading = NULL,
                                 const wxFont *font = NULL) const wxOVERRIDE;
    virtual void DoClientToScreen( int *x, int *y ) const wxOVERRIDE;
    virtual void DoScreenToClient( int *x, int *y ) const wxOVERRIDE;
    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE;
    virtual void DoGetSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoGetClientSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoSetSize(int x, int y,
        int width, int height,
        int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;
    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;
    virtual void DoSetSizeHints(int minW, int minH,
        int maxW, int maxH,
        int incW, int incH) wxOVERRIDE;
    virtual void DoCaptureMouse() wxOVERRIDE;
    virtual void DoReleaseMouse() wxOVERRIDE;
    virtual void KillFocus();

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) wxOVERRIDE;
#endif // wxUSE_TOOLTIPS

private:
    // common part of all ctors
    void Init();

    wxString m_Label;

    wxDECLARE_DYNAMIC_CLASS(wxWindowWasm);
    wxDECLARE_NO_COPY_CLASS(wxWindowWasm);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_WINDOW_H_
