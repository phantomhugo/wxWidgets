/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/spinctrl.h
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_SPINCTRL_H_
#define _WX_WASM_SPINCTRL_H_

class WXDLLIMPEXP_CORE wxSpinCtrl : public wxSpinCtrlBase
{
public:
    wxSpinCtrl();
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxT("wxSpinCtrl"));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = wxT("wxSpinCtrl"));
    virtual int GetBase() const override { return m_base; }
    bool GetSnapToTicks() const override;
    int GetValue() const;
    int GetMin() const;
    int GetMax() const;
    int GetIncrement() const;
    wxString GetTextValue() const override;
    virtual bool SetBase(int base) override;
    void SetSnapToTicks(bool snap_to_ticks) override;
    void SetSelection(long from, long to) override;
    virtual void SetValue(const wxString & val) override;
    virtual void SetValue(int val);

private:
    // Common part of all ctors.
    void Init()
    {
        m_base = 10;
    }
    int m_base;
    wxDECLARE_DYNAMIC_CLASS(wxSpinCtrl);
};

class WXDLLIMPEXP_CORE wxSpinCtrlDouble : public wxSpinCtrl
{
public:
    wxSpinCtrlDouble();
    wxSpinCtrlDouble(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_ARROW_KEYS,
                     double min = 0, double max = 100, double initial = 0,
                     double inc = 1,
                     const wxString& name = wxT("wxSpinCtrlDouble"));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxT("wxSpinCtrlDouble"));

    void SetDigits(unsigned digits);
    unsigned GetDigits() const;

    virtual int GetBase() const override { return 10; }
    double GetValue() const;
    double GetMin() const;
    double GetMax() const;
    double GetIncrement() const;
    virtual bool SetBase(int WXUNUSED(base)) override { return false; }
    virtual void SetValue(const wxString & val) override;
    virtual void SetValue(double val);

private:
    wxDECLARE_DYNAMIC_CLASS( wxSpinCtrlDouble );
};

#endif // _WX_WASM_SPINCTRL_H_

