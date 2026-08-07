/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/msgdlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MSGDLG

#include "wx/modalhook.h"
#include "wx/msgdlg.h"
#include "wx/wasm/cssstyles.h"
#include <emscripten.h>

// The result of each modal dialog is published by JS in
// window.wxMsgDlgResult_<key>, with a key derived from the dialog address,
// so that nested modal dialogs don't overwrite each other's state.

// Helper function to get the icon for the style
static const char* GetIconForStyle(long style)
{
    if (style & wxICON_ERROR)
        return "⚠️";  // Or use an SVG icon/emoji
    else if (style & wxICON_WARNING)
        return "⚠️";
    else if (style & wxICON_QUESTION)
        return "❓";
    else if (style & wxICON_INFORMATION)
        return "ℹ️";
    else if (style & wxICON_EXCLAMATION)
        return "⚠️";
    else if (style & wxICON_HAND)
        return "🛑";
    else if (style & wxICON_AUTH_NEEDED)
        return "🔒";
    else
        return "";  // No icon
}

// Helper function to get the CSS class of the icon
static const char* GetIconClass(long style)
{
    if (style & wxICON_ERROR)
        return "wxMessageDialog-icon-error";
    else if (style & wxICON_WARNING)
        return "wxMessageDialog-icon-warning";
    else if (style & wxICON_QUESTION)
        return "wxMessageDialog-icon-question";
    else if (style & wxICON_INFORMATION)
        return "wxMessageDialog-icon-info";
    else
        return "";
}

// Helper function to get the button labels
wxString wxMessageDialog::GetButtonLabel(int id, const wxString& customLabel)
{
    if (!customLabel.empty())
        return customLabel;
    
    switch (id)
    {
        case wxID_OK:
            return wxGetStockLabel(wxID_OK, wxSTOCK_NOFLAGS);
        case wxID_CANCEL:
            return wxGetStockLabel(wxID_CANCEL, wxSTOCK_NOFLAGS);
        case wxID_YES:
            return wxGetStockLabel(wxID_YES, wxSTOCK_NOFLAGS);
        case wxID_NO:
            return wxGetStockLabel(wxID_NO, wxSTOCK_NOFLAGS);
        case wxID_HELP:
            return wxGetStockLabel(wxID_HELP, wxSTOCK_NOFLAGS);
        case wxID_ABORT:
            return wxGetStockLabel(wxID_ABORT, wxSTOCK_NOFLAGS);
        case wxID_RETRY:
            return wxGetStockLabel(wxID_RETRY, wxSTOCK_NOFLAGS);
        case wxID_IGNORE:
            return wxGetStockLabel(wxID_IGNORE, wxSTOCK_NOFLAGS);
        default:
            return wxT("Button");
    }
}

wxMessageDialog::wxMessageDialog(wxWindow *parent, const wxString& message,
        const wxString& caption, long style, const wxPoint& pos )
    : wxMessageDialogBase( parent, message, caption, style )
{
    // PostCreation() needs a valid unique id for the DOM element: the base
    // ctor leaves it at wxID_ANY (-1), which would collide in the DOM with
    // other id-less elements (e.g. the menubar).
    SetId(wxNewId());

    PostCreation();

    if (pos != wxDefaultPosition)
        Centre(wxBOTH | wxCENTER_FRAME);
}

wxIMPLEMENT_CLASS(wxMessageDialog, wxDialog);

wxMessageDialog::~wxMessageDialog()
{
    // Remove the dialog from the DOM if it still exists
    EM_ASM_({
        var dialog = document.getElementById('wxMsgDlg_' + $0);
        if (dialog) {
            // Make sure to close it first
            if (dialog.open) {
                dialog.close();
            }
            dialog.remove();
        }
    }, GetId());
}

