/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/app.cpp
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/app.h"
#include "wx/apptrait.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxApp, wxEvtHandler);

wxApp::wxApp()
{
}


wxApp::~wxApp()
{

}

bool wxApp::Initialize( int &argc, wxChar **argv )
{


    return true;
}
