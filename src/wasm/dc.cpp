/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/dc.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/icon.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/wasm/dc.h"
#include "wx/colour.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include <emscripten.h>
#include <cstdlib>

static int gs_canvasCounter = 0;

std::string GenerateCanvasId()
{
    return "wx_canvas_" + std::to_string(++gs_canvasCounter);
}

wxWasmDCImpl::wxWasmDCImpl(wxDC *owner)
    : wxDCImpl(owner),
      m_size(0, 0),
      m_penWidth(1),
      m_hasClipping(false)
{
}

wxWasmDCImpl::~wxWasmDCImpl()
{
    if (!m_canvasId.empty())
    {
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (canvas) canvas.remove();
        }, m_canvasId.c_str());
    }
}

void wxWasmDCImpl::EnsureCanvasCreated()
{
    if (m_canvasId.empty())
    {
        m_canvasId = GenerateCanvasId();
        EM_ASM_({
            var canvas = document.createElement('canvas');
            canvas.id = UTF8ToString($0);
            canvas.width = $1;
            canvas.height = $2;
            canvas.style.position = 'absolute';
            canvas.style.left = '0px';
            canvas.style.top = '0px';
            canvas.style.width = '100%';
            canvas.style.height = '100%';
            canvas.style.pointerEvents = 'none';
            document.body.appendChild(canvas);
        }, m_canvasId.c_str(), m_size.x, m_size.y);
    }
}

void wxWasmDCImpl::ApplyPen()
{
    if (m_canvasId.empty()) return;
    wxString col = m_penColour.GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.strokeStyle = UTF8ToString($1);
        ctx.lineWidth = $2;
    }, m_canvasId.c_str(), col.ToUTF8().data(), m_penWidth);
}

void wxWasmDCImpl::ApplyBrush()
{
    if (m_canvasId.empty()) return;
    wxString col = m_brushColour.GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = UTF8ToString($1);
    }, m_canvasId.c_str(), col.ToUTF8().data());
}

void wxWasmDCImpl::ApplyFont()
{
    if (m_canvasId.empty()) return;
    int size = m_font.IsOk() ? m_font.GetPointSize() : 12;
    wxString face = m_font.IsOk() ? m_font.GetFaceName() : wxString("sans-serif");
    if (face.empty()) face = wxT("sans-serif");
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.font = $1 + 'px ' + UTF8ToString($2);
    }, m_canvasId.c_str(), size, face.ToUTF8().data());
}

void wxWasmDCImpl::ApplyTextColour()
{
    if (m_canvasId.empty()) return;
    wxString col = m_textColour.GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = UTF8ToString($1);
    }, m_canvasId.c_str(), col.ToUTF8().data());
}

void wxWasmDCImpl::DoGetSize(int *width, int *height) const
{
    if (width) *width = m_size.x;
    if (height) *height = m_size.y;
}

void wxWasmDCImpl::DoGetSizeMM(int* width, int* height) const
{
    if (width) *width = m_size.x * 25 / 96;
    if (height) *height = m_size.y * 25 / 96;
}

void wxWasmDCImpl::SetFont(const wxFont& font)
{
    m_font = font;
    ApplyFont();
}

void wxWasmDCImpl::SetPen(const wxPen& pen)
{
    m_penColour = pen.GetColour();
    m_penWidth = pen.GetWidth();
    if (m_penWidth < 1) m_penWidth = 1;
    ApplyPen();
}

void wxWasmDCImpl::SetBrush(const wxBrush& brush)
{
    m_brushColour = brush.GetColour();
    ApplyBrush();
}

void wxWasmDCImpl::SetBackground(const wxBrush& brush)
{
    // Not critical for Canvas 2D
}

void wxWasmDCImpl::SetBackgroundMode(int mode)
{
    // Not critical for Canvas 2D
}

#if wxUSE_PALETTE
void wxWasmDCImpl::SetPalette(const wxPalette& WXUNUSED(palette))
{
}
#endif // wxUSE_PALETTE

void wxWasmDCImpl::SetLogicalFunction(wxRasterOperationMode WXUNUSED(function))
{
}

wxCoord wxWasmDCImpl::GetCharHeight() const
{
    return m_font.IsOk() ? m_font.GetPointSize() : 12;
}

wxCoord wxWasmDCImpl::GetCharWidth() const
{
    return m_font.IsOk() ? m_font.GetPointSize() / 2 : 6;
}

