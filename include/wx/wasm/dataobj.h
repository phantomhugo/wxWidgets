/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dataobj.h
// Purpose:     wxDataObject class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.06.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DATAOBJ_H_
#define _WX_WASM_DATAOBJ_H_

// ----------------------------------------------------------------------------
// wxDataObject is the same as wxDataObjectBase under wxQT
// ----------------------------------------------------------------------------


class WXDLLIMPEXP_CORE wxDataObject : public wxDataObjectBase
{
public:
    wxDataObject();
    virtual ~wxDataObject();

    virtual bool IsSupportedFormat( const wxDataFormat& format, Direction dir = Get ) const;

private:
};

#endif // _WX_WASM_DATAOBJ_H_
