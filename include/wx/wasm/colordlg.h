/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/colordlg.h
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_COLORDLG_H_
#define _WX_WASM_COLORDLG_H_

#include "wx/dialog.h"
#include "wx/colourdata.h"

class WXDLLIMPEXP_CORE wxColourDialog : public wxDialog
{
public:
    wxColourDialog() { }
    wxColourDialog(wxWindow *parent,
                   const wxColourData *data = nullptr) { Create(parent, data); }

    bool Create(wxWindow *parent, const wxColourData *data = nullptr);

    virtual int ShowModal() override;

    wxColourData &GetColourData();

private:

    wxColourData m_data;

    wxDECLARE_DYNAMIC_CLASS(wxColourDialog);
};

#endif // _WX_WASM_COLORDLG_H_
