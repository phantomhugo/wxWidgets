/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/clipbrd.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/clipbrd.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"

// ----------------------------------------------------------------------------
// wxClipboard ctor/dtor
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxClipboard, wxObject);

typedef wxScopedArray<wxDataFormat> wxDataFormatArray;

wxClipboard::wxClipboard()
{
    m_open = false;
}

wxClipboard::~wxClipboard()
{
}

bool wxClipboard::Open()
{
    wxCHECK_MSG( !m_open, false, wxT("clipboard already open") );

    m_open = true;

    return true;
}

void wxClipboard::Close()
{
    wxCHECK_RET( m_open, wxT("clipboard not open") );

    m_open = false;
}

bool wxClipboard::IsOpened() const
{
    return m_open;
}

bool wxClipboard::AddData( wxDataObject *data )
{

    return true;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    Clear();

    return AddData( data );
}

bool wxClipboard::GetData( wxDataObject& data )
{
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

}

void wxClipboard::Clear()
{
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{

}

bool wxClipboard::IsSupportedAsync(wxEvtHandler *sink)
{
    if (m_sink.get())
        return false;  // currently busy, come back later

    wxCHECK_MSG( sink, false, wxT("no sink given") );

    m_sink = sink;

    return true;
}

int wxClipboard::Mode()
{

}
