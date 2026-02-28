/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statusbar.h
// Author:      Hugo Castellanos
// Copyright:   (c) 2023-2024 wxWidgets dev team
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

    // Sobrescribir para manejar cambios de estilo
    virtual void SetWindowStyleFlag(long style) override;
    
    // Sobrescribir para manejar múltiples fields
    virtual void SetFieldsCount(int number = 1, const int* widths = nullptr) override;
    
    // Sobrescribir para actualizar DOM
    virtual void PopStatusText(int field = 0) override;
    virtual void PushStatusText(const wxString& text, int field = 0) override;

protected:
    virtual void DoUpdateStatusText(int number) override;

private:
    void Init();
    void wxWasmCreateStatusBar();
    void UpdateFields();

    wxDECLARE_DYNAMIC_CLASS(wxStatusBar);
};

#endif // _WX_WASM_STATUSBAR_H_
