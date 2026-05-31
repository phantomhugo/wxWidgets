/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dcscreen.h
// Purpose:     wxScreenDcImpl class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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
    virtual void DoGetSize(int *width, int *height) const override;

    wxDECLARE_ABSTRACT_CLASS(wxScreenDCImpl);
};

#endif // _WX_WASM_DCSCREEN_H_
