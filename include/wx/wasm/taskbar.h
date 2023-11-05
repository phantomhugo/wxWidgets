/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/taskbar.h
// Purpose:     wxTaskBar classes
// Author:      Hugo Castellanos
// Created:     26.07.23
// Copyright:   (c) 2023 Hugo Castellanos
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TASKBAR_H_
#define _WX_WASM_TASKBAR_H_


class WXDLLIMPEXP_ADV wxTaskBarIcon : public wxTaskBarIconBase
{
public:
    wxTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);
    virtual ~wxTaskBarIcon();

    // Accessors
    bool IsOk() const { return false; }
    bool IsIconInstalled() const { return false; }

    // Operations
    virtual bool SetIcon(const wxIcon& icon,
                         const wxString& tooltip = wxEmptyString) override;
    virtual bool RemoveIcon() override;
    virtual bool PopupMenu(wxMenu *menu) override;

private:

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxTaskBarIcon);
};

#endif // _WX_WASM_TASKBAR_H_
