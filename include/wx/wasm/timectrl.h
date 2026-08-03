/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/timectrl.h
// Purpose:     wxTimePickerCtrl for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TIMECTRL_H_
#define _WX_WASM_TIMECTRL_H_

class WXDLLIMPEXP_ADV wxTimePickerCtrl : public wxTimePickerCtrlBase
{
public:
    wxTimePickerCtrl() = default;
    wxTimePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& date = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxTP_DEFAULT,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxTimePickerCtrlNameStr)
    {
        Create(parent, id, date, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTP_DEFAULT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxTimePickerCtrlNameStr);

    virtual void SetValue(const wxDateTime& dt) override;
    virtual wxDateTime GetValue() const override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTimePickerCtrl);
};

#endif // _WX_WASM_TIMECTRL_H_
