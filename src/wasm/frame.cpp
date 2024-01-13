/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/frame.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
#endif // WX_PRECOMP

#include "wx/frame.h"
#include <emscripten.h>

wxFrame::~wxFrame()
{
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    if(wxFrameBase::Create(parent, id, title, pos, size, style, name))
    {
        EM_ASM_INT(
            {
                //We change the className because this is an specialized class
                const currentFrame=document.getElementById($0);
                currentFrame.className="wxFrame";
                const frameContent=document.createElement("div");
                frameContent.id= "wxFrame_content";
                frameContent.className="wxFrame_content";
                frameContent.style.height="100%";
                frameContent.style.width="100%";
                frameContent.style.clear="both";
                frameContent.style.position="absolute";
                currentFrame.append(frameContent);
                return 1;
            },
            GetId()
        );
    }
    return true;
}

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    if ( menuBar )
    {
        EM_ASM_INT(
            {
                const currentFrame=document.getElementById($0);
                const menuBar=document.getElementById($1);
                currentFrame.prepend(menuBar);
                return 1;
            },
            GetId(),
            menuBar->GetId()
        );
    }
    else
    {
        EM_ASM_INT(
            {
                const currentParentless=document.getElementById("wxParentlessTags");
                const menuBar=document.getElementById($1);
                currentParentless.append(menuBar);
                return 1;
            },
            GetId(),
            menuBar->GetId()
        );
    }
    wxFrameBase::SetMenuBar( menuBar );
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    if ( statusBar != nullptr )
    {
        EM_ASM_INT(
            {
                const currentFrame=document.getElementById($0);
                const statusBar=document.getElementById($1);
                currentFrame.append(statusBar);
                return 1;
            },
            GetId(),
            statusBar->GetId()
        );
    }
    else
    {
        EM_ASM_INT(
            {
                const currentParentless=document.getElementById("wxParentlessTags");
                const statusBar=document.getElementById($1);
                currentParentless.append(statusBar);
                return 1;
            },
            GetId(),
            statusBar->GetId()
        );
    }
    wxFrameBase::SetStatusBar( statusBar );
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    if ( toolbar != nullptr )
    {

    }
    else if ( m_frameToolBar != nullptr )
    {

    }
    wxFrameBase::SetToolBar( toolbar );
}

void wxFrame::SetWindowStyleFlag( long style )
{
    wxWindow::SetWindowStyleFlag( style );
}

void wxFrame::AddChild( wxWindowBase *child )
{
    wxFrameBase::AddChild( child );
}

void wxFrame::RemoveChild( wxWindowBase *child )
{
    wxFrameBase::RemoveChild( child );
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxWindow::DoGetClientSize(width, height);
}

void wxFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);

    int adjustedWidth, adjustedHeight;
    DoGetClientSize(&adjustedWidth, &adjustedHeight);

}
