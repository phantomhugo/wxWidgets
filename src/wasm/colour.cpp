/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colour.cpp
// Purpose:     wxColour class implementation for wxQt
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include "wx/colour.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxColourImpl, wxColourBase);

bool wxColourImpl::operator==(const wxColourImpl& color) const
{
    return false;
}

unsigned long wxColourImpl::GetPixel() const
{
    return 0;
}

wxGDIRefData *wxColourImpl::CreateGDIRefData() const
{
    return nullptr;
}
wxGDIRefData *wxColourImpl::CloneGDIRefData(const wxGDIRefData *data) const
{
    return nullptr;
}
void wxColourImpl::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
}

bool wxColourImpl::FromString(const wxString& str)
{
    return false;
}

unsigned char wxColourImpl::Red() const
{
    return 0;
}
unsigned char wxColourImpl::Green() const
{
    return 0;
}
unsigned char wxColourImpl::Blue() const
{
    return 0;
}
