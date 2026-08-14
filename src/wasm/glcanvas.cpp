/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/glcanvas.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"
#include "wx/window.h"

#include <emscripten/html5.h>

// Defined in src/wasm/window.cpp: resolves a DOM element id to its window.
extern wxWindowWasm* wxWasmFindWindowByDomId(int domId);

// wxGLCanvas is implemented on top of WebGL (OpenGL ES 2/3): the window's
// div gets a <canvas> element (wx_glcanvas_<domId>) and the context is an
// Emscripten WebGL context created on it. GL calls made by the application
// are translated to WebGL calls on the current context by Emscripten's GL
// library. Known limitations, imposed by WebGL itself:
//   - no context sharing (the "other" wxGLContext parameter is ignored),
//   - no stereo or auxiliary buffers,
//   - fixed-function pipeline (glBegin/glEnd and friends) only works when
//     the application is linked with -sLEGACY_GL_EMULATION.

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

// Searches a wx attribute list (id/value pairs ended by 0, with the
// valueless flag attributes intermixed) for the given attribute and, when
// found, stores its value (1 for flags) and returns true.
static bool wxWasmGLFindAttr(const int* list, int attr, int* value)
{
    if ( !list )
        return false;

    for ( int i = 0; list[i]; )
    {
        const int id = list[i++];
        bool valued = true;
        switch ( id )
        {
            case WX_GL_RGBA:
            case WX_GL_DOUBLEBUFFER:
            case WX_GL_STEREO:
            case WX_GL_FRAMEBUFFER_SRGB:
            case WX_GL_CORE_PROFILE:
            case WX_GL_COMPAT_PROFILE:
            case WX_GL_FORWARD_COMPAT:
            case WX_GL_ES2:
            case WX_GL_DEBUG:
            case WX_GL_ROBUST_ACCESS:
            case WX_GL_NO_RESET_NOTIFY:
            case WX_GL_LOSE_ON_RESET:
            case WX_GL_RESET_ISOLATION:
                valued = false;
                break;
        }
        if ( id == attr )
        {
            *value = valued ? list[i] : 1;
            return true;
        }
        if ( valued )
            ++i;
    }
    return false;
}

// Resizes the WebGL canvas buffer to the real size of its window div,
// returning true when the buffer was actually resized (which resets it:
// whatever was drawn is gone and the window must be repainted).
static bool wxWasmGLSyncCanvasSize(int domId)
{
    // NB: assigning width/height resets the drawing buffer even when the
    // value is unchanged, so only touch it when it actually differs.
    return EM_ASM_INT({
        var div = document.getElementById($0);
        var c = document.getElementById('wx_glcanvas_' + $0);
        if (div && c && div.clientWidth > 0 && div.clientHeight > 0) {
            var changed = false;
            if (c.width !== div.clientWidth) { c.width = div.clientWidth; changed = true; }
            if (c.height !== div.clientHeight) { c.height = div.clientHeight; changed = true; }
            return changed ? 1 : 0;
        }
        return 0;
    }, domId) != 0;
}

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
    AddAttribute(WX_GL_CORE_PROFILE);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    if ( val > 0 )
    {
        AddAttribute(WX_GL_MAJOR_VERSION);
        AddAttribute(val);

        if ( val >= 3 )
            SetNeedsARB();
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_MINOR_VERSION);
        AddAttribute(val);
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    AddAttribute(WX_GL_COMPAT_PROFILE);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    AddAttribute(WX_GL_FORWARD_COMPAT);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    AddAttribute(WX_GL_ES2);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    AddAttribute(WX_GL_DEBUG);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    AddAttribute(WX_GL_ROBUST_ACCESS);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    AddAttribute(WX_GL_NO_RESET_NOTIFY);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    AddAttribute(WX_GL_LOSE_ON_RESET);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    AddAttribute(WX_GL_RESET_ISOLATION);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int val)
{
    AddAttribute(WX_GL_RELEASE_FLUSH);
    AddAttribute(val);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    return *this;
}

