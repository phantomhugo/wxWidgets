/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/frame.h
// Purpose:     wxFrame class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FRAME_H_
#define _WX_WASM_FRAME_H_

#include "wx/frame.h"

class QMainWindow;
class QScrollArea;

class WXDLLIMPEXP_CORE wxFrame : public wxFrameBase
{
public:
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxASCII_STR(wxFrameNameStr))
    {
        Init();

        Create( parent, id, title, pos, size, style, name );
    }
    virtual ~wxFrame();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual void SetMenuBar(wxMenuBar *menubar) wxOVERRIDE;
    virtual void SetStatusBar(wxStatusBar *statusBar ) wxOVERRIDE;
    virtual void SetToolBar(wxToolBar *toolbar) wxOVERRIDE;

    virtual void SetWindowStyleFlag( long style ) wxOVERRIDE;

    virtual void AddChild( wxWindowBase *child ) wxOVERRIDE;
    virtual void RemoveChild( wxWindowBase *child ) wxOVERRIDE;

protected:
    virtual void DoGetClientSize(int *width, int *height) const wxOVERRIDE;
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;

private:
    // Common part of all ctors.
    void Init()
    {
        m_qtToolBar = NULL;
    }


    // Currently active native toolbar.
    class QToolBar* m_qtToolBar;

    wxDECLARE_DYNAMIC_CLASS( wxFrame );
};


#endif // _WX_WASM_FRAME_H_
