/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/menu.h
// Purpose:     wxListMenu class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MENU_H_
#define _WX_WASM_MENU_H_

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    wxMenu(long style = 0);
    wxMenu(const wxString& title, long style = 0);

    virtual void *GetHandle() const;

    // Each menu needs a unique DOM id for HTML element mapping.
    // wxMenuBase doesn't inherit from wxWindow, so we add our own id.
    int GetId() const { return m_id; }

protected:
    virtual wxMenuItem *DoAppend(wxMenuItem *item) override;
    virtual wxMenuItem *DoInsert(size_t pos, wxMenuItem *item) override;
    virtual wxMenuItem *DoRemove(wxMenuItem *item) override;

private:
    int m_id;
    static int GetNextMenuId();

    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};



class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar(long style=0);
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);

    virtual bool Append(wxMenu *menu, const wxString& title) override;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) override;
    virtual wxMenu *Remove(size_t pos) override;

    virtual void EnableTop(size_t pos, bool enable) override;
    virtual bool IsEnabledTop(size_t pos) const override;

    virtual void SetMenuLabel(size_t pos, const wxString& label) override;
    virtual wxString GetMenuLabel(size_t pos) const override;

    virtual WXWidget GetHandle() const override;

    virtual void Attach(wxFrame *frame) override;
    virtual void Detach() override;

private:
    void wxWasmCreateMenuBar(long style);
    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif // _WX_WASM_MENU_H_
