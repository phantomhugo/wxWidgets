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

    virtual int GetValue() const wxOVERRIDE;
    virtual void SetValue(int value) wxOVERRIDE;

    virtual void SetRange(int minValue, int maxValue) wxOVERRIDE;
    virtual int GetMin() const wxOVERRIDE;
    virtual int GetMax() const wxOVERRIDE;

    virtual void DoSetTickFreq(int freq) wxOVERRIDE;
    virtual int GetTickFreq() const wxOVERRIDE;

    virtual void SetLineSize(int lineSize) wxOVERRIDE;
    virtual void SetPageSize(int pageSize) wxOVERRIDE;
    virtual int GetLineSize() const wxOVERRIDE;
    virtual int GetPageSize() const wxOVERRIDE;

    virtual void SetThumbLength(int lenPixels) wxOVERRIDE;
    virtual int GetThumbLength() const wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS( wxSlider );
};

#endif // _WX_WASM_SLIDER_H_
