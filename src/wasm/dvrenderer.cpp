/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dvrenderer.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// NOTE: this port uses the generic wxDataViewCtrl implementation
// (wxHAS_GENERIC_DATAVIEWCTRL is defined), so everything below is compiled
// out. This file exists only for build compatibility: it is listed in
// build/cmake/files.cmake and removing it would require changing that file.

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_DATAVIEWCTRL

#include "wx/dataview.h"

#ifndef wxHAS_GENERIC_DATAVIEWCTRL

wxDataViewRenderer::wxDataViewRenderer( const wxString &variantType,
                    wxDataViewCellMode mode,
                    int align)
    : wxDataViewRendererBase( variantType, mode, align )
{
}

void wxDataViewRenderer::SetMode( wxDataViewCellMode mode )
{
}

wxDataViewCellMode wxDataViewRenderer::GetMode() const
{
    return wxDataViewCellMode();
}

void wxDataViewRenderer::SetAlignment( int align )
{
}

int wxDataViewRenderer::GetAlignment() const
{
    return 0;
}

void wxDataViewRenderer::EnableEllipsize(wxEllipsizeMode mode)
{
}

wxEllipsizeMode wxDataViewRenderer::GetEllipsizeMode() const
{
    return wxEllipsizeMode();
}

#endif // !wxHAS_GENERIC_DATAVIEWCTRL

#endif // wxUSE_DATAVIEWCTRL
