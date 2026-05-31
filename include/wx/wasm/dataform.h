/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dataform.h
// Purpose:     wxDataFormat class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.06.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DATAFORM_H
#define _WX_WASM_DATAFORM_H

class WXDLLIMPEXP_CORE wxDataFormat
{
public:
    wxDataFormat();
    wxDataFormat( wxDataFormatId type );
    wxDataFormat( const wxString &id );
    wxDataFormat( NativeFormat format );

    wxDataFormat& operator=(NativeFormat format)
        { SetId(format); return *this; }

    // comparison (must have both versions)
    bool operator==(NativeFormat format) const
        { return m_format == (NativeFormat)format; }
    bool operator!=(NativeFormat format) const
        { return m_format != (NativeFormat)format; }
    bool operator==(wxDataFormatId format) const;
    bool operator!=(wxDataFormatId format) const
        { return m_type != (wxDataFormatId)format; }

    // explicit and implicit conversions to NativeFormat which is one of
    // standard data types (implicit conversion is useful for preserving the
    // compatibility with old code)
    NativeFormat GetFormatId() const { return m_format; }
    operator NativeFormat() const { return m_format; }

    void SetId( NativeFormat format );

    // string ids are used for custom types - this SetId() must be used for
    // application-specific formats
    wxString GetId() const;
    void SetId( const wxString& id );

    // implementation
    wxDataFormatId GetType() const;

private:
    wxDataFormatId   m_type;
    NativeFormat     m_format;

    void PrepareFormats();
    void SetType( wxDataFormatId type );
};


#endif // _WX_WASM_DATAFORM_H

