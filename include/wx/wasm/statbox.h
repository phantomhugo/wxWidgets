/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statbox.h
// Purpose:     wxStaticBox class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_STATBOX_H_
#define _WX_WASM_STATBOX_H_

class WXDLLIMPEXP_CORE wxStaticBox : public wxStaticBoxBase
{
public:
    wxStaticBox();

    wxStaticBox(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxASCII_STR(wxStaticBoxNameStr));

    virtual void GetBordersForSizer(int *borderTop, int *borderOther) const wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

    virtual void SetLabel(const wxString& label) wxOVERRIDE;
    virtual wxString GetLabel() const wxOVERRIDE;

protected:

private:


    wxDECLARE_DYNAMIC_CLASS( wxStaticBox );
};

#endif // _WX_WASM_STATBOX_H_
