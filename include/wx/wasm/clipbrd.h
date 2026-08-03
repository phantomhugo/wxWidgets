/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/clipbrd.h
// Purpose:     wxClipboard class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CLIPBRD_H_
#define _WX_WASM_CLIPBRD_H_

#include "wx/weakref.h"


class WXDLLIMPEXP_CORE wxClipboard : public wxClipboardBase
{
public:
    wxClipboard();
    ~wxClipboard();

    virtual bool Open() override;
    virtual void Close() override;
    virtual bool IsOpened() const override;

    virtual bool AddData( wxDataObject *data ) override;
    virtual bool SetData( wxDataObject *data ) override;
    virtual bool GetData( wxDataObject& data ) override;
    virtual void Clear() override;
    virtual bool IsSupported( const wxDataFormat& format ) override;
    virtual bool IsSupportedAsync(wxEvtHandler *sink) override;

    // Sends wxEVT_CLIPBOARD_CHANGED to the current sink, reporting text as
    // supported or not, and releases the sink. Public because it is called
    // from the extern "C" JavaScript callback in clipbrd.cpp.
    void SendClipboardEvent(bool hasText);

private:
    int Mode();

    wxEvtHandlerRef    m_sink;

    bool m_open;

    // Local copy of the last text put on the clipboard: the browser
    // clipboard API is asynchronous, so this cache is what makes
    // copy/paste work synchronously inside the application.
    wxString m_textCache;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_WASM_CLIPBRD_H_
