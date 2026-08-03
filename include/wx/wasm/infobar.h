/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/infobar.h
// Purpose:     wxInfoBar class
// Author:      Hugo Armando Castellanos Morales
// Created:     31.07.26
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_INFOBAR_H_
#define _WX_WASM_INFOBAR_H_

#include "wx/infobar.h"

// Also pull in the generic implementation: src/generic/infobar.cpp is part of
// GUI_CMN_SRC and needs wxInfoBarGeneric to be declared (same pattern as
// wx/gtk/infobar.h).
#include "wx/generic/infobar.h"

// event sent when the user dismisses the info bar by clicking its close
// button (not when Dismiss() is called programmatically)
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_INFOBAR_DISMISSED, wxCommandEvent );

class WXDLLIMPEXP_CORE wxInfoBar : public wxInfoBarBase
{
public:
    // the usual ctors and Create() but remember that info bar is created
    // hidden
    wxInfoBar() { Init(); }

    wxInfoBar(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0)
    {
        Init();
        Create(parent, winid, style);
    }

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY, long style = 0);


    // implement base class methods
    // ----------------------------

    virtual void ShowMessage(const wxString& msg,
                             int flags = wxICON_INFORMATION) override;

    virtual void Dismiss() override;

    virtual void AddButton(wxWindowID btnid,
                           const wxString& label = wxString()) override;

    virtual void RemoveButton(wxWindowID btnid) override;

    virtual size_t GetButtonCount() const override;
    virtual wxWindowID GetButtonId(size_t idx) const override;
    virtual bool HasButtonId(wxWindowID btnid) const override;

    // methods specific to this version
    // --------------------------------

    // show a checkbox with the given text in the bar (empty text hides it),
    // same as in the generic version
    void ShowCheckBox(const wxString& checkBoxText, bool checked = false);

    // return true if the checkbox shown by ShowCheckBox() is checked
    bool IsCheckBoxChecked() const;

    // effects are not supported by this implementation, the values are just
    // stored for compatibility with the generic version
    void SetShowHideEffects(wxShowEffect showEffect, wxShowEffect hideEffect)
    {
        m_showEffect = showEffect;
        m_hideEffect = hideEffect;
    }

    wxShowEffect GetShowEffect() const { return m_showEffect; }
    wxShowEffect GetHideEffect() const { return m_hideEffect; }

    void SetEffectDuration(int duration) { m_effectDuration = duration; }
    int GetEffectDuration() const { return m_effectDuration; }

    void WasmNotifyEvent(const wxWasmEvent& event) override;

protected:
    // info bar shouldn't have any border by default, the colour difference
    // between it and the main window separates it well enough
    virtual wxBorder GetDefaultBorder() const override { return wxBORDER_NONE; }

    virtual wxSize DoGetBestSize() const override;

private:
    // common part of all ctors
    void Init();

    // show/hide the bar in the DOM without using effects
    void DoShowBar(bool show);

    // ids of the buttons added with AddButton()
    wxVector<wxWindowID> m_buttons;

    // the effects to use when showing/hiding and duration for them: unused
    // by this implementation, see SetShowHideEffects() comment above
    wxShowEffect m_showEffect,
                 m_hideEffect;
    int m_effectDuration;

    wxDECLARE_DYNAMIC_CLASS( wxInfoBar );
    wxDECLARE_NO_COPY_CLASS(wxInfoBar);
};

#endif // _WX_WASM_INFOBAR_H_
