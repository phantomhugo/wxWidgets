/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dcmemory.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
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
