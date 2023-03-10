/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/frame.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/menu.h"
    #include "wx/toolbar.h"
#endif // WX_PRECOMP

#include "wx/frame.h"

wxFrame::~wxFrame()
{
}

bool wxFrame::Create( wxWindow *parent, wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name )
{
    return true;
}

void wxFrame::SetMenuBar( wxMenuBar *menuBar )
{
    if ( menuBar )
    {

    }
    else
    {

    }
    wxFrameBase::SetMenuBar( menuBar );
}

void wxFrame::SetStatusBar( wxStatusBar *statusBar )
{
    // The current status bar could be deleted by Qt when dereferencing it
    // TODO: add a mechanism like Detach in menus to avoid issues
    if ( statusBar != nullptr )
    {
    }
    else
    {
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
