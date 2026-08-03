/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/bmpcbox.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/bmpcbox.h"
#include "wx/bitmap.h"

#if wxUSE_IMAGE
    #include "wx/image.h"
#endif

#include <emscripten.h>

// defined in src/wasm/statbmp.cpp
void wxWasmSetImgFromPixels(int domId, const char* selector,
                            unsigned char* rgb, unsigned char* alpha,
                            int w, int h);

wxIMPLEMENT_DYNAMIC_CLASS(wxBitmapComboBox, wxComboBox);

wxBitmapComboBox::wxBitmapComboBox()
{
}

wxBitmapComboBox::wxBitmapComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           int n, const wxString choices[],
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Create(parent, id, value, pos, size, n, choices, style, validator, name);
}

wxBitmapComboBox::wxBitmapComboBox(wxWindow *parent,
           wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
    Create(parent, id, value, pos, size, choices, style, validator, name);
}

bool wxBitmapComboBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    const wxString *pChoices = choices.size() ? &choices[ 0 ] : nullptr;
    return Create(parent, id, value, pos, size, choices.size(), pChoices,
                  style, validator, name);
}

bool wxBitmapComboBox::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& value,
            const wxPoint& pos,
            const wxSize& size,
            int n, const wxString choices[],
            long style,
            const wxValidator& validator,
            const wxString& name)
{
    // This creates the <select> (read-only) or <input>+<datalist> (editable)
    // element and fills m_bitmaps with empty bitmaps via DoInsertOneItem().
    if ( !wxComboBox::Create(parent, id, value, pos, size, n, choices, style,
                             validator, name) )
        return false;

    // HTML <option> elements cannot show images, so the dropdown remains
    // text-only: add an <img> next to the control showing the bitmap of the
    // selected item and indent the text to make room for it.
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var img = document.createElement('img');
        img.className = 'wxBitmapComboBox-bitmap';
        img.style.position = 'absolute';
        img.style.left = '4px';
        img.style.top = '50%';
        img.style.transform = 'translateY(-50%)';
        img.style.width = '20px';
        img.style.height = '20px';
        img.style.objectFit = 'contain';
        img.style.pointerEvents = 'none';
        img.style.display = 'none';

        container.style.position = 'relative';
        container.insertBefore(img, container.firstChild);

        var ctrl = container.querySelector('.wxChoice, .wxComboBox');
        if (ctrl) ctrl.style.paddingLeft = '28px';
    }, GetId());

    return true;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap)
{
    const int n = wxComboBox::Append(item);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             void *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Append(const wxString& item, const wxBitmapBundle& bitmap,
                             wxClientData *clientData)
{
    const int n = wxComboBox::Append(item, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos)
{
    const int n = wxComboBox::Insert(item, pos);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, void *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

int wxBitmapComboBox::Insert(const wxString& item, const wxBitmapBundle& bitmap,
                             unsigned int pos, wxClientData *clientData)
{
    const int n = wxComboBox::Insert(item, pos, clientData);
    if ( n != wxNOT_FOUND )
        SetItemBitmap(n, bitmap);
    return n;
}

void wxBitmapComboBox::SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap)
{
    wxCHECK_RET(n < GetCount(), "invalid index");

    if ( n >= m_bitmaps.size() )
        m_bitmaps.resize(n + 1);

    m_bitmaps[n] = bitmap.IsOk() ? bitmap.GetBitmapFor(this) : wxBitmap();

    // Refresh the <img> if the changed item is the currently selected one.
    if ( (int)n == GetSelection() )
        UpdateBitmap();
}

wxBitmap wxBitmapComboBox::GetItemBitmap(unsigned int n) const
{
    wxCHECK_MSG(n < GetCount(), wxNullBitmap, "invalid index");

    if ( n < m_bitmaps.size() )
        return m_bitmaps[n];

    return wxNullBitmap;
}

wxSize wxBitmapComboBox::GetBitmapSize() const
{
    for ( const wxBitmap& bitmap : m_bitmaps )
    {
        if ( bitmap.IsOk() )
            return bitmap.GetSize();
    }

    return wxSize(0, 0);
}

void wxBitmapComboBox::SetSelection(int n)
{
    wxComboBox::SetSelection(n);

    UpdateBitmap();
}

void wxBitmapComboBox::SetValue(const wxString& value)
{
    wxComboBox::SetValue(value);

    UpdateBitmap();
}

int wxBitmapComboBox::DoInsertOneItem(const wxString& item, unsigned int pos)
{
    const int idx = wxComboBox::DoInsertOneItem(item, pos);

    if ( idx != wxNOT_FOUND )
        m_bitmaps.insert(m_bitmaps.begin() + idx, wxBitmap());

    return idx;
}

void wxBitmapComboBox::DoClear()
{
    wxComboBox::DoClear();

    m_bitmaps.clear();

    UpdateBitmap();
}

void wxBitmapComboBox::DoDeleteOneItem(unsigned int pos)
{
    wxComboBox::DoDeleteOneItem(pos);

    if ( pos < m_bitmaps.size() )
        m_bitmaps.erase(m_bitmaps.begin() + pos);

    UpdateBitmap();
}

void wxBitmapComboBox::UpdateBitmap()
{
#if wxUSE_IMAGE
    int sel = GetSelection();
    if ( sel == wxNOT_FOUND )
        sel = FindString(GetValue());

    if ( sel >= 0 && (unsigned int)sel < m_bitmaps.size() &&
            m_bitmaps[sel].IsOk() )
    {
        wxImage image = m_bitmaps[sel].ConvertToImage();
        if ( image.IsOk() )
        {
            wxWasmSetImgFromPixels(GetId(), ".wxBitmapComboBox-bitmap",
                image.GetData(),
                image.HasAlpha() ? image.GetAlpha() : nullptr,
                image.GetWidth(), image.GetHeight());

            EM_ASM_({
                var container = document.getElementById($0);
                if (!container) return;
                var img = container.querySelector('.wxBitmapComboBox-bitmap');
                if (img) img.style.display = "";
            }, GetId());

            return;
        }
    }
#endif // wxUSE_IMAGE

    // No bitmap for the current item: hide the <img>.
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var img = container.querySelector('.wxBitmapComboBox-bitmap');
        if (img) img.style.display = 'none';
    }, GetId());
}

void wxBitmapComboBox::WasmNotifyEvent(const wxWasmEvent& event)
{
    wxComboBox::WasmNotifyEvent(event);

    if ( event.id == m_windowId && event.eventType == "change" )
        UpdateBitmap();
}