void wxWasmDCImpl::DoGetTextExtent(const wxString& string,
                                wxCoord *x, wxCoord *y,
                                wxCoord *descent, wxCoord *externalLeading,
                                const wxFont *theFont) const
{
    int size = theFont && theFont->IsOk() ? theFont->GetPointSize() : (m_font.IsOk() ? m_font.GetPointSize() : 12);
    if (x) *x = string.length() * size / 2;
    if (y) *y = size;
    if (descent) *descent = size / 4;
    if (externalLeading) *externalLeading = 0;
}

void wxWasmDCImpl::Clear()
{
    EnsureCanvasCreated();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    }, m_canvasId.c_str());
}

void wxWasmDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height)
{
    EnsureCanvasCreated();
    m_hasClipping = true;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.save();
        ctx.beginPath();
        ctx.rect($1, $2, $3, $4);
        ctx.clip();
    }, m_canvasId.c_str(), x, y, width, height);
}

void wxWasmDCImpl::DoSetDeviceClippingRegion(const wxRegion& WXUNUSED(region))
{
}

void wxWasmDCImpl::DestroyClippingRegion()
{
    if (m_hasClipping)
    {
        m_hasClipping = false;
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            ctx.restore();
        }, m_canvasId.c_str());
    }
}

bool wxWasmDCImpl::DoFloodFill(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                             const wxColour& WXUNUSED(col),
                             wxFloodFillStyle WXUNUSED(style))
{
    return false;
}

bool wxWasmDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    if (!col || m_canvasId.empty()) return false;
    int r = EM_ASM_INT({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return 0;
        var ctx = canvas.getContext('2d');
        var data = ctx.getImageData($1, $2, 1, 1).data;
        return data[0];
    }, m_canvasId.c_str(), x, y);
    int g = EM_ASM_INT({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return 0;
        var ctx = canvas.getContext('2d');
        var data = ctx.getImageData($1, $2, 1, 1).data;
        return data[1];
    }, m_canvasId.c_str(), x, y);
    int b = EM_ASM_INT({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return 0;
        var ctx = canvas.getContext('2d');
        var data = ctx.getImageData($1, $2, 1, 1).data;
        return data[2];
    }, m_canvasId.c_str(), x, y);
    *col = wxColour(r, g, b);
    return true;
}

void wxWasmDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    EnsureCanvasCreated();
    ApplyPen();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillRect($1, $2, 1, 1);
    }, m_canvasId.c_str(), x, y);
}

void wxWasmDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    EnsureCanvasCreated();
    ApplyPen();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo($1, $2);
        ctx.lineTo($3, $4);
        ctx.stroke();
    }, m_canvasId.c_str(), x1, y1, x2, y2);
}

void wxWasmDCImpl::DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc)
{
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    double radius = sqrt((double)(x1 - xc) * (x1 - xc) + (double)(y1 - yc) * (y1 - yc));
    double startAngle = atan2((double)(y1 - yc), (double)(x1 - xc));
    double endAngle = atan2((double)(y2 - yc), (double)(x2 - xc));
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.arc($1, $2, $3, $4, $5);
        ctx.stroke();
        ctx.fill();
    }, m_canvasId.c_str(), xc, yc, radius, startAngle, endAngle);
}

void wxWasmDCImpl::DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea)
{
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    double rx = w / 2.0;
    double ry = h / 2.0;
    double cx = x + rx;
    double cy = y + ry;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.ellipse($1, $2, $3, $4, 0, $5, $6);
        ctx.stroke();
        ctx.fill();
    }, m_canvasId.c_str(), cx, cy, rx, ry, sa, ea);
}

void wxWasmDCImpl::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillRect($1, $2, $3, $4);
        ctx.strokeRect($1, $2, $3, $4);
    }, m_canvasId.c_str(), x, y, width, height);
}

void wxWasmDCImpl::DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                    wxCoord width, wxCoord height,
                                    double radius)
{
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.roundRect($1, $2, $3, $4, $5);
        ctx.fill();
        ctx.stroke();
    }, m_canvasId.c_str(), x, y, width, height, radius);
}

void wxWasmDCImpl::DoDrawEllipse(wxCoord x, wxCoord y,
                            wxCoord width, wxCoord height)
{
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    double rx = width / 2.0;
    double ry = height / 2.0;
    double cx = x + rx;
    double cy = y + ry;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.ellipse($1, $2, $3, $4, 0, 0, 2 * Math.PI);
        ctx.fill();
        ctx.stroke();
    }, m_canvasId.c_str(), cx, cy, rx, ry);
}

void wxWasmDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    EnsureCanvasCreated();
    ApplyPen();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo(0, $1);
        ctx.lineTo(canvas.width, $1);
        ctx.moveTo($1, 0);
        ctx.lineTo($1, canvas.height);
        ctx.stroke();
    }, m_canvasId.c_str(), x, y);
}

void wxWasmDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    // TODO: implement using icon's bitmap data
}

void wxWasmDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool WXUNUSED(useMask))
{
    if (!bmp.IsOk()) return;
    wxImage image = bmp.ConvertToImage();
    if (!image.IsOk()) return;
    EnsureCanvasCreated();
    int w = image.GetWidth();
    int h = image.GetHeight();
    unsigned char* rgb = image.GetData();
    unsigned char* alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var w = $3;
        var h = $4;
        var imgData = ctx.createImageData(w, h);
        var rgb = $5;
        var alpha = $6;
        for (var i = 0; i < w * h; i++) {
            imgData.data[i * 4 + 0] = HEAPU8[rgb + i * 3 + 0];
            imgData.data[i * 4 + 1] = HEAPU8[rgb + i * 3 + 1];
            imgData.data[i * 4 + 2] = HEAPU8[rgb + i * 3 + 2];
            imgData.data[i * 4 + 3] = alpha ? HEAPU8[alpha + i] : 255;
        }
        var tmpCanvas = document.createElement('canvas');
        tmpCanvas.width = w;
        tmpCanvas.height = h;
        var tmpCtx = tmpCanvas.getContext('2d');
        tmpCtx.putImageData(imgData, 0, 0);
        ctx.drawImage(tmpCanvas, $1, $2);
    }, m_canvasId.c_str(), x, y, w, h, rgb, alpha);
}

void wxWasmDCImpl::DoDrawText(const wxString& text, wxCoord x, wxCoord y)
{
    EnsureCanvasCreated();
    ApplyTextColour();
    ApplyFont();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillText(UTF8ToString($1), $2, $3);
    }, m_canvasId.c_str(), text.ToUTF8().data(), x, y);
}

void wxWasmDCImpl::DoDrawRotatedText(const wxString& text,
                                wxCoord x, wxCoord y, double angle)
{
    EnsureCanvasCreated();
    ApplyTextColour();
    ApplyFont();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.save();
        ctx.translate($2, $3);
        ctx.rotate($4);
        ctx.fillText(UTF8ToString($1), 0, 0);
        ctx.restore();
    }, m_canvasId.c_str(), text.ToUTF8().data(), x, y, angle * M_PI / 180.0);
}

bool wxWasmDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                    wxCoord width, wxCoord height,
                    wxDC *source,
                    wxCoord xsrc, wxCoord ysrc,
                    wxRasterOperationMode WXUNUSED(rop),
                    bool WXUNUSED(useMask),
                    wxCoord WXUNUSED(xsrcMask),
                    wxCoord WXUNUSED(ysrcMask))
{
    wxWasmDCImpl* srcImpl = dynamic_cast<wxWasmDCImpl*>(source->GetImpl());
    if (!srcImpl || srcImpl->m_canvasId.empty()) return false;
    EnsureCanvasCreated();
    EM_ASM_({
        var srcCanvas = document.getElementById(UTF8ToString($0));
        var dstCanvas = document.getElementById(UTF8ToString($1));
        if (!srcCanvas || !dstCanvas) return;
        var ctx = dstCanvas.getContext('2d');
        ctx.drawImage(srcCanvas, $2, $3, $4, $5, $6, $7, $4, $5);
    }, srcImpl->m_canvasId.c_str(), m_canvasId.c_str(), xsrc, ysrc, width, height, xdest, ydest);
    return true;
}

void wxWasmDCImpl::DoDrawLines(int n, const wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset)
{
    if (n < 2) return;
    EnsureCanvasCreated();
    ApplyPen();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo($1, $2);
    }, m_canvasId.c_str(), points[0].x + xoffset, points[0].y + yoffset);
    for (int i = 1; i < n; ++i)
    {
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            ctx.lineTo($1, $2);
        }, m_canvasId.c_str(), points[i].x + xoffset, points[i].y + yoffset);
    }
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.stroke();
    }, m_canvasId.c_str());
}

void wxWasmDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode WXUNUSED(fillStyle))
{
    if (n < 2) return;
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo($1, $2);
    }, m_canvasId.c_str(), points[0].x + xoffset, points[0].y + yoffset);
    for (int i = 1; i < n; ++i)
    {
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            ctx.lineTo($1, $2);
        }, m_canvasId.c_str(), points[i].x + xoffset, points[i].y + yoffset);
    }
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.closePath();
        ctx.fill();
        ctx.stroke();
    }, m_canvasId.c_str());
}

void wxWasmDCImpl::ComputeScaleAndOrigin()
{
    wxDCImpl::ComputeScaleAndOrigin();
}

void wxWasmDCImpl::ApplyRasterColourOp()
{
}
