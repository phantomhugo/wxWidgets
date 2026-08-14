/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/webview.h
// Purpose:     wxWebView classes
// Author:      Hugo Armando Castellanos Morales
// Created:     2026-08-13
// Copyright:   (c) 2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_WEBVIEW_H_
#define _WX_WASM_WEBVIEW_H_

#include "wx/control.h"
#include "wx/webview.h"

extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendWasm[];

// ----------------------------------------------------------------------------
// wxWebViewWasm: web view backed by an <iframe>
// ----------------------------------------------------------------------------

// The wxWebView control already runs inside a browser, so no native engine
// (Edge/WebKit/...) is needed: the backend is a plain <iframe> filling the
// window div. Limitations come from the same-origin policy of the browser
// sandbox: reading the page (title, source, selection, zoom text) only
// works for same-origin documents (same server, srcdoc, blob:), while
// navigation (load, back/forward, reload) works for any URL.
class WXDLLIMPEXP_WEBVIEW wxWebViewWasm : public wxWebView
{
public:
    wxWebViewWasm() {}

    wxWebViewWasm(wxWindow* parent,
                  wxWindowID id,
                  const wxString& url = wxASCII_STR(wxWebViewDefaultURLStr),
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0,
                  const wxString& name = wxASCII_STR(wxWebViewNameStr))
    {
        Create(parent, id, url, pos, size, style, name);
    }

    bool Create(wxWindow* parent,
                wxWindowID id,
                const wxString& url = wxASCII_STR(wxWebViewDefaultURLStr),
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxWebViewNameStr));

    virtual ~wxWebViewWasm();

    virtual wxString GetCurrentTitle() const override;
    virtual wxString GetCurrentURL() const override;
    virtual bool IsBusy() const override;
    virtual bool IsEditable() const override { return m_editable; }
    virtual void LoadURL(const wxString& url) override;
    virtual void Print() override;
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler) override;
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT) override;
    virtual bool RunScript(const wxString& javascript, wxString* output = nullptr) const override;
    virtual void SetEditable(bool enable = true) override;
    virtual void Stop() override;

    virtual bool CanGoBack() const override;
    virtual bool CanGoForward() const override;
    virtual void GoBack() override;
    virtual void GoForward() override;
    virtual void ClearHistory() override;
    virtual void EnableHistory(bool enable = true) override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory() override;
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory() override;
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item) override;

    virtual bool CanSetZoomType(wxWebViewZoomType type) const override;
    virtual float GetZoomFactor() const override { return m_zoomFactor; }
    virtual wxWebViewZoomType GetZoomType() const override { return m_zoomType; }
    virtual void SetZoomFactor(float zoom) override;
    virtual void SetZoomType(wxWebViewZoomType zoomType) override;

    virtual bool CanUndo() const override { return false; }
    virtual bool CanRedo() const override { return false; }
    virtual void Undo() override {}
    virtual void Redo() override {}

    virtual void* GetNativeBackend() const override { return nullptr; }

    virtual void DoSetPage(const wxString& html, const wxString& baseUrl) override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    // Sends a wxWebViewEvent of the given type with the current URL.
    void SendWebViewEvent(wxEventType type);

    // Reads a property of the iframe document (same-origin only), or
    // returns an empty string.
    wxString GetIframeDocumentProp(const char* prop) const;

    wxString m_currentUrl;
    float m_zoomFactor = 1.0f;
    wxWebViewZoomType m_zoomType = wxWEBVIEW_ZOOM_TYPE_LAYOUT;
    bool m_editable = false;
    bool m_loading = false;

    wxDECLARE_DYNAMIC_CLASS(wxWebViewWasm);
};

// ----------------------------------------------------------------------------
// wxWebViewFactoryWasm
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryWasm : public wxWebViewFactory
{
public:
    virtual wxWebView* Create() override { return new wxWebViewWasm; }
    virtual wxWebView* Create(wxWindow* parent,
                              wxWindowID id,
                              const wxString& url = wxASCII_STR(wxWebViewDefaultURLStr),
                              const wxPoint& pos = wxDefaultPosition,
                              const wxSize& size = wxDefaultSize,
                              long style = 0,
                              const wxString& name = wxASCII_STR(wxWebViewNameStr)) override
    {
        return new wxWebViewWasm(parent, id, url, pos, size, style, name);
    }
    virtual bool IsAvailable() override { return true; }

    // Defined out of line: wxWebViewConfigurationImpl is declared in
    // wx/private/webview.h, not available in this public header.
    virtual wxWebViewConfiguration CreateConfiguration() override;
};

#endif // _WX_WASM_WEBVIEW_H_
