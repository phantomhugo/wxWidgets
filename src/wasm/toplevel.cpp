/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/toplevel.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/toplevel.h"

#include <emscripten.h>

wxTopLevelWindowWasm::wxTopLevelWindowWasm()
{
}

wxTopLevelWindowWasm::wxTopLevelWindowWasm(wxWindow *parent,
            wxWindowID winid,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    Create(parent,winid,title,pos,size,style,name);
}

bool wxTopLevelWindowWasm::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    // wxWindow::Create already creates a <div> with id = GetId().
    // We must NOT create a second div; instead we style the existing one.
    bool result = wxWindow::Create(parent,id,pos,size,style,name);

    SetTitle(title);

    EM_ASM_INT(
        {
            const tlw = document.getElementById($0);
            if (tlw) {
                tlw.className = "wxTopLevelWindow";
                tlw.style.display = "none";
                tlw.style.position = "absolute";
            }
            return 1;
        },
        GetId()
    );

    if(parent == nullptr)
    {
        EM_ASM_INT(
        {
            document.body.height="100%";
            document.body.width="100%";
            const currentWindow=document.getElementById($0);
            if (currentWindow) {
                currentWindow.height="100%";
                currentWindow.width="100%";
            }
            return 1;
        },
        GetId()
    );
    }

    SetSize(0,0,800,600);
    return result;
}

void wxTopLevelWindowWasm::Maximize(bool maximize)
{

}

void wxTopLevelWindowWasm::Restore()
{

}

void wxTopLevelWindowWasm::Iconize(bool iconize)
{

}

bool wxTopLevelWindowWasm::IsMaximized() const
{
    return false;
}

bool wxTopLevelWindowWasm::IsIconized() const
{
    return false;
}

bool wxTopLevelWindowWasm::ShowFullScreen(bool show, long style)
{
    return false;
}

bool wxTopLevelWindowWasm::IsFullScreen() const
{
    return false;
}

void wxTopLevelWindowWasm::SetTitle(const wxString& title)
{
    m_title = title;
    wxCharBuffer buffer = title.ToUTF8();

    // Update browser tab title
    EM_ASM_({
        document.title = UTF8ToString($0);
    }, buffer.data());

    // Update visual title inside the frame if the element exists
    EM_ASM_({
        var tlw = document.getElementById($0);
        if (!tlw) return;

        var titleBar = tlw.querySelector('.wxTopLevelWindow-title');
        if (titleBar) {
            titleBar.textContent = UTF8ToString($1);
        }
    }, GetId(), buffer.data());
}

wxString wxTopLevelWindowWasm::GetTitle() const
{
    return m_title;
}

void wxTopLevelWindowWasm::SetWindowStyleFlag( long style )
{

}

long wxTopLevelWindowWasm::GetWindowStyleFlag() const
{
    return m_windowStyle;
}
