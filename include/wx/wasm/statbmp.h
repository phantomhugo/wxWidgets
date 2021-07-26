/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Hugo Castellanos
// Created:     26.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_STATBMP_H_
#define _WX_WASM_STATBMP_H_

class WXDLLIMPEXP_CORE wxStaticBitmap : public wxStaticBitmapBase
{
public:
    wxStaticBitmap();
    wxStaticBitmap( wxWindow *parent,
                    wxWindowID id,
                    const wxBitmap& label,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxASCII_STR(wxStaticBitmapNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxBitmap& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetIcon(const wxIcon& icon) wxOVERRIDE;
    virtual void SetBitmap(const wxBitmap& bitmap) wxOVERRIDE;
    virtual wxBitmap GetBitmap() const wxOVERRIDE;
    virtual wxIcon GetIcon() const wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;
protected:

private:

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif // _WX_WASM_STATBMP_H_
