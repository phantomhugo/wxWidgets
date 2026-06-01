/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/checkbox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/checkbox.h"
#include <emscripten.h>

wxCheckBox::wxCheckBox()
{
    Init();
}

wxCheckBox::wxCheckBox(wxWindow *parent, wxWindowID id, const wxString& label,
        const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
        const wxString& name )
{
    Create(parent, id, label, pos, size, style, validator, name);
}

bool wxCheckBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
            const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator,
            const wxString& name )
{
    Init();

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    int domId = GetId();
    wxCharBuffer labelBuffer = label.ToUTF8();

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var label = document.createElement('label');
        label.className = 'wxCheckBox';

        var input = document.createElement('input');
        input.type = 'checkbox';

        var span = document.createElement('span');
        span.textContent = UTF8ToString($1);

        label.appendChild(input);
        label.appendChild(span);

        input.onchange = function(e) {
            e.stopPropagation();
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        };

        container.appendChild(label);
    }, domId, labelBuffer.data());

    return true;
}

void wxCheckBox::SetValue(bool value)
{
    m_status = value ? Status_Checked : Status_Unchecked;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var checkbox = container.querySelector('.wxCheckBox input[type="checkbox"]');
        if (checkbox) checkbox.checked = $1;
    }, GetId(), static_cast<int>(value));
}

bool wxCheckBox::GetValue() const
{
    int checked = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var checkbox = container.querySelector('.wxCheckBox input[type="checkbox"]');
        return checkbox ? checkbox.checked : 0;
    }, GetId());

    return checked != 0;
}

void wxCheckBox::DoSet3StateValue(wxCheckBoxState state)
{
    if (state == wxCHK_CHECKED)
        SetValue(true);
    else
        SetValue(false);
}

wxCheckBoxState wxCheckBox::DoGet3StateValue() const
{
    return GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED;
}

void wxCheckBox::Press()
{
    m_isPressed = true;
}

void wxCheckBox::Release()
{
    m_isPressed = false;
}

wxBitmap wxCheckBox::GetBitmap(State WXUNUSED(state), Status WXUNUSED(status)) const
{
    return wxBitmap();
}

void wxCheckBox::ChangeValue(bool value)
{
    SetValue(value);
}

void wxCheckBox::SendEvent()
{
    wxCommandEvent event(wxEVT_CHECKBOX, GetId());
    event.SetInt(GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
    HandleWindowEvent(event);
}

void wxCheckBox::OnCheck()
{
}

wxSize wxCheckBox::DoGetBestClientSize() const
{
    return wxSize(20 + GetLabel().length() * 6, 20);
}

wxSize wxCheckBox::GetBitmapSize() const
{
    return wxSize();
}

void wxCheckBox::Init()
{
    m_isPressed = false;
    m_status = Status_Unchecked;
}

void wxCheckBox::Toggle()
{
    m_isPressed = false;
    SetValue(!GetValue());
    SendEvent();
}

void wxCheckBox::SetBitmap(const wxBitmap& WXUNUSED(bmp), State WXUNUSED(state), Status WXUNUSED(status))
{
}

wxCheckBox::State wxCheckBox::GetState(int WXUNUSED(flags)) const
{
    return State_Normal;
}

void wxCheckBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id == m_windowId && event.eventType == "change" )
    {
        m_status = GetValue() ? Status_Checked : Status_Unchecked;

        wxCommandEvent evt(wxEVT_CHECKBOX, m_windowId);
        evt.SetInt(GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED);
        HandleWindowEvent(evt);
    }
}
