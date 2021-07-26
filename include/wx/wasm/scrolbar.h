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

    virtual int GetThumbPosition() const wxOVERRIDE;
    virtual int GetThumbSize() const wxOVERRIDE;
    virtual int GetPageSize() const wxOVERRIDE;
    virtual int GetRange() const wxOVERRIDE;

    virtual void SetThumbPosition(int viewStart) wxOVERRIDE;
    virtual void SetScrollbar(int position, int thumbSize,
                              int range, int pageSize,
                              bool refresh = true) wxOVERRIDE;

    WXWidget GetHandle() const wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS(wxScrollBar);
};


#endif // _WX_WASM_SCROLBAR_H_
