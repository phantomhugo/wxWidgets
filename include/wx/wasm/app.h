/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/app.h
// Purpose:     wxApp class
// Author:      Hugo Armando Castellanos Morales
// Created:     17.06.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_APP_H_
#define _WX_WASM_APP_H_

class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    ~wxApp();

    virtual bool Initialize(int& argc, wxChar **argv) override;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxApp );
};

#endif // _WX_WASM_APP_H_
