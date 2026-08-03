/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dialog.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/modalhook.h"
#include "wx/dialog.h"
#include "wx/evtloop.h"
#include "wx/window.h"
#include <emscripten.h>

wxDialog::wxDialog()
    : m_isModal(false),
      m_modalLoop(nullptr)
{
}

wxDialog::wxDialog(wxWindow *parent, wxWindowID id,
                   const wxString &title,
                   const wxPoint &pos,
                   const wxSize &size,
                   long style,
                   const wxString &name)
    : m_isModal(false),
      m_modalLoop(nullptr)
{
    Create(parent, id, title, pos, size, style, name);
}

wxDialog::~wxDialog()
{
    if (IsModal()) {
        EndModal(wxID_CANCEL);
    }

    // Remove the <dialog> element from the DOM if it still exists
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog) {
            if (dialog.open) {
                dialog.close();
            }
            dialog.remove();
        }
    }, GetId());
}

bool wxDialog::Create(wxWindow *parent, wxWindowID id,
                      const wxString &title,
                      const wxPoint &pos,
                      const wxSize &size,
                      long style,
                      const wxString &name)
{
    SetExtraStyle(GetExtraStyle() | wxTOPLEVEL_EX_DIALOG);
    style |= wxTAB_TRAVERSAL;

    if (!wxTopLevelWindow::Create(parent, id, title, pos, size, style, name))
        return false;

    // Replace the <div> created by wxTopLevelWindowWasm::Create() with <dialog>
    EM_ASM_({
        var oldDiv = document.getElementById($0);
        if (oldDiv && oldDiv.tagName !== 'DIALOG') {
            var dialog = document.createElement('dialog');
            dialog.id = $0;
            dialog.className = oldDiv.className + ' wxDialog';
            // Transfer existing children
            while (oldDiv.firstChild) {
                dialog.appendChild(oldDiv.firstChild);
            }
            if (oldDiv.parentNode) {
                oldDiv.parentNode.replaceChild(dialog, oldDiv);
            }
        }

        var dlg = document.getElementById($0);
        if (dlg) {
            dlg.addEventListener('cancel', function(e) {
                e.preventDefault();
                if (typeof Module !== 'undefined' && Module.ccall) {
                    Module.ccall('wxDialogHandleCancel', null, ['number'], [$0]);
                }
            });
        }
    }, GetId());

    return true;
}

int wxDialog::ShowModal()
{
    wxCHECK_MSG(!IsModal(), wxID_CANCEL,
                "ShowModal() can't be called twice");

    // Create the modal loop (wxWindowDisabler is created automatically)
    wxModalEventLoop modalLoop(this);

    m_isModal = true;
    m_modalLoop = &modalLoop;

    // Open the native <dialog>
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog && dialog.showModal) {
            dialog.showModal();
        }
    }, GetId());

    Show(true);

    modalLoop.Run();

    m_isModal = false;
    m_modalLoop = nullptr;

    // Close the native dialog if it is still open
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog && dialog.open) {
            dialog.close();
        }
    }, GetId());

    Show(false);

    return GetReturnCode();
}

void wxDialog::EndModal(int retCode)
{
    SetReturnCode(retCode);

    // Exit the modal loop created by ShowModal(): the currently active
    // loop may be a nested one (timer/idle handlers) and must not be
    // exited here.
    if ( m_modalLoop )
        m_modalLoop->ScheduleExit(GetReturnCode());

    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog && dialog.open) {
            dialog.close();
        }
    }, GetId());
}

bool wxDialog::IsModal() const
{
    return m_isModal;
}

bool wxDialog::Show(bool show)
{
    if (show == IsShown())
        return false;

    if (!show && IsModal())
        EndModal(wxID_CANCEL);

    if (show && CanDoLayoutAdaptation())
        DoLayoutAdaptation();

    const bool ret = wxDialogBase::Show(show);

    if (show)
        InitDialog();

    return ret;
}

void *wxDialog::GetDialogHandle() const
{
    return const_cast<wxDialog*>(this);
}

// C function exposed to JavaScript (ESC handling in <dialog>)
extern "C" EMSCRIPTEN_KEEPALIVE void wxDialogHandleCancel(int dialogId)
{
    wxDialog *dlg = dynamic_cast<wxDialog*>(wxWindow::FindWindowById(dialogId));
    if (dlg && dlg->IsModal()) {
        int escapeId = dlg->GetEscapeId();
        if (escapeId == wxID_NONE) {
            return;
        }
        if (escapeId == wxID_ANY) {
            escapeId = wxID_CANCEL;
        }
        dlg->EndModal(escapeId);
    }
}