void wxGLContextAttrs::EndList()
{
    AddAttribute(0);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------

wxGLAttributes& wxGLAttributes::RGBA()
{
    AddAttribute(WX_GL_RGBA);
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_BUFFER_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
    AddAttribute(WX_GL_LEVEL);
    AddAttribute(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    AddAttribute(WX_GL_DOUBLEBUFFER);
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    AddAttribute(WX_GL_STEREO);
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_AUX_BUFFERS);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        AddAttribute(WX_GL_MIN_RED);
        AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        AddAttribute(WX_GL_MIN_GREEN);
        AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        AddAttribute(WX_GL_MIN_BLUE);
        AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        AddAttribute(WX_GL_MIN_ALPHA);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_DEPTH_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_STENCIL_SIZE);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_RED);
        AddAttribute(mRed);
    }
    if ( mGreen >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_GREEN);
        AddAttribute(mGreen);
    }
    if ( mBlue >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_BLUE);
        AddAttribute(mBlue);
    }
    if ( mAlpha >= 0)
    {
        AddAttribute(WX_GL_MIN_ACCUM_ALPHA);
        AddAttribute(mAlpha);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_SAMPLE_BUFFERS);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
    if ( val >= 0 )
    {
        AddAttribute(WX_GL_SAMPLES);
        AddAttribute(val);
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
    AddAttribute(WX_GL_FRAMEBUFFER_SRGB);
    return *this;
}

void wxGLAttributes::EndList()
{
    AddAttribute(0);
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    // No WebGL specific values
    return *this;
}

//---------------------------------------------------------------------------
// wxGLContext
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLContext, wxGLContextBase);

wxGLContext::wxGLContext(wxGLCanvas *win,
                         const wxGLContext* WXUNUSED(other),
                         const wxGLContextAttrs *ctxAttrs)
{
    // "other" (context sharing) is not supported: WebGL has no share lists.

    const wxGLContextAttrs& attrs =
        ctxAttrs ? *ctxAttrs : win->GetGLCTXAttrs();
    const wxGLAttributes& dispAttrs = win->GetDispAttrs();

    // Map the wx attributes onto the WebGL context attributes. Only the
    // explicitly given ones are overridden; the rest keep the WebGL
    // defaults (alpha, depth and antialias on, stencil off).
    EmscriptenWebGLContextAttributes glAttrs;
    emscripten_webgl_init_context_attributes(&glAttrs);

    int val;
    if ( wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_MIN_ALPHA, &val) )
        glAttrs.alpha = val > 0;
    if ( wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_DEPTH_SIZE, &val) )
        glAttrs.depth = val > 0;
    if ( wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_STENCIL_SIZE, &val) )
        glAttrs.stencil = val > 0;
    if ( wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_SAMPLE_BUFFERS, &val) )
        glAttrs.antialias = val > 0;

    // WebGL2 implements OpenGL ES 3: use it when an OpenGL >= 3 context is
    // requested, ES 2 maps to WebGL1. Default to WebGL1 (most compatible,
    // and the only one the legacy fixed-function emulation supports).
    glAttrs.majorVersion = 1;
    if ( wxWasmGLFindAttr(attrs.GetGLAttrs(), WX_GL_MAJOR_VERSION, &val) )
    {
        if ( val > 3 )
        {
            // There is no OpenGL ES beyond 3.x, so no WebGL context can
            // satisfy this request.
            wxLogMessage("wxGLContext: OpenGL %d.x is not supported by WebGL",
                         val);
            return;
        }
        if ( val == 3 )
            glAttrs.majorVersion = 2;
    }

    wxString selector = "#" + win->GetGLCanvasId();
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
        emscripten_webgl_create_context(selector.ToUTF8().data(), &glAttrs);
    if ( ctx <= 0 )
    {
        // Retry with the other WebGL version, the browser may not support
        // the one we asked for.
        glAttrs.majorVersion = glAttrs.majorVersion == 1 ? 2 : 1;
        ctx = emscripten_webgl_create_context(selector.ToUTF8().data(),
                                              &glAttrs);
    }

    if ( ctx <= 0 )
    {
        wxLogError("wxGLContext: WebGL context creation failed");
        return;
    }

    m_glContext = ctx;
    m_isOk = true;
}

wxGLContext::~wxGLContext()
{
    if ( m_glContext )
        emscripten_webgl_destroy_context(m_glContext);
}

bool wxGLContext::SetCurrent(const wxGLCanvas& win) const
{
    if ( !m_glContext )
        return false;

    if ( emscripten_webgl_make_context_current(m_glContext) !=
             EMSCRIPTEN_RESULT_SUCCESS )
        return false;

    // The canvas buffer starts at the default 300x150 when the window was
    // created before being laid out (and no wxEVT_SIZE arrives when the
    // explicit creation size already matched). Sync it here, before the
    // application draws: resizing resets the drawing buffer, but at this
    // point nothing has been drawn yet in this frame.
    wxWasmGLSyncCanvasSize(
        static_cast<const wxWindowWasm*>(&win)->GetDomWindowId());

    // When the application is linked with -sLEGACY_GL_EMULATION, the
    // immediate-mode emulation initializes itself from the Browser module
    // context-creation callbacks, which only run for contexts created
    // through Module.canvas — not for ones created with
    // emscripten_webgl_create_context() like ours. Initialize it here,
    // once, with the context already current.
    EM_ASM_({
        if (typeof GLImmediate !== 'undefined' && !GLImmediate.initted) {
            Module.useWebGL = true;
            GLImmediate.init();
        }
    });

    return true;
}

