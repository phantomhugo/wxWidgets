/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/collpane.h
// Purpose:     wxCollapsiblePane class
// Author:      Hugo Armando Castellanos Morales
// Created:     31.07.26
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_COLLPANE_H_
#define _WX_WASM_COLLPANE_H_

#include "wx/collpane.h"

// ----------------------------------------------------------------------------
// wxCollapsiblePane
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCollapsiblePane : public wxCollapsiblePaneBase
{
public:
    wxCollapsiblePane() { Init(); }

    wxCollapsiblePane(wxWindow *parent,
                      wxWindowID winid,
                      const wxString& label,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxCP_DEFAULT_STYLE,
                      const wxValidator& val = wxDefaultValidator,
                      const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr))
    {
        Init();

        Create(parent, winid, label, pos, size, style, val, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID winid,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCP_DEFAULT_STYLE,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCollapsiblePaneNameStr));

    // public wxCollapsiblePane API
    virtual void Collapse(bool collapse = true) override;
    virtual void SetLabel(const wxString& label) override;

    virtual bool IsCollapsed() const override
        { return m_pPane == nullptr || m_collapsed; }
    virtual wxWindow *GetPane() const override
        { return m_pPane; }
    virtual wxString GetLabel() const override
        { return m_label; }

    // implementation only, don't use
    void OnStateChange(const wxSize& sizeNew);

    void WasmNotifyEvent(const wxWasmEvent& event) override;

protected:
    virtual wxSize DoGetBestSize() const override;

private:
    void Init();

    // the container window holding the pane children
    wxWindow *m_pPane;

    // current collapsed state, kept in sync with the DOM
    bool m_collapsed;

    wxString m_label;

    wxDECLARE_DYNAMIC_CLASS( wxCollapsiblePane );
    wxDECLARE_NO_COPY_CLASS(wxCollapsiblePane);
};

#endif // _WX_WASM_COLLPANE_H_
