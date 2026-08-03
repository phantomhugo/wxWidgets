/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statusbar.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/statusbr.h"
#include "wx/frame.h"
#include "wx/wasm/cssstyles.h"
#include <emscripten.h>

wxStatusBar::wxStatusBar()
{
    Init();
}

wxStatusBar::wxStatusBar(wxWindow *parent, wxWindowID winid,
            long style,
            const wxString& name)
{
    Init();

    if ( winid == wxID_ANY )
    {
        SetId(NewControlId());
    }
    else // valid id specified
    {
        SetId(winid);
    }

    Create( parent, GetId(), style, name );
}

bool wxStatusBar::Create(wxWindow *parent, wxWindowID WXUNUSED(winid),
                         long style, const wxString& WXUNUSED(name))
{
    // Create the base DOM element of the status bar
    wxWasmCreateStatusBar();

    // Move to the parent (frame)
    EM_ASM_({
        var parent = document.getElementById($0);
        var statusBar = document.getElementById($1);
        
        if (parent && statusBar) {
            // Append at the end of the frame
            parent.appendChild(statusBar);
            statusBar.style.display = 'flex';
        }
    }, parent->GetId(), GetId());

    PostCreation();

    // By default, create 1 field
    SetFieldsCount(1);

    return true;
}

void wxStatusBar::wxWasmCreateStatusBar()
{
    EM_ASM_({
        var statusBar = document.createElement("div");
        statusBar.id = $0;
        statusBar.className = 'wxStatusBar';
        
        // Create container for the fields
        var fieldsContainer = document.createElement("div");
        fieldsContainer.className = 'wxStatusBar-fields';
        fieldsContainer.id = $0 + '_fields';
        statusBar.appendChild(fieldsContainer);
        
        // Insert into the parentless div initially
        var parentlessDiv = document.getElementById("wxParentlessTags");
        if (parentlessDiv) {
            parentlessDiv.appendChild(statusBar);
            statusBar.style.display = 'none';
        }
    }, GetId());
}

bool wxStatusBar::GetFieldRect(int i, wxRect& rect) const
{
    if (i < 0 || i >= GetFieldsCount())
        return false;

    // Get the field dimensions from the DOM
    int x = EM_ASM_INT({
        var field = document.getElementById('wxStatusBar_field_' + $0 + '_' + $1);
        if (field) {
            var rect = field.getBoundingClientRect();
            return rect.left;
        }
        return 0;
    }, GetId(), i);

    int y = EM_ASM_INT({
        var field = document.getElementById('wxStatusBar_field_' + $0 + '_' + $1);
        if (field) {
            var rect = field.getBoundingClientRect();
            return rect.top;
        }
        return 0;
    }, GetId(), i);

    int width = EM_ASM_INT({
        var field = document.getElementById('wxStatusBar_field_' + $0 + '_' + $1);
        if (field) {
            var rect = field.getBoundingClientRect();
            return rect.width;
        }
        return 0;
    }, GetId(), i);

    int height = EM_ASM_INT({
        var field = document.getElementById('wxStatusBar_field_' + $0 + '_' + $1);
        if (field) {
            var rect = field.getBoundingClientRect();
            return rect.height;
        }
        return 0;
    }, GetId(), i);

    rect = wxRect(x, y, width, height);
    return true;
}

void wxStatusBar::SetMinHeight(int height)
{
    // Apply minimum height to the DOM
    EM_ASM_({
        var statusBar = document.getElementById($0);
        if (statusBar) {
            statusBar.style.minHeight = $1 + 'px';
        }
    }, GetId(), height);
}

int wxStatusBar::GetBorderX() const
{
    // Return the horizontal border defined in CSS (usually 1px)
    return 1;
}

int wxStatusBar::GetBorderY() const
{
    return 1;
}

void wxStatusBar::DoUpdateStatusText(int number)
{
    if (number < 0 || number >= GetFieldsCount())
        return;

    wxString text = GetStatusText(number);
    wxCharBuffer textBuffer = text.ToUTF8();
    
    // Update the text in the DOM
    EM_ASM_({
        var fieldId = 'wxStatusBar_field_' + $0 + '_' + $1;
        var field = document.getElementById(fieldId);
        
        if (field) {
            var textElem = field.querySelector('.wxStatusBar-field-text');
            if (textElem) {
                textElem.textContent = UTF8ToString($2);
            }
        }
    }, GetId(), number, textBuffer.data());
}

