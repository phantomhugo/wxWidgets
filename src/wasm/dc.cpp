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
    #include "wx/region.h"
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

// ----------------------------------------------------------------------------
// JS helpers for text measurement
//
// NB: these are plain C++ functions (not EM_JS) because they are also called
// from other translation units (e.g. window.cpp): EM_JS symbols referenced
// from another TU of a static library are not resolved by the JS linker.
// ----------------------------------------------------------------------------

double wxWasmMeasureTextWidth(const char* fontSpec, const char* text)
{
    return EM_ASM_DOUBLE({
        var ctx = window._wxWasmMeasureCtx;
        if (!ctx) { ctx = document.createElement('canvas').getContext('2d'); window._wxWasmMeasureCtx = ctx; }
        ctx.font = UTF8ToString($0);
        return ctx.measureText(UTF8ToString($1)).width;
    }, fontSpec, text);
}

double wxWasmMeasureCharHeight(const char* fontSpec)
{
    return EM_ASM_DOUBLE({
        var ctx = window._wxWasmMeasureCtx;
        if (!ctx) { ctx = document.createElement('canvas').getContext('2d'); window._wxWasmMeasureCtx = ctx; }
        ctx.font = UTF8ToString($0);
        var metrics = ctx.measureText('Mg');
        var ascent = metrics.actualBoundingBoxAscent || 0;
        var descent = metrics.actualBoundingBoxDescent || 0;
        if (ascent === 0 && descent === 0) {
            // Fallback: parse size from font spec (look for pt or px)
            var match = ctx.font.match(/(\\d+(?:\\.\\d+)?)\\s*(px|pt)/i);
            if (match) {
                var size = parseFloat(match[1]);
                var unit = match[2].toLowerCase();
                if (unit === 'pt') size = size * 96 / 72;
                return Math.round(size * 1.2);
            }
            return 12;
        }
        return ascent + descent;
    }, fontSpec);
}

double wxWasmMeasureCharWidth(const char* fontSpec)
{
    return EM_ASM_DOUBLE({
        var ctx = window._wxWasmMeasureCtx;
        if (!ctx) { ctx = document.createElement('canvas').getContext('2d'); window._wxWasmMeasureCtx = ctx; }
        ctx.font = UTF8ToString($0);
        return ctx.measureText('M').width;
    }, fontSpec);
}

double wxWasmMeasureDescent(const char* fontSpec)
{
    return EM_ASM_DOUBLE({
        var ctx = window._wxWasmMeasureCtx;
        if (!ctx) { ctx = document.createElement('canvas').getContext('2d'); window._wxWasmMeasureCtx = ctx; }
        ctx.font = UTF8ToString($0);
        var metrics = ctx.measureText('Mg');
        return metrics.actualBoundingBoxDescent || 0;
    }, fontSpec);
}

double wxWasmMeasureAscent(const char* fontSpec)
{
    return EM_ASM_DOUBLE({
        var ctx = window._wxWasmMeasureCtx;
        if (!ctx) { ctx = document.createElement('canvas').getContext('2d'); window._wxWasmMeasureCtx = ctx; }
        ctx.font = UTF8ToString($0);
        var metrics = ctx.measureText('Mg');
        return metrics.actualBoundingBoxAscent || 0;
    }, fontSpec);
}

// Returns the pixel at (x, y) of the given canvas packed as
// r | g<<8 | b<<16 | a<<24 (as an unsigned value in a double, which can
// represent it exactly), or -1 if the canvas does not exist.
double wxWasmGetPixel(const char* canvasId, int x, int y)
{
    return EM_ASM_DOUBLE({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return -1;
        var ctx = canvas.getContext('2d');
        var data = ctx.getImageData($1, $2, 1, 1).data;
        return data[0] + data[1] * 256 + data[2] * 65536 + data[3] * 16777216;
    }, canvasId, x, y);
}

// ----------------------------------------------------------------------------
// Raster operations
//
// The canvas 2D compositing model is Porter-Duff alpha compositing, so most
// of the bitwise wx raster operations have no exact equivalent. The mapping
// below uses the closest reasonable canvas mode:
//   - wxCOPY: 'source-over' (the default, alpha-weighted src over dst).
//   - wxXOR: 'xor' (Porter-Duff xor; matches a bitwise XOR for opaque pixels).
//   - wxOR, wxOR_REVERSE, wxOR_INVERT: 'lighter' (additive blending is the
//     closest canvas gets to a bitwise OR; colours may saturate).
//   - wxAND: 'source-in', wxAND_REVERSE: 'source-out', wxAND_INVERT:
//     'destination-out' (alpha-based approximations of the bitwise ANDs).
//   - wxINVERT, wxSRC_INVERT, wxEQUIV: 'difference' (|src - dst|; an exact
//     NOT of dst only when the source is white).
//   - wxCLEAR: 'destination-out' (erases the destination where drawn, i.e.
//     clears to transparent instead of to black).
//   - wxNO_OP: no composite equivalent; emulated with globalAlpha == 0 in
//     EnsureCanvasCreated().
//   - wxSET, wxNOR, wxNAND: no reasonable canvas equivalent at all, they fall
//     back to the wxCOPY behaviour ('source-over').
// ----------------------------------------------------------------------------
static const char* wxWasmRasterOpToComposite(wxRasterOperationMode rop)
{
    switch (rop)
    {
        case wxXOR:         return "xor";
        case wxOR:
        case wxOR_REVERSE:
        case wxOR_INVERT:   return "lighter";
        case wxAND:         return "source-in";
        case wxAND_REVERSE: return "source-out";
        case wxAND_INVERT:
        case wxCLEAR:       return "destination-out";
        case wxINVERT:
        case wxSRC_INVERT:
        case wxEQUIV:       return "difference";
        // wxCOPY, wxNO_OP (handled via globalAlpha), wxSET, wxNOR, wxNAND.
        default:            return "source-over";
    }
}

