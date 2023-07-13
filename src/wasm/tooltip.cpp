/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/tooltip.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/tooltip.h"

/* static */ void wxToolTip::Enable(bool WXUNUSED(flag))
{

}

/* static */ void wxToolTip::SetDelay(long WXUNUSED(milliseconds))
{

}

/* static */ void wxToolTip::SetAutoPop(long WXUNUSED(milliseconds))
{

}

/* static */ void wxToolTip::SetReshow(long WXUNUSED(milliseconds))
{

}



wxToolTip::wxToolTip(const wxString &tip)
{
    m_window = nullptr;
    SetTip(tip);
}

void wxToolTip::SetTip(const wxString& tip)
{
    m_text = tip;

    if ( m_window )
        m_window->QtApplyToolTip(m_text);
}

const wxString &wxToolTip::GetTip() const
{
    return m_text;
}


void wxToolTip::SetWindow(wxWindow *win)
{
//
}