void wxStatusBar::Refresh( bool eraseBackground, const wxRect *rect )
{
    UpdateFields();
    wxWindow::Refresh( eraseBackground, rect );
}

void wxStatusBar::Init()
{
}

void wxStatusBar::UpdateFields()
{
    // Recreate the fields in the DOM according to the current configuration
    int numFields = GetFieldsCount();
    if (numFields <= 0)
        return;

    wxCharBuffer styleVar;
    if (GetWindowStyle() & wxSTB_SIZEGRIP)
        styleVar = "wxStatusBar-sizegrip";
    else
        styleVar = "";

    EM_ASM_({
        var containerId = $0 + '_fields';
        var container = document.getElementById(containerId);
        if (!container) return;
        
        // Clear existing fields
        container.innerHTML = "";
        
        var numFields = $1;
        var hasSizeGrip = UTF8ToString($2) !== "";
        
        // Create each field
        for (var i = 0; i < numFields; i++) {
            var field = document.createElement("div");
            field.id = 'wxStatusBar_field_' + $0 + '_' + i;
            field.className = 'wxStatusBar-field';
            
            // The last field can have the size grip
            if (hasSizeGrip && i === numFields - 1) {
                field.classList.add('wxStatusBar-field-with-grip');
                
                // Create the grip element
                var grip = document.createElement("div");
                grip.className = 'wxStatusBar-sizegrip';
                grip.innerHTML = '◢'; // Unicode character for the grip
                field.appendChild(grip);
            }
            
            // Create text element
            var textElem = document.createElement("span");
            textElem.className = 'wxStatusBar-field-text';
            field.appendChild(textElem);
            
            container.appendChild(field);
        }
        
        // Apply widths if defined
        if (Module.statusBarWidths && Module.statusBarWidths[$0]) {
            var widths = Module.statusBarWidths[$0];
            var fields = container.querySelectorAll('.wxStatusBar-field');
            
            if (widths.length === 1 && widths[0] === -1) {
                // wxSTB_DEFAULT_STYLE: distribute evenly
                var widthPercent = 100 / fields.length;
                fields.forEach(function(f) {
                    f.style.flex = '1 1 ' + widthPercent + '%';
                });
            } else if (widths.length === fields.length) {
                // Specific widths
                fields.forEach(function(f, idx) {
                    if (widths[idx] === -1) {
                        f.style.flex = '1 1 auto';
                    } else {
                        f.style.flex = '0 0 ' + widths[idx] + 'px';
                    }
                });
            }
        }
    }, GetId(), numFields, styleVar.data());

    // Update texts
    for (int i = 0; i < numFields; i++) {
        DoUpdateStatusText(i);
    }
}

// Override SetFieldsCount to handle changes in the number of fields
void wxStatusBar::SetFieldsCount(int number, const int* widths)
{
    wxStatusBarBase::SetFieldsCount(number, widths);
    
    // Save the widths to use them in UpdateFields
    if (widths) {
        EM_ASM_({
            if (!Module.statusBarWidths) Module.statusBarWidths = {};
            Module.statusBarWidths[$0] = [];
            for (var i = 0; i < $1; i++) {
                Module.statusBarWidths[$0].push(Module.HEAP32[$2 + i * 4 >> 2]);
            }
        }, GetId(), number, widths);
    } else {
        // Default: all fields with the same width
        EM_ASM_({
            if (!Module.statusBarWidths) Module.statusBarWidths = {};
            Module.statusBarWidths[$0] = [-1]; // -1 means automatic distribution
        }, GetId());
    }
    
    UpdateFields();
}

// Method to set the status bar style
void wxStatusBar::SetWindowStyleFlag(long style)
{
    wxWindow::SetWindowStyleFlag(style);
    
    // Update size grip visibility
    bool showGrip = (style & wxSTB_SIZEGRIP) != 0;
    
    EM_ASM_({
        var statusBar = document.getElementById($0);
        if (statusBar) {
            if ($1) {
                statusBar.classList.add('wxStatusBar-with-grip');
            } else {
                statusBar.classList.remove('wxStatusBar-with-grip');
            }
        }
    }, GetId(), showGrip ? 1 : 0);
    
    UpdateFields();
}

// PopStatusText and PushStatusText for text stack handling
void wxStatusBar::PopStatusText(int field)
{
    wxStatusBarBase::PopStatusText(field);
    DoUpdateStatusText(field);
}

void wxStatusBar::PushStatusText(const wxString& text, int field)
{
    wxStatusBarBase::PushStatusText(text, field);
    DoUpdateStatusText(field);
}
