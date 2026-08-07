/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/filepicker.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"
#include "wx/filepicker.h"
#include <emscripten.h>

// ============================================================================
// wxFilePickerWidget
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxFilePickerWidget, wxControl);

wxFilePickerWidget::wxFilePickerWidget(wxWindow *parent,
                                       wxWindowID id,
                                       const wxString& label,
                                       const wxString& path,
                                       const wxString& message,
                                       const wxString& wildcard,
                                       const wxPoint& pos,
                                       const wxSize& size,
                                       long style,
                                       const wxValidator& validator,
                                       const wxString& name)
{
    Create(parent, id, label, path, message, wildcard, pos, size, style, validator, name);
}

bool wxFilePickerWidget::Create(wxWindow *parent,
                                wxWindowID id,
                                const wxString& label,
                                const wxString& path,
                                const wxString& message,
                                const wxString& wildcard,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxValidator& validator,
                                const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_path = path;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var input = document.createElement('input');
        input.type = 'file';
        input.className = 'wxFilePickerWidget';
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';

        input.addEventListener('change', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(input);
    }, GetId());

    return true;
}

wxString wxFilePickerWidget::GetPath() const
{
    return m_path;
}

void wxFilePickerWidget::SetPath(const wxString& str)
{
    m_path = str;
}

void wxFilePickerWidget::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        char* val = (char*)EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 0;
            var input = container.querySelector('.wxFilePickerWidget');
            if (!input || !input.files.length) return 0;
            var str = input.files[0].name;
            var len = lengthBytesUTF8(str) + 1;
            var buf = _malloc(len);
            stringToUTF8(str, buf, len);
            return buf;
        }, GetId());

        if (val)
        {
            m_path = wxString::FromUTF8(val);
            free(val);
        }

        wxFileDirPickerEvent evt(wxEVT_FILEPICKER_CHANGED, this, GetId(), m_path);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}

// ============================================================================
// wxDirPickerWidget
// ============================================================================

wxIMPLEMENT_DYNAMIC_CLASS(wxDirPickerWidget, wxControl);

wxDirPickerWidget::wxDirPickerWidget(wxWindow *parent,
                                     wxWindowID id,
                                     const wxString& label,
                                     const wxString& path,
                                     const wxString& message,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    Create(parent, id, label, path, message, pos, size, style, validator, name);
}

bool wxDirPickerWidget::Create(wxWindow *parent,
                               wxWindowID id,
                               const wxString& label,
                               const wxString& path,
                               const wxString& message,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, validator, name))
        return false;

    m_path = path;

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var input = document.createElement('input');
        input.type = 'file';
        input.className = 'wxDirPickerWidget';
        input.style.width = '100%';
        input.style.height = '100%';
        input.style.boxSizing = 'border-box';
        // Non-standard but supported in some browsers for directory selection
        input.setAttribute('webkitdirectory', "");
        input.setAttribute('directory', "");

        input.addEventListener('change', function(e) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, 'change', 0, 0]);
            }
        });

        container.appendChild(input);
    }, GetId());

    return true;
}

wxString wxDirPickerWidget::GetPath() const
{
    return m_path;
}

void wxDirPickerWidget::SetPath(const wxString& str)
{
    m_path = str;
}

void wxDirPickerWidget::WasmNotifyEvent(const wxWasmEvent& event)
{
    if (event.id == m_windowId && event.eventType == "change")
    {
        char* val = (char*)EM_ASM_INT({
            var container = document.getElementById($0);
            if (!container) return 0;
            var input = container.querySelector('.wxDirPickerWidget');
            if (!input || !input.files.length) return 0;
            // Try to get the directory path from the first file's webkitRelativePath
            var str = input.files[0].webkitRelativePath || input.files[0].name;
            var len = lengthBytesUTF8(str) + 1;
            var buf = _malloc(len);
            stringToUTF8(str, buf, len);
            return buf;
        }, GetId());

        if (val)
        {
            wxString relPath = wxString::FromUTF8(val);
            free(val);
            // Extract directory from path/filename
            size_t pos = relPath.find_last_of('/');
            if (pos != wxString::npos)
                m_path = relPath.substr(0, pos);
            else
                m_path = relPath;
        }

        wxFileDirPickerEvent evt(wxEVT_DIRPICKER_CHANGED, this, GetId(), m_path);
        HandleWindowEvent(evt);
    }
    else
    {
        wxWindowWasm::WasmNotifyEvent(event);
    }
}
