/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/display.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/display.h"
#include "wx/private/display.h"
#include <emscripten.h>

// In the browser there is a single "display", described by window.screen.

class wxDisplayImplWasm : public wxDisplayImpl
{
public:
    wxDisplayImplWasm( unsigned n );

    virtual wxRect GetGeometry() const override;
    virtual wxRect GetClientArea() const override;
    virtual int GetDepth() const override;
    virtual double GetScaleFactor() const override;
    virtual wxString GetName() const override;
    virtual bool IsPrimary() const override;

#if wxUSE_DISPLAY
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const override;
    virtual wxVideoMode GetCurrentMode() const override;
    virtual bool ChangeMode(const wxVideoMode& mode) override;
#endif // wxUSE_DISPLAY
};

wxDisplayImplWasm::wxDisplayImplWasm( unsigned n )
    : wxDisplayImpl( n )
{
}

wxRect wxDisplayImplWasm::GetGeometry() const
{
    // In a browser the app's usable display is the page viewport, not the
    // OS screen (window.screen is a fake 800x600 in headless Chrome and
    // does not reflect the window size); fall back to it if unavailable.
    int width = EM_ASM_INT({
        return window.innerWidth || window.screen.width;
    });

    int height = EM_ASM_INT({
        return window.innerHeight || window.screen.height;
    });

    return wxRect(0, 0, width, height);
}

wxRect wxDisplayImplWasm::GetClientArea() const
{
    // No browser chrome intrudes in the page: client area == viewport.
    return GetGeometry();
}

int wxDisplayImplWasm::GetDepth() const
{
    return EM_ASM_INT({
        return window.screen.colorDepth || 24;
    });
}

double wxDisplayImplWasm::GetScaleFactor() const
{
    return EM_ASM_DOUBLE({
        return window.devicePixelRatio || 1.0;
    });
}

wxString wxDisplayImplWasm::GetName() const
{
    return wxString("Browser Screen");
}

bool wxDisplayImplWasm::IsPrimary() const
{
    return true;
}

#if wxUSE_DISPLAY
wxArrayVideoModes wxDisplayImplWasm::GetModes(const wxVideoMode& WXUNUSED(mode)) const
{
    return wxArrayVideoModes();
}

wxVideoMode wxDisplayImplWasm::GetCurrentMode() const
{
    const wxRect geometry = GetGeometry();

    return wxVideoMode( geometry.GetWidth(), geometry.GetHeight(), GetDepth() );
}

bool wxDisplayImplWasm::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    return false;
}
#endif // wxUSE_DISPLAY

//##############################################################################

#if wxUSE_DISPLAY

class wxDisplayFactoryWasm : public wxDisplayFactory
{
public:
    virtual wxDisplayImpl *CreateDisplay(unsigned n) override;
    virtual unsigned GetCount() override;
    virtual int GetFromPoint(const wxPoint& pt) override;
};

wxDisplayImpl *wxDisplayFactoryWasm::CreateDisplay(unsigned n)
{
    return n == 0 ? new wxDisplayImplWasm(n) : nullptr;
}

unsigned wxDisplayFactoryWasm::GetCount()
{
    return 1;
}

int wxDisplayFactoryWasm::GetFromPoint(const wxPoint& pt)
{
    wxDisplayImplWasm impl(0);

    return impl.GetGeometry().Contains(pt) ? 0 : wxNOT_FOUND;
}

//##############################################################################

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryWasm;
}

#else // wxUSE_DISPLAY

class wxDisplayFactorySingleWasm : public wxDisplayFactorySingle
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay() override
    {
        return new wxDisplayImplWasm(0);
    }
};

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleWasm;
}

#endif // wxUSE_DISPLAY/!wxUSE_DISPLAY
