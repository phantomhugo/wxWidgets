/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/menu.h
// Purpose:     wxListMenu class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MENU_H_
#define _WX_WASM_MENU_H_

class QMenu;
class QMenuBar;

class WXDLLIMPEXP_CORE wxMenu : public wxMenuBase
{
public:
    wxMenu(long style = 0);
    wxMenu(const wxString& title, long style = 0);

    virtual QMenu *GetHandle() const;

protected:
    virtual wxMenuItem *DoAppend(wxMenuItem *item) override;
    virtual wxMenuItem *DoInsert(size_t pos, wxMenuItem *item) override;
    virtual wxMenuItem *DoRemove(wxMenuItem *item) override;

private:
    QMenu *m_qtMenu;

    wxDECLARE_DYNAMIC_CLASS(wxMenu);
};



class WXDLLIMPEXP_CORE wxMenuBar : public wxMenuBarBase
{
public:
    wxMenuBar();
    wxMenuBar(long style);
    wxMenuBar(size_t n, wxMenu *menus[], const wxString titles[], long style = 0);

    virtual bool Append(wxMenu *menu, const wxString& title) override;
    virtual bool Insert(size_t pos, wxMenu *menu, const wxString& title) override;
    virtual wxMenu *Remove(size_t pos) override;

    virtual void EnableTop(size_t pos, bool enable) override;
    virtual bool IsEnabledTop(size_t pos) const override;

    virtual void SetMenuLabel(size_t pos, const wxString& label) override;
    virtual wxString GetMenuLabel(size_t pos) const override;

    QMenuBar *GetQMenuBar() const { return m_qtMenuBar; }
    virtual WXWidget GetHandle() const override;

    virtual void Attach(wxFrame *frame) override;
    virtual void Detach() override;

private:
    QMenuBar *m_qtMenuBar;

    wxDECLARE_DYNAMIC_CLASS(wxMenuBar);
};

#endif // _WX_WASM_MENU_H_
