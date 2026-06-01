/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/toolbar.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_TOOLBAR

#ifndef WX_PRECOMP
    #include "wx/menu.h"
#endif // WX_PRECOMP

#include "wx/toolbar.h"
#include <emscripten.h>

class wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool(wxToolBar *tbar, int id, const wxString& label, const wxBitmapBundle& bitmap1,
                  const wxBitmapBundle& bitmap2, wxItemKind kind, wxObject *clientData,
                  const wxString& shortHelpString, const wxString& longHelpString)
        : wxToolBarToolBase(tbar, id, label, bitmap1, bitmap2, kind,
                            clientData, shortHelpString, longHelpString),
          m_domElementId(0)
    {
    }

    wxToolBarTool(wxToolBar *tbar, wxControl *control, const wxString& label)
        : wxToolBarToolBase(tbar, control, label),
          m_domElementId(0)
    {
    }

    virtual void SetLabel( const wxString &label ) override;
    virtual void SetDropdownMenu(wxMenu* menu) override;

    void SetIcon();
    void ClearToolTip();
    void SetToolTip();

    int m_domElementId;
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

    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return false;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var toolbar = document.createElement('div');
        toolbar.className = 'wxToolBar';
        toolbar.style.display = 'flex';
        toolbar.style.flexDirection = 'row';
        toolbar.style.alignItems = 'center';
        toolbar.style.background = '#e8e7e6';
        toolbar.style.borderBottom = '1px solid #bfb8b1';
        toolbar.style.padding = '2px';
        toolbar.style.height = '100%';
        toolbar.style.width = '100%';
        toolbar.style.boxSizing = 'border-box';

        container.appendChild(toolbar);
    }, GetId());

    return true;
}

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord WXUNUSED(x),
                                                  wxCoord WXUNUSED(y)) const
{
    return nullptr;
}

void wxToolBar::SetToolShortHelp( int id, const wxString& helpString )
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(FindById(id));
    if ( tool )
    {
        (void)tool->SetShortHelp(helpString);
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
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);
    tool->m_domElementId = wxWindow::NewControlId();

    int toolbarId = GetId();
    int domId = tool->m_domElementId;

    if (tool->IsSeparator())
    {
        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var toolbar = container.querySelector('.wxToolBar');
            if (!toolbar) return;

            var sep = document.createElement('div');
            sep.id = $1;
            sep.className = 'wxToolBar-separator';
            sep.style.width = '1px';
            sep.style.background = '#bfb8b1';
            sep.style.margin = '4px';
            sep.style.alignSelf = 'stretch';

            if ($2 >= 0 && $2 < toolbar.children.length) {
                toolbar.insertBefore(sep, toolbar.children[$2]);
            } else {
                toolbar.appendChild(sep);
            }
        }, toolbarId, domId, (int)pos);
    }
    else if (tool->IsButton())
    {
        wxString label = tool->GetLabel();
        wxCharBuffer labelBuf = label.ToUTF8();
        int toolId = tool->GetId();

        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var toolbar = container.querySelector('.wxToolBar');
            if (!toolbar) return;

            var btn = document.createElement('button');
            btn.id = $1;
            btn.className = 'wxToolBar-tool';
            btn.textContent = UTF8ToString($2);
            btn.style.border = '1px solid transparent';
            btn.style.background = 'transparent';
            btn.style.padding = '4px 8px';
            btn.style.cursor = 'pointer';
            btn.style.fontSize = '14px';
            btn.style.borderRadius = '3px';

            btn.onclick = function(e) {
                e.stopPropagation();
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('addEvent', null,
                        ['number', 'string', 'number', 'number'],
                        [$0, 'click', $3, 0]);
                }
            };

            if ($4 >= 0 && $4 < toolbar.children.length) {
                toolbar.insertBefore(btn, toolbar.children[$4]);
            } else {
                toolbar.appendChild(btn);
            }
        }, toolbarId, domId, labelBuf.data(), toolId, (int)pos);
    }
    else if (tool->IsControl())
    {
        // Controls are handled by their own window creation;
        // reparenting into the toolbar div could be added here.
    }

    return true;
}

bool wxToolBar::DoDeleteTool(size_t /* pos */, wxToolBarToolBase *toolBase)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);

    EM_ASM_({
        var elem = document.getElementById($0);
        if (elem && elem.parentNode) {
            elem.parentNode.removeChild(elem);
        }
    }, tool->m_domElementId);

    InvalidateBestSize();
    return true;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *toolBase, bool enable)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);

    if (tool->IsButton())
    {
        EM_ASM_({
            var btn = document.getElementById($0);
            if (btn) {
                btn.disabled = !$1;
                btn.style.opacity = $1 ? '1' : '0.5';
                btn.style.cursor = $1 ? 'pointer' : 'not-allowed';
            }
        }, tool->m_domElementId, enable);
    }
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *toolBase, bool toggle)
{
    wxToolBarTool* tool = static_cast<wxToolBarTool*>(toolBase);

    if (tool->IsButton())
    {
        EM_ASM_({
            var btn = document.getElementById($0);
            if (btn) {
                if ($1) {
                    btn.classList.add('active');
                    btn.style.background = '#d0d0d0';
                    btn.style.border = '1px solid #bfb8b1';
                } else {
                    btn.classList.remove('active');
                    btn.style.background = 'transparent';
                    btn.style.border = '1px solid transparent';
                }
            }
        }, tool->m_domElementId, toggle);
    }
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

WXWidget wxToolBar::GetHandle() const
{

}

void wxToolBar::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "click")
    {
        int toolId = event.x;
        wxCommandEvent generatedEvent(wxEVT_TOOL, toolId);
        generatedEvent.SetEventObject(this);
        HandleWindowEvent(generatedEvent);
    }
}

#endif // wxUSE_TOOLBAR
