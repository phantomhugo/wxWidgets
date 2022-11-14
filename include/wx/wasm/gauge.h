/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/gauge.h
// Purpose:     wxGauge class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_GAUGE_H_
#define _WX_WASM_GAUGE_H_

class WXDLLIMPEXP_CORE wxGauge : public wxGaugeBase
{
public:
    wxGauge();

    wxGauge(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxGA_HORIZONTAL,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxASCII_STR(wxGaugeNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                int range,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxGA_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxGaugeNameStr));

    virtual WXWidget GetHandle() const override;

    // set/get the control range
    virtual void SetRange(int range) override;
    virtual int GetRange() const override;

    virtual void SetValue(int pos) override;
    virtual int GetValue() const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxGauge);
};

#endif // _WX_WASM_GAUGE_H_
