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
    bool result = wxWindow::Create(parent,id,pos,size,style);

    SetTitle(title);

    EM_ASM_INT(
        {
            const newTopLevelWindow=document.createElement("div");
            newTopLevelWindow.id= $0;
            newTopLevelWindow.className="wxTopLevelWindow";
            newTopLevelWindow.style.display="none";
            newTopLevelWindow.style.position="absolute";
            document.body.append(newTopLevelWindow);
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
            currentWindow.height="100%";
            currentWindow.width="100%";
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

}

bool wxTopLevelWindowWasm::IsIconized() const
{

}

bool wxTopLevelWindowWasm::ShowFullScreen(bool show, long style)
{

}

bool wxTopLevelWindowWasm::IsFullScreen() const
{

}

void wxTopLevelWindowWasm::SetTitle(const wxString& title)
{

}

wxString wxTopLevelWindowWasm::GetTitle() const
{

}

void wxTopLevelWindowWasm::SetWindowStyleFlag( long style )
{

}

long wxTopLevelWindowWasm::GetWindowStyleFlag() const
{

}
