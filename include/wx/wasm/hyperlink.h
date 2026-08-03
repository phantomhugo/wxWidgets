/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/hyperlink.h
// Purpose:     wxHyperlinkCtrl class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_HYPERLINK_H_
#define _WX_WASM_HYPERLINK_H_

#if wxUSE_HYPERLINKCTRL

// Also pull in the generic implementation: src/generic/hyperlinkg.cpp is part
// of GUI_CMN_SRC and needs wxGenericHyperlinkCtrl to be declared (same
// pattern as wx/msw/hyperlink.h).
#include "wx/generic/hyperlink.h"

// ----------------------------------------------------------------------------
// wxHyperlinkCtrl
// ----------------------------------------------------------------------------

// NB: derives from the generic control (and not directly from
// wxHyperlinkCtrlBase) so that code using wxGenericHyperlinkCtrl pointers
// keeps working, as with the GTK and MSW native implementations; all the
// inherited methods dealing with the appearance are overridden below to use
// the native <a> element instead of the generic rendering.
class WXDLLIMPEXP_CORE wxHyperlinkCtrl : public wxGenericHyperlinkCtrl
{
public:
    // Default constructor (for two-step construction).
    wxHyperlinkCtrl()
    {
        Init();
    }

    // Constructor.
    wxHyperlinkCtrl(wxWindow *parent,
                    wxWindowID id,
                    const wxString& label,
                    const wxString& url,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxHL_DEFAULT_STYLE,
                    const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr))
    {
        Init();
        Create(parent, id, label, url, pos, size, style, name);
    }

    // Creation function (for two-step construction).
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxString& url,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHL_DEFAULT_STYLE,
                const wxString& name = wxASCII_STR(wxHyperlinkCtrlNameStr));

    // get/set
    virtual wxColour GetHoverColour() const override { return m_hoverColour; }
    virtual void SetHoverColour(const wxColour &colour) override;

    virtual wxColour GetNormalColour() const override { return m_normalColour; }
    virtual void SetNormalColour(const wxColour &colour) override;

    virtual wxColour GetVisitedColour() const override { return m_visitedColour; }
    virtual void SetVisitedColour(const wxColour &colour) override;

    virtual wxString GetURL() const override { return m_url; }
    virtual void SetURL (const wxString &url) override;

    virtual void SetVisited(bool visited = true) override;
    virtual bool GetVisited() const override { return m_visited; }

    // updates the text of the native <a> element (the URL is used as the
    // text when the label is empty)
    virtual void SetLabel(const wxString& label) override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

protected:
    virtual wxSize DoGetBestSize() const override;

private:
    // Common part of all ctors.
    void Init();

    // Apply the cached colours to the native <a> element.
    void UpdateLinkColour();

    // URL associated with the link. This is transmitted inside
    // the HyperlinkEvent fired when the user clicks on the label.
    wxString m_url;

    // Foreground colours for the various link states.
    wxColour m_hoverColour;
    wxColour m_normalColour;
    wxColour m_visitedColour;

    // True if the link has been clicked before.
    bool m_visited;

    wxDECLARE_DYNAMIC_CLASS( wxHyperlinkCtrl );
};

#endif // wxUSE_HYPERLINKCTRL

#endif // _WX_WASM_HYPERLINK_H_
