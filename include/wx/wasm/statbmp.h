/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/statbmp.h
// Purpose:     wxStaticBitmap class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
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
                    const wxBitmapBundle& label,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxASCII_STR(wxStaticBitmapNameStr) );

    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxBitmapBundle& label,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxASCII_STR(wxStaticBitmapNameStr));

    virtual void SetIcon(const wxIcon& icon) override;
    virtual void SetBitmap(const wxBitmapBundle& bitmap) override;
    virtual wxBitmap GetBitmap() const override;
    virtual wxIcon GetIcon() const override;

    virtual WXWidget GetHandle() const override;
protected:

private:

    wxDECLARE_DYNAMIC_CLASS(wxStaticBitmap);
};

#endif // _WX_WASM_STATBMP_H_
