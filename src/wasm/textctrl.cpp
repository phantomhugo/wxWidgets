/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/textctrl.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textctrl.h"
#include "wx/settings.h"
wxTextCtrl::wxTextCtrl()
{
}

wxTextCtrl::wxTextCtrl(wxWindow *parent,
           wxWindowID id,
           const wxString &value,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxValidator& validator,
           const wxString &name)
{
    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &value,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxValidator& validator,
            const wxString &name)
{

    return false;
}

wxTextCtrl::~wxTextCtrl()
{

}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return wxTextCtrlBase::DoGetBestSize();
}

int wxTextCtrl::GetLineLength(long lineNo) const
{

}

wxString wxTextCtrl::GetLineText(long lineNo) const
{

}

int wxTextCtrl::GetNumberOfLines() const
{

}

bool wxTextCtrl::IsModified() const
{

}

void wxTextCtrl::MarkDirty()
{

}

void wxTextCtrl::DiscardEdits()
{

}

bool wxTextCtrl::SetStyle(long WXUNUSED(start), long WXUNUSED(end), const wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::GetStyle(long WXUNUSED(position), wxTextAttr& WXUNUSED(style))
{
    return false;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& WXUNUSED(style))
{
    return false;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{

}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    if ( x == nullptr || y == nullptr || pos < 0 )
        return false;


}

void wxTextCtrl::ShowPosition(long WXUNUSED(pos))
{
}

bool wxTextCtrl::DoLoadFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

bool wxTextCtrl::DoSaveFile(const wxString& WXUNUSED(file), int WXUNUSED(fileType))
{
    return false;
}

void wxTextCtrl::SetInsertionPoint(long pos)
{

}

long wxTextCtrl::GetInsertionPoint() const
{

}

wxString wxTextCtrl::DoGetValue() const
{

}

void wxTextCtrl::SetSelection( long from, long to )
{
    // SelectAll uses -1 to -1, adjust for qt:
    if ( to == -1 )
        to = GetValue().length();

    if ( from == -1 )
        from = 0;


}

void wxTextCtrl::GetSelection(long* from, long* to) const
{
    // No selection, call base for default behaviour:
    wxTextEntry::GetSelection(from, to);
}

void wxTextCtrl::WriteText( const wxString &text )
{
    // Insert the text

}

void wxTextCtrl::DoSetValue( const wxString &text, int flags )
{
    // do not fire qt signals for certain methods (i.e. ChangeText)
    SetInsertionPoint(0);
}

WXWidget wxTextCtrl::GetHandle() const
{

}
