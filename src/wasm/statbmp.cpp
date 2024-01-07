/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statbmp.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/wasm/statbmp.h"

wxStaticBitmap::wxStaticBitmap()
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBitmap::Create( wxWindow *parent,
             wxWindowID id,
             const wxBitmapBundle& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{

    SetBitmap( label );


}

void wxStaticBitmap::SetIcon(const wxIcon& icon)
{

}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
}


wxBitmap wxStaticBitmap::GetBitmap() const
{
}

wxIcon wxStaticBitmap::GetIcon() const
{

}

WXWidget wxStaticBitmap::GetHandle() const
{

}
