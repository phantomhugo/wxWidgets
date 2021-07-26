/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dcscreen.h
// Purpose:     wxScreenDcImpl class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_wasm_DCSCREEN_H_
#define _WX_wasm_DCSCREEN_H_

#include "wx/wasm/dcclient.h"

class WXDLLIMPEXP_CORE wxScreenDCImpl : public wxWindowDCImpl
{
public:
    wxScreenDCImpl( wxScreenDC *owner );

    ~wxScreenDCImpl();

protected:
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;

    wxDECLARE_ABSTRACT_CLASS(wxScreenDCImpl);
};

#endif // _WX_WASM_DCSCREEN_H_
