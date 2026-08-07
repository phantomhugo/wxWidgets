/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dirdlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dirdlg.h"
#include <emscripten.h>

// ----------------------------------------------------------------------------
// wxDirDialog for the wasm port
//
// Browser sandbox limitation: a web page cannot freely browse the user file
// system, so the native directory picker is emulated with an
// <input type="file" webkitdirectory>. The user picks a directory, all its
// files are copied into the Emscripten MEMFS under "/tmp/<dir>" and the
// returned path is that MEMFS location. Empty directories cannot be picked
// at all, as the browser only reports the files inside the selection.
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxDirDialog, wxDialog);

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& pos,
                         const wxSize& size,
                         const wxString& name)
{
    Create(parent, message, defaultPath, style, pos, size, name);
}

bool wxDirDialog::Create(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& WXUNUSED(pos),
                         const wxSize& WXUNUSED(size),
                         const wxString& WXUNUSED(name))
{
    // No wx window is needed for the native browser directory picker, so
    // just store the parameters instead of creating the <dialog> element.
    m_message = message;
    m_path = defaultPath;
    m_parent = parent;
    m_windowStyle = style;

    return true;
}

int wxDirDialog::ShowModal()
{
    // The result of the asynchronous browser picker is published by JS in
    // window.wxFilePickerResult_<key>; use this dialog address as key.
    const int key = (int)(size_t)this;

    EM_ASM_({
        var resultKey = 'wxFilePickerResult_' + $0;

        window[resultKey] = null;

        var input = document.createElement('input');
        input.type = 'file';
        input.style.display = 'none';
        input.setAttribute('webkitdirectory', 'webkitdirectory');
        document.body.appendChild(input);

        var settled = false;
        function finish(result) {
            if (settled)
                return;
            settled = true;
            window[resultKey] = result;
            input.remove();
        }

        input.addEventListener('change', function() {
            var files = input.files;
            if (!files || files.length === 0) {
                finish({ status: 'cancel', paths: [], names: [] });
                return;
            }

            // The common first level of webkitRelativePath is the name of
            // the picked directory.
            var relative = files[0].webkitRelativePath || files[0].name;
            var topDir = relative.split('/')[0];

            // Note: EM_ASM_ code cannot contain commas outside parentheses,
            // so the result object is built property by property.
            var result = {};
            result.status = 'ok';
            result.paths = [];
            result.names = [];
            var remaining = files.length;

            for (var i = 0; i < files.length; i++) {
                (function(file) {
                    var reader = new FileReader();
                    reader.onload = function(e) {
                        // Copy the file into the Emscripten MEMFS preserving
                        // the directory structure below "/tmp/<dir>".
                        var rel = file.webkitRelativePath || file.name;
                        var path = '/tmp/' + rel;
                        try {
                            var slash = path.lastIndexOf('/');
                            FS.createPath('/', path.substring(1, slash), true, true);
                            FS.writeFile(path, new Uint8Array(e.target.result));
                        } catch (err) {
                        }
                        if (--remaining === 0)
                            finish(result);
                    };
                    reader.onerror = function() {
                        if (--remaining === 0)
                            finish(result);
                    };
                    reader.readAsArrayBuffer(file);
                })(files[i]);
            }

            result.paths.push('/tmp/' + topDir);
            result.names.push(topDir);
        });

        // Same cancellation heuristic as in wxFileDialog: recent browsers
        // fire "cancel" on the input; for the others, "window regained
        // focus and no change event shortly after" is treated as a
        // cancellation.
        input.addEventListener('cancel', function() {
            finish({ status: 'cancel', paths: [], names: [] });
        });
        window.addEventListener('focus', function onFocus() {
            window.removeEventListener('focus', onFocus);
            setTimeout(function() {
                finish({ status: 'cancel', paths: [], names: [] });
            }, 300);
        });

        input.click();
    }, key);

    // Wait for the asynchronous browser picker to publish its result.
    // Emscripten requires yielding control periodically.
    while (EM_ASM_INT({
        return window['wxFilePickerResult_' + $0] ? 1 : 0;
    }, key) == 0)
    {
        emscripten_sleep(10);
    }

    const bool accepted = EM_ASM_INT({
        var result = window['wxFilePickerResult_' + $0];
        return result && result.status === 'ok' ? 1 : 0;
    }, key) != 0;

    // Read the resulting MEMFS directory path.
    char* str = (char*)EM_ASM_INT({
        var result = window['wxFilePickerResult_' + $0];
        if (!result || !result.paths.length) return 0;
        var s = result.paths[0];
        var len = lengthBytesUTF8(s) + 1;
        var buf = _malloc(len);
        stringToUTF8(s, buf, len);
        return buf;
    }, key);

    EM_ASM_({
        delete window['wxFilePickerResult_' + $0];
    }, key);

    if (!accepted || !str)
    {
        if (str)
            free(str);
        return wxID_CANCEL;
    }

    m_path = wxString::FromUTF8(str);
    free(str);

    m_paths.Clear();
    m_paths.Add(m_path);

    return wxID_OK;
}
