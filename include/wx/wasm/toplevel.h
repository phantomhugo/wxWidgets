/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/toplevel.h
// Purpose:     wxTopLevelWindow class, WASM implementation
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TOPLEVEL_H_
#define _WX_WASM_TOPLEVEL_H_

class WXDLLIMPEXP_CORE wxTopLevelWindowWasm : public wxTopLevelWindowBase
{
public:
    wxTopLevelWindowWasm();
    wxTopLevelWindowWasm(wxWindow *parent,
               wxWindowID winid,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxDEFAULT_FRAME_STYLE,
               const wxString& name = wxASCII_STR(wxFrameNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual bool Show(bool show = true) override;
    virtual void Maximize(bool maximize = true) override;
    virtual void Restore() override;
    virtual void Iconize(bool iconize = true) override;
    virtual bool IsMaximized() const override;
    virtual bool IsIconized() const override;

    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL) override;
    virtual bool IsFullScreen() const override;
    virtual void SetTitle(const wxString& title) override;
    virtual wxString GetTitle() const override;
    virtual void SetIcons(const wxIconBundle& icons) override;

    // Styles
    virtual void SetWindowStyleFlag( long style ) override;
    virtual long GetWindowStyleFlag() const override;
};

#endif // _WX_WASM_TOPLEVEL_H_
