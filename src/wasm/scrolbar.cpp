/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/scrolbar.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/scrolbar.h"

wxScrollBar::wxScrollBar()
{
}

wxScrollBar::wxScrollBar( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{
    Create( parent, id, pos, size, style, validator, name );
}

bool wxScrollBar::Create( wxWindow *parent, wxWindowID id,
       const wxPoint& pos,
       const wxSize& size,
       long style,
       const wxValidator& validator,
       const wxString& name)
{

}

int wxScrollBar::GetThumbPosition() const
{

}

int wxScrollBar::GetThumbSize() const
{
}

int wxScrollBar::GetPageSize() const
{

}

int wxScrollBar::GetRange() const
{

}

void wxScrollBar::SetThumbPosition(int viewStart)
{

}

void wxScrollBar::SetScrollbar(int position, int WXUNUSED(thumbSize),
                          int range, int pageSize,
                          bool WXUNUSED(refresh))
{

}

WXWidget wxScrollBar::GetHandle() const
{

}
