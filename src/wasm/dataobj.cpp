/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dataobj.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/dataobj.h"
#include "wx/scopedarray.h"

typedef wxScopedArray<wxDataFormat> wxDataFormatArray;

namespace
{

wxString DataFormatIdToMimeType(wxDataFormatId formatId)
{
    switch ( formatId )
    {
        case wxDF_TEXT: return "text/plain";
        case wxDF_BITMAP: return "image/bmp";
        case wxDF_TIFF: return "image/tiff";
        case wxDF_WAVE: return "audio/x-wav";
        case wxDF_UNICODETEXT: return "text/plain";
        case wxDF_HTML: return "text/html";
        case wxDF_METAFILE:
        case wxDF_SYLK:
        case wxDF_DIF:
        case wxDF_OEMTEXT:
        case wxDF_DIB:
        case wxDF_PALETTE:
        case wxDF_PENDATA:
        case wxDF_RIFF:
        case wxDF_ENHMETAFILE:
        case wxDF_FILENAME:
        case wxDF_LOCALE:
        case wxDF_PRIVATE:
        case wxDF_INVALID:
        case wxDF_MAX:
        default:
            return "";
    }
}

} // anonymous namespace

wxDataFormat::wxDataFormat()
{
    m_type = wxDF_INVALID;
    m_format = nullptr;
}

wxDataFormat::wxDataFormat(wxDataFormatId formatId)
{
    SetType(formatId);
}

wxDataFormat::wxDataFormat(NativeFormat format)
{
    SetId(format);
}

wxDataFormat::wxDataFormat(const wxString &id)
{
    SetId(id);
}

void wxDataFormat::SetId(NativeFormat format)
{
    // There is no native clipboard format type in the browser, so a
    // NativeFormat simply encodes one of the standard wxDataFormatId values.
    SetType((wxDataFormatId)(size_t)format);
}

void wxDataFormat::SetId(const wxString& id)
{
    // String ids are used for custom, application-specific formats.
    m_type = wxDF_PRIVATE;
    m_format = (NativeFormat)(size_t)wxDF_PRIVATE;
    m_id = id;
}

wxString wxDataFormat::GetId() const
{
    if ( !m_id.empty() )
        return m_id;

    return DataFormatIdToMimeType(m_type);
}

wxDataFormatId wxDataFormat::GetType() const
{
    return m_type;
}

void wxDataFormat::SetType(wxDataFormatId formatId)
{
    m_type = formatId;
    m_format = (NativeFormat)(size_t)formatId;
    m_id = DataFormatIdToMimeType(formatId);
}

bool wxDataFormat::operator==(wxDataFormatId format) const
{
    return m_type == format;
}

bool wxDataFormat::operator==(const wxDataFormat& other) const
{
    if ( m_type != other.m_type )
        return false;

    // Custom formats are equal only if their string ids match.
    return m_type != wxDF_PRIVATE || m_id == other.m_id;
}

//############################################################################

wxDataObject::wxDataObject()
{
}

wxDataObject::~wxDataObject()
{
}

bool wxDataObject::IsSupportedFormat(const wxDataFormat& format,
                                     Direction dir) const
{
    const size_t formatCount = GetFormatCount(dir);
    if ( formatCount == 1 )
    {
        return format == GetPreferredFormat();
    }

    wxScopedArray<wxDataFormat> formats(formatCount);
    GetAllFormats(formats.get(), dir);

    for ( size_t n = 0; n < formatCount; ++n )
    {
        if ( formats[n] == format )
            return true;
    }

    return false;
}

//############################################################################

wxBitmapDataObject::wxBitmapDataObject()
{
}

wxBitmapDataObject::wxBitmapDataObject( const wxBitmap &bitmap )
    : wxBitmapDataObjectBase( bitmap )
{
}

//#############################################################################

wxFileDataObject::wxFileDataObject()
{
}

void wxFileDataObject::AddFile( const wxString &WXUNUSED(filename) )
{

}