// ----------------------------------------------------------------------------
// CSS font spec helper
// ----------------------------------------------------------------------------

static wxString GetCSSFontSpec(const wxFont& font)
{
    if (!font.IsOk())
        return wxString("12px sans-serif");

    wxString spec;

    // Style
    switch (font.GetStyle())
    {
        case wxFONTSTYLE_ITALIC:
            spec += "italic ";
            break;
        case wxFONTSTYLE_SLANT:
            spec += "oblique ";
            break;
        default:
            break;
    }

    // Weight (numeric)
    spec += wxString::Format("%d ", font.GetNumericWeight());

    // Size
    double size = font.GetFractionalPointSize();
    if (size <= 0)
        size = 12;
    spec += wxString::Format("%.1fpt ", size);

    // Face name or generic family
    wxString face = font.GetFaceName();
    if (face.empty())
    {
        switch (font.GetFamily())
        {
            case wxFONTFAMILY_ROMAN:
                face = "serif";
                break;
            case wxFONTFAMILY_SCRIPT:
                face = "cursive";
                break;
            case wxFONTFAMILY_DECORATIVE:
                face = "fantasy";
                break;
            case wxFONTFAMILY_MODERN:
            case wxFONTFAMILY_TELETYPE:
                face = "monospace";
                break;
            case wxFONTFAMILY_SWISS:
            default:
                face = "sans-serif";
                break;
        }
    }
    else
    {
        face.Replace("\"", "\\\"");
        face = wxString::Format("\"%s\"", face);
    }

    spec += face;

    return spec;
}

wxWasmDCImpl::wxWasmDCImpl(wxDC *owner)
    : wxDCImpl(owner),
      m_size(0, 0),
      m_backgroundColour(*wxWHITE),
      m_backgroundMode(wxTRANSPARENT),
      m_logicalFunction(wxCOPY),
      m_penWidth(1),
      m_penStyle(wxPENSTYLE_SOLID),
      m_brushStyle(wxBRUSHSTYLE_SOLID),
      m_canvasOwnedByDC(true),
      m_hasClip(false)
{
}

wxWasmDCImpl::~wxWasmDCImpl()
{
    // Only remove canvases owned by this DC (memory DCs and anonymous
    // canvases created by EnsureCanvasCreated()). The per-window canvas is
    // shared by all DCs of its window and must survive the DC so that what
    // was drawn on it remains visible; it dies with the window element.
    if (m_canvasOwnedByDC && !m_canvasId.empty())
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

    SyncCanvasBuffer();

    // Re-apply the clipping state recorded on the canvas element: the 2D
    // context state persists between drawing operations, so pop the state
    // saved for the previous operation (restore() is a no-op when the stack
    // is empty, e.g. after a canvas resize which resets the whole state)
    // and re-apply the current clip, if any. This keeps the save/restore
    // pairs balanced and makes the clip coherent for all the DCs sharing
    // the same canvas (e.g. the per-window canvas).
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        if (canvas._wxClipSaved)
        {
            ctx.restore();
            canvas._wxClipSaved = false;
        }
        if (canvas._wxClip)
        {
            ctx.save();
            ctx.beginPath();
            ctx.rect(canvas._wxClip.x, canvas._wxClip.y,
                     canvas._wxClip.w, canvas._wxClip.h);
            ctx.clip();
            canvas._wxClipSaved = true;
        }
    }, m_canvasId.c_str());

    // Apply the current logical function as a canvas composite operation.
    // wxNO_OP ("leave dst unchanged") has no composite equivalent, so it is
    // emulated with a fully transparent source (globalAlpha == 0).
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.globalCompositeOperation = UTF8ToString($1);
        ctx.globalAlpha = $2;
    }, m_canvasId.c_str(), wxWasmRasterOpToComposite(m_logicalFunction),
       m_logicalFunction == wxNO_OP ? 0.0 : 1.0);
}

void wxWasmDCImpl::SetCanvasClip(int x, int y, int w, int h)
{
    if (m_canvasId.empty()) return;
    // Only record the clip box here; it is applied to the 2D context by
    // EnsureCanvasCreated() before the next drawing operation. NB: the
    // fields are assigned one by one because commas inside braces would be
    // parsed as macro argument separators by EM_ASM_.
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        if (!canvas._wxClip) canvas._wxClip = {};
        canvas._wxClip.x = $1;
        canvas._wxClip.y = $2;
        canvas._wxClip.w = $3;
        canvas._wxClip.h = $4;
    }, m_canvasId.c_str(), x, y, w, h);
}

