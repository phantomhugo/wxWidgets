/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statline.h
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_STATLINE_H_
#define _WX_WASM_STATLINE_H_

class WXDLLIMPEXP_CORE wxStaticLine : public wxStaticLineBase
{
public:
    wxStaticLine();

    wxStaticLine( wxWindow *parent,
                  wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxLI_HORIZONTAL,
                  const wxString &name = wxASCII_STR(wxStaticLineNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxLI_HORIZONTAL,
                 const wxString &name = wxASCII_STR(wxStaticLineNameStr) );

private:

    wxDECLARE_DYNAMIC_CLASS( wxStaticLine );
};

#endif // _WX_WASM_STATLINE_H_
