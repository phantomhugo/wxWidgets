/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcmemory.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/dcmemory.h"

wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner ):wxWasmDCImpl(owner)
{

}
wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxBitmap& bitmap ):wxWasmDCImpl(owner)
{

}
wxMemoryDCImpl::wxMemoryDCImpl( wxMemoryDC *owner, wxDC *dc ):wxWasmDCImpl(owner)
{

}
wxMemoryDCImpl::~wxMemoryDCImpl()
{

}

wxBitmap wxMemoryDCImpl::DoGetAsBitmap(const wxRect *subrect) const
{

}
void wxMemoryDCImpl::DoSelect(const wxBitmap& bitmap)
{

}

const wxBitmap& wxMemoryDCImpl::GetSelectedBitmap() const
{

}

wxBitmap& wxMemoryDCImpl::GetSelectedBitmap()
{

}
