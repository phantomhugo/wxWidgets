/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/frame.h
// Purpose:     wxFrame class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.06.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FRAME_H_
#define _WX_WASM_FRAME_H_

#include "wx/frame.h"

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

    virtual void SetMenuBar(wxMenuBar *menubar) override;
    virtual void SetStatusBar(wxStatusBar *statusBar ) override;
    virtual void SetToolBar(wxToolBar *toolbar) override;

    virtual void SetWindowStyleFlag( long style ) override;

    virtual void AddChild( wxWindowBase *child ) override;
    virtual void RemoveChild( wxWindowBase *child ) override;

    // Obtener el ID del contenedor de contenido del frame (para DOM)
    wxString GetContentContainerId() const;

protected:
    virtual void DoGetClientSize(int *width, int *height) const override;
    virtual void DoSetClientSize(int width, int height) override;

private:
    // Common part of all ctors.
    void Init()
    {
    }

    wxDECLARE_DYNAMIC_CLASS( wxFrame );
};


#endif // _WX_WASM_FRAME_H_
