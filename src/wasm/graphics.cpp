/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/graphics.cpp
// Purpose:     Graphics context methods for the Qt platform
// Author:      Jay Nabonne
// Created:     2018-12-13
// Copyright:   (c) Jay Nabonne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_GRAPHICS_CONTEXT

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/icon.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/dcprint.h"
    #include "wx/window.h"
#endif

#include "wx/graphics.h"
#include "wx/scopedptr.h"
#include "wx/tokenzr.h"

#include "wx/private/graphics.h"

namespace
{

// Ensure that the given painter is active by calling begin() if it isn't. If
// it already is, don't do anything.
class EnsurePainterIsActive
{
public:
    explicit EnsurePainterIsActive(QPainter* painter)
        : m_painter(painter),
          m_wasActive(painter->isActive())
    {
        if ( !m_wasActive )
            m_painter->begin(&m_picture);
    }

    ~EnsurePainterIsActive()
    {
        if ( !m_wasActive )
            m_painter->end();
    }

private:
    bool m_wasActive;

    wxDECLARE_NO_COPY_CLASS(EnsurePainterIsActive);
};

} // anonymous namespace

wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return &gs_qtGraphicsRenderer;
}

#endif // wxUSE_GRAPHICS_CONTEXT
