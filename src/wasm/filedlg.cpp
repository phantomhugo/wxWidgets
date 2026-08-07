/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/filedlg.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/filedlg.h"
#include "wx/arrstr.h"
#include <emscripten.h>

// ----------------------------------------------------------------------------
// wxFileDialog for the wasm port
//
// Browser sandbox limitation: a web page has no free access to the user file
// system, so this dialog cannot behave exactly like the native one:
//
//  - wxFD_OPEN/wxFD_FILE_MUST_EXIST: the browser's own file picker is used
//    (an <input type="file"> clicked programmatically) and the chosen files
//    are copied into the Emscripten MEMFS under "/tmp", so that they can be
//    opened afterwards with wxFile/wxFFile as usual.
//  - wxFD_SAVE: "saving" means triggering a browser download with the
//    default file name. The browser decides the final location and handles
//    overwriting itself, so wxFD_OVERWRITE_PROMPT cannot be honored and
//    success is always reported.
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxFileDialog, wxDialog);

// Convert a wx wildcard (e.g. "BMP files (*.bmp)|*.bmp|JPG files|*.jpeg;*.jpg")
// to the comma separated list used by the HTML "accept" attribute
// (".bmp,.jpg,.jpeg"). If filterIndex selects a valid filter, only its
// patterns are used, as "accept" cannot express several named filters.
static wxString wxWasmWildcardToAccept(const wxString& wildCard, int filterIndex)
{
    const wxArrayString tokens = wxSplit(wildCard, '|');

    // Patterns are the odd fields of the description|pattern pairs.
    wxString patterns;
    for (size_t i = 1, filter = 0; i < tokens.size(); i += 2, filter++)
    {
        if (filterIndex >= 0 && (int)filter != filterIndex)
            continue;

        if (!patterns.empty())
            patterns += ';';
        patterns += tokens[i];
    }

    wxString accept;
    const wxArrayString exts = wxSplit(patterns, ';');
    for (size_t i = 0; i < exts.size(); i++)
    {
        wxString ext = exts[i];
        ext.Trim(true);
        ext.Trim(false);

        if (ext.empty() || ext == "*" || ext == "*.*")
            continue;   // All files: no restriction needed.

        if (!accept.empty())
            accept += ',';

        if (ext.StartsWith("*."))
            accept += '.' + ext.Mid(2);
        else if (ext.StartsWith("*"))
            accept += '.' + ext.Mid(1);
        else if (ext.find('.') == wxString::npos)
            accept += '.' + ext;
        else
            accept += ext;
    }

    return accept;
}

// Read a string from the result object published by the JS picker in
// window.wxFilePickerResult_<key>.
static wxString wxWasmGetPickerResultString(int key, const char* field, int index)
{
    char* str = (char*)EM_ASM_INT({
        var result = window['wxFilePickerResult_' + $0];
        if (!result) return 0;
        var s = result[UTF8ToString($1)][$2];
        if (!s) return 0;
        var len = lengthBytesUTF8(s) + 1;
        var buf = _malloc(len);
        stringToUTF8(s, buf, len);
        return buf;
    }, key, field, index);

    if (!str)
        return wxString();

    wxString result = wxString::FromUTF8(str);
    free(str);
    return result;
}

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFile,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
{
    Create(parent, message, defaultDir, defaultFile,
           wildCard, style, pos, sz, name);
}

bool wxFileDialog::Create(wxWindow *parent,
                          const wxString& message,
                          const wxString& defaultDir,
                          const wxString& defaultFile,
                          const wxString& wildCard,
                          long style,
                          const wxPoint& pos,
                          const wxSize& sz,
                          const wxString& name)
{
    // No wx window is needed for the native browser file picker, the base
    // class Create() just stores the parameters.
    return wxFileDialogBase::Create(parent, message, defaultDir, defaultFile,
                                    wildCard, style, pos, sz, name);
}