int wxMessageDialog::ShowModal()
{
    wxCHECK_MSG(!GetMessage().empty(), wxID_CANCEL, "Message box must have a non-empty message");

    // The dialog result is published by JS in window.wxMsgDlgResult_<key>;
    // use this dialog address as key.
    const int key = (int)(size_t)this;

    // Create the dialog in the DOM
    CreateDialogElement();

    // Show the modal dialog using HTML5 <dialog>
    EM_ASM_({
        var dialog = document.getElementById('wxMsgDlg_' + $0);
        if (dialog && dialog.showModal) {
            dialog.showModal();
        }
    }, GetId());

    // Synchronous wait: yield to the browser until the dialog is closed
    while (EM_ASM_INT({
        return window['wxMsgDlgResult_' + $0] === undefined ? 0 : 1;
    }, key) == 0)
    {
        emscripten_sleep(10);
    }

    const int returnCode = EM_ASM_INT({
        return window['wxMsgDlgResult_' + $0];
    }, key);

    EM_ASM_({
        delete window['wxMsgDlgResult_' + $0];
    }, key);

    // Remove the dialog from the DOM
    EM_ASM_({
        var dialog = document.getElementById('wxMsgDlg_' + $0);
        if (dialog) {
            dialog.remove();
        }
    }, GetId());

    return returnCode;
}

