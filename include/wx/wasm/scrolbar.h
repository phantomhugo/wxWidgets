/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/scrolbar.h
// Purpose:     wxScrollBar class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_SCROLBAR_H_
#define _WX_WASM_SCROLBAR_H_

#include "wx/scrolbar.h"

class WXDLLIMPEXP_CORE wxScrollBar : public wxScrollBarBase
{
public:
    wxScrollBar();
    wxScrollBar( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) );

    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = wxSB_HORIZONTAL,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxScrollBarNameStr) );

    virtual int GetThumbPosition() const override;
    virtual int GetThumbSize() const override;
    virtual int GetPageSize() const override;
    virtual int GetRange() const override;

    virtual void SetThumbPosition(int viewStart) override;
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true) override;

    WXWidget GetHandle() const override;

private:

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};


#endif // _WX_WASM_SCROLBAR_H_
