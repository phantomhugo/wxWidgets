/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/accel.h
// Purpose:     wxAcceleratorTable class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_ACCEL_H_
#define _WX_WASM_ACCEL_H_

class WXDLLIMPEXP_CORE wxAcceleratorTable : public wxObject
{
public:
    wxAcceleratorTable();
    wxAcceleratorTable(int n, const wxAcceleratorEntry entries[]);

    bool Ok() const { return IsOk(); }
    bool IsOk() const;

    // Returns the entry matching the given modifier flags (combination of
    // wxACCEL_ALT/CTRL/SHIFT) and key code, or nullptr if there is none.
    // Letters are compared case-insensitively: FromString() uppercases
    // letters combined with modifiers ("Ctrl+o" -> 'O') while keyboard
    // events report the Shift-dependent case.
    const wxAcceleratorEntry* FindEntry(int flags, int keyCode) const;

protected:
    // ref counting code
    virtual wxObjectRefData *CreateRefData() const override;
    virtual wxObjectRefData *CloneRefData(const wxObjectRefData *data) const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxAcceleratorTable);
};

#endif // _WX_WASM_ACCEL_H_

