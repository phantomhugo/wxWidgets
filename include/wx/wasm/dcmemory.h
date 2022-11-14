/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dcmemory.h
// Purpose:     wxMemoryDcImple class
// Author:      Hugo Castellanos
// Created:     18.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DCMEMORY_H_
#define _WX_WASM_DCMEMORY_H_

#include "wx/wasm/dcclient.h"

class WXDLLIMPEXP_CORE wxMemoryDCImpl : public wxWasmDCImpl
{
public:
    wxMemoryDCImpl( wxMemoryDC *owner );
    wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap );
    wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc );
    ~wxMemoryDCImpl();

    virtual wxBitmap DoGetAsBitmap(const wxRect *subrect) const override;
    virtual void DoSelect(const wxBitmap& bitmap) override;

    virtual const wxBitmap& GetSelectedBitmap() const override;
    virtual wxBitmap& GetSelectedBitmap() override;

private:
    wxBitmap m_selected;

    DECLARE_CLASS(wxMemoryDCImpl);
    DECLARE_NO_COPY_CLASS(wxMemoryDCImpl);
};

#endif // _WX_WASM_DCMEMORY_H_