/* static */
void wxGLContextBase::ClearCurrent()
{
    emscripten_webgl_make_context_current(0);
}

/* static */
wxGLExtFunction wxGLContextBase::GetProcAddress(const wxString& name)
{
    void* p = reinterpret_cast<void*>(
        emscripten_webgl_get_proc_address(name.ToUTF8().data()));
    return reinterpret_cast<wxGLExtFunction>(p);
}

//---------------------------------------------------------------------------
// wxGLCanvas
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLCanvas, wxWindow);

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       const wxGLAttributes& dispAttrs,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, dispAttrs, id, pos, size, style, name, palette);
}

wxGLCanvas::wxGLCanvas(wxWindow *parent,
                       wxWindowID id,
                       const int *attribList,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name,
                       const wxPalette& palette)
{
    Create(parent, id, pos, size, style, name, attribList, palette);
}

wxGLCanvas::~wxGLCanvas()
{
}

bool wxGLCanvas::Create(wxWindow *parent,
                        const wxGLAttributes& dispAttrs,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const wxPalette& WXUNUSED(palette))
{
    m_dispAttrs = dispAttrs;

    if ( !wxWindow::Create( parent, id, pos, size, style, name ) )
        return false;

    // The GL output goes to a <canvas> element filling the window's div
    // (this canvas gets a WebGL context, so it is separate from the 2D
    // wx_canvas_<id> used by the wxDC machinery).
    const int domId = static_cast<wxWindowWasm*>(this)->GetDomWindowId();
    EM_ASM_({
        var div = document.getElementById($0);
        if (!div) return;
        var c = document.getElementById('wx_glcanvas_' + $0);
        if (!c) {
            c = document.createElement('canvas');
            c.id = 'wx_glcanvas_' + $0;
            c.className = 'wxGLCanvas';
            c.style.position = 'absolute';
            c.style.left = '0';
            c.style.top = '0';
            c.style.width = '100%';
            c.style.height = '100%';
            div.appendChild(c);
        }
        if (div.clientWidth > 0 && div.clientHeight > 0) {
            c.width = div.clientWidth;
            c.height = div.clientHeight;
        }
    }, domId);

    // Keep the drawing buffer in sync with the window size. A real resize
    // resets the buffer, so the window must repaint (DoSetSize() may skip
    // its paint event when the effective size didn't change).
    Bind(wxEVT_SIZE, [domId](wxSizeEvent& event)
    {
        if ( wxWasmGLSyncCanvasSize(domId) )
        {
            if ( auto* win = wxWasmFindWindowByDomId(domId) )
                win->Refresh();
        }
        event.Skip();
    });
    return true;
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& WXUNUSED(palette))
{
    if ( !ParseAttribList(attribList, m_dispAttrs, &m_GLCTXAttrs) )
        return false;

    return Create(parent, m_dispAttrs, id, pos, size, style, name);
}

wxString wxGLCanvas::GetGLCanvasId() const
{
    return wxString::Format("wx_glcanvas_%d",
        static_cast<const wxWindowWasm*>(this)->GetDomWindowId());
}

bool wxGLCanvas::SwapBuffers()
{
    // Present the frame now (otherwise Emscripten presents it when the
    // execution yields back to the browser event loop). The buffer size is
    // synced from the wxEVT_SIZE handler instead of here: resizing the
    // buffer at this point would wipe the frame that was just drawn.
    return emscripten_webgl_commit_frame() == EMSCRIPTEN_RESULT_SUCCESS;
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{
    wxGLAttributes dispAttrs;
    if ( !ParseAttribList(attribList, dispAttrs) )
        return false;

    return IsDisplaySupported(dispAttrs);
}

/* static */
bool wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    // WebGL cannot do stereoscopic displays nor auxiliary buffers; the rest
    // (RGBA, double buffer, depth, stencil, alpha, multisampling) is either
    // supported or silently approximated by the browser.
    int val;
    if ( wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_STEREO, &val) ||
         wxWasmGLFindAttr(dispAttrs.GetGLAttrs(), WX_GL_AUX_BUFFERS, &val) )
        return false;

    return true;
}

/* static */
bool wxGLCanvasBase::IsExtensionSupported(const char *extension)
{
    // Valid only when a context is current.
    const char* exts =
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    return exts && IsExtensionInList(extension, exts);
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *WXUNUSED(attribList))
{
    // There is no visual to choose with WebGL, the canvas element always
    // works.
    return true;
}

#endif // wxUSE_GLCANVAS
