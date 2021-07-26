/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dataobj.h
// Purpose:     wxDataObject class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
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
