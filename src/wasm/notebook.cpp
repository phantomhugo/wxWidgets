/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/notebook.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_NOTEBOOK

#include "wx/notebook.h"
#include <emscripten.h>

wxBEGIN_EVENT_TABLE(wxNotebook, wxBookCtrlBase)
    EVT_SIZE(wxNotebook::OnSize)
wxEND_EVENT_TABLE()

// Returns the pixel size of the pages area (the container below the tabs),
// first fixing its height to the space left by the tabs bar.
static wxSize wxWasmNotebookGetPagesSize(int domId)
{
    int w = EM_ASM_INT({
        var c = document.getElementById($0);
        if (!c) return 0;
        var tabs = c.querySelector('.wxNotebook-tabs');
        var p = c.querySelector('.wxNotebook-pages');
        if (!p) return 0;
        if (tabs && tabs.offsetHeight > 0)
            p.style.height = 'calc(100% - ' + tabs.offsetHeight + 'px)';
        return p.clientWidth;
    }, domId);
    int h = EM_ASM_INT({
        var c = document.getElementById($0);
        var p = c && c.querySelector('.wxNotebook-pages');
        return p ? p.clientHeight : 0;
    }, domId);
    return wxSize(w, h);
}

// Gives every page the pixel geometry of the pages area so that the page
// sizers, if any, lay out their children. The DOM-only display:none/block
// page switching does not generate any wx sizing/layout by itself, and the
// base class wxBookCtrlBase::DoSize() can't do it either because this port
// has no controller window (m_bookctrl is null).
void wxNotebook::WasmLayoutPages()
{
    const wxSize size = wxWasmNotebookGetPagesSize(GetId());
    if ( size.x <= 0 || size.y <= 0 )
        return;

    for ( size_t i = 0; i < m_pages.size(); ++i )
    {
        if ( m_pages[i] )
        {
            m_pages[i]->SetSize(0, 0, size.x, size.y);
            // SetSize() may not generate any size event (the page already
            // had the same effective size through its CSS 100% sizing), so
            // run the page sizer, if any, explicitly.
            m_pages[i]->Layout();
        }
    }
}

void wxNotebook::OnSize(wxSizeEvent& event)
{
    event.Skip();
    WasmLayoutPages();
}

wxNotebook::wxNotebook()
{
}

wxNotebook::wxNotebook(wxWindow *parent,
         wxWindowID id,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxNotebook::Create(wxWindow *parent,
          wxWindowID id,
          const wxPoint& pos,
          const wxSize& size,
          long style,
          const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    int domId = GetDomWindowId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = document.createElement('div');
        tabs.className = 'wxNotebook-tabs';

        // Pages are positioned absolutely inside this area. Its height is
        // fixed to the space left by the tabs when the first tab is added
        // (Show() resets the container display, so flexbox cannot be used).
        var pages = document.createElement('div');
        pages.className = 'wxNotebook-pages';
        pages.style.width = '100%';
        pages.style.position = 'relative';
        pages.style.overflow = 'hidden';

        container.appendChild(tabs);
        container.appendChild(pages);

        // The pages area only gets its real size once the control and its
        // ancestors become visible (offset sizes are 0 while hidden), which
        // does not necessarily produce any wx size event. Watch it and let
        // the control lay out its pages whenever the size changes; the
        // observer also fires once when registered.
        if (typeof ResizeObserver !== 'undefined') {
            var ro = new ResizeObserver(function() {
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'pages_resize', 0, 0]);
                }
            });
            ro.observe(pages);
        }
    }, domId);

    return true;
}

void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
}


bool wxNotebook::SetPageText(size_t n, const wxString &text)
{
    wxCHECK_MSG(n < GetPageCount(), false, "invalid notebook index");

    int domId = GetDomWindowId();
    wxCharBuffer textBuffer = text.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelectorAll('.wxNotebook-tab');
        if (tabs[$1]) {
            tabs[$1].textContent = UTF8ToString($2);
        }
    }, domId, (int)n, textBuffer.data());

    return true;
}

wxString wxNotebook::GetPageText(size_t n) const
{
    wxCHECK_MSG(n < GetPageCount(), wxString(), "invalid notebook index");

    int domId = GetDomWindowId();

    char *buf = (char*)EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;

        var tabs = container.querySelectorAll('.wxNotebook-tab');
        if (!tabs[$1]) return 0;

        var text = tabs[$1].textContent;
        var length = lengthBytesUTF8(text) + 1;
        var buffer = _malloc(length);
        stringToUTF8(text, buffer, length);
        return buffer;
    }, domId, (int)n);

    wxString result;
    if (buf) {
        result = wxString::FromUTF8(buf);
        free(buf);
    }
    return result;
}

