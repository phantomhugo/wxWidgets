/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/ctrlsub.h
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CTRLSUB_H_
#define _WX_WASM_CTRLSUB_H_

class WXDLLIMPEXP_CORE wxControlWithItems : public wxControlWithItemsBase
{
public:
    wxControlWithItems();

protected:

private:
    wxDECLARE_ABSTRACT_CLASS(wxControlWithItems);
};

#endif // _WX_WASM_CTRLSUB_H_

