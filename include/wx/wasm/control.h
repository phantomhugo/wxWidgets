/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/control.h
// Purpose:     wxControl class
// Author:      Hugo Castellanos
// Created:     17.06.21
// Copyright:   (c) 2021 Logo Texo S.A.S. (www.logotexo.com)
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_CONTROL_H_
#define _WX_WASM_CONTROL_H_

class WXDLLIMPEXP_FWD_CORE wxControlRenderer;
class WXDLLIMPEXP_FWD_CORE wxInputHandler;
class WXDLLIMPEXP_FWD_CORE wxRenderer;

// ----------------------------------------------------------------------------
// wxControlAction: the action is currently just a string which identifies it,
// later it might become an atom (i.e. an opaque handler to string).
// ----------------------------------------------------------------------------

typedef wxString wxControlAction;

// the list of actions which apply to all controls (other actions are defined
// in the controls headers)

#define wxACTION_NONE    wxT("")           // no action to perform

// ----------------------------------------------------------------------------
// wxControl: the base class for all GUI controls
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxControl : public wxControlBase
{
public:
    wxControl() { Init(); }

    wxControl(wxWindow *parent,
              wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxASCII_STR(wxControlNameStr))
    {
        Init();

        Create(parent, id, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxASCII_STR(wxControlNameStr));

    // this function will filter out '&' characters and will put the
    // accelerator char (the one immediately after '&') into m_chAccel
    virtual void SetLabel(const wxString& label) wxOVERRIDE;

    // return the current label
    virtual wxString GetLabel() const wxOVERRIDE { return wxControlBase::GetLabel(); }

    // wxUniversal-specific methods

    // return the index of the accel char in the label or -1 if none
    int GetAccelIndex() const { return m_indexAccel; }

    // return the accel char itself or 0 if none
    wxChar GetAccelChar() const
    {
        return m_indexAccel == -1 ? wxT('\0') : (wxChar)m_label[m_indexAccel];
    }

protected:
    // common part of all ctors
    void Init();

private:
    // label and accel info
    wxString   m_label;
    int        m_indexAccel;

    wxDECLARE_DYNAMIC_CLASS(wxControl);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_WASM_CONTROL_H_
