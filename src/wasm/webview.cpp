/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/webview.cpp
// Purpose:     wxWebView implementation for wxWasm (iframe backend)
// Author:      Hugo Armando Castellanos Morales
// Created:     2026-08-13
// Copyright:   (c) 2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_WEBVIEW

#include "wx/webview.h"
#include "wx/wasm/webview.h"
#include "wx/private/webview.h"
#include <emscripten.h>

extern WXDLLIMPEXP_DATA_WEBVIEW(const char) wxWebViewBackendWasm[] = "wxWebViewWasm";

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewWasm, wxControl);

// ============================================================================
// wxWebViewWasm implementation
// ============================================================================

bool wxWebViewWasm::Create(wxWindow* parent, wxWindowID id,
    const wxString& url, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var iframe = document.createElement('iframe');
        iframe.className = 'wxWebView-iframe';
        iframe.style.width = '100%';
        iframe.style.height = '100%';
        iframe.style.border = 'none';
        iframe.style.boxSizing = 'border-box';

        iframe.addEventListener('load', function() {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'webview_loaded', 0, 0]);
            }
        });

        container.appendChild(iframe);
    }, GetId());

    // Let the application know the control is ready.
    wxWebViewEvent event(wxEVT_WEBVIEW_CREATED, GetId(), "", "");
    event.SetEventObject(this);
    HandleWindowEvent(event);

    LoadURL(url);

    return true;
}

wxWebViewWasm::~wxWebViewWasm()
{
}

void wxWebViewWasm::SendWebViewEvent(wxEventType type)
{
    wxWebViewEvent event(type, GetId(), m_currentUrl, wxEmptyString,
                         wxWEBVIEW_NAV_ACTION_OTHER);
    event.SetEventObject(this);
    HandleWindowEvent(event);
}

// Reads a string property of the iframe document (same-origin only).
wxString wxWebViewWasm::GetIframeDocumentProp(const char* prop) const
{
    wxCharBuffer propBuf = wxString::FromUTF8(prop).ToUTF8();

    char *buf = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var iframe = container.querySelector('.wxWebView-iframe');
        if (!iframe) return 0;
        try {
            var doc = iframe.contentDocument;
            if (!doc) return 0;
            var value = doc[UTF8ToString($1)];
            if (value === undefined || value === null) return 0;
            var str = String(value);
            var len = lengthBytesUTF8(str) + 1;
            var buffer = _malloc(len);
            stringToUTF8(str, buffer, len);
            return buffer;
        } catch (e) {
            // Cross-origin document: not readable from the outer page.
            return 0;
        }
    }, GetId(), propBuf.data());

    wxString result;
    if ( buf )
    {
        result = wxString::FromUTF8(buf);
        free(buf);
    }
    return result;
}

wxString wxWebViewWasm::GetCurrentTitle() const
{
    return GetIframeDocumentProp("title");
}

wxString wxWebViewWasm::GetCurrentURL() const
{
    return m_currentUrl;
}

bool wxWebViewWasm::IsBusy() const
{
    return m_loading;
}

void wxWebViewWasm::LoadURL(const wxString& url)
{
    // Allow the application to veto the navigation.
    wxWebViewEvent eventNavigating(wxEVT_WEBVIEW_NAVIGATING, GetId(), url,
                                   wxEmptyString, wxWEBVIEW_NAV_ACTION_USER);
    eventNavigating.SetEventObject(this);
    HandleWindowEvent(eventNavigating);
    if ( !eventNavigating.IsAllowed() )
        return;

    m_currentUrl = url;
    m_loading = true;
    wxCharBuffer urlBuf = url.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        if (iframe) iframe.src = UTF8ToString($1);
    }, GetId(), urlBuf.data());
}

void wxWebViewWasm::Print()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentWindow) iframe.contentWindow.print();
        } catch (e) { /* cross-origin */ }
    }, GetId());
}

void wxWebViewWasm::RegisterHandler(wxSharedPtr<wxWebViewHandler> WXUNUSED(handler))
{
    // Custom scheme handlers are not supported by the iframe backend.
}

void wxWebViewWasm::Reload(wxWebViewReloadFlags WXUNUSED(flags))
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        if (!iframe) return;
        try {
            iframe.contentWindow.location.reload();
        } catch (e) {
            // Cross-origin: force a reload by reassigning the source.
            var src = iframe.src;
            iframe.src = src;
        }
    }, GetId());
}

