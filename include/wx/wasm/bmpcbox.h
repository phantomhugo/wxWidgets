/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/bmpcbox.h
// Purpose:     wxBitmapComboBox class
// Author:      Hugo Armando Castellanos Morales
// Created:     31.07.26
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_BMPCBOX_H_
#define _WX_WASM_BMPCBOX_H_

#include "wx/combobox.h"

#include <vector>

// ----------------------------------------------------------------------------
// wxBitmapComboBox: a wxComboBox that allows images to be shown
// in front of string items.
//
// HTML <option> elements cannot show images, so in this port the dropdown
// list is text-only and the bitmap of the currently selected item is shown
// in an <img> element placed on the left of the control instead.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_ADV wxBitmapComboBox : public wxComboBox,
                                         public wxBitmapComboBoxBase
{
public:
    // ctors and such
    wxBitmapComboBox();

    wxBitmapComboBox(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     int n = 0,
                     const wxString choices[] = NULL,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

    wxBitmapComboBox(wxWindow *parent,
                     wxWindowID id,
                     const wxString& value,
                     const wxPoint& pos,
                     const wxSize& size,
                     const wxArrayString& choices,
                     long style = 0,
                     const wxValidator& validator = wxDefaultValidator,
                     const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxBitmapComboBoxNameStr));

    // Adds item with image to the end of the combo box.
    int Append(const wxString& item, const wxBitmapBundle& bitmap = wxBitmapBundle());
    int Append(const wxString& item, const wxBitmapBundle& bitmap, void *clientData);
    int Append(const wxString& item, const wxBitmapBundle& bitmap, wxClientData *clientData);

    // Inserts item with image into the list before pos. Not valid for wxCB_SORT
    // styles, use Append instead.
    int Insert(const wxString& item, const wxBitmapBundle& bitmap, unsigned int pos);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, void *clientData);
    int Insert(const wxString& item, const wxBitmapBundle& bitmap,
               unsigned int pos, wxClientData *clientData);

    // Sets the image for the given item.
    virtual void SetItemBitmap(unsigned int n, const wxBitmapBundle& bitmap) override;

    // Returns the image of the item with the given index.
    virtual wxBitmap GetItemBitmap(unsigned int n) const override;

    // Returns size of the image used in list
    virtual wxSize GetBitmapSize() const override;

    virtual void SetSelection(int n) override;
    virtual void SetValue(const wxString& value) override;

    void WasmNotifyEvent(const wxWasmEvent& event) override;

protected:
    // From wxComboBox (keep m_bitmaps in sync with the items):
    virtual int DoInsertOneItem(const wxString& item, unsigned int pos) override;
    virtual void DoClear() override;
    virtual void DoDeleteOneItem(unsigned int pos) override;

private:
    // Shows the bitmap of the currently selected item in the <img> next to
    // the control, or hides it if the item has no bitmap.
    void UpdateBitmap();

    // Images associated with the items, by index.
    std::vector<wxBitmap> m_bitmaps;

    wxDECLARE_DYNAMIC_CLASS(wxBitmapComboBox);
};

#endif // _WX_WASM_BMPCBOX_H_
