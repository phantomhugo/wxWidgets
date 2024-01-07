/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colour.cpp
// Purpose:     wxColour class implementation for wxQt
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/colour.h"
#endif // WX_PRECOMP

#include "wx/colour.h"

bool wxColour::operator==(const wxColour& color) const
{

}

bool wxColour::operator!=(const wxColour& color) const
{
    return !(*this == color);
}

unsigned long wxColour::GetPixel() const
{
    return 0;
}

wxGDIRefData *wxColour::CreateGDIRefData() const
{

}
wxGDIRefData *wxColour::CloneGDIRefData(const wxGDIRefData *data) const
{

}
void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{

}

bool wxColour::FromString(const wxString& str)
{

}

unsigned char wxColour::Red() const
{

}
unsigned char wxColour::Green() const
{

}
unsigned char wxColour::Blue() const
{

}
