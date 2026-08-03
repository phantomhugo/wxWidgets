/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/activityindicator.h
// Purpose:     wxActivityIndicator class
// Author:      Hugo Armando Castellanos Morales
// Created:     31.07.26
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_ACTIVITYINDICATOR_H_
#define _WX_WASM_ACTIVITYINDICATOR_H_

// ----------------------------------------------------------------------------
// wxActivityIndicator: implementation using an animated HTML/CSS spinner.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxActivityIndicator : public wxActivityIndicatorBase
{
public:
    wxActivityIndicator();

    explicit
    wxActivityIndicator(wxWindow* parent,
                        wxWindowID winid = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxActivityIndicatorNameStr);

    bool Create(wxWindow* parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxActivityIndicatorNameStr);

    virtual void Start() override;
    virtual void Stop() override;
    virtual bool IsRunning() const override;

protected:
    virtual wxSize DoGetBestSize() const override;

private:
    bool m_running;

    wxDECLARE_DYNAMIC_CLASS(wxActivityIndicator);
    wxDECLARE_NO_COPY_CLASS(wxActivityIndicator);
};

#endif // _WX_WASM_ACTIVITYINDICATOR_H_
