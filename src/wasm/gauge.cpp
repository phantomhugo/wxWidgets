/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/gauge.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/gauge.h"


wxGauge::wxGauge()
{
}

wxGauge::wxGauge(wxWindow *parent,
        wxWindowID id,
        int range,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxValidator& validator,
        const wxString& name)
{
    Create( parent, id, range, pos, size, style, validator, name );
}

bool wxGauge::Create(wxWindow *parent,
            wxWindowID id,
            int range,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxValidator& validator,
            const wxString& name)
{

}


void *wxGauge::GetHandle() const
{
}

// set/get the control range and value

void wxGauge::SetRange(int range)
{

}

int wxGauge::GetRange() const
{

}

void wxGauge::SetValue(int pos)
{

}

int wxGauge::GetValue() const
{

}
