/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/minifram.h
// Purpose:     wxMiniFrame class
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MINIFRAM_H_
#define _WX_MINIFRAM_H_

#include "wx/frame.h"

class WXDLLIMPEXP_CORE wxMiniFrame : public wxFrame
{
public:
  wxMiniFrame() { }

  bool Create(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER,
              const wxString& name = wxASCII_STR(wxFrameNameStr))
  {
      return wxFrame::Create(parent, id, title, pos, size,
                             style | wxFRAME_TOOL_WINDOW | wxFRAME_NO_TASKBAR,
                             name);
  }

  wxMiniFrame(wxWindow *parent,
              wxWindowID id,
              const wxString& title,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER,
              const wxString& name = wxASCII_STR(wxFrameNameStr))
  {
      Create(parent, id, title, pos, size, style, name);
  }

protected:
  wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxMiniFrame);
};

#endif
    // _WX_MINIFRAM_H_
