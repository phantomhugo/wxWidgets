/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/clipbrd.h
// Purpose:     wxClipboard class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
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

    virtual bool Open() wxOVERRIDE;
    virtual void Close() wxOVERRIDE;
    virtual bool IsOpened() const wxOVERRIDE;

    virtual bool AddData( wxDataObject *data ) wxOVERRIDE;
    virtual bool SetData( wxDataObject *data ) wxOVERRIDE;
    virtual bool GetData( wxDataObject& data ) wxOVERRIDE;
    virtual void Clear() wxOVERRIDE;
    virtual bool IsSupported( const wxDataFormat& format ) wxOVERRIDE;
    virtual bool IsSupportedAsync(wxEvtHandler *sink) wxOVERRIDE;

private:
    int Mode();

    wxEvtHandlerRef    m_sink;

    bool m_open;

    wxDECLARE_DYNAMIC_CLASS(wxClipboard);
};

#endif // _WX_WASM_CLIPBRD_H_
