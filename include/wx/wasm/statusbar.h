/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statusbar.h
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_STATUSBAR_H_
#define _WX_WASM_STATUSBAR_H_

#include "wx/statusbr.h"

class WXDLLIMPEXP_CORE wxStatusBar : public wxStatusBarBase
{
public:
    wxStatusBar();
    wxStatusBar(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    bool Create(wxWindow *parent, wxWindowID winid = wxID_ANY,
                long style = wxSTB_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual bool GetFieldRect(int i, wxRect& rect) const override;
    virtual void SetMinHeight(int height) override;
    virtual int GetBorderX() const override;
    virtual int GetBorderY() const override;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = nullptr ) override;

protected:
    virtual void DoUpdateStatusText(int number) override;

private:
    void Init();
    void UpdateFields();

    wxDECLARE_DYNAMIC_CLASS(wxStatusBar);
};


#endif // _WX_QT_STATUSBAR_H_
