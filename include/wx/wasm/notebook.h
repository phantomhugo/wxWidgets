/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/notebook.h
// Purpose:     wxNotebook classes
// Author:      Hugo Castellanos
// Created:     19.07.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_NOTEBOOK_H_
#define _WX_WASM_NOTEBOOK_H_

class WXDLLIMPEXP_CORE wxNotebook : public wxNotebookBase
{
public:
    wxNotebook();
    wxNotebook(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxASCII_STR(wxNotebookNameStr));

    bool Create(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0,
              const wxString& name = wxASCII_STR(wxNotebookNameStr));

    virtual void SetPadding(const wxSize& padding) wxOVERRIDE;
    virtual void SetTabSize(const wxSize& sz) wxOVERRIDE;

    virtual bool SetPageText(size_t n, const wxString& strText) wxOVERRIDE;
    virtual wxString GetPageText(size_t n) const wxOVERRIDE;

    virtual int GetPageImage(size_t n) const wxOVERRIDE;
    virtual bool SetPageImage(size_t n, int imageId) wxOVERRIDE;

    virtual bool InsertPage(size_t n, wxWindow *page, const wxString& text,
        bool bSelect = false, int imageId = -1) wxOVERRIDE;

    virtual wxSize CalcSizeFromPage(const wxSize& sizePage) const wxOVERRIDE;

    int SetSelection(size_t nPage) wxOVERRIDE;
    int ChangeSelection(size_t nPage) wxOVERRIDE;

    virtual bool DeleteAllPages() wxOVERRIDE;

protected:
    virtual wxWindow *DoRemovePage(size_t page) wxOVERRIDE;

private:

    // internal array to store imageId for each page:
    wxVector<int> m_images;

    wxDECLARE_DYNAMIC_CLASS( wxNotebook );
};


#endif // _WX_WASM_NOTEBOOK_H_
