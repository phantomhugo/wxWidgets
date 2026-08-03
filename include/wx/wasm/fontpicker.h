/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/fontpicker.h
// Purpose:     wxFontPickerWidget for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FONTPICKER_H_
#define _WX_WASM_FONTPICKER_H_

#include "wx/fontdata.h"

class WXDLLIMPEXP_CORE wxFontPickerWidget : public wxControl,
                                             public wxFontPickerWidgetBase
{
public:
    wxFontPickerWidget() = default;
    wxFontPickerWidget(wxWindow *parent,
                       wxWindowID id,
                       const wxFont& initial = wxNullFont,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxFONTBTN_DEFAULT_STYLE,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxASCII_STR(wxFontPickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxFont& initial = wxNullFont,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFONTBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxFontPickerWidgetNameStr));

    virtual wxColour GetSelectedColour() const override;
    virtual void SetSelectedColour(const wxColour& colour) override;

    wxFontData *GetFontData() { return &m_data; }

    void WasmNotifyEvent(const wxWasmEvent& event) override;

protected:
    virtual void UpdateFont() override;

private:
    wxColour m_selectedColour;
    wxFontData m_data;
    wxDECLARE_DYNAMIC_CLASS(wxFontPickerWidget);
};

#endif // _WX_WASM_FONTPICKER_H_
