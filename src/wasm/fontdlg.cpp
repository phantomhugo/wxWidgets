/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/fontdlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTDLG

#ifndef WX_PRECOMP
    #include "wx/intl.h"
#endif

#include "wx/fontdlg.h"
#include "wx/stockitem.h"
#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

// The result of each modal dialog is published by JS in
// window.wxFontDlgResult_<key>, with a key derived from the dialog address,
// so that nested modal dialogs don't overwrite each other's state.
// Browser limitation: the system fonts can't be enumerated from JavaScript,
// so the dialog offers the generic CSS font families (serif, sans-serif,
// monospace, cursive, fantasy) plus, on a best-effort basis, the families
// exposed by the document.fonts API (fonts loaded by the page itself).

// Map a generic CSS family name to the closest wxFontFamily value
static wxFontFamily wxFamilyFromCSS(const wxString& name)
{
    if (name == "serif")
        return wxFONTFAMILY_ROMAN;
    if (name == "sans-serif")
        return wxFONTFAMILY_SWISS;
    if (name == "monospace")
        return wxFONTFAMILY_TELETYPE;
    if (name == "cursive")
        return wxFONTFAMILY_SCRIPT;
    if (name == "fantasy")
        return wxFONTFAMILY_DECORATIVE;

    return wxFONTFAMILY_DEFAULT;
}

// Map a wxFontFamily value to its generic CSS family name
static wxString wxFamilyToCSS(wxFontFamily family)
{
    switch (family)
    {
        case wxFONTFAMILY_ROMAN:
            return "serif";
        case wxFONTFAMILY_SWISS:
            return "sans-serif";
        case wxFONTFAMILY_TELETYPE:
            return "monospace";
        case wxFONTFAMILY_SCRIPT:
            return "cursive";
        case wxFONTFAMILY_DECORATIVE:
            return "fantasy";
        default:
            return "sans-serif";
    }
}

bool wxFontDialog::DoCreate(wxWindow *parent)
{
    if (!wxFontDialogBase::DoCreate(parent))
        return false;

    m_parent = GetParentForModalDialog(parent, 0);

    if (GetId() == wxID_ANY)
        SetId(wxWindow::NewControlId());

    return true;
}

