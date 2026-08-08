/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/frame.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
    #include "wx/statusbr.h"
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
                
                // Create container for the frame content
                const frameContent=document.createElement("div");
                frameContent.id= "wxFrame_content_" + $0;
                frameContent.className="wxFrame_content";
                frameContent.style.flex="1";
                frameContent.style.position="relative";
                // Children are clipped by the client area, as in the native
                // ports: there are no frame-level scrollbars in wxWidgets.
                frameContent.style.overflow="hidden";
                frameContent.style.minHeight="0";
                
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
    wxFrameBase::SetMenuBar( menuBar );
    
    if ( menuBar )
    {
        EM_ASM_INT(
            {
                const currentFrame=document.getElementById($0);
                const menuBar=document.getElementById($1);
                if (currentFrame && menuBar) {
                    // Insert at the beginning
                    currentFrame.insertBefore(menuBar, currentFrame.firstChild);
                    menuBar.style.display = 'flex';
                }
                return 1;
            },
            GetId(),
            menuBar->GetId()
        );
    }
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    wxStatusBar* oldStatusBar = GetStatusBar();
    
    // If there is a previous status bar, move it to parentless
    if (oldStatusBar && oldStatusBar != statusBar)
    {
        EM_ASM_INT(
            {
                const currentParentless=document.getElementById("wxParentlessTags");
                const oldStatusBar=document.getElementById($0);
                if (currentParentless && oldStatusBar) {
                    currentParentless.appendChild(oldStatusBar);
                    oldStatusBar.style.display = 'none';
                }
                return 1;
            },
            oldStatusBar->GetId()
        );
    }
    
    wxFrameBase::SetStatusBar( statusBar );
    
    if ( statusBar )
    {
        EM_ASM_INT(
            {
                const currentFrame=document.getElementById($0);
                const statusBar=document.getElementById($1);
                if (currentFrame && statusBar) {
                    currentFrame.appendChild(statusBar);
                    statusBar.style.display = 'flex';
                }
                return 1;
            },
            GetId(),
            statusBar->GetId()
        );
    }
}

void wxFrame::SetToolBar(wxToolBar *toolbar)
{
    wxToolBar* oldToolBar = GetToolBar();

    // If there is a previous toolbar, move it to the parentless container
    if (oldToolBar && oldToolBar != toolbar)
    {
        EM_ASM_INT(
            {
                const currentParentless=document.getElementById("wxParentlessTags");
                const oldToolBar=document.getElementById($0);
                if (currentParentless && oldToolBar) {
                    currentParentless.appendChild(oldToolBar);
                    oldToolBar.style.display = 'none';
                }
                return 1;
            },
            oldToolBar->GetId()
        );
    }

    wxFrameBase::SetToolBar( toolbar );

    if ( toolbar != nullptr )
    {
        EM_ASM_INT(
            {
                const currentFrame=document.getElementById($0);
                const toolBar=document.getElementById($1);
                if (currentFrame && toolBar) {
                    // Keep the order: menubar > toolbar > content > statusbar.
                    // The menubar is always inserted as firstChild by
                    // SetMenuBar(), so inserting the toolbar right before the
                    // content container keeps it below the menubar.
                    const content=document.getElementById("wxFrame_content_" + $0);
                    if (content && content.parentNode === currentFrame) {
                        currentFrame.insertBefore(toolBar, content);
                    } else {
                        currentFrame.appendChild(toolBar);
                    }
                    // PostCreation() gave the control div position:absolute;
                    // reset it so it takes part in the frame flex column.
                    toolBar.style.position = 'relative';
                    toolBar.style.left = "";
                    toolBar.style.top = "";
                    toolBar.style.width = '100%';
                    toolBar.style.height = "";
                    toolBar.style.display = 'flex';
                }
                return 1;
            },
            GetId(),
            toolbar->GetId()
        );
    }
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

// Total height in pixels of the frame decorations (menubar, toolbar and
// statusbar) as laid out in the DOM. While the frame is hidden the DOM
// reports 0, so fall back to the theme metrics.
static int wxWasmFrameDecorHeight(int domId)
{
    return EM_ASM_INT({
        var f = document.getElementById($0);
        if (!f) return 0;
        var h = 0;
        var mb = f.querySelector(':scope > .wxMenuBar');
        if (mb) h += mb.offsetHeight > 0 ? mb.offsetHeight : 36;
        var tb = f.querySelector(':scope > .wxToolBar');
        if (tb) h += tb.offsetHeight;
        var sb = f.querySelector(':scope > .wxStatusBar');
        if (sb) h += sb.offsetHeight > 0 ? sb.offsetHeight : 30;
        return h;
    }, domId);
}

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    wxWindow::DoGetClientSize(width, height);

    // The client area excludes the menubar, the toolbar and the statusbar.
    if (height)
    {
        *height -= wxWasmFrameDecorHeight(GetId());
        if (*height < 0)
            *height = 0;
    }
}

void wxFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height + wxWasmFrameDecorHeight(GetId()));
}

// Helper method to get the frame content container
wxString wxFrame::GetContentContainerId() const
{
    return wxString::Format(wxT("wxFrame_content_%d"), GetId());
}
