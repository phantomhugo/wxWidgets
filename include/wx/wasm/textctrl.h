/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/region.h
// Purpose:     wxTextCtrl class
// Author:      Hugo Castellanos
// Created:     17.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_TEXTCTRL_H_
#define _WX_WASM_TEXTCTRL_H_

class WXDLLIMPEXP_CORE wxTextCtrl : public wxTextCtrlBase
{
public:
    wxTextCtrl();
    wxTextCtrl(wxWindow *parent,
               wxWindowID id,
               const wxString &value = wxEmptyString,
               const wxPoint &pos = wxDefaultPosition,
               const wxSize &size = wxDefaultSize,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString &name = wxASCII_STR(wxTextCtrlNameStr));

    virtual ~wxTextCtrl();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString &value = wxEmptyString,
                const wxPoint &pos = wxDefaultPosition,
                const wxSize &size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString &name = wxASCII_STR(wxTextCtrlNameStr));

    virtual int GetLineLength(long lineNo) const wxOVERRIDE;
    virtual wxString GetLineText(long lineNo) const wxOVERRIDE;
    virtual int GetNumberOfLines() const wxOVERRIDE;

    virtual bool IsModified() const wxOVERRIDE;
    virtual void MarkDirty() wxOVERRIDE;
    virtual void DiscardEdits() wxOVERRIDE;

    virtual bool SetStyle(long start, long end, const wxTextAttr& style) wxOVERRIDE;
    virtual bool GetStyle(long position, wxTextAttr& style) wxOVERRIDE;
    virtual bool SetDefaultStyle(const wxTextAttr& style) wxOVERRIDE;

    virtual long XYToPosition(long x, long y) const wxOVERRIDE;
    virtual bool PositionToXY(long pos, long *x, long *y) const wxOVERRIDE;

    virtual void ShowPosition(long pos) wxOVERRIDE;

    virtual void SetInsertionPoint(long pos) wxOVERRIDE;
    virtual long GetInsertionPoint() const wxOVERRIDE;
    virtual void SetSelection( long from, long to ) wxOVERRIDE;
    virtual void GetSelection(long *from, long *to) const wxOVERRIDE;

    virtual wxString DoGetValue() const wxOVERRIDE;
    virtual void DoSetValue(const wxString &text, int flags = 0) wxOVERRIDE;
    virtual void WriteText(const wxString& text) wxOVERRIDE;

    virtual WXWidget GetHandle() const wxOVERRIDE;

protected:
    virtual wxSize DoGetBestSize() const wxOVERRIDE;

    virtual bool DoLoadFile(const wxString& file, int fileType) wxOVERRIDE;
    virtual bool DoSaveFile(const wxString& file, int fileType) wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS( wxTextCtrl );
};

#endif // _WX_WASM_TEXTCTRL_H_
