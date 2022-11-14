/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/slider.h
// Purpose:     wxSlider class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_SLIDER_H_
#define _WX_WASM_SLIDER_H_

class WXDLLIMPEXP_CORE wxSlider : public wxSliderBase
{
public:
    wxSlider();
    wxSlider(wxWindow *parent,
             wxWindowID id,
             int value, int minValue, int maxValue,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxSL_HORIZONTAL,
             const wxValidator& validator = wxDefaultValidator,
             const wxString& name = wxASCII_STR(wxSliderNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                int value, int minValue, int maxValue,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSL_HORIZONTAL,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxSliderNameStr));

    virtual int GetValue() const override;
    virtual void SetValue(int value) override;

    virtual void SetRange(int minValue, int maxValue) override;
    virtual int GetMin() const override;
    virtual int GetMax() const override;

    virtual void DoSetTickFreq(int freq) override;
    virtual int GetTickFreq() const override;

    virtual void SetLineSize(int lineSize) override;
    virtual void SetPageSize(int pageSize) override;
    virtual int GetLineSize() const override;
    virtual int GetPageSize() const override;

    virtual void SetThumbLength(int lenPixels) override;
    virtual int GetThumbLength() const override;

    virtual WXWidget GetHandle() const override;

private:

    wxDECLARE_DYNAMIC_CLASS( wxSlider );
};

#endif // _WX_WASM_SLIDER_H_
