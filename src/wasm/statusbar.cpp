/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statusbar.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statusbr.h"
#include <emscripten.h>

wxStatusBar::wxStatusBar()
{
    Init();
}

wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    Init();

    if ( winid == wxID_ANY )
    {
        SetId(NewControlId());
    }
    else // valid id specified
    {
        SetId(winid);
    }

    Create( parent, GetId(), style, name );
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID WXUNUSED(winid),
                         long style, const wxString& WXUNUSED(name))
{
    wxWasmCreateMenuBar();

    EM_ASM_INT(
        {
            const parent=document.getElementById($0);
            const statusBar=document.getElementById($1);
            parent.append(statusBar);
            return 1;
        },
        parent->GetId(),
        GetId()
    );

    PostCreation();

    SetFieldsCount(1);

    return true;
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    return true;
}

void wxStatusBar::SetMinHeight(int height)
{

}

int wxStatusBar::GetBorderX() const
{
    return 0;
}

int wxStatusBar::GetBorderY() const
{
    return 0;
}

void wxStatusBar::DoUpdateStatusText(int number)
{

}

// Called each time number/size of panes changes
void wxStatusBar::Refresh( bool eraseBackground, const wxRect *rect )
{
    UpdateFields();

    wxWindow::Refresh( eraseBackground, rect );
}

void wxStatusBar::Init()
{

}

void wxStatusBar::wxWasmCreateMenuBar()
{
    EM_ASM_INT(
        {
            const statusBar=document.createElement("div");
            statusBar.id= $0;
            statusBar.className="wxStatusBar";
            statusBar.style.clear="both";
            const parentlessDiv=document.getElementById("wxParentlessTags");
            parentlessDiv.append(statusBar);
            return 1;
        },
        GetId()
    );
}

void wxStatusBar::UpdateFields()
{
    // is it a good idea to recreate all the panes every update?


}
