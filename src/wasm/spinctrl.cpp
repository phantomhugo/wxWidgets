/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/spinctrl.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_SPINCTRL

#include "wx/spinctrl.h"

wxSpinCtrl::wxSpinCtrl()
{
    Init();
}

wxSpinCtrl::wxSpinCtrl(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{
    Init();
    Create( parent, id, value, pos, size, style, min, max, initial, name );
}

bool wxSpinCtrl::Create( wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    int min, int max, int initial,
    const wxString& name )
{

}

bool wxSpinCtrl::GetSnapToTicks() const
{

}

wxString wxSpinCtrl::GetTextValue() const
{

}
int wxSpinCtrl::GetValue() const
{

}
int wxSpinCtrl::GetMin() const
{

}
int wxSpinCtrl::GetMax() const
{

}

int wxSpinCtrl::GetIncrement() const
{
    return 1;
}

void wxSpinCtrl::SetIncrement(int WXUNUSED(inc))
{
}

void wxSpinCtrl::SetRange(int WXUNUSED(min), int WXUNUSED(max))
{
}

void wxSpinCtrl::SetSnapToTicks(bool snap_to_ticks)
{

}

bool wxSpinCtrl::SetBase(int base)
{
    return true;
}

void wxSpinCtrl::SetSelection(long from, long to)
{

}

void wxSpinCtrl::SetValue( const wxString &value )
{
}

void wxSpinCtrl::SetValue(int val)
{

}

//##############################################################################

wxIMPLEMENT_DYNAMIC_CLASS( wxSpinCtrlDouble, wxSpinCtrlBase );

wxSpinCtrlDouble::wxSpinCtrlDouble()
{
}

wxSpinCtrlDouble::wxSpinCtrlDouble(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )

{
    Create( parent, id, value, pos, size, style, min, max, initial, inc, name );
}

bool wxSpinCtrlDouble::Create(wxWindow *parent, wxWindowID id, const wxString& value,
    const wxPoint& pos, const wxSize& size, long style,
    double min, double max, double initial, double inc,
    const wxString& name )
{

}

void wxSpinCtrlDouble::SetDigits(unsigned digits)
{

}

unsigned wxSpinCtrlDouble::GetDigits() const
{

}

void wxSpinCtrlDouble::SetValue( const wxString &value )
{

}

void wxSpinCtrlDouble::SetValue(double val)
{
}

double wxSpinCtrlDouble::GetValue() const
{
    return 0.0;
}

double wxSpinCtrlDouble::GetMin() const
{
    return 0.0;
}

double wxSpinCtrlDouble::GetMax() const
{
    return 100.0;
}

double wxSpinCtrlDouble::GetIncrement() const
{
    return 1.0;
}

void wxSpinCtrlDouble::SetIncrement(double WXUNUSED(inc))
{
}

void wxSpinCtrlDouble::SetRange(double WXUNUSED(min), double WXUNUSED(max))
{
}

#endif // wxUSE_SPINCTRL