bool wxWebViewWasm::RunScript(const wxString& javascript, wxString* output) const
{
    wxCharBuffer scriptBuf = javascript.ToUTF8();

    char *resultBuf = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        var okName = '_wxWebViewScriptOk_' + $0;
        window[okName] = 0;
        if (!container) return 0;
        var iframe = container.querySelector('.wxWebView-iframe');
        if (!iframe) return 0;
        try {
            var result = iframe.contentWindow.eval(UTF8ToString($1));
            window[okName] = 1;
            if (result === undefined || result === null) return 0;
            var str = String(result);
            var len = lengthBytesUTF8(str) + 1;
            var buffer = _malloc(len);
            stringToUTF8(str, buffer, len);
            return buffer;
        } catch (e) {
            // Cross-origin document or script error.
            return 0;
        }
    }, GetId(), scriptBuf.data());

    const int ok = EM_ASM_INT({
        return window['_wxWebViewScriptOk_' + $0] || 0;
    }, GetId());

    if ( ok && output && resultBuf )
        *output = wxString::FromUTF8(resultBuf);
    if ( resultBuf )
        free(resultBuf);
    return ok != 0;
}

void wxWebViewWasm::SetEditable(bool enable)
{
    m_editable = enable;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentDocument)
                iframe.contentDocument.designMode = $1 ? 'on' : 'off';
        } catch (e) { /* cross-origin */ }
    }, GetId(), enable ? 1 : 0);
}

void wxWebViewWasm::Stop()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentWindow) iframe.contentWindow.stop();
        } catch (e) { /* cross-origin */ }
    }, GetId());
    m_loading = false;
}

bool wxWebViewWasm::CanGoBack() const
{
    // The History API does not expose the position within the history;
    // report the optimistic default (the iframe history ignores a
    // back/forward with nothing to go to).
    return true;
}

bool wxWebViewWasm::CanGoForward() const
{
    return true;
}

void wxWebViewWasm::GoBack()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentWindow) iframe.contentWindow.history.back();
        } catch (e) { /* cross-origin */ }
    }, GetId());
}

void wxWebViewWasm::GoForward()
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentWindow) iframe.contentWindow.history.forward();
        } catch (e) { /* cross-origin */ }
    }, GetId());
}

void wxWebViewWasm::ClearHistory()
{
    // The iframe session history cannot be cleared from the outer page.
}

void wxWebViewWasm::EnableHistory(bool WXUNUSED(enable))
{
    // History is always managed by the iframe itself.
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWasm::GetBackwardHistory()
{
    // The History API does not expose the entries; report an empty list.
    return wxVector<wxSharedPtr<wxWebViewHistoryItem> >();
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewWasm::GetForwardHistory()
{
    return wxVector<wxSharedPtr<wxWebViewHistoryItem> >();
}

void wxWebViewWasm::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    if ( item )
        LoadURL(item->GetUrl());
}

bool wxWebViewWasm::CanSetZoomType(wxWebViewZoomType type) const
{
    return type == wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

void wxWebViewWasm::SetZoomFactor(float zoom)
{
    m_zoomFactor = zoom;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        try {
            if (iframe && iframe.contentDocument && iframe.contentDocument.body)
                iframe.contentDocument.body.style.zoom = $1;
        } catch (e) { /* cross-origin */ }
    }, GetId(), zoom);
}

void wxWebViewWasm::SetZoomType(wxWebViewZoomType zoomType)
{
    m_zoomType = zoomType;
}

void wxWebViewWasm::DoSetPage(const wxString& html, const wxString& WXUNUSED(baseUrl))
{
    m_currentUrl = "about:srcdoc";
    m_loading = true;
    wxCharBuffer htmlBuf = html.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var iframe = container.querySelector('.wxWebView-iframe');
        if (iframe) iframe.srcdoc = UTF8ToString($1);
    }, GetId(), htmlBuf.data());
}

void wxWebViewWasm::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "webview_loaded" )
    {
        // The iframe finished loading a document.
        m_loading = false;

        // The URL might have changed through an in-page navigation; keep
        // the tracked URL if the document is cross-origin (unreadable).
        wxString location = GetIframeDocumentProp("URL");
        if ( !location.empty() && location != "about:blank" &&
             location != "about:srcdoc" )
            m_currentUrl = location;

        SendWebViewEvent(wxEVT_WEBVIEW_NAVIGATED);

        // Title change, if any (empty for cross-origin documents).
        wxString title = GetCurrentTitle();
        if ( !title.empty() )
        {
            wxWebViewEvent titleEvent(wxEVT_WEBVIEW_TITLE_CHANGED, GetId(),
                                      m_currentUrl, wxEmptyString);
            titleEvent.SetString(title);
            titleEvent.SetEventObject(this);
            HandleWindowEvent(titleEvent);
        }

        SendWebViewEvent(wxEVT_WEBVIEW_LOADED);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

// ============================================================================
// wxWebViewFactoryWasm
// ============================================================================

wxWebViewConfiguration wxWebViewFactoryWasm::CreateConfiguration()
{
    return wxWebViewConfiguration(wxWebViewBackendWasm,
                                  new wxWebViewConfigurationImpl);
}

#endif // wxUSE_WEBVIEW
