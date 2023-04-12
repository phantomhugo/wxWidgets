/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/notebook.cpp
// Author:      Mariano Reingart, Peter Most
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/notebook.h"
wxNotebook::wxNotebook()
{
}

wxNotebook::wxNotebook(wxWindow *parent,
         wxWindowID id,
         const wxPoint& pos,
         const wxSize& size,
         long style,
         const wxString& name)
{
    Create( parent, id, pos, size, style, name );
}

bool wxNotebook::Create(wxWindow *parent,
          wxWindowID id,
          const wxPoint& pos,
          const wxSize& size,
          long style,
          const wxString& name)
{
}

void wxNotebook::SetPadding(const wxSize& WXUNUSED(padding))
{
}

void wxNotebook::SetTabSize(const wxSize& WXUNUSED(sz))
{
}


bool wxNotebook::SetPageText(size_t n, const wxString &text)
{
    return true;
}

wxString wxNotebook::GetPageText(size_t n) const
{

}

int wxNotebook::GetPageImage(size_t n) const
{
    wxCHECK_MSG(n < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

}

bool wxNotebook::SetPageImage(size_t n, int imageId)
{
    wxCHECK_MSG(n < GetPageCount(), false, "invalid notebook index");

    return true;
}

bool wxNotebook::InsertPage(size_t n, wxWindow *page, const wxString& text,
    bool bSelect, int imageId)
{
    DoSetSelectionAfterInsertion(n, bSelect);

    return true;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{

}

bool wxNotebook::DeleteAllPages()
{
    return wxNotebookBase::DeleteAllPages();
}

int wxNotebook::SetSelection(size_t page)
{
    wxCHECK_MSG(page < GetPageCount(), wxNOT_FOUND, "invalid notebook index");

    int selOld = GetSelection();

    // change the QTabWidget selected page:
    m_selection = page;

    return selOld;
}

int wxNotebook::ChangeSelection(size_t nPage)
{

    return SetSelection(nPage);
}

wxWindow *wxNotebook::DoRemovePage(size_t page)
{
    wxNotebookBase::DoRemovePage(page);
    m_images.erase( m_images.begin() + page );
}
