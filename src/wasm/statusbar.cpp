/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/statusbar.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2023-2024 wxWidgets dev team
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
    // Crear el elemento DOM base del status bar
    wxWasmCreateStatusBar();

    // Mover al parent (frame)
    EM_ASM_({
        var parent = document.getElementById($0);
        var statusBar = document.getElementById($1);
        
        if (parent && statusBar) {
            // Agregar al final del frame
            parent.appendChild(statusBar);
            statusBar.style.display = 'flex';
        }
    }, parent->GetId(), GetId());

    PostCreation();

    // Por defecto, crear 1 field
    SetFieldsCount(1);

    return true;
}

void wxStatusBar::wxWasmCreateStatusBar()
{
    EM_ASM_({
        var statusBar = document.createElement("div");
        statusBar.id = $0;
        statusBar.className = 'wxStatusBar';
        
        // Crear contenedor para los fields
        var fieldsContainer = document.createElement("div");
        fieldsContainer.className = 'wxStatusBar-fields';
        fieldsContainer.id = $0 + '_fields';
        statusBar.appendChild(fieldsContainer);
        
        // Insertar en el div de parentless inicialmente
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

    // Obtener las dimensiones del field desde el DOM
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
    wxStatusBarBase::SetMinHeight(height);
    
    // Aplicar altura mínima al DOM
    EM_ASM_({
        var statusBar = document.getElementById($0);
        if (statusBar) {
            statusBar.style.minHeight = $1 + 'px';
        }
    }, GetId(), height);
}

int wxStatusBar::GetBorderX() const
{
    // Retornar el borde horizontal definido en CSS (generalmente 1px)
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
    
    // Actualizar el texto en el DOM
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
    // Recrear los fields en el DOM según la configuración actual
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
        
        // Limpiar fields existentes
        container.innerHTML = '';
        
        var numFields = $1;
        var hasSizeGrip = UTF8ToString($2) !== '';
        
        // Crear cada field
        for (var i = 0; i < numFields; i++) {
            var field = document.createElement("div");
            field.id = 'wxStatusBar_field_' + $0 + '_' + i;
            field.className = 'wxStatusBar-field';
            
            // El último field puede tener el size grip
            if (hasSizeGrip && i === numFields - 1) {
                field.classList.add('wxStatusBar-field-with-grip');
                
                // Crear el grip element
                var grip = document.createElement("div");
                grip.className = 'wxStatusBar-sizegrip';
                grip.innerHTML = '◢'; // Carácter Unicode para el grip
                field.appendChild(grip);
            }
            
            // Crear elemento de texto
            var textElem = document.createElement("span");
            textElem.className = 'wxStatusBar-field-text';
            field.appendChild(textElem);
            
            container.appendChild(field);
        }
        
        // Aplicar anchos si están definidos
        if (Module.statusBarWidths && Module.statusBarWidths[$0]) {
            var widths = Module.statusBarWidths[$0];
            var fields = container.querySelectorAll('.wxStatusBar-field');
            
            if (widths.length === 1 && widths[0] === -1) {
                // wxSTB_DEFAULT_STYLE: distribuir equitativamente
                var widthPercent = 100 / fields.length;
                fields.forEach(function(f) {
                    f.style.flex = '1 1 ' + widthPercent + '%';
                });
            } else if (widths.length === fields.length) {
                // Anchos específicos
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

    // Actualizar textos
    for (int i = 0; i < numFields; i++) {
        DoUpdateStatusText(i);
    }
}

// Sobrescribir SetFieldsCount para manejar cambios en el número de fields
void wxStatusBar::SetFieldsCount(int number, const int* widths)
{
    wxStatusBarBase::SetFieldsCount(number, widths);
    
    // Guardar los anchos para usarlos en UpdateFields
    if (widths) {
        EM_ASM_({
            if (!Module.statusBarWidths) Module.statusBarWidths = {};
            Module.statusBarWidths[$0] = [];
            for (var i = 0; i < $1; i++) {
                Module.statusBarWidths[$0].push(Module.HEAP32[$2 + i * 4 >> 2]);
            }
        }, GetId(), number, widths);
    } else {
        // Default: todos los fields con el mismo ancho
        EM_ASM_({
            if (!Module.statusBarWidths) Module.statusBarWidths = {};
            Module.statusBarWidths[$0] = [-1]; // -1 indica distribución automática
        }, GetId());
    }
    
    UpdateFields();
}

// Método para establecer el estilo del status bar
void wxStatusBar::SetWindowStyleFlag(long style)
{
    wxWindow::SetWindowStyleFlag(style);
    
    // Actualizar visibilidad del size grip
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

// PopStatusText y PushStatusText para manejo de pila de textos
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