void wxMessageDialog::CreateDialogElement()
{
    wxString message = GetMessage();
    wxString extendedMessage = GetExtendedMessage();
    wxString caption = GetCaption();
    long style = GetMessageDialogStyle();
    
    wxCharBuffer msgBuffer = message.ToUTF8();
    wxCharBuffer extMsgBuffer = extendedMessage.ToUTF8();
    wxCharBuffer capBuffer = caption.ToUTF8();
    
    const char* iconChar = GetIconForStyle(style);
    const char* iconClass = GetIconClass(style);
    
    // Determine which buttons to create
    bool hasYesNo = (style & wxYES_NO) != 0;
    bool hasOk = (style & wxOK) != 0;
    bool hasCancel = (style & wxCANCEL) != 0;
    bool hasHelp = (style & wxHELP) != 0;
    
    // Determine the default button
    int defaultButton = wxID_OK;
    if (style & wxNO_DEFAULT)
        defaultButton = wxID_NO;
    else if (style & wxCANCEL_DEFAULT)
        defaultButton = wxID_CANCEL;
    else if (hasYesNo)
        defaultButton = wxID_YES;  // YES is the default when there is YES_NO

    // EM_ASM is limited to 16 arguments by Emscripten (create_asm_consts
    // only scans $0..$15), so the button ids and the result key travel in a
    // window global set up before the main EM_ASM below.
    const int key = (int)(size_t)this;
    EM_ASM_({
        var codes = {};
        codes.resultKey = $1;
        codes.yes = $2;
        codes.no = $3;
        codes.cancel = $4;
        codes.ok = $5;
        codes.help = $6;
        window['wxMsgDlgCodes_' + $0] = codes;
    }, GetId(), key,
       (int)wxID_YES, (int)wxID_NO, (int)wxID_CANCEL,
       (int)wxID_OK, (int)wxID_HELP);

    const int buttonFlags = (hasYesNo ? 1 : 0) | (hasOk ? 2 : 0) |
                            (hasCancel ? 4 : 0) | (hasHelp ? 8 : 0);
    
    EM_ASM_({
        var dialogId = $0;
        // NB: the window div created by PostCreation() already uses the bare
        // numeric id, so the <dialog> element itself lives in a namespaced
        // id to keep getElementById() unambiguous.
        var dialogDomId = 'wxMsgDlg_' + dialogId;
        var message = UTF8ToString($1);
        var extendedMessage = UTF8ToString($2);
        var caption = UTF8ToString($3);
        var iconChar = UTF8ToString($4);
        var iconClass = UTF8ToString($5);
        var hasYesNo = ($6 & 1) !== 0;
        var hasOk = ($6 & 2) !== 0;
        var hasCancel = ($6 & 4) !== 0;
        var hasHelp = ($6 & 8) !== 0;
        var defaultButton = $7;
        // Codes and result key travel in a window global (EM_ASM argument
        // limit), see the C++ side.
        var codes = window['wxMsgDlgCodes_' + dialogId];
        var resultKey = codes.resultKey;
        var yesCode = codes.yes;
        var noCode = codes.no;
        var cancelCode = codes.cancel;
        var okCode = codes.ok;
        var helpCode = codes.help;

        // Create <dialog> element
        var dialog = document.createElement('dialog');
        dialog.id = dialogDomId;
        dialog.className = 'wxMessageDialog';

        // Create internal structure
        var content = document.createElement('div');
        content.className = 'wxMessageDialog-content';

        // Header with title if it is not the default
        if (caption && caption !== 'Message') {
            var header = document.createElement('div');
            header.className = 'wxMessageDialog-header';
            header.textContent = caption;
            content.appendChild(header);
        }

        // Message area
        var messageArea = document.createElement('div');
        messageArea.className = 'wxMessageDialog-message-area';

        // Icon
        if (iconChar) {
            var iconElem = document.createElement('div');
            iconElem.className = 'wxMessageDialog-icon';
            if (iconClass) iconElem.classList.add(iconClass);
            iconElem.textContent = iconChar;
            messageArea.appendChild(iconElem);
        }

        // Text container
        var textContainer = document.createElement('div');
        textContainer.className = 'wxMessageDialog-text';

        // Main message
        var mainMsg = document.createElement('div');
        mainMsg.className = 'wxMessageDialog-main-message';
        mainMsg.textContent = message;
        textContainer.appendChild(mainMsg);

        // Extended message if any
        if (extendedMessage) {
            var extMsg = document.createElement('div');
            extMsg.className = 'wxMessageDialog-extended-message';
            extMsg.textContent = extendedMessage;
            textContainer.appendChild(extMsg);
        }

        messageArea.appendChild(textContainer);
        content.appendChild(messageArea);

        // Buttons
        var buttonArea = document.createElement('div');
        buttonArea.className = 'wxMessageDialog-buttons';

        // Publish the return code of this dialog instance and close it
        var settled = false;
        function finish(returnCode) {
            if (settled) return;
            settled = true;
            Module.ccall('SetDialogReturnCode', null,
                         (['number', 'number']), ([resultKey, returnCode]));
            dialog.close();
        }

        // Helper to create a button
        function createButton(id, label, isDefault) {
            var btn = document.createElement('button');
            btn.className = 'wxMessageDialog-button';
            btn.textContent = label;
            btn.dataset.returnCode = id;

            if (isDefault) {
                btn.classList.add('wxMessageDialog-button-default');
                btn.autofocus = true;
            }

            btn.onclick = function() {
                finish(id);
            };

            return btn;
        }

        // Create buttons according to the style
        if (hasYesNo) {
            var yesBtn = createButton(yesCode, UTF8ToString($8), defaultButton === yesCode);
            var noBtn = createButton(noCode, UTF8ToString($9), defaultButton === noCode);
            buttonArea.appendChild(noBtn);  // NO on the left
            buttonArea.appendChild(yesBtn); // YES on the right (standard)

            if (hasCancel) {
                var cancelBtn = createButton(cancelCode, UTF8ToString($10), defaultButton === cancelCode);
                buttonArea.appendChild(cancelBtn);
            }
        } else if (hasOk) {
            var okBtn = createButton(okCode, UTF8ToString($11), defaultButton === okCode);
            buttonArea.appendChild(okBtn);

            if (hasCancel) {
                var cancelBtn = createButton(cancelCode, UTF8ToString($10), defaultButton === cancelCode);
                buttonArea.insertBefore(cancelBtn, okBtn);
            }
        }

        if (hasHelp) {
            var helpBtn = createButton(helpCode, UTF8ToString($12), false);
            helpBtn.classList.add('wxMessageDialog-button-help');
            // Help goes on the left
            buttonArea.insertBefore(helpBtn, buttonArea.firstChild);
        }

        content.appendChild(buttonArea);
        dialog.appendChild(content);

        // Close event - when closed without a button (ESC)
        dialog.addEventListener('close', function() {
            finish(cancelCode);
        });

        // Cancel event (ESC pressed)
        dialog.addEventListener('cancel', function(e) {
            e.preventDefault();
            finish(cancelCode);
        });

        document.body.appendChild(dialog);
        delete window['wxMsgDlgCodes_' + dialogId];

    },
    GetId(),
    msgBuffer.data(),
    extMsgBuffer.data(),
    capBuffer.data(),
    iconChar,
    iconClass,
    buttonFlags,
    defaultButton,
    GetButtonLabel(wxID_YES, GetCustomYesLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_NO, GetCustomNoLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_CANCEL, GetCustomCancelLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_OK, GetCustomOKLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_HELP, GetCustomHelpLabel()).ToUTF8().data()
    );
}

// Function exported to JavaScript: publishes the result of the dialog
// identified by key in window.wxMsgDlgResult_<key>
extern "C" EMSCRIPTEN_KEEPALIVE void SetDialogReturnCode(int key, int code)
{
    EM_ASM_({
        window['wxMsgDlgResult_' + $0] = $1;
    }, key, code);
}

#endif // wxUSE_MSGDLG
