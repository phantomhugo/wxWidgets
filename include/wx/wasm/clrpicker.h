/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/clrpicker.h
// Purpose:     wxColourPickerWidget for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CLRPICKER_H_
#define _WX_WASM_CLRPICKER_H_

class WXDLLIMPEXP_CORE wxColourPickerWidget : public wxControl
{
public:
    wxColourPickerWidget();
    wxColourPickerWidget(wxWindow *parent,
                   wxWindowID id,
                   const wxColour& initial = *wxBLACK,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxCLRBTN_DEFAULT_STYLE,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxColour& initial = *wxBLACK,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxColourPickerWidgetNameStr));

    wxColour GetColour() const { return m_colour; }
    void SetColour(const wxColour& c) { m_colour = c; UpdateColour(); }

    void UpdateColour();

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxColour m_colour;
    wxDECLARE_DYNAMIC_CLASS(wxColourPickerWidget);
};

#endif // _WX_WASM_CLRPICKER_H_
