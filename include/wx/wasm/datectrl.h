/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/datectrl.h
// Purpose:     wxDatePickerCtrl for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DATECTRL_H_
#define _WX_WASM_DATECTRL_H_

class WXDLLIMPEXP_ADV wxDatePickerCtrl : public wxDatePickerCtrlBase
{
public:
    wxDatePickerCtrl() = default;
    wxDatePickerCtrl(wxWindow *parent,
                     wxWindowID id,
                     const wxDateTime& date = wxDefaultDateTime,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxDatePickerCtrlNameStr)
    {
        Create(parent, id, date, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxDateTime& date = wxDefaultDateTime,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDP_DEFAULT | wxDP_SHOWCENTURY,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxDatePickerCtrlNameStr);

    virtual void SetValue(const wxDateTime& dt) override;
    virtual wxDateTime GetValue() const override;

    virtual void SetRange(const wxDateTime& dt1, const wxDateTime& dt2) override;
    virtual bool GetRange(wxDateTime *dt1, wxDateTime *dt2) const override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxDatePickerCtrl);
};

#endif // _WX_WASM_DATECTRL_H_
