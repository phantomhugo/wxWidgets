/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/toolbar.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TOOLBAR

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/toolbar.h"

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar, int id, const wxString& label, const wxBitmapBundle& bitmap1,
                  const wxBitmapBundle& bitmap2, wxItemKind kind, wxObject *clientData,
                  const wxString& shortHelpString, const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString)
    {
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label)
    {
    }

    virtual void SetLabel( const wxString &label ) override;
    virtual void SetDropdownMenu(wxMenu* menu) override;

    void SetIcon();
    void ClearToolTip();
    void SetToolTip();


};

wxIMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

void wxToolBarTool::SetLabel( const wxString &label )
{
    wxToolBarToolBase::SetLabel( label );
}


void wxToolBarTool::SetDropdownMenu(wxMenu* menu)
{
    wxToolBarToolBase::SetDropdownMenu(menu);
    menu->SetInvokingWindow(GetToolBar());
}

void wxToolBarTool::SetIcon()
{

}

void wxToolBarTool::ClearToolTip()
{

}

void wxToolBarTool::SetToolTip()
{

}


void wxToolBar::Init()
{

}

wxToolBar::~wxToolBar()
{
}

bool wxToolBar::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos,
                       const wxSize& size, long style, const wxString& name)
{
    SetWindowStyleFlag(style);

    // not calling to wxWindow::Create, so do the rest of initialization:
    if (parent)
        parent->AddChild( this );

    PostCreation();

    return wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name );
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
//    actionAt(x, y);
    wxFAIL_MSG( wxT("wxToolBar::FindToolForPosition() not implemented") );
    return nullptr;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
        //TODO - other qt actions for tool tip string
//        if (tool->m_item)
//        {}
    }
}

void wxToolBar::SetToolNormalBitmap( int id, const wxBitmapBundle& bitmap )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetNormalBitmap(bitmap);
        tool->SetIcon();
    }
}

void wxToolBar::SetToolDisabledBitmap( int id, const wxBitmapBundle& bitmap )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        wxCHECK_RET( tool->IsButton(), wxT("Can only set bitmap on button tools."));

        tool->SetDisabledBitmap(bitmap);
    }
}

void wxToolBar::SetWindowStyleFlag( long style )
{
    wxToolBarBase::SetWindowStyleFlag(style);

}

bool wxToolBar::Realize()
{
    if ( !wxToolBarBase::Realize() )
        return false;

    return true;
}

bool wxToolBar::DoInsertTool(size_t pos, wxToolBarToolBase *toolBase)
{

    return true;
}

bool wxToolBar::DoDeleteTool(size_t /* pos */, wxToolBarToolBase *toolBase)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);

    InvalidateBestSize();
    return true;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *toolBase, bool toggle)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
}

void wxToolBar::DoSetToggle(wxToolBarToolBase * WXUNUSED(tool),
                            bool WXUNUSED(toggle))
{
    // VZ: absolutely no idea about how to do it
    wxFAIL_MSG( wxT("not implemented") );
}

wxToolBarToolBase *wxToolBar::CreateTool(int id, const wxString& label, const wxBitmapBundle& bmpNormal,
                                      const wxBitmapBundle& bmpDisabled, wxItemKind kind, wxObject *clientData,
                                      const wxString& shortHelp, const wxString& longHelp)
{
    return new wxToolBarTool(this, id, label, bmpNormal, bmpDisabled, kind,
                             clientData, shortHelp, longHelp);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control,
                                          const wxString& label)
{
    return new wxToolBarTool(this, control, label);
}

long wxToolBar::GetButtonStyle()
{

}

#endif // wxUSE_TOOLBAR

