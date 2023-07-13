/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/mdi.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MDI

#include "wx/mdi.h"
wxIMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame);

wxMDIParentFrame::wxMDIParentFrame()
{
}

wxMDIParentFrame::wxMDIParentFrame(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
{
    (void)Create(parent, id, title, pos, size, style, name);
}

bool wxMDIParentFrame::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{


    return true;
}

void wxMDIParentFrame::ActivateNext()
{
}

void wxMDIParentFrame::ActivatePrevious()
{
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIChildFrame,wxMDIChildFrameBase)

wxMDIChildFrame::wxMDIChildFrame()
{
}

wxMDIChildFrame::wxMDIChildFrame(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create(parent, id, title, pos, size, style, name);
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    m_mdiParent = parent;
    bool ok = wxFrame::Create(parent->GetClientWindow(), id,
                              title,
                               pos, size, style, name);
    return ok;
}

void wxMDIChildFrame::Activate()
{
}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS(wxMDIClientWindow,wxMDIClientWindowBase)

wxMDIClientWindow::wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long WXUNUSED(style))
{
    // create the MDI client area where the children window are displayed:
    return true;
}

#endif // wxUSE_MDI
