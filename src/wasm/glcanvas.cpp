/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/glcanvas.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GLCANVAS

#include "wx/glcanvas.h"

#if defined(__VISUALC__)
    #pragma message("OpenGL support is not implemented in wxQt")
#else
    #warning "OpenGL support is not implemented in wxWasm"
#endif
wxGCC_WARNING_SUPPRESS(unused-parameter)

// ----------------------------------------------------------------------------
// wxGLContextAttrs: OpenGL rendering context attributes
// ----------------------------------------------------------------------------
// GLX specific values

wxGLContextAttrs& wxGLContextAttrs::CoreProfile()
{
//    AddAttribBits(GLX_CONTEXT_PROFILE_MASK_ARB,
//                  GLX_CONTEXT_CORE_PROFILE_BIT_ARB);
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MajorVersion(int val)
{
    if ( val > 0 )
    {
        if ( val >= 3 )
            SetNeedsARB();
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::MinorVersion(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::CompatibilityProfile()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ForwardCompatible()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ES2()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::DebugCtx()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::Robust()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::NoResetNotify()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::LoseOnReset()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ResetIsolation()
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::ReleaseFlush(int val)
{
    SetNeedsARB();
    return *this;
}

wxGLContextAttrs& wxGLContextAttrs::PlatformDefaults()
{
    renderTypeRGBA = true;
    return *this;
}

void wxGLContextAttrs::EndList()
{
//    AddAttribute(None);
}

// ----------------------------------------------------------------------------
// wxGLAttributes: Visual/FBconfig attributes
// ----------------------------------------------------------------------------
// GLX specific values

//   Different versions of GLX API use rather different attributes lists, see
//   the following URLs:
//
//   - <= 1.2: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseVisual.xml
//   - >= 1.3: http://www.opengl.org/sdk/docs/man/xhtml/glXChooseFBConfig.xml
//
//   Notice in particular that
//   - GLX_RGBA is boolean attribute in the old version of the API but a
//     value of GLX_RENDER_TYPE in the new one
//   - Boolean attributes such as GLX_DOUBLEBUFFER don't take values in the
//     old version but must be followed by True or False in the new one.

wxGLAttributes& wxGLAttributes::RGBA()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::BufferSize(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Level(int val)
{
//    AddAttribute(GLX_LEVEL);
    AddAttribute(val);
    return *this;
}

wxGLAttributes& wxGLAttributes::DoubleBuffer()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::Stereo()
{
    return *this;
}

wxGLAttributes& wxGLAttributes::AuxBuffers(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
    }
    if ( mGreen >= 0)
    {
    }
    if ( mBlue >= 0)
    {
    }
    if ( mAlpha >= 0)
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Depth(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::Stencil(int val)
{
    if ( val >= 0 )
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::MinAcumRGBA(int mRed, int mGreen, int mBlue, int mAlpha)
{
    if ( mRed >= 0)
    {
    }
    if ( mGreen >= 0)
    {
    }
    if ( mBlue >= 0)
    {
    }
    if ( mAlpha >= 0)
    {
    }
    return *this;
}

wxGLAttributes& wxGLAttributes::SampleBuffers(int val)
{
#ifdef GLX_SAMPLE_BUFFERS_ARB
    if ( val >= 0 && wxGLCanvasX11::IsGLXMultiSampleAvailable() )
    {
        AddAttribute(GLX_SAMPLE_BUFFERS_ARB);
        AddAttribute(val);
    }
#endif
    return *this;
}

wxGLAttributes& wxGLAttributes::Samplers(int val)
{
#ifdef GLX_SAMPLES_ARB
    if ( val >= 0 && wxGLCanvasX11::IsGLXMultiSampleAvailable() )
    {
        AddAttribute(GLX_SAMPLES_ARB);
        AddAttribute(val);
    }
#endif
    return *this;
}

wxGLAttributes& wxGLAttributes::FrameBuffersRGB()
{
//    AddAttribute(GLX_FRAMEBUFFER_SRGB_CAPABLE_ARB);
//    AddAttribute(True);
    return *this;
}

void wxGLAttributes::EndList()
{
}

wxGLAttributes& wxGLAttributes::PlatformDefaults()
{
    // No GLX specific values
    return *this;
}

//---------------------------------------------------------------------------
// wxGlContext
//---------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxGLContext, wxWindow);

wxGLContext::wxGLContext(wxGLCanvas *WXUNUSED(win), const wxGLContext* WXUNUSED(other), const wxGLContextAttrs *WXUNUSED(ctxAttrs))
{
}

bool wxGLContext::SetCurrent(const wxGLCanvas&) const
{
// I think I must destroy and recreate the QGLWidget to change the context?
//    win->GetHandle()->makeCurrent();
    return false;
}

//---------------------------------------------------------------------------
// wxGlCanvas
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
                        const wxPalette& palette)
{
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
    return false;
}

bool wxGLCanvas::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name,
                        const int *attribList,
                        const wxPalette& palette)
{
#if wxUSE_PALETTE
    wxASSERT_MSG( !palette.IsOk(), wxT("palettes not supported") );
#endif // wxUSE_PALETTE
    wxUnusedVar(palette); // Unused when wxDEBUG_LEVEL==0

    return wxWindow::Create( parent, id, pos, size, style, name );
}

bool wxGLCanvas::SwapBuffers()
{
    return true;
}

/* static */
bool
wxGLCanvasBase::IsDisplaySupported(const int *attribList)
{

}

/* static */
bool
wxGLCanvasBase::IsDisplaySupported(const wxGLAttributes& dispAttrs)
{
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
    return false;
}

// ----------------------------------------------------------------------------
// wxGLApp
// ----------------------------------------------------------------------------

bool wxGLApp::InitGLVisual(const int *attribList)
{
    wxLogError("Missing implementation of " + wxString(__FUNCTION__));
    return false;
}

#endif // wxUSE_GLCANVAS
