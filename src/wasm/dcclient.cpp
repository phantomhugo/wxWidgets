/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcclient.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/wasm/dcclient.h"

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner ):wxWasmDCImpl(owner)
{

}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win ):wxWasmDCImpl(owner)
{

}

//##############################################################################

wxClientDCImpl::wxClientDCImpl( wxDC *owner ):wxWindowDCImpl(owner)
{

}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win ):wxWindowDCImpl(owner,win)
{

}

//##############################################################################

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner ):wxWindowDCImpl(owner)
{

}
wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win ):wxWindowDCImpl(owner,win)
{

}