// Builds a repeating canvas pattern from an image and assigns it to the
// stroke (forStroke) or fill style of the given canvas 2D context.
static void ApplyImagePattern(const std::string& canvasId,
                              const wxImage& image, bool forStroke)
{
    const int w = image.GetWidth();
    const int h = image.GetHeight();
    unsigned char* rgb = image.GetData();
    unsigned char* alpha = image.HasAlpha() ? image.GetAlpha() : nullptr;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var w = $2;
        var h = $3;
        var imgData = ctx.createImageData(w, h);
        var rgb = $4;
        var alpha = $5;
        for (var i = 0; i < w * h; i++) {
            imgData.data[i * 4 + 0] = HEAPU8[rgb + i * 3 + 0];
            imgData.data[i * 4 + 1] = HEAPU8[rgb + i * 3 + 1];
            imgData.data[i * 4 + 2] = HEAPU8[rgb + i * 3 + 2];
            imgData.data[i * 4 + 3] = alpha ? HEAPU8[alpha + i] : 255;
        }
        var tmpCanvas = document.createElement('canvas');
        tmpCanvas.width = w;
        tmpCanvas.height = h;
        tmpCanvas.getContext('2d').putImageData(imgData, 0, 0);
        var pattern = ctx.createPattern(tmpCanvas, 'repeat');
        if ($1) ctx.strokeStyle = pattern;
        else ctx.fillStyle = pattern;
    }, canvasId.c_str(), forStroke ? 1 : 0, w, h, rgb, alpha);
}

void wxWasmDCImpl::ApplyPen()
{
    if (m_canvasId.empty()) return;
    wxString col = m_penColour.GetAsString(wxC2S_CSS_SYNTAX);

    if ( m_penStyle == wxPENSTYLE_STIPPLE && m_penStipple.IsOk() )
    {
        // Stippled pen: the stroke style is a repeating pattern made from
        // the stipple bitmap pixels.
        wxImage image = m_penStipple.ConvertToImage();
        if ( !image.IsOk() )
            return;
        ApplyImagePattern(m_canvasId, image, true);
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            ctx.lineWidth = $1;
            ctx.setLineDash([]);
        }, m_canvasId.c_str(), m_penWidth);
        return;
    }

    // The dash pattern is passed as a pointer to the int copy kept in
    // m_penDashes (empty when the pen is solid, which resets setLineDash).
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.strokeStyle = UTF8ToString($1);
        ctx.lineWidth = $2;
        var dashes = [];
        for (var i = 0; i < $4; i++) dashes.push(HEAP32[($3 >> 2) + i]);
        ctx.setLineDash(dashes);
    }, m_canvasId.c_str(), col.ToUTF8().data(), m_penWidth,
       m_penDashes.empty() ? nullptr : m_penDashes.data(),
       (int)m_penDashes.size());
}

void wxWasmDCImpl::ApplyBrush()
{
    if (m_canvasId.empty()) return;
    wxString col = m_brushColour.GetAsString(wxC2S_CSS_SYNTAX);

    if ( m_brushStyle == wxBRUSHSTYLE_STIPPLE && m_brushStipple.IsOk() )
    {
        // Stippled brush: the fill style is a repeating pattern made from
        // the stipple bitmap pixels.
        wxImage image = m_brushStipple.ConvertToImage();
        if ( !image.IsOk() )
            return;
        ApplyImagePattern(m_canvasId, image, false);
        return;
    }

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
    wxString fontSpec = GetCSSFontSpec(m_font);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.font = UTF8ToString($1);
    }, m_canvasId.c_str(), fontSpec.ToUTF8().data());
}

void wxWasmDCImpl::ApplyTextColour()
{
    if (m_canvasId.empty()) return;
    // NB: use the wxDCImpl member, kept in sync by SetTextForeground().
    wxString col = m_textForegroundColour.IsOk()
                       ? m_textForegroundColour.GetAsString(wxC2S_CSS_SYNTAX)
                       : wxString("#000000");
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
    if (pen.IsOk())
    {
        m_penColour = pen.GetColour();
        m_penWidth = pen.GetWidth();
        if (m_penWidth < 1) m_penWidth = 1;
        m_penStyle = pen.GetStyle();

        // Copy the dash pattern: wxPen only stores a pointer to the
        // caller-owned array, so it can't be kept around directly.
        m_penDashes.clear();
        wxDash *dashes = nullptr;
        const int dashCount = pen.GetDashes(&dashes);
        for (int i = 0; i < dashCount; ++i)
            m_penDashes.push_back(dashes[i]);

        wxBitmap *stipple = pen.GetStipple();
        m_penStipple = stipple ? *stipple : wxBitmap();
    }
    else
    {
        // An invalid pen draws nothing (same as wxPENSTYLE_TRANSPARENT).
        m_penStyle = wxPENSTYLE_TRANSPARENT;
        m_penDashes.clear();
        m_penStipple = wxBitmap();
    }
    ApplyPen();
}

