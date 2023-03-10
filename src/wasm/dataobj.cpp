/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dataobj.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
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

wxDataFormat::wxDataFormat(wxDataFormatId formatId)
{
    SetType(formatId);
}

wxDataFormat::wxDataFormat(const wxString &id)
{
    SetId(id);
}

const wxString& wxDataFormat::GetMimeType() const
{
    return m_mimeType;
}

void wxDataFormat::SetMimeType(const wxString& mimeType)
{
    m_mimeType = mimeType;
    m_formatId = wxDF_INVALID;
}

void wxDataFormat::SetId(const wxString& id)
{
    SetMimeType(id);
}

const wxString& wxDataFormat::GetId() const
{
    return m_mimeType;
}

wxDataFormatId wxDataFormat::GetType() const
{
    return m_formatId;
}

void wxDataFormat::SetType(wxDataFormatId formatId)
{
    m_mimeType = DataFormatIdToMimeType(formatId);
    m_formatId = formatId;
}

bool wxDataFormat::operator==(wxDataFormatId format) const
{
    return m_mimeType == DataFormatIdToMimeType(format)
        && m_formatId == format;
}

bool wxDataFormat::operator!=(wxDataFormatId format) const
{
    return !operator==(format);
}

bool wxDataFormat::operator!=(const wxDataFormat& format) const
{
    return !operator==(format);
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
// ----------------------------------------------------------------------------
// wxTextDataObject
// ---------------------------------------------------------------------------

void wxTextDataObject::GetAllFormats(wxDataFormat *formats,
    wxDataObjectBase::Direction WXUNUSED(dir)) const
{
    formats[0] = wxDataFormat(wxDF_UNICODETEXT);
    formats[1] = wxDataFormat(wxDF_TEXT);
}

//#############################################################################

wxFileDataObject::wxFileDataObject()
{
}

void wxFileDataObject::AddFile( const wxString &WXUNUSED(filename) )
{

}
