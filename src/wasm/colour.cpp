/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colour.cpp
// Purpose:     wxColour class implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
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

void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{

}
