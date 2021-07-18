/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/menu.h
// Purpose:     wxListMenu class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MENUITEM_H_
#define _WX_WASM_MENUITEM_H_

#include "wx/menuitem.h"
#include "wx/bitmap.h"

class QAction;
class wxQtAction;

class WXDLLIMPEXP_FWD_CORE wxMenu;

class WXDLLIMPEXP_CORE wxMenuItem : public wxMenuItemBase
{
public:
    wxMenuItem(wxMenu *parentMenu = NULL,
               int id = wxID_SEPARATOR,
               const wxString& text = wxEmptyString,
               const wxString& help = wxEmptyString,
               wxItemKind kind = wxITEM_NORMAL,
               wxMenu *subMenu = NULL);

    virtual void SetItemLabel(const wxString& str) wxOVERRIDE;
    virtual void SetCheckable(bool checkable) wxOVERRIDE;

    virtual void Enable(bool enable = true) wxOVERRIDE;
    virtual bool IsEnabled() const wxOVERRIDE;

    virtual void Check(bool check = true) wxOVERRIDE;
    virtual bool IsChecked() const wxOVERRIDE;

    virtual void SetBitmap(const wxBitmap& bitmap);
    virtual const wxBitmap& GetBitmap() const { return m_bitmap; }

    virtual QAction *GetHandle() const;

    virtual void SetFont(const wxFont& font);
private:
    // Qt is using an action instead of a menu item.
    wxQtAction *m_qtAction;
    wxBitmap m_bitmap;

    wxDECLARE_DYNAMIC_CLASS( wxMenuItem );
};



#endif // _WX_WASM_MENUITEM_H_