int wxFontDialog::ShowModal()
{
    wxCHECK_MSG(!IsModal(), wxID_CANCEL,
                "ShowModal() can't be called twice");

    // The dialog result is published by JS in window.wxFontDlgResult_<key>;
    // use this dialog address as key.
    const int key = (int)(size_t)this;

    // Initial selection, from the initial font if it is valid
    wxString initialFamily = "generic:sans-serif";
    int initialItalic = 0;
    int initialBold = 0;
    int initialSize = 12;

    const wxFont initialFont = m_fontData.GetInitialFont();
    if (initialFont.IsOk())
    {
        if (!initialFont.GetFaceName().empty())
            initialFamily = "face:" + initialFont.GetFaceName();
        else
            initialFamily = "generic:" + wxFamilyToCSS(initialFont.GetFamily());

        initialItalic = initialFont.GetStyle() != wxFONTSTYLE_NORMAL ? 1 : 0;
        initialBold = initialFont.GetNumericWeight() > wxFONTWEIGHT_NORMAL ? 1 : 0;

        int pointSize = wxRound(initialFont.GetFractionalPointSize());
        if (pointSize > 0)
            initialSize = pointSize;
    }

    // Optional size range from wxFontData::SetRange()
    int minSize = m_fontData.m_minSize > 0 ? m_fontData.m_minSize : 1;
    int maxSize = m_fontData.m_maxSize > minSize ? m_fontData.m_maxSize : 100;

    wxCharBuffer titleBuffer = wxString(_("Choose font")).ToUTF8();
    wxCharBuffer familyBuffer = initialFamily.ToUTF8();
    wxCharBuffer okBuffer = wxGetStockLabel(wxID_OK, wxSTOCK_NOFLAGS).ToUTF8();
    wxCharBuffer cancelBuffer = wxGetStockLabel(wxID_CANCEL, wxSTOCK_NOFLAGS).ToUTF8();

    // Build the <dialog> element with the font controls and OK/Cancel
    EM_ASM_({
        var dialogId = $0;
        var title = UTF8ToString($1);
        var initialFamily = UTF8ToString($2);
        var okLabel = UTF8ToString($3);
        var cancelLabel = UTF8ToString($4);
        var initialItalic = $5;
        var initialBold = $6;
        var initialSize = $7;
        var minSize = $8;
        var maxSize = $9;
        var okCode = $10;
        var cancelCode = $11;
        var resultKey = $12;

        // Remove a previous element with the same id if it still exists
        var old = document.getElementById(dialogId);
        if (old) {
            old.remove();
        }

        var dialog = document.createElement('dialog');
        dialog.id = dialogId;
        dialog.className = 'wxFontDialog';

        var content = document.createElement('div');
        content.className = 'wxFontDialog-content';

        var header = document.createElement('div');
        header.className = 'wxFontDialog-header';
        header.textContent = title;
        content.appendChild(header);

        function addRow(labelText, control) {
            var row = document.createElement('div');
            row.className = 'wxFontDialog-row';
            var label = document.createElement('label');
            label.className = 'wxFontDialog-label';
            label.textContent = labelText;
            row.appendChild(label);
            row.appendChild(control);
            content.appendChild(row);
        }

        // Font family: generic CSS families plus document.fonts best effort
        var familySelect = document.createElement('select');
        familySelect.className = 'wxFontDialog-family';
        var genericFamilies = (['serif', 'sans-serif', 'monospace', 'cursive', 'fantasy']);
        genericFamilies.forEach(function(name) {
            var opt = document.createElement('option');
            opt.value = 'generic:' + name;
            opt.textContent = name;
            familySelect.appendChild(opt);
        });
        try {
            if (document.fonts && document.fonts.forEach) {
                var seen = {};
                document.fonts.forEach(function(font) {
                    if (!seen[font.family]) {
                        seen[font.family] = true;
                        var opt = document.createElement('option');
                        opt.value = 'face:' + font.family;
                        opt.textContent = font.family;
                        familySelect.appendChild(opt);
                    }
                });
            }
        } catch (e) {
            // document.fonts not available: keep only the generic families
        }
        familySelect.value = initialFamily;
        if (familySelect.selectedIndex < 0) {
            familySelect.selectedIndex = 0;
        }
        addRow('Family', familySelect);

        // Style
        var styleSelect = document.createElement('select');
        styleSelect.className = 'wxFontDialog-style';
        ([['0', 'normal'], ['1', 'italic']]).forEach(function(pair) {
            var opt = document.createElement('option');
            opt.value = pair[0];
            opt.textContent = pair[1];
            styleSelect.appendChild(opt);
        });
        styleSelect.value = initialItalic ? '1' : '0';
        addRow('Style', styleSelect);

        // Weight
        var weightSelect = document.createElement('select');
        weightSelect.className = 'wxFontDialog-weight';
        ([['0', 'normal'], ['1', 'bold']]).forEach(function(pair) {
            var opt = document.createElement('option');
            opt.value = pair[0];
            opt.textContent = pair[1];
            weightSelect.appendChild(opt);
        });
        weightSelect.value = initialBold ? '1' : '0';
        addRow('Weight', weightSelect);

        // Size in points
        var sizeInput = document.createElement('input');
        sizeInput.type = 'number';
        sizeInput.className = 'wxFontDialog-size';
        sizeInput.min = minSize;
        sizeInput.max = maxSize;
        sizeInput.value = initialSize;
        addRow('Size', sizeInput);

        var buttonArea = document.createElement('div');
        buttonArea.className = 'wxFontDialog-buttons';

        var settled = false;
        function finish(returnCode) {
            if (settled) return;
            settled = true;
            var size = parseInt(sizeInput.value, 10);
            if (isNaN(size)) size = initialSize;
            size = Math.max(minSize, Math.min(maxSize, size));
            Module.ccall('wxFontDialogSetResult', null,
                         (['number', 'number', 'string', 'number', 'number', 'number']),
                         ([resultKey,
                           returnCode,
                           returnCode === okCode ? familySelect.value : "",
                           styleSelect.value === '1' ? 1 : 0,
                           weightSelect.value === '1' ? 1 : 0,
                           size]));
            dialog.close();
        }

        var cancelBtn = document.createElement('button');
        cancelBtn.className = 'wxFontDialog-button';
        cancelBtn.textContent = cancelLabel;
        cancelBtn.onclick = function() { finish(cancelCode); };
        buttonArea.appendChild(cancelBtn);

        var okBtn = document.createElement('button');
        okBtn.className = 'wxFontDialog-button wxFontDialog-button-default';
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
    familyBuffer.data(),
    okBuffer.data(),
    cancelBuffer.data(),
    initialItalic,
    initialBold,
    initialSize,
    minSize,
    maxSize,
    (int)wxID_OK,
    (int)wxID_CANCEL,
    key);

    // Synchronous wait: yield to the browser until the dialog is closed
    while (EM_ASM_INT({
        return window['wxFontDlgResult_' + $0] ? 1 : 0;
    }, key) == 0)
    {
        emscripten_sleep(10);
    }

    const int returnCode = EM_ASM_INT({
        return window['wxFontDlgResult_' + $0].returnCode;
    }, key);
    const int chosenItalic = EM_ASM_INT({
        return window['wxFontDlgResult_' + $0].italic;
    }, key);
    const int chosenBold = EM_ASM_INT({
        return window['wxFontDlgResult_' + $0].bold;
    }, key);
    const int chosenSize = EM_ASM_INT({
        return window['wxFontDlgResult_' + $0].size;
    }, key);

    wxString chosenFamily;
    char* familyStr = (char*)EM_ASM_INT({
        var result = window['wxFontDlgResult_' + $0];
        var s = result ? result.family : "";
        var len = lengthBytesUTF8(s) + 1;
        var buf = _malloc(len);
        stringToUTF8(s, buf, len);
        return buf;
    }, key);
    if (familyStr)
    {
        chosenFamily = wxString::FromUTF8(familyStr);
        free(familyStr);
    }

    EM_ASM_({
        delete window['wxFontDlgResult_' + $0];
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
        wxFontInfo info(chosenSize);

        wxString faceName;
        if (chosenFamily.StartsWith("face:", &faceName) && !faceName.empty())
        {
            info.FaceName(faceName);
        }
        else
        {
            wxString cssName = chosenFamily.AfterFirst(':');
            info.Family(wxFamilyFromCSS(cssName));
        }

        info.Style(chosenItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL);
        info.Weight(chosenBold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);

        m_fontData.SetChosenFont(wxFont(info));
    }

    SetReturnCode(returnCode);

    return returnCode;
}

// Function exported to JavaScript: publishes the result of the dialog
// identified by key in window.wxFontDlgResult_<key>
extern "C" EMSCRIPTEN_KEEPALIVE void wxFontDialogSetResult(int key,
                                                           int returnCode,
                                                           const char *family,
                                                           int italic,
                                                           int bold,
                                                           int pointSize)
{
    // Note: EM_ASM_ code cannot contain commas outside parentheses,
    // so the result object is built property by property.
    EM_ASM_({
        var result = {};
        result.returnCode = $1;
        result.family = UTF8ToString($2);
        result.italic = $3;
        result.bold = $4;
        result.size = $5;
        window['wxFontDlgResult_' + $0] = result;
    }, key, returnCode, family ? family : "", italic, bold, pointSize);
}

#endif // wxUSE_FONTDLG
