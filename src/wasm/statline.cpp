/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statline.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) Hugo Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statline.h"

wxStaticLine::wxStaticLine()
{
}

wxStaticLine::wxStaticLine( wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              long style,
              const wxString &name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxStaticLine::Create( wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString &name)
{
}

