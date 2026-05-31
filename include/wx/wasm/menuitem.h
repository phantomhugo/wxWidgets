/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/menuitem.h
// Purpose:     wxMenuItem class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MENUITEM_H_
#define _WX_WASM_MENUITEM_H_

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = nullptr,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = nullptr);

    virtual void SetItemLabel(const wxString& label) override;

    virtual void SetCheckable(bool checkable) override;
    virtual void Enable(bool enable = true) override;
    virtual bool IsEnabled() const override;
    virtual void Check(bool checked = true) override;
    virtual bool IsChecked() const override;
    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;

    virtual void *GetHandle() const;
    virtual void SetFont(const wxFont& font);
    
    // Métodos específicos de WASM
    void CreateDOM(wxMenu* parentMenu);
    void UpdateDOMState();

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMenuItem);
};

#endif // _WX_WASM_MENUITEM_H_
