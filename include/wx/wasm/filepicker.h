/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/filepicker.h
// Purpose:     wxFilePickerWidget, wxDirPickerWidget for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_FILEPICKER_H_
#define _WX_WASM_FILEPICKER_H_

class WXDLLIMPEXP_CORE wxFilePickerWidget : public wxControl,
                                             public wxFileDirPickerWidgetBase
{
public:
    wxFilePickerWidget() = default;
    wxFilePickerWidget(wxWindow *parent,
                       wxWindowID id,
                       const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                       const wxString& path = wxEmptyString,
                       const wxString& message = wxFileSelectorPromptStr,
                       const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxFILEBTN_DEFAULT_STYLE,
                       const wxValidator& validator = wxDefaultValidator,
                       const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxASCII_STR(wxFilePickerWidgetLabel),
                const wxString& path = wxEmptyString,
                const wxString& message = wxFileSelectorPromptStr,
                const wxString& wildcard = wxFileSelectorDefaultWildcardStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFILEBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxFilePickerWidgetNameStr));

    virtual wxString GetPath() const override;
    virtual void SetPath(const wxString& str) override;

    virtual wxControl *AsControl() override { return this; }

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFilePickerWidget);
};

class WXDLLIMPEXP_CORE wxDirPickerWidget : public wxControl,
                                            public wxFileDirPickerWidgetBase
{
public:
    wxDirPickerWidget() = default;
    wxDirPickerWidget(wxWindow *parent,
                      wxWindowID id,
                      const wxString& label = wxASCII_STR(wxDirPickerWidgetLabel),
                      const wxString& path = wxEmptyString,
                      const wxString& message = wxDirSelectorPromptStr,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxDIRBTN_DEFAULT_STYLE,
                      const wxValidator& validator = wxDefaultValidator,
                      const wxString& name = wxASCII_STR(wxDirPickerWidgetNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxASCII_STR(wxDirPickerWidgetLabel),
                const wxString& path = wxEmptyString,
                const wxString& message = wxDirSelectorPromptStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxDirPickerWidgetNameStr));

    virtual wxString GetPath() const override;
    virtual void SetPath(const wxString& str) override;

    virtual wxControl *AsControl() override { return this; }

    void WasmNotifyEvent(const wxWasmEvent& event) override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxDirPickerWidget);
};

#endif // _WX_WASM_FILEPICKER_H_
