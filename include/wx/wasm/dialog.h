/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dialog.h
// Purpose:     wxDialog class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DIALOG_H_
#define _WX_WASM_DIALOG_H_

class WXDLLIMPEXP_FWD_BASE wxEventLoopBase;

class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
public:
    wxDialog();

    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString &title,
             const wxPoint &pos = wxDefaultPosition,
             const wxSize &size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString &name = wxASCII_STR(wxDialogNameStr));

    virtual ~wxDialog();

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString &title,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString &name = wxASCII_STR(wxDialogNameStr));

    virtual int ShowModal() override;
    virtual void EndModal(int retCode) override;
    virtual bool IsModal() const override;
    virtual bool Show(bool show = true) override;

    void *GetDialogHandle() const;

private:
    bool m_isModal;

    // The modal event loop created by ShowModal(), valid only while it
    // runs; EndModal() must exit this loop and not whatever loop happens
    // to be active when it is called (e.g. from a timer or idle handler).
    wxEventLoopBase* m_modalLoop;

    wxDECLARE_DYNAMIC_CLASS(wxDialog);
};

#endif // _WX_WASM_DIALOG_H_
