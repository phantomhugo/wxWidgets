/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/nonownedwnd.h
// Purpose:     wxNonOwnedWindow class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_NONOWNEDWND_H_
#define _WX_WASM_NONOWNEDWND_H_

// ----------------------------------------------------------------------------
// wxNonOwnedWindow contains code common to wx{Popup,TopLevel}Window in wxQT.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxNonOwnedWindow : public wxNonOwnedWindowBase
{
public:
    wxNonOwnedWindow();

protected:
    virtual bool DoClearShape() wxOVERRIDE;
    virtual bool DoSetRegionShape(const wxRegion& region) wxOVERRIDE;
#if wxUSE_GRAPHICS_CONTEXT
    virtual bool DoSetPathShape(const wxGraphicsPath& path) wxOVERRIDE;
#endif // wxUSE_GRAPHICS_CONTEXT

    wxDECLARE_NO_COPY_CLASS(wxNonOwnedWindow);
};

#endif // _WX_WASM_NONOWNEDWND_H_
