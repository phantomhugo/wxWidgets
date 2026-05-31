/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dcmemory.h
// Purpose:     wxMemoryDcImple class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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

    DECLARE_NO_COPY_CLASS(wxMemoryDCImpl);
};

#endif // _WX_WASM_DCMEMORY_H_