void wxWasmDCImpl::SetBrush(const wxBrush& brush)
{
    // An invalid brush fills nothing (same as wxBRUSHSTYLE_TRANSPARENT).
    m_brushStyle = brush.IsOk() ? brush.GetStyle() : wxBRUSHSTYLE_TRANSPARENT;
    if (brush.IsOk())
    {
        m_brushColour = brush.GetColour();

        // GetStipple() returns a caller-owned copy, take it over directly.
        m_brushStipple = wxNullBitmap;
        wxBitmap *stipple = brush.GetStipple();
        if ( stipple )
        {
            if ( stipple->IsOk() )
                m_brushStipple = *stipple;
            delete stipple;
        }
    }
    ApplyBrush();
}

void wxWasmDCImpl::SetBackground(const wxBrush& brush)
{
    if (brush.IsOk())
        m_backgroundColour = brush.GetColour();
}

void wxWasmDCImpl::SetBackgroundMode(int mode)
{
    m_backgroundMode = mode;
}

#if wxUSE_PALETTE
void wxWasmDCImpl::SetPalette(const wxPalette& WXUNUSED(palette))
{
}
#endif // wxUSE_PALETTE

void wxWasmDCImpl::SetLogicalFunction(wxRasterOperationMode function)
{
    // Only recorded here; EnsureCanvasCreated() maps it onto the canvas
    // globalCompositeOperation before every drawing operation.
    m_logicalFunction = function;
}

wxCoord wxWasmDCImpl::GetCharHeight() const
{
    wxString fontSpec = GetCSSFontSpec(m_font);
    double h = wxWasmMeasureCharHeight(fontSpec.ToUTF8().data());
    return wxCoord(h / m_scaleY);
}

wxCoord wxWasmDCImpl::GetCharWidth() const
{
    wxString fontSpec = GetCSSFontSpec(m_font);
    double w = wxWasmMeasureCharWidth(fontSpec.ToUTF8().data());
    return wxCoord(w / m_scaleX);
}

void wxWasmDCImpl::DoGetTextExtent(const wxString& string,
                                wxCoord *x, wxCoord *y,
                                wxCoord *descent, wxCoord *externalLeading,
                                const wxFont *theFont) const
{
    wxFont font = (theFont && theFont->IsOk()) ? *theFont : m_font;
    wxString fontSpec = GetCSSFontSpec(font);
    const char* fontCStr = fontSpec.ToUTF8().data();

    if (x)
    {
        double w = wxWasmMeasureTextWidth(fontCStr, string.ToUTF8().data());
        *x = wxCoord(w / m_scaleX);
    }
    if (y)
    {
        double h = wxWasmMeasureCharHeight(fontCStr);
        *y = wxCoord(h / m_scaleY);
    }
    if (descent)
    {
        double d = wxWasmMeasureDescent(fontCStr);
        *descent = wxCoord(d / m_scaleY);
    }
    if (externalLeading)
        *externalLeading = 0;
}

void wxWasmDCImpl::Clear()
{
    EnsureCanvasCreated();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        // Clear() must erase the whole surface even when a clipping region
        // is active: drop the clip first (EnsureCanvasCreated() re-applies
        // it before the next drawing operation).
        if (canvas._wxClipSaved)
        {
            ctx.restore();
            canvas._wxClipSaved = false;
        }
        ctx.clearRect(0, 0, canvas.width, canvas.height);
    }, m_canvasId.c_str());
}

void wxWasmDCImpl::DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height)
{
    EnsureCanvasCreated();

    // Convert to device coordinates and intersect with the previous clip,
    // if any (same semantics as the base class, whose clip members are
    // private and so cannot be reused here).
    wxRect clipRect(LogicalToDevice(x, y), LogicalToDeviceRel(width, height));
    if ( m_hasClip )
    {
        clipRect.Intersect(m_clipRectDev);
    }
    else
    {
        int dcWidth, dcHeight;
        DoGetSize(&dcWidth, &dcHeight);
        clipRect.Intersect(wxRect(0, 0, dcWidth, dcHeight));
        m_hasClip = true;
    }
    m_clipRectDev = clipRect;

    SetCanvasClip(clipRect.GetLeft(), clipRect.GetTop(),
                  wxMax(clipRect.GetWidth(), 0), wxMax(clipRect.GetHeight(), 0));
}

void wxWasmDCImpl::DoSetDeviceClippingRegion(const wxRegion& region)
{
    EnsureCanvasCreated();

    // The 2D canvas API clips to a path, so a possibly complex region is
    // approximated by its bounding box (documented limitation). The box is
    // already in device coordinates here.
    wxRect clipRect = region.GetBox();
    if ( m_hasClip )
        clipRect.Intersect(m_clipRectDev);
    m_hasClip = true;
    m_clipRectDev = clipRect;

    SetCanvasClip(clipRect.GetLeft(), clipRect.GetTop(),
                  wxMax(clipRect.GetWidth(), 0), wxMax(clipRect.GetHeight(), 0));
}

