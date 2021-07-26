/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/region.h
// Purpose:     wxStaticText class
// Author:      Hugo Castellanos
// Created:     19.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_STATTEXT_H_
#define _WX_WASM_STATTEXT_H_

class WXDLLIMPEXP_CORE wxStaticText : public wxStaticTextBase
{
public:
    wxStaticText();
    wxStaticText(wxWindow *parent,
                 wxWindowID id,
                 const wxString &label,
                 const wxPoint &pos = wxDefaultPosition,
                 const wxSize &size = wxDefaultSize,
                 long style = 0,
                 const wxString &name = wxASCII_STR(wxStaticTextNameStr) );

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &label,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxString &name = wxASCII_STR(wxStaticTextNameStr) );

    virtual void SetLabel(const wxString& label) wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

protected:
    virtual wxString WXGetVisibleLabel() const wxOVERRIDE;
    virtual void WXSetVisibleLabel(const wxString& str) wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS( wxStaticText );
};

#endif // _WX_WASM_STATTEXT_H_
