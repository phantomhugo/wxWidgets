/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dc.cpp
// Author:      Hugo Castellanos
// Copyright:   (c) 2024 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/wasm/dc.h"

wxWasmDCImpl::wxWasmDCImpl( wxDC *owner ):wxDCImpl(owner)
{

}
bool wxWasmDCImpl::CanDrawBitmap() const
{

}
bool wxWasmDCImpl::CanGetTextExtent() const
{

}

void wxWasmDCImpl::DoGetSize(int *width, int *height) const
{

}
void wxWasmDCImpl::DoGetSizeMM(int* width, int* height) const
{

}

int wxWasmDCImpl::GetDepth() const
{

}
wxSize wxWasmDCImpl::GetPPI() const
{

}

void wxWasmDCImpl::SetFont(const wxFont& font)
{

}
void wxWasmDCImpl::SetPen(const wxPen& pen)
{

}
void wxWasmDCImpl::SetBrush(const wxBrush& brush)
{

}
void wxWasmDCImpl::SetBackground(const wxBrush& brush)
{

}
void wxWasmDCImpl::SetBackgroundMode(int mode)
{

}

#if wxUSE_PALETTE
void wxWasmDCImpl::SetPalette(const wxPalette& palette)
{

}
#endif // wxUSE_PALETTE

void wxWasmDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{

}

wxCoord wxWasmDCImpl::GetCharHeight() const
{

}
wxCoord wxWasmDCImpl::GetCharWidth() const
{

}
void wxWasmDCImpl::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                                wxCoord *descent, wxCoord *externalLeading, const wxFont *theFont) const
{

}

void wxWasmDCImpl::Clear()
{

}

void wxWasmDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height)
{

}

void wxWasmDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{

}
void wxWasmDCImpl::DestroyClippingRegion()
{

}

bool wxWasmDCImpl::DoFloodFill(wxCoord x, wxCoord y, const wxColour& col, wxFloodFillStyle style)
{

}
bool wxWasmDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{

}

void wxWasmDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{

}
void wxWasmDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{

}

void wxWasmDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                        wxCoord x2, wxCoord y2,
                        wxCoord xc, wxCoord yc)
{

}

void wxWasmDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                double sa, double ea)
{

}

void wxWasmDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{

}
void wxWasmDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{

}
void wxWasmDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                            wxCoord width, wxCoord height)
{

}

void wxWasmDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{

}

void wxWasmDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{

}
void wxWasmDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask)
{

}

void wxWasmDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{

}
void wxWasmDCImpl::DoDrawRotatedText(const wxString& text,
                                wxCoord x, wxCoord y, double angle)
{

}

bool wxWasmDCImpl::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
                    wxDC *source, wxCoord xsrc, wxCoord ysrc, wxRasterOperationMode rop,
                    bool useMask, wxCoord xsrcMask, wxCoord ysrcMask)
{

}

void wxWasmDCImpl::DoDrawLines(int n, const wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset )
{

}

void wxWasmDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                        wxCoord xoffset, wxCoord yoffset,
                        wxPolygonFillMode fillStyle)
{

}


void wxWasmDCImpl::ComputeScaleAndOrigin()
{

}
