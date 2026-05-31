/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/msgdlg.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart, Hugo Castellanos
// Copyright:   (c) 2010-2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MSGDLG

#include "wx/modalhook.h"
#include "wx/msgdlg.h"
#include "wx/wasm/cssstyles.h"
#include <emscripten.h>

// Variable estática para almacenar el resultado del diálogo
static int gs_dialogReturnCode = wxID_CANCEL;
static bool gs_dialogClosed = false;

// Función auxiliar para obtener el ícono según el estilo
static const char* GetIconForStyle(long style)
{
    if (style & wxICON_ERROR)
        return "⚠️";  // O usar un icono SVG/emojis
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
        return "";  // Sin icono
}

// Función auxiliar para obtener la clase CSS del icono
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

// Función auxiliar para obtener las etiquetas de botones
static wxString GetButtonLabel(int id, const wxString& customLabel)
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
    PostCreation();

    if (pos != wxDefaultPosition)
        Centre(wxBOTH | wxCENTER_FRAME);
}

wxIMPLEMENT_CLASS(wxMessageDialog, wxDialog);

wxMessageDialog::~wxMessageDialog()
{
    // Limpiar el diálogo del DOM si aún existe
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog) {
            // Asegurarse de cerrarlo primero
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

    // Crear el diálogo en el DOM
    CreateDialogElement();

    // Resetear estado
    gs_dialogClosed = false;
    gs_dialogReturnCode = wxID_CANCEL;

    // Mostrar el diálogo modal usando HTML5 <dialog>
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog && dialog.showModal) {
            dialog.showModal();
        }
    }, GetId());

    // Bucle de eventos síncrono para esperar el cierre del diálogo
    // Emscripten requiere ceder el control periódicamente
    while (!gs_dialogClosed)
    {
        // Procesar eventos pendientes
        emscripten_sleep(10);  // Dormir 10ms, permite que el navegador procese eventos
    }

    // Limpiar el diálogo del DOM
    EM_ASM_({
        var dialog = document.getElementById($0);
        if (dialog) {
            dialog.remove();
        }
    }, GetId());

    return gs_dialogReturnCode;
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
    
    // Determinar qué botones crear
    bool hasYesNo = (style & wxYES_NO) != 0;
    bool hasOk = (style & wxOK) != 0;
    bool hasCancel = (style & wxCANCEL) != 0;
    bool hasHelp = (style & wxHELP) != 0;
    
    // Determinar el botón por defecto
    int defaultButton = wxID_OK;
    if (style & wxNO_DEFAULT)
        defaultButton = wxID_NO;
    else if (style & wxCANCEL_DEFAULT)
        defaultButton = wxID_CANCEL;
    else if (hasYesNo)
        defaultButton = wxID_YES;  // YES es default por defecto cuando hay YES_NO
    
    EM_ASM_({
        var dialogId = $0;
        var message = UTF8ToString($1);
        var extendedMessage = UTF8ToString($2);
        var caption = UTF8ToString($3);
        var iconChar = UTF8ToString($4);
        var iconClass = UTF8ToString($5);
        var hasYesNo = $6;
        var hasOk = $7;
        var hasCancel = $8;
        var hasHelp = $9;
        var defaultButton = $10;
        
        // Crear elemento <dialog>
        var dialog = document.createElement('dialog');
        dialog.id = dialogId;
        dialog.className = 'wxMessageDialog';
        
        // Crear estructura interna
        var content = document.createElement('div');
        content.className = 'wxMessageDialog-content';
        
        // Header con título si no es el default
        if (caption && caption !== 'Message') {
            var header = document.createElement('div');
            header.className = 'wxMessageDialog-header';
            header.textContent = caption;
            content.appendChild(header);
        }
        
        // Área del mensaje
        var messageArea = document.createElement('div');
        messageArea.className = 'wxMessageDialog-message-area';
        
        // Icono
        if (iconChar) {
            var iconElem = document.createElement('div');
            iconElem.className = 'wxMessageDialog-icon';
            if (iconClass) iconElem.classList.add(iconClass);
            iconElem.textContent = iconChar;
            messageArea.appendChild(iconElem);
        }
        
        // Contenedor de textos
        var textContainer = document.createElement('div');
        textContainer.className = 'wxMessageDialog-text';
        
        // Mensaje principal
        var mainMsg = document.createElement('div');
        mainMsg.className = 'wxMessageDialog-main-message';
        mainMsg.textContent = message;
        textContainer.appendChild(mainMsg);
        
        // Mensaje extendido si existe
        if (extendedMessage) {
            var extMsg = document.createElement('div');
            extMsg.className = 'wxMessageDialog-extended-message';
            extMsg.textContent = extendedMessage;
            textContainer.appendChild(extMsg);
        }
        
        messageArea.appendChild(textContainer);
        content.appendChild(messageArea);
        
        // Botones
        var buttonArea = document.createElement('div');
        buttonArea.className = 'wxMessageDialog-buttons';
        
        // Helper para crear botón
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
                // Establecer código de retorno
                Module.ccall('SetDialogReturnCode', null, ['number'], [id]);
                dialog.close();
            };
            
            return btn;
        }
        
        // Crear botones según el estilo
        if (hasYesNo) {
            var yesBtn = createButton(wxID_YES, UTF8ToString($11), defaultButton === wxID_YES);
            var noBtn = createButton(wxID_NO, UTF8ToString($12), defaultButton === wxID_NO);
            buttonArea.appendChild(noBtn);  // NO a la izquierda
            buttonArea.appendChild(yesBtn); // YES a la derecha (estándar)
            
            if (hasCancel) {
                var cancelBtn = createButton(wxID_CANCEL, UTF8ToString($13), defaultButton === wxID_CANCEL);
                buttonArea.appendChild(cancelBtn);
            }
        } else if (hasOk) {
            var okBtn = createButton(wxID_OK, UTF8ToString($14), defaultButton === wxID_OK);
            buttonArea.appendChild(okBtn);
            
            if (hasCancel) {
                var cancelBtn = createButton(wxID_CANCEL, UTF8ToString($13), defaultButton === wxID_CANCEL);
                buttonArea.insertBefore(cancelBtn, okBtn);
            }
        }
        
        if (hasHelp) {
            var helpBtn = createButton(wxID_HELP, UTF8ToString($15), false);
            helpBtn.classList.add('wxMessageDialog-button-help');
            // Help va a la izquierda
            buttonArea.insertBefore(helpBtn, buttonArea.firstChild);
        }
        
        content.appendChild(buttonArea);
        dialog.appendChild(content);
        
        // Evento close - cuando se cierra sin botón (ESC)
        dialog.addEventListener('close', function() {
            if (!Module.dialogReturnCodeSet) {
                Module.ccall('SetDialogReturnCode', null, ['number'], [wxID_CANCEL]);
            }
        });
        
        // Evento cancel (ESC presionado)
        dialog.addEventListener('cancel', function(e) {
            e.preventDefault();
            Module.ccall('SetDialogReturnCode', null, ['number'], [wxID_CANCEL]);
            dialog.close();
        });
        
        document.body.appendChild(dialog);
        Module.dialogReturnCodeSet = false;
        
    }, 
    GetId(),
    msgBuffer.data(),
    extMsgBuffer.data(),
    capBuffer.data(),
    iconChar,
    iconClass,
    hasYesNo ? 1 : 0,
    hasOk ? 1 : 0,
    hasCancel ? 1 : 0,
    hasHelp ? 1 : 0,
    defaultButton,
    GetButtonLabel(wxID_YES, GetCustomYesLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_NO, GetCustomNoLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_CANCEL, GetCustomCancelLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_OK, GetCustomOKLabel()).ToUTF8().data(),
    GetButtonLabel(wxID_HELP, GetCustomHelpLabel()).ToUTF8().data()
    );
}

// Función C expuesta para JavaScript
extern "C" EMSCRIPTEN_KEEPALIVE void SetDialogReturnCode(int code)
{
    gs_dialogReturnCode = code;
    gs_dialogClosed = true;
    
    EM_ASM({
        Module.dialogReturnCodeSet = true;
    });
}

#endif // wxUSE_MSGDLG