void wxWasmDCImpl::DestroyClippingRegion()
{
    m_hasClip = false;
    m_clipRectDev = wxRect();

    // Only clear the recorded clip; the context state saved for it is
    // restored by EnsureCanvasCreated() before the next drawing operation.
    if (!m_canvasId.empty())
    {
        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (canvas) canvas._wxClip = null;
        }, m_canvasId.c_str());
    }
}

bool wxWasmDCImpl::DoGetClippingRect(wxRect& rect) const
{
    if ( m_hasClip )
    {
        // Return the clip box in logical coordinates, as the base class does.
        rect = wxRect(DeviceToLogical(m_clipRectDev.GetLeft(),
                                      m_clipRectDev.GetTop()),
                      DeviceToLogicalRel(m_clipRectDev.GetWidth(),
                                         m_clipRectDev.GetHeight()));
        return true;
    }

    // No active clipping region: return the whole DC area (same as the
    // base class, whose GetLogicalArea() helper is private).
    const wxSize size = GetSize();
    rect = wxRect(DeviceToLogical(0, 0), DeviceToLogicalRel(size.x, size.y));
    return false;
}

bool wxWasmDCImpl::DoFloodFill(wxCoord x, wxCoord y,
                             const wxColour& col,
                             wxFloodFillStyle style)
{
    EnsureCanvasCreated();

    // Scanline stack fill over a single read-back of the whole canvas. This
    // is O(width*height) in both time and memory (the ImageData buffer plus
    // the visited mask), so it should be reserved for small areas. NB:
    // putImageData() bypasses the canvas state, so the fill ignores any
    // active clipping region and the current logical function.
    const int ok = EM_ASM_INT({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return 0;
        var w = canvas.width;
        var h = canvas.height;
        var sx = $1;
        var sy = $2;
        if (sx < 0 || sy < 0 || sx >= w || sy >= h) return 0;
        var ctx = canvas.getContext('2d');
        var img = ctx.getImageData(0, 0, w, h);
        var data = img.data;
        // Fill colour (current brush) and comparison colour (the surface
        // colour for wxFLOOD_SURFACE, the border colour for wxFLOOD_BORDER).
        // The comparison is on RGB only: the alpha channel is ignored.
        var fr = $3;
        var fg = $4;
        var fb = $5;
        var fa = $6;
        var cr = $7;
        var cg = $8;
        var cb = $9;
        var borderMode = $10;
        var visited = new Uint8Array(w * h);
        function canFill(px, py) {
            var p = py * w + px;
            if (visited[p]) return false;
            var i = p * 4;
            var same = data[i] === cr && data[i + 1] === cg && data[i + 2] === cb;
            return borderMode ? !same : same;
        }
        if (!canFill(sx, sy)) return 0;
        // NB: no commas outside parentheses (they would be parsed as macro
        // argument separators by EM_ASM), so the stack is seeded with two
        // pushes instead of an array literal.
        var stack = [];
        stack.push(sx);
        stack.push(sy);
        while (stack.length > 0) {
            var cy = stack.pop();
            var cx = stack.pop();
            var lx = cx;
            while (lx >= 0 && canFill(lx, cy)) lx--;
            lx++;
            var spanUp = false;
            var spanDown = false;
            while (lx < w && canFill(lx, cy)) {
                var p = cy * w + lx;
                visited[p] = 1;
                var i = p * 4;
                data[i] = fr;
                data[i + 1] = fg;
                data[i + 2] = fb;
                data[i + 3] = fa;
                if (cy > 0) {
                    if (canFill(lx, cy - 1)) {
                        if (!spanUp) { stack.push(lx, cy - 1); spanUp = true; }
                    } else spanUp = false;
                }
                if (cy < h - 1) {
                    if (canFill(lx, cy + 1)) {
                        if (!spanDown) { stack.push(lx, cy + 1); spanDown = true; }
                    } else spanDown = false;
                }
                lx++;
            }
        }
        ctx.putImageData(img, 0, 0);
        return 1;
    }, m_canvasId.c_str(), x, y,
       m_brushColour.Red(), m_brushColour.Green(), m_brushColour.Blue(),
       m_brushColour.Alpha(), col.Red(), col.Green(), col.Blue(),
       style == wxFLOOD_BORDER ? 1 : 0);
    return ok != 0;
}

bool wxWasmDCImpl::DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const
{
    if (!col || m_canvasId.empty()) return false;
    double packed = wxWasmGetPixel(m_canvasId.c_str(), x, y);
    if (packed < 0) return false;
    unsigned int rgba = (unsigned int)packed;
    *col = wxColour((unsigned char)(rgba & 0xff),
                    (unsigned char)((rgba >> 8) & 0xff),
                    (unsigned char)((rgba >> 16) & 0xff),
                    (unsigned char)((rgba >> 24) & 0xff));
    return true;
}

void wxWasmDCImpl::DoDrawPoint(wxCoord x, wxCoord y)
{
    if (IsPenTransparent()) return;
    EnsureCanvasCreated();
    ApplyPen();
    // fillRect() uses fillStyle, so set it to the pen colour too.
    wxString col = m_penColour.GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.fillStyle = UTF8ToString($3);
        ctx.fillRect($1, $2, 1, 1);
    }, m_canvasId.c_str(), x, y, col.ToUTF8().data());
}

