/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/colordlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_COLOURDLG

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/colordlg.h"
#include "wx/stockitem.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog);

// The result of each modal dialog is published by JS in
// window.wxColourDlgResult_<key>, with a key derived from the dialog
// address, so that nested modal dialogs don't overwrite each other's state.
// Browsers can't enumerate system colour pickers, so the dialog is built
// around the native <input type="color"> control. wxColourData::ChooseFull
// and the custom colours are ignored: the HTML control doesn't expose them.

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data )
{
    if (data)
        m_data = *data;

    m_parent = GetParentForModalDialog(parent, 0);

    if (GetId() == wxID_ANY)
        SetId(wxWindow::NewControlId());

    return true;
}

int wxColourDialog::ShowModal()
{
    wxCHECK_MSG(!IsModal(), wxID_CANCEL,
                "ShowModal() can't be called twice");

    // The dialog result is published by JS in
    // window.wxColourDlgResult_<key>; use this dialog address as key.
    const int key = (int)(size_t)this;

    // Initial colour in #rrggbb format for the HTML colour input
    wxString initialColour = "#000000";
    const wxColour& col = m_data.GetColour();
    if (col.IsOk())
    {
        initialColour = wxString::Format("#%02x%02x%02x",
                                         col.Red(), col.Green(), col.Blue());
    }

    wxCharBuffer titleBuffer = wxString(_("Choose colour")).ToUTF8();
    wxCharBuffer colourBuffer = initialColour.ToUTF8();
    wxCharBuffer okBuffer = wxGetStockLabel(wxID_OK, wxSTOCK_NOFLAGS).ToUTF8();
    wxCharBuffer cancelBuffer = wxGetStockLabel(wxID_CANCEL, wxSTOCK_NOFLAGS).ToUTF8();

    // Build the <dialog> element with a native colour input and OK/Cancel
    EM_ASM_({
        var dialogId = $0;
        var title = UTF8ToString($1);
        var initial = UTF8ToString($2);
        var okLabel = UTF8ToString($3);
        var cancelLabel = UTF8ToString($4);
        var okCode = $5;
        var cancelCode = $6;
        var resultKey = $7;

        // Remove a previous element with the same id if it still exists
        var old = document.getElementById(dialogId);
        if (old) {
            old.remove();
        }

        var dialog = document.createElement('dialog');
        dialog.id = dialogId;
        dialog.className = 'wxColourDialog';

        var content = document.createElement('div');
        content.className = 'wxColourDialog-content';

        var header = document.createElement('div');
        header.className = 'wxColourDialog-header';
        header.textContent = title;
        content.appendChild(header);

        var pickerArea = document.createElement('div');
        pickerArea.className = 'wxColourDialog-picker';

        var input = document.createElement('input');
        input.type = 'color';
        input.className = 'wxColourDialog-input';
        input.value = /^#[0-9a-fA-F]{6}$/.test(initial) ? initial : '#000000';
        pickerArea.appendChild(input);
        content.appendChild(pickerArea);

        var buttonArea = document.createElement('div');
        buttonArea.className = 'wxColourDialog-buttons';

        var settled = false;
        function finish(returnCode) {
            if (settled) return;
            settled = true;
            var value = (returnCode === okCode) ? input.value : "";
            Module.ccall('wxColourDialogSetResult', null,
                         (['number', 'number', 'string']),
                         ([resultKey, returnCode, value]));
            dialog.close();
        }

        var cancelBtn = document.createElement('button');
        cancelBtn.className = 'wxColourDialog-button';
        cancelBtn.textContent = cancelLabel;
        cancelBtn.onclick = function() { finish(cancelCode); };
        buttonArea.appendChild(cancelBtn);

        var okBtn = document.createElement('button');
        okBtn.className = 'wxColourDialog-button wxColourDialog-button-default';
        okBtn.textContent = okLabel;
        okBtn.autofocus = true;
        okBtn.onclick = function() { finish(okCode); };
        buttonArea.appendChild(okBtn);

        content.appendChild(buttonArea);
        dialog.appendChild(content);

        // ESC pressed: treat as Cancel
        dialog.addEventListener('cancel', function(e) {
            e.preventDefault();
            finish(cancelCode);
        });

        document.body.appendChild(dialog);

        if (dialog.showModal) {
            dialog.showModal();
        }
    },
    GetId(),
    titleBuffer.data(),
    colourBuffer.data(),
    okBuffer.data(),
    cancelBuffer.data(),
    (int)wxID_OK,
    (int)wxID_CANCEL,
    key);

    // Synchronous wait: yield to the browser until the dialog is closed
    while (EM_ASM_INT({
        return window['wxColourDlgResult_' + $0] ? 1 : 0;
    }, key) == 0)
    {
        emscripten_sleep(10);
    }

    const int returnCode = EM_ASM_INT({
        return window['wxColourDlgResult_' + $0].returnCode;
    }, key);

    wxString hexValue;
    char* hexStr = (char*)EM_ASM_INT({
        var result = window['wxColourDlgResult_' + $0];
        var s = result ? result.hex : "";
        var len = lengthBytesUTF8(s) + 1;
        var buf = _malloc(len);
        stringToUTF8(s, buf, len);
        return buf;
    }, key);
    if (hexStr)
    {
        hexValue = wxString::FromUTF8(hexStr);
        free(hexStr);
    }

    EM_ASM_({
        delete window['wxColourDlgResult_' + $0];
    }, key);

    // Remove the dialog from the DOM
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog) {
            if (dialog.open) {
                dialog.close();
            }
            dialog.remove();
        }
    }, GetId());

    if (returnCode == wxID_OK)
    {
        // Parse the "#rrggbb" value returned by the colour input
        unsigned long rgb = 0;
        if (hexValue.length() == 7 &&
            hexValue[0] == '#' &&
            hexValue.Mid(1).ToULong(&rgb, 16))
        {
            m_data.SetColour(wxColour((unsigned char)((rgb >> 16) & 0xff),
                                      (unsigned char)((rgb >> 8) & 0xff),
                                      (unsigned char)(rgb & 0xff)));
        }
    }

    SetReturnCode(returnCode);

    return returnCode;
}

wxColourData &wxColourDialog::GetColourData()
{
    return m_data;
}

// Function exported to JavaScript: publishes the result of the dialog
// identified by key in window.wxColourDlgResult_<key>
extern "C" EMSCRIPTEN_KEEPALIVE void wxColourDialogSetResult(int key,
                                                             int returnCode,
                                                             const char *hexColour)
{
    // Note: EM_ASM_ code cannot contain commas outside parentheses,
    // so the result object is built property by property.
    EM_ASM_({
        var result = {};
        result.returnCode = $1;
        result.hex = UTF8ToString($2);
        window['wxColourDlgResult_' + $0] = result;
    }, key, returnCode, hexColour ? hexColour : "");
}

#endif // wxUSE_COLOURDLG
