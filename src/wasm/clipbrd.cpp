/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/clipbrd.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/clipbrd.h"
#include "wx/scopedarray.h"
#include "wx/scopeguard.h"
#include <emscripten.h>

// ----------------------------------------------------------------------------
// wxClipboard ctor/dtor
// ----------------------------------------------------------------------------
//
// Implementation note: the browser clipboard API (navigator.clipboard) is
// asynchronous and requires a user gesture, while the wxClipboard API is
// synchronous. We therefore keep a local copy of the text in m_textCache,
// which makes copy/paste work inside the application, and additionally
// forward it to the system clipboard on a best-effort basis without
// waiting for the result. Reading the real system clipboard contents is
// not possible synchronously, so GetData() can only return the text that
// was previously copied from inside this application.

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
    wxCHECK_MSG( m_open, false, wxT("clipboard not open") );

    wxCHECK_MSG( data, false, wxT("data is invalid") );

    // Only text is supported for now.
    wxTextDataObject *textData = dynamic_cast<wxTextDataObject*>(data);
    if ( !textData )
    {
        delete data;
        return false;
    }

    m_textCache = textData->GetText();

    // The clipboard owns the data object.
    delete data;

    // Forward the text to the system clipboard on a best-effort basis:
    // navigator.clipboard.writeText() is asynchronous and may fail if
    // there is no user gesture or permission, so we just ignore the result.
    wxCharBuffer buf = m_textCache.ToUTF8();
    EM_ASM_({
        var text = UTF8ToString($0);
        if (navigator.clipboard && navigator.clipboard.writeText) {
            navigator.clipboard.writeText(text).catch(function(err) {});
        }
    }, buf.data());

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

    // navigator.clipboard.readText() returns a Promise and cannot be used
    // synchronously, so only the text cached by AddData() is available.
    wxTextDataObject *textData = dynamic_cast<wxTextDataObject*>(&data);
    if ( !textData || m_textCache.empty() )
        return false;

    textData->SetText(m_textCache);

    return true;
}

void wxClipboard::Clear()
{
    m_textCache.clear();
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    return format.GetType() == wxDF_TEXT || format.GetType() == wxDF_UNICODETEXT;
}

bool wxClipboard::IsSupportedAsync(wxEvtHandler *sink)
{
    if (m_sink.get())
        return false;  // currently busy, come back later

    wxCHECK_MSG( sink, false, wxT("no sink given") );

    // The local cache reliably reflects the text copied from inside this
    // application, so when it is not empty the event can be sent right away
    // without going through the asynchronous browser clipboard API.
    if ( !m_textCache.empty() )
    {
        wxClipboardEvent* event = new wxClipboardEvent(wxEVT_CLIPBOARD_CHANGED);
        event->SetEventObject(this);
        event->AddFormat(wxDF_UNICODETEXT);
        sink->QueueEvent(event);

        return true;
    }

    m_sink = sink;

    // Ask the browser for the real system clipboard contents.
    // navigator.clipboard.readText() returns a Promise and requires a user
    // gesture and/or the "clipboard-read" permission, so it may be
    // rejected: in that case, or when the API is not available at all, the
    // clipboard is reported as not supporting text.
    EM_ASM_({
        var clipboard = $0;
        function done(hasText) {
            Module.ccall('wxWasmClipboardTextRead', null,
                         ['number', 'number'], [clipboard, hasText]);
        }
        if (navigator.clipboard && navigator.clipboard.readText) {
            navigator.clipboard.readText().then(function(text) {
                done(text.length > 0 ? 1 : 0);
            }, function(err) {
                done(0);
            });
        } else {
            done(0);
        }
    }, this);

    return true;
}

void wxClipboard::SendClipboardEvent(bool hasText)
{
    if ( !m_sink )
        return;

    wxClipboardEvent* event = new wxClipboardEvent(wxEVT_CLIPBOARD_CHANGED);
    event->SetEventObject(this);
    if ( hasText )
        event->AddFormat(wxDF_UNICODETEXT);

    m_sink->QueueEvent(event);
    m_sink.Release();
}

// C function exposed to JavaScript: called when the
// navigator.clipboard.readText() promise settles, with hasText != 0 if the
// system clipboard contains non-empty text.
extern "C" EMSCRIPTEN_KEEPALIVE
void wxWasmClipboardTextRead(wxClipboard* clipboard, int hasText)
{
    clipboard->SendClipboardEvent(hasText != 0);
}

int wxClipboard::Mode()
{
    return 0;
}