void wxWasmDCImpl::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    if (IsPenTransparent()) return;
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
        // Stroke the arc itself, then fill the sector bounded by the arc
        // and the two radii.
        if ($6) ctx.stroke();
        ctx.lineTo($1, $2);
        ctx.closePath();
        if ($7) ctx.fill();
    }, m_canvasId.c_str(), xc, yc, radius, startAngle, endAngle,
       IsPenTransparent() ? 0 : 1, IsBrushTransparent() ? 0 : 1);
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
    // wx angles are in degrees measured counter-clockwise from 3 o'clock,
    // while the canvas API takes radians measured clockwise (the y axis
    // points down in both systems, so negating the angles maps one
    // convention onto the other, as DoDrawArc() does with atan2()). The
    // arc is then traced in the counter-clockwise direction to keep the
    // wx "from sa to ea counter-clockwise" semantics.
    double startRad = -sa * M_PI / 180.0;
    double endRad = -ea * M_PI / 180.0;
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.ellipse($1, $2, $3, $4, 0, $5, $6, true);
        if ($7) ctx.stroke();
        if ($8) ctx.fill();
    }, m_canvasId.c_str(), cx, cy, rx, ry, startRad, endRad,
       IsPenTransparent() ? 0 : 1, IsBrushTransparent() ? 0 : 1);
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
        if ($5) ctx.fillRect($1, $2, $3, $4);
        if ($6) ctx.strokeRect($1, $2, $3, $4);
    }, m_canvasId.c_str(), x, y, width, height,
       IsBrushTransparent() ? 0 : 1, IsPenTransparent() ? 0 : 1);
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
        if ($6) ctx.fill();
        if ($7) ctx.stroke();
    }, m_canvasId.c_str(), x, y, width, height, radius,
       IsBrushTransparent() ? 0 : 1, IsPenTransparent() ? 0 : 1);
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
        if ($5) ctx.fill();
        if ($6) ctx.stroke();
    }, m_canvasId.c_str(), cx, cy, rx, ry,
       IsBrushTransparent() ? 0 : 1, IsPenTransparent() ? 0 : 1);
}

void wxWasmDCImpl::DoCrossHair(wxCoord x, wxCoord y)
{
    if (IsPenTransparent()) return;
    EnsureCanvasCreated();
    ApplyPen();
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.beginPath();
        ctx.moveTo(0, $2);
        ctx.lineTo(canvas.width, $2);
        ctx.moveTo($1, 0);
        ctx.lineTo($1, canvas.height);
        ctx.stroke();
    }, m_canvasId.c_str(), x, y);
}

void wxWasmDCImpl::DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y)
{
    // In this port wxIcon is the generic icon class (wx/generic/icon.h)
    // which simply derives from wxBitmap, so drawing an icon is exactly the
    // same as drawing its bitmap with its mask, if any.
    DoDrawBitmap(icon, x, y, true);
}

void wxWasmDCImpl::DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y, bool useMask)
{
    if (!bmp.IsOk()) return;
    wxImage image = bmp.ConvertToImage();
    if (!image.IsOk()) return;
    EnsureCanvasCreated();
    int w = image.GetWidth();
    int h = image.GetHeight();
    unsigned char* rgb = image.GetData();
    // ConvertToImage() folds the bitmap mask into the image alpha channel;
    // use that alpha only when asked to (useMask), as the GTK port does,
    // while real alpha is always honoured.
    const bool useAlpha = image.HasAlpha() && (bmp.HasAlpha() || useMask);
    unsigned char* alpha = useAlpha ? image.GetAlpha() : nullptr;
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
    wxString fontSpec = GetCSSFontSpec(m_font);
    wxCharBuffer fontCStr = fontSpec.ToUTF8();
    double ascent = wxWasmMeasureAscent(fontCStr.data());
    // With an opaque background mode, paint the background brush behind the
    // text first.
    bool solidBg = (m_backgroundMode == wxSOLID);
    double width = 0, descent = 0;
    wxString bgCol;
    if (solidBg)
    {
        wxCharBuffer textCStr = text.ToUTF8();
        width = wxWasmMeasureTextWidth(fontCStr.data(), textCStr.data());
        descent = wxWasmMeasureDescent(fontCStr.data());
        bgCol = m_backgroundColour.GetAsString(wxC2S_CSS_SYNTAX);
    }
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        if ($4) {
            var prevFill = ctx.fillStyle;
            ctx.fillStyle = UTF8ToString($5);
            ctx.fillRect($2, $6, $7, $8);
            ctx.fillStyle = prevFill;
        }
        ctx.fillText(UTF8ToString($1), $2, $3);
    }, m_canvasId.c_str(), text.ToUTF8().data(), x, y + (wxCoord)ascent,
       solidBg ? 1 : 0, bgCol.ToUTF8().data(),
       (double)y, width, ascent + descent);
}

