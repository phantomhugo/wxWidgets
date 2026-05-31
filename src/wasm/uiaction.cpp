/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/uiaction.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"
#include "wx/private/uiaction.h"

wxUIActionSimulator::wxUIActionSimulator()
                   : m_impl(nullptr)
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
    // We can use a static wxUIActionSimulatorQtImpl object because it's
    // stateless, so no need to delete it.
}

#endif // wxUSE_UIACTIONSIMULATOR
