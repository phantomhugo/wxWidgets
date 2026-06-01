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

    int domId = GetId();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = document.createElement('div');
        tabs.className = 'wxNotebook-tabs';

        var pages = document.createElement('div');
        pages.className = 'wxNotebook-pages';
        pages.style.width = '100%';
        pages.style.height = '100%';

        container.appendChild(tabs);
        container.appendChild(pages);
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

    int domId = GetId();
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

    int domId = GetId();

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

    int domId = GetId();
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
    int domId = GetId();
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var tabs = container.querySelector('.wxNotebook-tabs');
        var pages = container.querySelector('.wxNotebook-pages');
        if (tabs) tabs.innerHTML = '';
        if (pages) pages.innerHTML = '';
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

    m_selection = page;

    int domId = GetId();
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

    int domId = GetId();
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
    return wxSize(200, 150);
}

void wxNotebook::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "tab_click")
    {
        int newSel = event.x;
        int oldSel = GetSelection();

        if (newSel >= 0 && newSel < (int)GetPageCount() && newSel != oldSel)
        {
            SetSelection(newSel);

            wxBookCtrlEvent notebookEvent(wxEVT_NOTEBOOK_PAGE_CHANGED, m_windowId, newSel, oldSel);
            HandleWindowEvent(notebookEvent);
        }
    }
}

#endif // wxUSE_NOTEBOOK