int wxNotebook::GetPageImage(size_t n) const
{
    wxCHECK_MSG(n < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    return m_images[n];
}

bool wxNotebook::SetPageImage(size_t n, int imageId)
{
    wxCHECK_MSG(n < GetPageCount(), false, "invalid notebook index");

    m_images[n] = imageId;
    return true;
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    wxCHECK_MSG(page != nullptr, false, "null page in wxNotebook::InsertPage");
    wxCHECK_MSG(n <= GetPageCount(), false, "invalid index in wxNotebook::InsertPage");

    int domId = GetDomWindowId();
    int pageId = page->GetId();
    wxCharBuffer textBuffer = text.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelector('.wxNotebook-tabs');
        var pages = container.querySelector('.wxNotebook-pages');
        if (!tabs || !pages) return;

        var btn = document.createElement('button');
        btn.className = 'wxNotebook-tab';
        btn.textContent = UTF8ToString($2);

        btn.onclick = function(e) {
            e.stopPropagation();
            var allTabs = container.querySelectorAll('.wxNotebook-tab');
            var idx = Array.prototype.indexOf.call(allTabs, btn);
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'tab_click', idx, 0]);
            }
        };

        var existingTabs = tabs.querySelectorAll('.wxNotebook-tab');
        if ($3 >= 0 && $3 < existingTabs.length) {
            tabs.insertBefore(btn, existingTabs[$3]);
        } else {
            tabs.appendChild(btn);
        }

        // The pages area fills the control below the tabs bar.
        pages.style.height = 'calc(100% - ' + tabs.offsetHeight + 'px)';

        var pageElem = document.getElementById($1);
        if (pageElem) {
            pageElem.style.display = 'none';
            pageElem.style.width = '100%';
            pageElem.style.height = '100%';
            var existingPages = pages.children;
            if ($3 >= 0 && $3 < existingPages.length) {
                pages.insertBefore(pageElem, existingPages[$3]);
            } else {
                pages.appendChild(pageElem);
            }
        }
    }, domId, pageId, textBuffer.data(), (int)n);

    m_pages.insert(m_pages.begin() + n, page);
    m_images.insert(m_images.begin() + n, imageId);

    DoSetSelectionAfterInsertion(n, bSelect);

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    return sizePage;
}

bool wxNotebook::DeleteAllPages()
{
    int domId = GetDomWindowId();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelector('.wxNotebook-tabs');
        var pages = container.querySelector('.wxNotebook-pages');
        if (tabs) tabs.innerHTML = "";
        if (pages) pages.innerHTML = "";
    }, domId);

    m_images.clear();

    return wxNotebookBase::DeleteAllPages();
}

int wxNotebook::SetSelection(size_t page)
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    int selOld = GetSelection();
    if ((int)page == selOld)
        return selOld;

    if ( selOld != wxNOT_FOUND && (size_t)selOld < GetPageCount() && m_pages[selOld] )
        DoShowPage(m_pages[selOld], false);

    m_selection = page;

    // Size the pages before showing the new one so that its sizer, if any,
    // lays out its children.
    WasmLayoutPages();

    if ( m_pages[page] )
        DoShowPage(m_pages[page], true);

    int domId = GetDomWindowId();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelectorAll('.wxNotebook-tab');
        var pages = container.querySelector('.wxNotebook-pages');
        if (!pages) return;

        for (var i = 0; i < tabs.length; i++) {
            if (i === $1) {
                tabs[i].classList.add('active');
            } else {
                tabs[i].classList.remove('active');
            }
        }

        var pageElems = pages.children;
        for (var i = 0; i < pageElems.length; i++) {
            pageElems[i].style.display = (i === $1) ? 'block' : 'none';
        }
    }, domId, (int)page);

    return selOld;
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    return SetSelection(nPage);
}

wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    wxCHECK_MSG(page < GetPageCount(), nullptr, "invalid page index in wxNotebook::DoRemovePage");

    int domId = GetDomWindowId();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelector('.wxNotebook-tabs');
        var pages = container.querySelector('.wxNotebook-pages');
        if (!tabs || !pages) return;

        var tab = tabs.querySelectorAll('.wxNotebook-tab')[$1];
        if (tab) tab.remove();

        var pageElem = pages.children[$1];
        if (pageElem) pageElem.remove();
    }, domId, (int)page);

    wxWindow *pageRemoved = wxNotebookBase::DoRemovePage(page);
    if (pageRemoved && page < m_images.size())
        m_images.erase(m_images.begin() + page);

    return pageRemoved;
}

wxSize wxNotebook::DoGetBestSize() const
{
    // Defer to the base class, which computes the size from the best sizes
    // of the pages; a fixed size here would make SetSizerAndFit() shrink
    // the top level window far below what the pages need.
    return wxNotebookBase::DoGetBestSize();
}

void wxNotebook::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "tab_click")    {
        int newSel = event.x;
        int oldSel = GetSelection();

        if (newSel >= 0 && newSel < (int)GetPageCount() && newSel != oldSel)
        {
            SetSelection(newSel);

            wxBookCtrlEvent notebookEvent(wxEVT_NOTEBOOK_PAGE_CHANGED, m_windowId, newSel, oldSel);
            notebookEvent.SetEventObject(this);
            HandleWindowEvent(notebookEvent);
        }
    }
    else if (event.id == m_windowId && event.eventType == "pages_resize")
    {
        // Sent by the ResizeObserver of the pages area.
        WasmLayoutPages();
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

#endif // wxUSE_NOTEBOOK
