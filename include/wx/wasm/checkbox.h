/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/checkbox.h
// Purpose:     wxCheckBox class
// Author:      Hugo Castellanos
// Created:     11.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CHECKBOX_H_
#define _WX_WASM_CHECKBOX_H_

#include "wx/button.h" // for wxStdButtonInputHandler

// ----------------------------------------------------------------------------
// the actions supported by wxCheckBox
// ----------------------------------------------------------------------------

#define wxACTION_CHECKBOX_CHECK   wxT("check")   // SetValue(true)
#define wxACTION_CHECKBOX_CLEAR   wxT("clear")   // SetValue(false)
#define wxACTION_CHECKBOX_TOGGLE  wxT("toggle")  // toggle the check state

// additionally it accepts wxACTION_BUTTON_PRESS and RELEASE

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxCheckBox : public wxCheckBoxBase
{
public:
    // checkbox constants
    enum State
    {
        State_Normal,
        State_Pressed,
        State_Disabled,
        State_Current,
        State_Max
    };

    enum Status
    {
        Status_Checked,
        Status_Unchecked,
        Status_3rdState,
        Status_Max
    };

    // constructors
    wxCheckBox() { Init(); }

    wxCheckBox(wxWindow *parent,
               wxWindowID id,
               const wxString& label,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxASCII_STR(wxCheckBoxNameStr))
    {
        Init();

        Create(parent, id, label, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxCheckBoxNameStr));

    // implement the checkbox interface
    virtual void SetValue(bool value) wxOVERRIDE;
    virtual bool GetValue() const wxOVERRIDE;

    // set/get the bitmaps to use for the checkbox indicator
    void SetBitmap(const wxBitmap& bmp, State state, Status status);
    virtual wxBitmap GetBitmap(State state, Status status) const;

    // wxCheckBox actions
    void Toggle();
    virtual void Press();
    virtual void Release();
    virtual void ChangeValue(bool value);

protected:
    virtual void DoSet3StateValue(wxCheckBoxState WXUNUSED(state)) wxOVERRIDE;
    virtual wxCheckBoxState DoGet3StateValue() const wxOVERRIDE;

    virtual wxSize DoGetBestClientSize() const wxOVERRIDE;

    // get the size of the bitmap using either the current one or the default
    // one (query renderer then)
    virtual wxSize GetBitmapSize() const;

    // common part of all ctors
    void Init();

    // send command event notifying about the checkbox state change
    virtual void SendEvent();

    // called when the checkbox becomes checked - radio button hook
    virtual void OnCheck();

    // get the state corresponding to the flags (combination of wxCONTROL_XXX)
    wxCheckBox::State GetState(int flags) const;

    // directly access the bitmaps array without trying to find a valid bitmap
    // to use as GetBitmap() does
    wxBitmap DoGetBitmap(State state, Status status) const
        { return m_bitmaps[state][status]; }

    // get the current status
    Status GetStatus() const { return m_status; }

private:
    // the current check status
    Status m_status;

    // the bitmaps to use for the different states
    wxBitmap m_bitmaps[State_Max][Status_Max];

    // is the checkbox currently pressed?
    bool m_isPressed;

    wxDECLARE_DYNAMIC_CLASS(wxCheckBox);
};

#endif // _WX_WASM_CHECKBOX_H_
