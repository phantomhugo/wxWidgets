/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/stattext.cpp
// Purpose:     wxStaticText
// Author:      Julian Smart
// Created:     04/01/98
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATTEXT

#include "wx/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/dcclient.h"
    #include "wx/settings.h"
#endif

#include "wx/msw/darkmode.h"
#include "wx/msw/private.h"
#include "wx/msw/private/darkmode.h"
#include "wx/msw/private/winstyle.h"

#if wxUSE_MARKUP
    #include "wx/generic/private/markuptext.h"
#endif // wxUSE_MARKUP

bool wxStaticText::Create(wxWindow *parent,
                          wxWindowID id,
                          const wxString& label,
                          const wxPoint& pos,
                          const wxSize& size,
                          long style,
                          const wxString& name)
{
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(wxT("STATIC"), wxEmptyString, pos, size) )
        return false;

    // we set the label here and not through MSWCreateControl() because we
    // need to do many operation on it for ellipsization&markup support
    SetLabel(label);

    // as we didn't pass the correct label to MSWCreateControl(), it didn't set
    // the initial size correctly -- do it now
    SetInitialSize(size);

    // NOTE: if the label contains ampersand characters which are interpreted as
    //       accelerators, they will be rendered (at least on WinXP) only if the
    //       static text is placed inside a window class which correctly handles
    //       focusing by TAB traversal (e.g. wxPanel).

    return true;
}

wxStaticText::~wxStaticText()
{
#if wxUSE_MARKUP
    delete m_markupText;
#endif // wxUSE_MARKUP
}

WXDWORD wxStaticText::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD msStyle = wxControl::MSWGetStyle(style, exstyle);

    // translate the alignment flags to the Windows ones
    //
    // note that both wxALIGN_LEFT and SS_LEFT are equal to 0 so we shouldn't
    // test for them using & operator
    if ( style & wxALIGN_CENTRE_HORIZONTAL )
        msStyle |= SS_CENTER;
    else if ( style & wxALIGN_RIGHT )
        msStyle |= SS_RIGHT;
    else
        msStyle |= SS_LEFT;

#ifdef SS_ENDELLIPSIS
    // for now, add the SS_ENDELLIPSIS style if wxST_ELLIPSIZE_END is given;
    // we may need to remove it later in ::SetLabel() if the given label
    // has newlines
    if ( style & wxST_ELLIPSIZE_END )
        msStyle |= SS_ENDELLIPSIS;
#endif // SS_ENDELLIPSIS

    // this style is necessary to receive mouse events
    msStyle |= SS_NOTIFY;

    return msStyle;
}

wxSize wxStaticText::DoGetBestClientSize() const
{
    wxInfoDC dc(const_cast<wxStaticText *>(this));

#if wxUSE_MARKUP
    if ( m_markupText )
        return m_markupText->Measure(dc);
#endif // wxUSE_MARKUP

    wxCoord widthTextMax, heightTextTotal;
    dc.GetMultiLineTextExtent(GetLabelText(), &widthTextMax, &heightTextTotal);

    // This extra pixel is a hack we use to ensure that a wxStaticText
    // vertically centered around the same position as a wxTextCtrl shows its
    // text on exactly the same baseline. It is not clear why is this needed
    // nor even whether this works in all cases, but it does work, at least
    // with the default fonts, under Windows XP, 7 and 8, so just use it for
    // now.
    //
    // In the future we really ought to provide a way for each of the controls
    // to provide information about the position of the baseline for the text
    // it shows and use this information in the sizer code when centering the
    // controls vertically, otherwise we simply can't ensure that the text is
    // always on the same line, e.g. even with this hack wxComboBox text is
    // still not aligned to the same position.
    heightTextTotal += 1;

    // And this extra pixel is an even worse hack which is somehow needed to
    // avoid the problem with the native control now showing any text at all
    // for some particular width values: e.g. without this, using " AJ" as a
    // label doesn't show anything at all on the screen, even though the
    // control text is properly set and it has rougly the correct (definitely
    // not empty) size. This looks like a bug in the native control because it
    // really should show at least the first characters, but it's not clear
    // what else can we do about it than just add this extra pixel.
    widthTextMax++;

    return wxSize(widthTextMax, heightTextTotal);
}

void wxStaticText::DoSetSize(int x, int y, int w, int h, int sizeFlags)
{
    // Keep track of the size before so we can see if it changed
    const wxSize sizeBefore = GetSize();

    // note: we first need to set the size and _then_ call UpdateLabel
    wxStaticTextBase::DoSetSize(x, y, w, h, sizeFlags);

    // Avoid flicker by not refreshing or updating the label if the size didn't
    // change.
    if ( sizeBefore == GetSize() )
        return;

#ifdef SS_ENDELLIPSIS
    // do we need to ellipsize the contents?
    long styleReal = ::GetWindowLong(GetHwnd(), GWL_STYLE);
    if ( !(styleReal & SS_ENDELLIPSIS) )
    {
        // we don't have SS_ENDELLIPSIS style:
        // we need to (eventually) do ellipsization ourselves
        UpdateLabel();
    }
    //else: we don't or the OS will do it for us
#endif // SS_ENDELLIPSIS

    // we need to refresh the window after changing its size as the standard
    // control doesn't always update itself properly
    Refresh();
}

