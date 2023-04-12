/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcclient.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/dcclient.h"

//##############################################################################

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner )
{
    m_window = nullptr;
    m_ok = false;
}

wxWindowDCImpl::wxWindowDCImpl( wxDC *owner, wxWindow *win )
{
    m_window = win;
}

wxWindowDCImpl::~wxWindowDCImpl()
{
    if ( m_ok )
    {
        m_ok = false;
    }
    if ( m_window )
    {
        // do not destroy in base class as it is owned by the window
        m_qtPainter = nullptr;
    }
}

//##############################################################################

wxIMPLEMENT_CLASS(wxClientDCImpl,wxWindowDCImpl);

wxClientDCImpl::wxClientDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
}

wxClientDCImpl::wxClientDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner )
{
    m_window = win;

}

wxClientDCImpl::~wxClientDCImpl()
{

}

//##############################################################################

wxIMPLEMENT_CLASS(wxPaintDCImpl,wxClientDCImpl);

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner )
    : wxWindowDCImpl( owner )
{
    if ( m_ok )
    {

    }
}

wxPaintDCImpl::wxPaintDCImpl( wxDC *owner, wxWindow *win )
    : wxWindowDCImpl( owner, win )
{
    if ( m_ok )
    {

    }
}
