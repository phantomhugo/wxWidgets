/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dc.h
// Purpose:     wxWindowDcImpl class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DCCLIENT_H_
#define _WX_WASM_DCCLIENT_H_

#include "wx/wasm/dc.h"

#include "wx/scopedptr.h"


class WXDLLIMPEXP_CORE wxWindowDCImpl : public wxWasmDCImpl
{
public:
    wxWindowDCImpl( wxDC *owner );
    wxWindowDCImpl( wxDC *owner, wxWindow *win );

    ~wxWindowDCImpl() = default;

protected:
    wxWindow *m_window;

private:
    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};


class WXDLLIMPEXP_CORE wxClientDCImpl : public wxWindowDCImpl
{
public:
    wxClientDCImpl( wxDC *owner );
    wxClientDCImpl( wxDC *owner, wxWindow *win );

    ~wxClientDCImpl() = default;
private:
    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};


class WXDLLIMPEXP_CORE wxPaintDCImpl : public wxWindowDCImpl
{
public:
    wxPaintDCImpl( wxDC *owner );
    wxPaintDCImpl( wxDC *owner, wxWindow *win );
private:
    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};

#endif // _WX_WASM_DCCLIENT_H_