int wxFileDialog::ShowModal()
{
    // The result of the asynchronous browser picker is published by JS in
    // window.wxFilePickerResult_<key>; use this dialog address as key.
    const int key = (int)(size_t)this;

    if (HasFdFlag(wxFD_SAVE))
    {
        wxString fileName = GetFilename();
        if (fileName.empty())
            fileName = "download";

        // Contents of the file being "saved": take them from the MEMFS file
        // given by the current path if it exists, otherwise save an empty
        // file.
        const wxString sourcePath = !m_path.empty()
                                        ? m_path
                                        : GetDirectory() + "/" + fileName;

        wxCharBuffer nameBuffer = fileName.ToUTF8();
        wxCharBuffer pathBuffer = sourcePath.ToUTF8();

        EM_ASM_({
            var resultKey = 'wxFilePickerResult_' + $0;
            var name = UTF8ToString($1);
            var path = UTF8ToString($2);

            var data;
            try {
                data = path ? FS.readFile(path) : new Uint8Array(0);
            } catch (e) {
                data = new Uint8Array(0);
            }

            // "Saving" in the browser sandbox means downloading a Blob: the
            // browser chooses the destination and asks about overwriting on
            // its own, so wxFD_OVERWRITE_PROMPT cannot be honored here.
            var blob = new Blob([data]);
            var a = document.createElement('a');
            a.href = URL.createObjectURL(blob);
            a.download = name;
            document.body.appendChild(a);
            a.click();
            setTimeout(function() {
                URL.revokeObjectURL(a.href);
                a.remove();
            }, 100);

            // Note: EM_ASM_ code cannot contain commas outside parentheses,
            // so the result object is built property by property.
            var saveResult = {};
            saveResult.status = 'ok';
            saveResult.paths = [path];
            saveResult.names = [name];
            window[resultKey] = saveResult;
        }, key, nameBuffer.data(), pathBuffer.data());

        m_path = sourcePath;
        m_fileName = fileName;
        m_dir = GetDirectory();
        m_paths.Clear();
        m_paths.Add(m_path);
        m_fileNames.Clear();
        m_fileNames.Add(m_fileName);

        EM_ASM_({
            delete window['wxFilePickerResult_' + $0];
        }, key);

        return wxID_OK;
    }

    // wxFD_OPEN: show the browser file picker.
    const wxString accept = wxWasmWildcardToAccept(m_wildCard, m_filterIndex);
    wxCharBuffer acceptBuffer = accept.ToUTF8();
    const int multiple = HasFdFlag(wxFD_MULTIPLE) ? 1 : 0;

    EM_ASM_({
        var resultKey = 'wxFilePickerResult_' + $0;
        var accept = UTF8ToString($1);
        var multiple = $2;

        window[resultKey] = null;

        var input = document.createElement('input');
        input.type = 'file';
        input.style.display = 'none';
        if (accept)
            input.accept = accept;
        if (multiple)
            input.multiple = true;
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

            // Note: EM_ASM_ code cannot contain commas outside parentheses,
            // so the result object is built property by property.
            var result = {};
            result.status = 'ok';
            result.paths = [];
            result.names = [];
            result.paths.length = files.length;
            result.names.length = files.length;
            var remaining = files.length;

            for (var i = 0; i < files.length; i++) {
                (function(file, index) {
                    var reader = new FileReader();
                    reader.onload = function(e) {
                        // Copy the file into the Emscripten MEMFS so it can
                        // be opened later with wxFile/wxFFile.
                        var path = '/tmp/' + file.name;
                        try {
                            FS.writeFile(path, new Uint8Array(e.target.result));
                        } catch (err) {
                        }
                        result.paths[index] = path;
                        result.names[index] = file.name;
                        if (--remaining === 0)
                            finish(result);
                    };
                    reader.onerror = function() {
                        result.paths[index] = "";
                        result.names[index] = file.name;
                        if (--remaining === 0)
                            finish(result);
                    };
                    reader.readAsArrayBuffer(file);
                })(files[i], i);
            }
        });

        // Recent browsers fire "cancel" on the input itself. As a fallback
        // for the ones that don't, treat "window regained focus and no
        // change event shortly after" as a cancellation. This heuristic can
        // in theory misfire if the picker is closed and reopened very
        // quickly, but it is the best a web page can do.
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
    }, key, acceptBuffer.data(), multiple);

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

    const int count = EM_ASM_INT({
        var result = window['wxFilePickerResult_' + $0];
        return result ? result.paths.length : 0;
    }, key);

    wxArrayString paths, names;
    for (int i = 0; i < count; i++)
    {
        const wxString path = wxWasmGetPickerResultString(key, "paths", i);
        const wxString name = wxWasmGetPickerResultString(key, "names", i);
        if (path.empty())
            continue;

        paths.Add(path);
        names.Add(name);
    }

    EM_ASM_({
        delete window['wxFilePickerResult_' + $0];
    }, key);

    if (!accepted || paths.empty())
        return wxID_CANCEL;

    m_paths = paths;
    m_fileNames = names;
    m_path = paths[0];
    m_fileName = names[0];
    m_dir = "/tmp";

    return wxID_OK;
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    if (HasFdFlag(wxFD_MULTIPLE))
    {
        paths = m_paths;
        return;
    }

    wxFileDialogBase::GetPaths(paths);
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    if (HasFdFlag(wxFD_MULTIPLE))
    {
        files = m_fileNames;
        return;
    }

    wxFileDialogBase::GetFilenames(files);
}