void wxWasmDCImpl::DoDrawRotatedText(const wxString& text,
                                wxCoord x, wxCoord y, double angle)
{
    EnsureCanvasCreated();
    ApplyTextColour();
    ApplyFont();
    wxString fontSpec = GetCSSFontSpec(m_font);
    wxCharBuffer fontCStr = fontSpec.ToUTF8();
    double ascent = wxWasmMeasureAscent(fontCStr.data());
    // With an opaque background mode, paint the background brush behind the
    // text first (in the rotated coordinate system).
    bool solidBg = (m_backgroundMode == wxSOLID);
    double width = 0, descent = 0;
    wxString bgCol;
    if (solidBg)
    {
        wxCharBuffer textCStr = text.ToUTF8();
        width = wxWasmMeasureTextWidth(fontCStr.data(), textCStr.data());
        descent = wxWasmMeasureDescent(fontCStr.data());
        bgCol = m_backgroundColour.GetAsString(wxC2S_CSS_SYNTAX);
    }
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        ctx.save();
        ctx.translate($2, $3);
        ctx.rotate($4);
        if ($5) {
            var prevFill = ctx.fillStyle;
            ctx.fillStyle = UTF8ToString($6);
            ctx.fillRect(0, -$7, $8, $9);
            ctx.fillStyle = prevFill;
        }
        ctx.fillText(UTF8ToString($1), 0, 0);
        ctx.restore();
    }, m_canvasId.c_str(), text.ToUTF8().data(), x, y + (wxCoord)ascent,
       angle * M_PI / 180.0, solidBg ? 1 : 0, bgCol.ToUTF8().data(),
       ascent, width, ascent + descent);
}

bool wxWasmDCImpl::DoBlit(wxCoord xdest, wxCoord ydest,
                    wxCoord width, wxCoord height,
                    wxDC *source,
                    wxCoord xsrc, wxCoord ysrc,
                    wxRasterOperationMode rop,
                    bool WXUNUSED(useMask),
                    wxCoord WXUNUSED(xsrcMask),
                    wxCoord WXUNUSED(ysrcMask))
{
    wxWasmDCImpl* srcImpl = dynamic_cast<wxWasmDCImpl*>(source->GetImpl());
    if (!srcImpl || srcImpl->m_canvasId.empty()) return false;
    EnsureCanvasCreated();
    // Honour the rop argument: temporarily replace the composite operation
    // set from the current logical function and restore it afterwards.
    EM_ASM_({
        var srcCanvas = document.getElementById(UTF8ToString($0));
        var dstCanvas = document.getElementById(UTF8ToString($1));
        if (!srcCanvas || !dstCanvas) return;
        var ctx = dstCanvas.getContext('2d');
        var prevOp = ctx.globalCompositeOperation;
        ctx.globalCompositeOperation = UTF8ToString($2);
        ctx.drawImage(srcCanvas, $3, $4, $5, $6, $7, $8, $5, $6);
        ctx.globalCompositeOperation = prevOp;
    }, srcImpl->m_canvasId.c_str(), m_canvasId.c_str(),
       wxWasmRasterOpToComposite(rop),
       xsrc, ysrc, width, height, xdest, ydest);
    return true;
}

bool wxWasmDCImpl::DoStretchBlit(wxCoord xdest, wxCoord ydest,
                                 wxCoord dstWidth, wxCoord dstHeight,
                                 wxDC *source,
                                 wxCoord xsrc, wxCoord ysrc,
                                 wxCoord srcWidth, wxCoord srcHeight,
                                 wxRasterOperationMode rop,
                                 bool WXUNUSED(useMask),
                                 wxCoord WXUNUSED(xsrcMask),
                                 wxCoord WXUNUSED(ysrcMask))
{
    wxCHECK_MSG( srcWidth && srcHeight && dstWidth && dstHeight, false,
                 wxT("invalid blit size") );
    wxWasmDCImpl* srcImpl = dynamic_cast<wxWasmDCImpl*>(source->GetImpl());
    if (!srcImpl || srcImpl->m_canvasId.empty()) return false;
    EnsureCanvasCreated();
    // The 9-argument drawImage() does the scaling natively (and honors the
    // clip, unlike the base class fallback which fakes it via the DC scale).
    EM_ASM_({
        var srcCanvas = document.getElementById(UTF8ToString($0));
        var dstCanvas = document.getElementById(UTF8ToString($1));
        if (!srcCanvas || !dstCanvas) return;
        var ctx = dstCanvas.getContext('2d');
        var prevOp = ctx.globalCompositeOperation;
        ctx.globalCompositeOperation = UTF8ToString($2);
        ctx.drawImage(srcCanvas, $3, $4, $5, $6, $7, $8, $9, $10);
        ctx.globalCompositeOperation = prevOp;
    }, srcImpl->m_canvasId.c_str(), m_canvasId.c_str(),
       wxWasmRasterOpToComposite(rop),
       xsrc, ysrc, srcWidth, srcHeight, xdest, ydest, dstWidth, dstHeight);
    return true;
}

