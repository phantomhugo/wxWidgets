/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/slider.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/slider.h"

wxSlider::wxSlider()
{
}

wxSlider::wxSlider(wxWindow *parent,
         wxWindowID id,
         int value, int minValue, int maxValue,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxValidator& validator,
         const wxString& name)
{
    Create( parent, id, value, minValue, maxValue, pos, size, style, validator, name );
}

bool wxSlider::Create(wxWindow *parent,
            wxWindowID id,
            int WXUNUSED(value), int minValue, int maxValue,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{

    SetRange( minValue, maxValue );
    SetPageSize(wxMax(1, (maxValue - minValue) / 10));

    return true;
}

int wxSlider::GetValue() const
{

}

void wxSlider::SetValue(int value)
{

}

void wxSlider::SetRange(int minValue, int maxValue)
{

}

int wxSlider::GetMin() const
{

}

int wxSlider::GetMax() const
{

}

void wxSlider::DoSetTickFreq(int freq)
{

}

int wxSlider::GetTickFreq() const
{

}

void wxSlider::SetLineSize(int WXUNUSED(lineSize))
{
}

void wxSlider::SetPageSize(int pageSize)
{

}

int wxSlider::GetLineSize() const
{
    return 0;
}

int wxSlider::GetPageSize() const
{

}

void wxSlider::SetThumbLength(int WXUNUSED(lenPixels))
{
}

int wxSlider::GetThumbLength() const
{
    return 0;
}


void *wxSlider::GetHandle() const
{

}

