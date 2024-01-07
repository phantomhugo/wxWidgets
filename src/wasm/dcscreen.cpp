/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcscreen.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/dcscreen.h"


wxIMPLEMENT_ABSTRACT_CLASS(wxScreenDCImpl, wxWasmDCImpl);

wxScreenDCImpl::wxScreenDCImpl( wxScreenDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxScreenDCImpl::~wxScreenDCImpl( )
{
}

void wxScreenDCImpl::DoGetSize(int *width, int *height) const
{
    wxDisplaySize(width, height);
}