void wxWasmDCImpl::DoGradientFillLinear(const wxRect& rect,
                                        const wxColour& initialColour,
                                        const wxColour& destColour,
                                        wxDirection nDirection)
{
    if (rect.width <= 0 || rect.height <= 0)
        return;

    EnsureCanvasCreated();

    // Native canvas gradient: a single fillRect() instead of the banded
    // DoDrawRectangle() calls of the generic wxDCImpl implementation, which
    // would cost O(bands) JS calls and rely on the pen being transparent to
    // suppress the band borders.
    const bool isHorizontal = (nDirection & (wxLEFT | wxRIGHT)) != 0;
    const bool isReversed = (nDirection & (wxLEFT | wxUP)) != 0;
    double x1 = isHorizontal ? rect.x + rect.width : rect.x;
    double y1 = isHorizontal ? rect.y : rect.y + rect.height;
    wxString col0 = (isReversed ? destColour : initialColour).GetAsString(wxC2S_CSS_SYNTAX);
    wxString col1 = (isReversed ? initialColour : destColour).GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var grad = ctx.createLinearGradient($1, $2, $3, $4);
        grad.addColorStop(0, UTF8ToString($5));
        grad.addColorStop(1, UTF8ToString($6));
        ctx.fillStyle = grad;
        ctx.fillRect($7, $8, $9, $10);
    }, m_canvasId.c_str(), (double)rect.x, (double)rect.y, x1, y1,
       col0.ToUTF8().data(), col1.ToUTF8().data(),
       rect.x, rect.y, rect.width, rect.height);
}

void wxWasmDCImpl::DoGradientFillConcentric(const wxRect& rect,
                                            const wxColour& initialColour,
                                            const wxColour& destColour,
                                            const wxPoint& circleCenter)
{
    if (rect.width <= 0 || rect.height <= 0)
        return;

    EnsureCanvasCreated();

    // Native radial gradient: the generic fallback paints the rect pixel by
    // pixel through DoDrawPoint(), i.e. O(width*height) JS calls here. The
    // canvas gradient pads with the last stop colour beyond the end circle,
    // which is exactly the wx semantics (destColour at the rect borders).
    double cx = rect.x + circleCenter.x;
    double cy = rect.y + circleCenter.y;
    double radius = wxMin(rect.width, rect.height) / 2.0;
    wxString col0 = initialColour.GetAsString(wxC2S_CSS_SYNTAX);
    wxString col1 = destColour.GetAsString(wxC2S_CSS_SYNTAX);
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var grad = ctx.createRadialGradient($1, $2, 0, $1, $2, $3);
        grad.addColorStop(0, UTF8ToString($4));
        grad.addColorStop(1, UTF8ToString($5));
        ctx.fillStyle = grad;
        ctx.fillRect($6, $7, $8, $9);
    }, m_canvasId.c_str(), cx, cy, radius,
       col0.ToUTF8().data(), col1.ToUTF8().data(),
       rect.x, rect.y, rect.width, rect.height);
}

void wxWasmDCImpl::DoDrawLines(int n, const wxPoint points[],
                            wxCoord xoffset, wxCoord yoffset)
{
    if (n < 2 || IsPenTransparent()) return;
    EnsureCanvasCreated();
    ApplyPen();
    // All the points are read in a single JS call: a wxPoint is a pair of
    // ints, so the array is reinterpreted as flat int pairs in HEAP32.
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var ptr = $1 >> 2;
        var xoff = $3;
        var yoff = $4;
        ctx.beginPath();
        ctx.moveTo(HEAP32[ptr] + xoff, HEAP32[ptr + 1] + yoff);
        for (var i = 1; i < $2; i++)
            ctx.lineTo(HEAP32[ptr + 2 * i] + xoff, HEAP32[ptr + 2 * i + 1] + yoff);
        ctx.stroke();
    }, m_canvasId.c_str(), points, n, xoffset, yoffset);
}

void wxWasmDCImpl::DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle)
{
    if (n < 2) return;
    EnsureCanvasCreated();
    ApplyPen();
    ApplyBrush();
    // All the points are read in a single JS call: a wxPoint is a pair of
    // ints, so the array is reinterpreted as flat int pairs in HEAP32.
    EM_ASM_({
        var canvas = document.getElementById(UTF8ToString($0));
        if (!canvas) return;
        var ctx = canvas.getContext('2d');
        var ptr = $1 >> 2;
        var xoff = $3;
        var yoff = $4;
        ctx.beginPath();
        ctx.moveTo(HEAP32[ptr] + xoff, HEAP32[ptr + 1] + yoff);
        for (var i = 1; i < $2; i++)
            ctx.lineTo(HEAP32[ptr + 2 * i] + xoff, HEAP32[ptr + 2 * i + 1] + yoff);
        ctx.closePath();
        if ($5) ctx.fill($6 ? 'evenodd' : 'nonzero');
        if ($7) ctx.stroke();
    }, m_canvasId.c_str(), points, n, xoffset, yoffset,
       IsBrushTransparent() ? 0 : 1,
       fillStyle == wxODDEVEN_RULE ? 1 : 0,
       IsPenTransparent() ? 0 : 1);
}

void wxWasmDCImpl::ComputeScaleAndOrigin()
{
    wxDCImpl::ComputeScaleAndOrigin();
}

void wxWasmDCImpl::ApplyRasterColourOp()
{
}