bool
wxStaticText::MSWHandleMessage(WXLRESULT *result,
                               WXUINT message,
                               WXWPARAM wParam,
                               WXLPARAM lParam)
{
    if ( wxStaticTextBase::MSWHandleMessage(result, message, wParam, lParam) )
        return true;

    switch ( message )
    {
        case WM_PAINT:
            // We only customize drawing of disabled plain labels in dark mode.
            if ( ::IsWindowEnabled(GetHwnd()) ||
#if wxUSE_MARKUP
                    m_markupText ||
#endif // wxUSE_MARKUP
                        !wxMSWDarkMode::IsActive() )
                break;

            // For them, the default "greying out" of the text for the disabled
            // controls looks ugly and unreadable in dark mode, so we draw it
            // as normal text but use a different colour for it.
            //
            // We could alternatively make the control owner-drawn, which would
            // be slightly cleaner, but would require more effort.
            wxMSWWinStyleUpdater updateStyle(GetHwnd());
            updateStyle.TurnOff(WS_DISABLED).Apply();

            // Don't use Get/SetForegroundColour() here as they do more than we
            // need, we just want to change m_foregroundColour temporarily
            // without any side effects.
            const auto colFgOrig = m_foregroundColour;
            wxDarkModeSettings darkModeSettings;
            m_foregroundColour = darkModeSettings.GetMenuColour(wxMenuColour::DisabledFg);

            *result = MSWDefWindowProc(WM_PAINT, wParam, lParam);

            updateStyle.TurnOn(WS_DISABLED).Apply();
            m_foregroundColour = colFgOrig;

            return true;
    }

    return false;
}

void wxStaticText::SetLabel(const wxString& label)
{
    // If the label doesn't really change, avoid flicker by not doing anything.
    if ( label == m_labelOrig )
        return;

#if wxUSE_MARKUP
    if ( m_markupText )
    {
        Unbind(wxEVT_PAINT, &wxStaticText::WXOnPaint, this);

        delete m_markupText;
        m_markupText = nullptr;
    }
#endif // wxUSE_MARKUP

#ifdef SS_ENDELLIPSIS
    wxMSWWinStyleUpdater updateStyle(GetHwnd());
    if ( HasFlag(wxST_ELLIPSIZE_END) )
    {
        // adding SS_ENDELLIPSIS or SS_ENDELLIPSIS "disables" the correct
        // newline handling in static texts: the newlines in the labels are
        // shown as square. Thus we don't use it even on newer OS when
        // the static label contains a newline.
        updateStyle.TurnOnOrOff(!label.Contains(wxT('\n')), SS_ENDELLIPSIS);
    }
    else // style not supported natively
    {
        updateStyle.TurnOff(SS_ENDELLIPSIS);
    }

    updateStyle.Apply();
#endif // SS_ENDELLIPSIS

    // save the label in m_labelOrig with both the markup (if any) and
    // the mnemonics characters (if any)
    m_labelOrig = label;

#ifdef SS_ENDELLIPSIS
    if ( updateStyle.IsOn(SS_ENDELLIPSIS) )
        WXSetVisibleLabel(GetLabel());
    else
#endif // SS_ENDELLIPSIS
        WXSetVisibleLabel(GetEllipsizedLabel());

    AutoResizeIfNecessary();
}

bool wxStaticText::SetFont(const wxFont& font)
{
    if ( !wxControl::SetFont(font) )
        return false;

    AutoResizeIfNecessary();

    return true;
}

// These functions are used by wxST_ELLIPSIZE_* supporting code in
// wxStaticTextBase which requires us to implement them, but actually the base
// wxWindow methods already do exactly what we need under this platform.

wxString wxStaticText::WXGetVisibleLabel() const
{
    return wxWindow::GetLabel();
}

void wxStaticText::WXSetVisibleLabel(const wxString& str)
{
    wxWindow::SetLabel(str);
}

#if wxUSE_MARKUP

bool wxStaticText::DoSetLabelMarkup(const wxString& markup)
{
    // Remove the non-markup label, we don't want the native control to show it
    // in addition to the one we draw ourselves.
    ::SetWindowText(GetHwnd(), wxT(""));

    const wxString label = RemoveMarkup(markup);
    if ( label.empty() && !markup.empty() )
        return false;

    m_labelOrig = label;

    // Don't do anything if the label didn't change.
    if ( m_markupText && !m_markupText->SetMarkup(markup) )
        return true;

    if ( !m_markupText )
    {
        Bind(wxEVT_PAINT, &wxStaticText::WXOnPaint, this);

        m_markupText = new wxMarkupText(markup);
    }

    AutoResizeIfNecessary();

    Refresh();

    return true;
}

void wxStaticText::WXOnPaint(wxPaintEvent& event)
{
    // We shouldn't normally be called in this case, but ensure we don't do
    // anything if we are, somehow.
    if ( !m_markupText )
    {
        event.Skip();
        return;
    }

    wxPaintDC dc(this);

    // Erase the background in the same way the native control does it, in
    // particular this lets the parent background show through.
    ::SendMessage(GetHwnd(), WM_PRINTCLIENT,
                  (WPARAM)dc.GetHDC(),
                  PRF_ERASEBKGND);

    const wxRect rect = GetClientRect();
    if ( !IsThisEnabled() )
    {
        if ( wxMSWDarkMode::IsActive() )
        {
            wxDarkModeSettings darkModeSettings;
            dc.SetTextForeground(
                darkModeSettings.GetMenuColour(wxMenuColour::DisabledFg)
            );
        }
        else // Emulate traditional greyed out disabled look.
        {
            dc.SetTextForeground(
                wxSystemSettings::GetColour(wxSYS_COLOUR_BTNHIGHLIGHT)
            );

            wxRect rectShadow = rect;
            rectShadow.Offset(1, 1);

            m_markupText->Render(dc, rectShadow, wxMarkupText::Render_ShowAccels,
                                 GetAlignment());

            dc.SetTextForeground(
                wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)
            );
        }
    }

    m_markupText->Render(dc, rect, wxMarkupText::Render_ShowAccels,
                         GetAlignment());
}

#endif // wxUSE_MARKUP

#endif // wxUSE_STATTEXT
