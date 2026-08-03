/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/srchctrl.h
// Purpose:     wxSearchCtrl class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_SRCHCTRL_H_
#define _WX_WASM_SRCHCTRL_H_

#if wxUSE_SEARCHCTRL

// ----------------------------------------------------------------------------
// wxSearchCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSearchCtrl : public wxSearchCtrlBase
{
public:
    wxSearchCtrl()
    {
        Init();
    }

    wxSearchCtrl(wxWindow *parent, wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxSearchCtrlNameStr))
    {
        Init();
        Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSearchCtrlNameStr));

#if wxUSE_MENUS
    // get/set search button menu
    //
    // NOTE: the menu is stored (and owned) by the control but there is no
    // search button in the browser to pop it up from.
    virtual void SetMenu( wxMenu* menu ) override;
    virtual wxMenu* GetMenu() override { return m_menu; }
#endif // wxUSE_MENUS

    // get/set search options
    //
    // NOTE: the browser draws the icons of the native <input type="search">
    // itself, so these methods only update the stored flags.
    virtual void ShowSearchButton( bool show ) override;
    virtual bool IsSearchButtonVisible() const override
        { return m_searchButtonVisible; }

    virtual void ShowCancelButton( bool show ) override;
    virtual bool IsCancelButtonVisible() const override
        { return m_cancelButtonVisible; }

    // the descriptive text is shown as the placeholder of the native input
    virtual void SetDescriptiveText(const wxString& text) override;
    virtual wxString GetDescriptiveText() const override
        { return m_descriptiveText; }

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    void Init();

#if wxUSE_MENUS
    wxMenu *m_menu;
#endif // wxUSE_MENUS

    wxString m_descriptiveText;

    bool m_searchButtonVisible;
    bool m_cancelButtonVisible;

    wxDECLARE_DYNAMIC_CLASS( wxSearchCtrl );
};

#endif // wxUSE_SEARCHCTRL

#endif // _WX_WASM_SRCHCTRL_H_
