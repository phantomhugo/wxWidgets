/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/wasm/glcanvas.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GLCANVAS_H_
#define _WX_GLCANVAS_H_

#include <GL/gl.h>
#include <stdint.h>

class WXDLLIMPEXP_GL wxGLContext : public wxGLContextBase
{
public:
    wxGLContext(wxGLCanvas *win,
                const wxGLContext *other = nullptr,
                const wxGLContextAttrs *ctxAttrs = nullptr);
    virtual ~wxGLContext();

    virtual bool SetCurrent(const wxGLCanvas& win) const override;

private:
    // The Emscripten WebGL context handle (EMSCRIPTEN_WEBGL_CONTEXT_HANDLE,
    // an integer; 0 when creation failed).
    intptr_t m_glContext = 0;

    wxDECLARE_CLASS(wxGLContext);
};

// ----------------------------------------------------------------------------
// wxGLCanvas: OpenGL output window
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GL wxGLCanvas : public wxGLCanvasBase
{
public:
    wxGLCanvas() = default;

    explicit // avoid implicitly converting a wxWindow* to wxGLCanvas
    wxGLCanvas(wxWindow *parent,
               const wxGLAttributes& dispAttrs,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    explicit
    wxGLCanvas(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = nullptr,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);

    ~wxGLCanvas();

    bool Create(wxWindow *parent,
                const wxGLAttributes& dispAttrs,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const wxPalette& palette = wxNullPalette);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxGLCanvasName,
                const int *attribList = nullptr,
                const wxPalette& palette = wxNullPalette);

    virtual bool SwapBuffers() override;

    // The WebGL <canvas> element id for this window ("wx_glcanvas_<domId>").
    wxString GetGLCanvasId() const;

    // The display attributes the canvas was created with, used by
    // wxGLContext when creating the WebGL context.
    const wxGLAttributes& GetDispAttrs() const { return m_dispAttrs; }

private:
    wxGLAttributes m_dispAttrs;

//    wxDECLARE_EVENT_TABLE();
    wxDECLARE_CLASS(wxGLCanvas);
};

#endif // _WX_GLCANVAS_H_

