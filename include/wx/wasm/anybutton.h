/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/anybutton.h
// Purpose:     wxWASM wxAnyButton class declaration
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_ANYBUTTON_H_
#define _WX_WASM_ANYBUTTON_H_

//-----------------------------------------------------------------------------
// wxAnyButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnyButton : public wxAnyButtonBase
{
public:
    wxAnyButton();

    // implementation
    // --------------

    virtual void SetLabel( const wxString &label ) override;
    virtual wxString GetLabel() const override;

    virtual void *GetHandle() const override;

protected:
    virtual wxBitmap DoGetBitmap(State state) const override;
    virtual void DoSetBitmap(const wxBitmapBundle& bitmap, State which) override;

private:

    typedef wxAnyButtonBase base_type;
    wxBitmapBundle m_bitmaps[State_Max];

    wxDECLARE_NO_COPY_CLASS(wxAnyButton);
};


#endif // _WX_WASM_ANYBUTTON_H_

