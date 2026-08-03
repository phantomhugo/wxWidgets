/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/mdi.h
// Purpose:     wxMdi classes
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_MDI_H_
#define _WX_WASM_MDI_H_

class WXDLLIMPEXP_CORE wxMDIParentFrame : public wxMDIParentFrameBase
{
public:
    wxMDIParentFrame();
    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxASCII_STR(wxFrameNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    // override/implement base class [pure] virtual methods
    // ----------------------------------------------------

    static bool IsTDI() { return false; }

    virtual void ActivateNext() override;
    virtual void ActivatePrevious() override;

    virtual void Cascade() override;
    virtual void Tile(wxOrientation orient = wxHORIZONTAL) override;
    virtual void ArrangeIcons() override;

#if wxUSE_MENUS
    // Override these to keep the "Window" menu in the current menu bar, as
    // the generic MDI implementation does.
    virtual void SetWindowMenu(wxMenu *menu) override;
    virtual void SetMenuBar(wxMenuBar *menuBar) override;
#endif // wxUSE_MENUS

protected:
#if wxUSE_MENUS
    void AddWindowMenu(wxMenuBar *menuBar);
    void RemoveWindowMenu(wxMenuBar *menuBar);
#endif // wxUSE_MENUS

private:
    wxDECLARE_DYNAMIC_CLASS(wxMDIParentFrame);
};



class WXDLLIMPEXP_CORE wxMDIChildFrame : public wxMDIChildFrameBase
{
public:
    wxMDIChildFrame();
    virtual ~wxMDIChildFrame();
    wxMDIChildFrame(wxMDIParentFrame *parent,
                    wxWindowID id,
                    const wxString& title,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_FRAME_STYLE,
                    const wxString& name = wxASCII_STR(wxFrameNameStr));

    bool Create(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxASCII_STR(wxFrameNameStr));

    virtual void Activate() override;

    wxDECLARE_DYNAMIC_CLASS(wxMDIChildFrame);
};



class WXDLLIMPEXP_CORE wxMDIClientWindow : public wxMDIClientWindowBase
{
public:
    wxMDIClientWindow();

    virtual bool CreateClient(wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL) override;
    wxDECLARE_DYNAMIC_CLASS(wxMDIClientWindow);
};

#endif // _WX_WASM_MDI_H_
