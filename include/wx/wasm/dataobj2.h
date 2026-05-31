/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dataobj.h
// Purpose:     wxDataObject class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.06.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DATAOBJ2_H_
#define _WX_WASM_DATAOBJ2_H_

class WXDLLIMPEXP_CORE wxBitmapDataObject : public wxBitmapDataObjectBase
{
public:
    wxBitmapDataObject();
    wxBitmapDataObject(const wxBitmap& bitmap);

protected:

private:
};



class WXDLLIMPEXP_CORE wxFileDataObject : public wxFileDataObjectBase
{
public:
    wxFileDataObject();

    void AddFile( const wxString &filename );
};

#endif // _WX_WASM_DATAOBJ2_H_
