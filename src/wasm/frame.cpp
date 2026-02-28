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
                
                // Crear contenedor para el contenido del frame
                const frameContent=document.createElement("div");
                frameContent.id= "wxFrame_content_" + $0;
                frameContent.className="wxFrame_content";
                frameContent.style.flex="1";
                frameContent.style.position="relative";
                frameContent.style.overflow="auto";
                
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
                    // Insertar al principio
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
    
    // Si hay un status bar anterior, moverlo a parentless
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
    wxFrameBase::SetToolBar( toolbar );
    
    if ( toolbar != nullptr )
    {
        // TODO: Implementar toolbar en el DOM
        // Debería ir entre el menú y el contenido
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

void wxFrame::DoGetClientSize(int *width, int *height) const
{
    // El tamaño del cliente debe excluir menú y status bar
    wxWindow::DoGetClientSize(width, height);
}

void wxFrame::DoSetClientSize(int width, int height)
{
    wxWindow::DoSetClientSize(width, height);
}

// Método auxiliar para obtener el contenedor de contenido del frame
wxString wxFrame::GetContentContainerId() const
{
    return wxString::Format(wxT("wxFrame_content_%d"), GetId());
}
