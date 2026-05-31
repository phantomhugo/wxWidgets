/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/app.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 Hugo Armando Castellanos
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"
#include "wx/wasm/cssstyles.h"

#include <emscripten.h>

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxApp::wxApp()
{
    WXAppConstructed();
}


wxApp::~wxApp()
{

}

bool wxApp::Initialize( int &argc, wxChar **argv )
{
    if ( !wxAppBase::Initialize(argc, argv) )
        return false;

    //We create a hidden div containing all temprarly parentless controls (line wxMenuBar) otherwise the browser is going to delete it
    EM_ASM_INT(
            const newTopLevelWindow=document.createElement("div");
            newTopLevelWindow.id= "wxParentlessTags";
            newTopLevelWindow.style.display="none";
            document.body.append(newTopLevelWindow);
            return 1;
    );

    // Inyectar estilos CSS por defecto (GTK3-like)
    wxWasmCSSManager::InjectDefaultStyles();

    // Register global keydown listener for menu accelerators
    EM_ASM_({
        document.addEventListener('keydown', function(e) {
            var key = e.key;
            var ctrl = e.ctrlKey ? 1 : 0;
            var alt = e.altKey ? 1 : 0;
            var shift = e.shiftKey ? 1 : 0;

            if (typeof Module !== 'undefined' && Module.ccall) {
                var len = lengthBytesUTF8(key) + 1;
                var buf = Module._malloc(len);
                stringToUTF8(key, buf, len);
                Module.ccall('ProcessAcceleratorKey', null,
                    ['number', 'number', 'number', 'number'],
                    [buf, ctrl, alt, shift]);
                Module._free(buf);
            }
        });
    });

    WakeUpIdle();

    return true;
}

extern "C" EMSCRIPTEN_KEEPALIVE void ProcessAcceleratorKey(const char* key, int ctrl, int alt, int shift)
{
    // Basic accelerator processing - find focused window and try to match
    wxWindow* focus = wxWindow::FindFocus();
    if (!focus)
        return;

    // Walk up to find the frame
    wxWindow* top = focus;
    while (top && !wxDynamicCast(top, wxFrame))
        top = top->GetParent();

    wxFrame* frame = wxDynamicCast(top, wxFrame);
    if (!frame)
        return;

    wxMenuBar* mb = frame->GetMenuBar();
    if (!mb)
        return;

    // TODO: Full accelerator table matching with wxAcceleratorTable
    // For now, this sets up the infrastructure for accelerator support
}
