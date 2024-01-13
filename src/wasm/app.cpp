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

    WakeUpIdle();

    return true;
}
