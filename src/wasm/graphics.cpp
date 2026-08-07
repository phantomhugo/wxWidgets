/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/graphics.cpp
// Purpose:     wxGraphicsContext backend over the browser Canvas 2D API
// Author:      Hugo Armando Castellanos Morales
// Created:     07.08.26
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_GRAPHICS_CONTEXT

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
    #include "wx/colour.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/font.h"
    #include "wx/icon.h"
    #include "wx/image.h"
    #include "wx/log.h"
    #include "wx/math.h"
    #include "wx/region.h"
    #include "wx/window.h"
#endif // WX_PRECOMP

#include "wx/graphics.h"
#include "wx/private/graphics.h"
#include "wx/wasm/dc.h"

#include <emscripten.h>

#include <cmath>
#include <string>
#include <vector>

// ----------------------------------------------------------------------------
// Shared helpers implemented in dc.cpp (plain C++ functions, not EM_JS, so
// they can be called from this translation unit).
// ----------------------------------------------------------------------------

extern double wxWasmMeasureTextWidth(const char* fontSpec, const char* text);
extern double wxWasmMeasureAscent(const char* fontSpec);
extern double wxWasmMeasureDescent(const char* fontSpec);

// ----------------------------------------------------------------------------
// CSS font spec helper (same logic as GetCSSFontSpec() in dc.cpp, which is
// file-local there).
// ----------------------------------------------------------------------------

static wxString wxWasmGraphicsFontSpec(const wxFont& font)
{
    if (!font.IsOk())
        return wxString("12px sans-serif");

    wxString spec;

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

    spec += wxString::Format("%d ", font.GetNumericWeight());

    double size = font.GetFractionalPointSize();
    if (size <= 0)
        size = 12;
    spec += wxString::Format("%.1fpt ", size);

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

// ----------------------------------------------------------------------------
// wxCompositionMode -> globalCompositeOperation mapping. The canvas 2D API
// implements the full Porter-Duff set, so the mapping is exact except for
// wxCOMPOSITION_ADD (mapped to 'lighter', additive clamped) and
// wxCOMPOSITION_DIFF ('difference').
// ----------------------------------------------------------------------------

static const char* wxWasmCompositionToCanvas(wxCompositionMode op)
{
    switch (op)
    {
        case wxCOMPOSITION_CLEAR:     return "clear";
        case wxCOMPOSITION_SOURCE:    return "copy";
        case wxCOMPOSITION_OVER:      return "source-over";
        case wxCOMPOSITION_IN:        return "source-in";
        case wxCOMPOSITION_OUT:       return "source-out";
        case wxCOMPOSITION_ATOP:      return "source-atop";
        case wxCOMPOSITION_DEST:      return "destination";
        case wxCOMPOSITION_DEST_OVER: return "destination-over";
        case wxCOMPOSITION_DEST_IN:   return "destination-in";
        case wxCOMPOSITION_DEST_OUT:  return "destination-out";
        case wxCOMPOSITION_DEST_ATOP: return "destination-atop";
        case wxCOMPOSITION_XOR:       return "xor";
        case wxCOMPOSITION_ADD:       return "lighter";
        case wxCOMPOSITION_DIFF:      return "difference";
        default:                      return "source-over";
    }
}

// ----------------------------------------------------------------------------
// wxWasmGraphicsMatrixData: plain affine matrix (a b c d tx ty) with the same
// layout and semantics as cairo/DOMMatrix:
//   x' = a*x + c*y + tx
//   y' = b*x + d*y + ty
// ----------------------------------------------------------------------------

class wxWasmGraphicsMatrixData : public wxGraphicsMatrixData
{
public:
    wxWasmGraphicsMatrixData(wxGraphicsRenderer* renderer)
        : wxGraphicsMatrixData(renderer),
          m_a(1.0), m_b(0.0), m_c(0.0), m_d(1.0), m_tx(0.0), m_ty(0.0)
    {
    }

    // wxObjectRefData is not copyable, so the copy ctor (needed by Clone())
    // re-initializes the base with the renderer of the source object.
    wxWasmGraphicsMatrixData(const wxWasmGraphicsMatrixData& other)
        : wxGraphicsMatrixData(other.GetRenderer()),
          m_a(other.m_a), m_b(other.m_b), m_c(other.m_c),
          m_d(other.m_d), m_tx(other.m_tx), m_ty(other.m_ty)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsMatrixData(*this);
    }

    // this = this * t (t is applied first, as in cairo_matrix_multiply)
    virtual void Concat( const wxGraphicsMatrixData *t ) override
    {
        wxDouble ta, tb, tc, td, ttx, tty;
        t->Get(&ta, &tb, &tc, &td, &ttx, &tty);
        PostMultiply(ta, tb, tc, td, ttx, tty);
    }

    virtual void Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0) override
    {
        m_a = a; m_b = b; m_c = c; m_d = d; m_tx = tx; m_ty = ty;
    }

    virtual void Get(wxDouble* a=nullptr, wxDouble* b=nullptr, wxDouble* c=nullptr,
                     wxDouble* d=nullptr, wxDouble* tx=nullptr, wxDouble* ty=nullptr) const override
    {
        if (a)  *a  = m_a;
        if (b)  *b  = m_b;
        if (c)  *c  = m_c;
        if (d)  *d  = m_d;
        if (tx) *tx = m_tx;
        if (ty) *ty = m_ty;
    }

    virtual void Invert() override
    {
        const wxDouble det = m_a * m_d - m_b * m_c;
        if ( det == 0.0 )
            return;

        const wxDouble a = m_d / det;
        const wxDouble b = -m_b / det;
        const wxDouble c = -m_c / det;
        const wxDouble d = m_a / det;
        const wxDouble tx = (m_c * m_ty - m_d * m_tx) / det;
        const wxDouble ty = (m_b * m_tx - m_a * m_ty) / det;
        Set(a, b, c, d, tx, ty);
    }

    virtual bool IsEqual( const wxGraphicsMatrixData* t) const override
    {
        wxDouble a, b, c, d, tx, ty;
        t->Get(&a, &b, &c, &d, &tx, &ty);
        return m_a == a && m_b == b && m_c == c &&
               m_d == d && m_tx == tx && m_ty == ty;
    }

    virtual bool IsIdentity() const override
    {
        return m_a == 1.0 && m_b == 0.0 && m_c == 0.0 &&
               m_d == 1.0 && m_tx == 0.0 && m_ty == 0.0;
    }

    virtual void Translate( wxDouble dx , wxDouble dy ) override
    {
        PostMultiply(1.0, 0.0, 0.0, 1.0, dx, dy);
    }

    virtual void Scale( wxDouble xScale , wxDouble yScale ) override
    {
        PostMultiply(xScale, 0.0, 0.0, yScale, 0.0, 0.0);
    }

    virtual void Rotate( wxDouble angle ) override
    {
        const wxDouble s = sin(angle);
        const wxDouble c = cos(angle);
        PostMultiply(c, s, -s, c, 0.0, 0.0);
    }

    virtual void TransformPoint( wxDouble *x, wxDouble *y ) const override
    {
        const wxDouble nx = m_a * *x + m_c * *y + m_tx;
        const wxDouble ny = m_b * *x + m_d * *y + m_ty;
        *x = nx;
        *y = ny;
    }

    virtual void TransformDistance( wxDouble *dx, wxDouble *dy ) const override
    {
        const wxDouble nx = m_a * *dx + m_c * *dy;
        const wxDouble ny = m_b * *dx + m_d * *dy;
        *dx = nx;
        *dy = ny;
    }

    virtual void * GetNativeMatrix() const override
    {
        return nullptr;
    }

private:
    void PostMultiply(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                      wxDouble tx, wxDouble ty)
    {
        Set(m_a * a + m_c * b,
            m_b * a + m_d * b,
            m_a * c + m_c * d,
            m_b * c + m_d * d,
            m_a * tx + m_c * ty + m_tx,
            m_b * tx + m_d * ty + m_ty);
    }

    wxDouble m_a, m_b, m_c, m_d, m_tx, m_ty;
};

// ----------------------------------------------------------------------------
// wxWasmGraphicsPathData: records the path commands in a flat list that is
// serialized to a HEAPF64 buffer (7 doubles per command: op + 6 values) and
// replayed into a JS Path2D in a single EM_ASM call (varargs would hit the
// 16-argument EM_ASM limit for anything but trivial paths).
//
// Angle convention: wxGraphicsPath angles are in radians, 0 along +x and
// growing towards +y (downwards), i.e. the same convention as canvas arc(),
// so the angles are passed through unchanged and the wx "clockwise" flag maps
// to the negated canvas "anticlockwise" one.
// ----------------------------------------------------------------------------

class wxWasmGraphicsPathData : public wxGraphicsPathData
{
public:
    enum Op
    {
        OpMove = 1,   // x y
        OpLine = 2,   // x y
        OpCubic = 3,  // cx1 cy1 cx2 cy2 x y
        OpQuad = 4,   // cx cy x y
        OpArc = 5,    // x y r startAngle endAngle anticlockwise(0/1)
        OpClose = 6,  // -
        OpRect = 7,   // x y w h
        OpEllipse = 8,// x y w h
        OpRoundRect = 9 // x y w h radius
    };

    struct Cmd
    {
        int op;
        wxDouble v[6];
    };

    explicit wxWasmGraphicsPathData(wxGraphicsRenderer* renderer)
        : wxGraphicsPathData(renderer),
          m_hasCurrent(false),
          m_curX(0.0), m_curY(0.0),
          m_subStartX(0.0), m_subStartY(0.0)
    {
    }

    // wxObjectRefData is not copyable, so the copy ctor (needed by Clone())
    // re-initializes the base with the renderer of the source object.
    wxWasmGraphicsPathData(const wxWasmGraphicsPathData& other)
        : wxGraphicsPathData(other.GetRenderer()),
          m_cmds(other.m_cmds),
          m_hasCurrent(other.m_hasCurrent),
          m_curX(other.m_curX), m_curY(other.m_curY),
          m_subStartX(other.m_subStartX), m_subStartY(other.m_subStartY)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsPathData(*this);
    }

    virtual void MoveToPoint( wxDouble x, wxDouble y ) override
    {
        Cmd cmd;
        cmd.op = OpMove;
        cmd.v[0] = x; cmd.v[1] = y;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
        m_subStartX = x; m_subStartY = y;
    }

    virtual void AddLineToPoint( wxDouble x, wxDouble y ) override
    {
        if ( !m_hasCurrent )
        {
            MoveToPoint(x, y);
            return;
        }
        Cmd cmd;
        cmd.op = OpLine;
        cmd.v[0] = x; cmd.v[1] = y;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
    }

    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1,
                                  wxDouble cx2, wxDouble cy2,
                                  wxDouble x, wxDouble y ) override
    {
        Cmd cmd;
        cmd.op = OpCubic;
        cmd.v[0] = cx1; cmd.v[1] = cy1;
        cmd.v[2] = cx2; cmd.v[3] = cy2;
        cmd.v[4] = x;   cmd.v[5] = y;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
    }

    virtual void AddQuadCurveToPoint( wxDouble cx, wxDouble cy,
                                      wxDouble x, wxDouble y ) override
    {
        Cmd cmd;
        cmd.op = OpQuad;
        cmd.v[0] = cx; cmd.v[1] = cy;
        cmd.v[2] = x;  cmd.v[3] = y;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
    }

    virtual void AddPath( const wxGraphicsPathData* path ) override
    {
        const wxWasmGraphicsPathData* p =
            static_cast<const wxWasmGraphicsPathData*>(path);
        for ( const Cmd& cmd : p->m_cmds )
        {
            switch ( cmd.op )
            {
                case OpMove:     MoveToPoint(cmd.v[0], cmd.v[1]); break;
                case OpLine:     AddLineToPoint(cmd.v[0], cmd.v[1]); break;
                case OpCubic:    AddCurveToPoint(cmd.v[0], cmd.v[1], cmd.v[2],
                                                 cmd.v[3], cmd.v[4], cmd.v[5]); break;
                case OpQuad:     AddQuadCurveToPoint(cmd.v[0], cmd.v[1],
                                                     cmd.v[2], cmd.v[3]); break;
                case OpArc:      AddArc(cmd.v[0], cmd.v[1], cmd.v[2], cmd.v[3],
                                        cmd.v[4], cmd.v[5] == 0.0); break;
                case OpClose:    CloseSubpath(); break;
                case OpRect:     AddRectangle(cmd.v[0], cmd.v[1], cmd.v[2], cmd.v[3]); break;
                case OpEllipse:  AddEllipse(cmd.v[0], cmd.v[1], cmd.v[2], cmd.v[3]); break;
                case OpRoundRect: AddRoundedRectangle(cmd.v[0], cmd.v[1], cmd.v[2],
                                                      cmd.v[3], cmd.v[4]); break;
            }
        }
    }

    virtual void CloseSubpath() override
    {
        Cmd cmd;
        cmd.op = OpClose;
        m_cmds.push_back(cmd);
        m_curX = m_subStartX;
        m_curY = m_subStartY;
    }

    virtual void GetCurrentPoint( wxDouble* x, wxDouble* y) const override
    {
        if (x) *x = m_hasCurrent ? m_curX : 0.0;
        if (y) *y = m_hasCurrent ? m_curY : 0.0;
    }

    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r,
                         wxDouble startAngle, wxDouble endAngle,
                         bool clockwise ) override
    {
        Cmd cmd;
        cmd.op = OpArc;
        cmd.v[0] = x; cmd.v[1] = y; cmd.v[2] = r;
        cmd.v[3] = startAngle; cmd.v[4] = endAngle;
        cmd.v[5] = clockwise ? 0.0 : 1.0; // canvas "anticlockwise" flag
        m_cmds.push_back(cmd);
        if ( !m_hasCurrent )
        {
            m_subStartX = x + r * cos(startAngle);
            m_subStartY = y + r * sin(startAngle);
            m_hasCurrent = true;
        }
        SetCurrent(x + r * cos(endAngle), y + r * sin(endAngle));
    }

    // The convenience functions are overridden to store single native
    // commands (canvas rect/ellipse/roundRect) instead of being decomposed
    // into primitives by the base class.

    virtual void AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) override
    {
        Cmd cmd;
        cmd.op = OpRect;
        cmd.v[0] = x; cmd.v[1] = y; cmd.v[2] = w; cmd.v[3] = h;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
        m_subStartX = x; m_subStartY = y;
    }

    virtual void AddEllipse( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) override
    {
        Cmd cmd;
        cmd.op = OpEllipse;
        cmd.v[0] = x; cmd.v[1] = y; cmd.v[2] = w; cmd.v[3] = h;
        m_cmds.push_back(cmd);
        SetCurrent(x + w, y + h / 2);
        m_subStartX = x + w; m_subStartY = y + h / 2;
    }

    virtual void AddRoundedRectangle( wxDouble x, wxDouble y, wxDouble w,
                                      wxDouble h, wxDouble radius ) override
    {
        if ( radius == 0 )
        {
            AddRectangle(x, y, w, h);
            return;
        }
        Cmd cmd;
        cmd.op = OpRoundRect;
        cmd.v[0] = x; cmd.v[1] = y; cmd.v[2] = w; cmd.v[3] = h;
        cmd.v[4] = radius;
        m_cmds.push_back(cmd);
        SetCurrent(x, y);
        m_subStartX = x; m_subStartY = y;
    }

    virtual void * GetNativePath() const override
    {
        return const_cast<std::vector<Cmd>*>(&m_cmds);
    }

    virtual void UnGetNativePath(void* WXUNUSED(p)) const override
    {
    }

    // Approximation: the whole path is flattened to polylines and the points
    // are transformed (curves/arcs become polygon chains). This is exact for
    // straight-line paths and visually indistinguishable for the rest at
    // normal scales.
    virtual void Transform( const wxGraphicsMatrixData* matrix ) override
    {
        wxDouble a, b, c, d, tx, ty;
        matrix->Get(&a, &b, &c, &d, &tx, &ty);
        if ( a == 1.0 && b == 0.0 && c == 0.0 &&
             d == 1.0 && tx == 0.0 && ty == 0.0 )
            return;

        std::vector<FlatPoly> polys;
        Flatten(polys);

        m_cmds.clear();
        m_hasCurrent = false;
        for ( const FlatPoly& poly : polys )
        {
            if ( poly.pts.empty() )
                continue;
            MoveToPoint(a * poly.pts[0].x + c * poly.pts[0].y + tx,
                        b * poly.pts[0].x + d * poly.pts[0].y + ty);
            for ( size_t i = 1; i < poly.pts.size(); ++i )
                AddLineToPoint(a * poly.pts[i].x + c * poly.pts[i].y + tx,
                               b * poly.pts[i].x + d * poly.pts[i].y + ty);
            if ( poly.closed )
                CloseSubpath();
        }
    }

    virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h) const override
    {
        bool empty = true;
        wxDouble minX = 0, minY = 0, maxX = 0, maxY = 0;

        auto include = [&](wxDouble px, wxDouble py)
        {
            if ( empty )
            {
                minX = maxX = px;
                minY = maxY = py;
                empty = false;
            }
            else
            {
                if ( px < minX ) minX = px;
                if ( px > maxX ) maxX = px;
                if ( py < minY ) minY = py;
                if ( py > maxY ) maxY = py;
            }
        };

        for ( const Cmd& cmd : m_cmds )
        {
            switch ( cmd.op )
            {
                case OpMove:
                case OpLine:
                    include(cmd.v[0], cmd.v[1]);
                    break;
                case OpCubic:
                    // Control points included, as allowed by the API.
                    include(cmd.v[0], cmd.v[1]);
                    include(cmd.v[2], cmd.v[3]);
                    include(cmd.v[4], cmd.v[5]);
                    break;
                case OpQuad:
                    include(cmd.v[0], cmd.v[1]);
                    include(cmd.v[2], cmd.v[3]);
                    break;
                case OpArc:
                    // Conservative: the full circle bounding box.
                    include(cmd.v[0] - cmd.v[2], cmd.v[1] - cmd.v[2]);
                    include(cmd.v[0] + cmd.v[2], cmd.v[1] + cmd.v[2]);
                    break;
                case OpRect:
                case OpEllipse:
                case OpRoundRect:
                    include(cmd.v[0], cmd.v[1]);
                    include(cmd.v[0] + cmd.v[2], cmd.v[1] + cmd.v[3]);
                    break;
                default:
                    break;
            }
        }

        if (x) *x = empty ? 0 : minX;
        if (y) *y = empty ? 0 : minY;
        if (w) *w = empty ? 0 : maxX - minX;
        if (h) *h = empty ? 0 : maxY - minY;
    }

    virtual bool Contains( wxDouble x, wxDouble y,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE) const override
    {
        std::vector<FlatPoly> polys;
        Flatten(polys);

        int crossings = 0;
        int winding = 0;
        for ( const FlatPoly& poly : polys )
        {
            const size_t n = poly.pts.size();
            if ( n < 3 )
                continue;
            for ( size_t i = 0, j = n - 1; i < n; j = i++ )
            {
                const wxDouble xi = poly.pts[i].x, yi = poly.pts[i].y;
                const wxDouble xj = poly.pts[j].x, yj = poly.pts[j].y;
                if ( (yi > y) != (yj > y) )
                {
                    const wxDouble xint = xj + (y - yj) * (xi - xj) / (yi - yj);
                    if ( x < xint )
                    {
                        crossings++;
                        winding += (yi > yj) ? 1 : -1;
                    }
                }
            }
        }

        if ( fillStyle == wxWINDING_RULE )
            return winding != 0;
        return (crossings % 2) != 0;
    }

    // Serializes the commands into a flat double buffer (7 values per
    // command) ready to be read from HEAPF64 in JS.
    void Serialize(std::vector<wxDouble>& buf) const
    {
        buf.clear();
        buf.reserve(m_cmds.size() * 7);
        for ( const Cmd& cmd : m_cmds )
        {
            buf.push_back((wxDouble)cmd.op);
            for ( int i = 0; i < 6; ++i )
                buf.push_back(cmd.v[i]);
        }
    }

    size_t GetCmdCount() const { return m_cmds.size(); }

private:
    struct FlatPoint
    {
        wxDouble x, y;
    };
    struct FlatPoly
    {
        std::vector<FlatPoint> pts;
        bool closed = false;
    };

    void SetCurrent(wxDouble x, wxDouble y)
    {
        m_curX = x;
        m_curY = y;
        m_hasCurrent = true;
    }

    // Approximates the whole path with polygons (used by Transform() and
    // Contains()).
    void Flatten(std::vector<FlatPoly>& polys) const
    {
        FlatPoly cur;
        bool hasCur = false;
        wxDouble cx = 0, cy = 0, sx = 0, sy = 0;

        auto flush = [&]()
        {
            if ( !cur.pts.empty() )
                polys.push_back(cur);
            cur = FlatPoly();
        };
        auto append = [&](wxDouble px, wxDouble py)
        {
            FlatPoint pt;
            pt.x = px; pt.y = py;
            cur.pts.push_back(pt);
        };

        for ( const Cmd& cmd : m_cmds )
        {
            switch ( cmd.op )
            {
                case OpMove:
                    flush();
                    append(cmd.v[0], cmd.v[1]);
                    hasCur = true;
                    cx = sx = cmd.v[0]; cy = sy = cmd.v[1];
                    break;
                case OpLine:
                    if ( !hasCur )
                    {
                        append(cmd.v[0], cmd.v[1]);
                        hasCur = true;
                        cx = sx = cmd.v[0]; cy = sy = cmd.v[1];
                        break;
                    }
                    append(cmd.v[0], cmd.v[1]);
                    cx = cmd.v[0]; cy = cmd.v[1];
                    break;
                case OpCubic:
                {
                    const int n = 24;
                    const wxDouble x0 = cx, y0 = cy;
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble t = (wxDouble)i / n;
                        const wxDouble u = 1 - t;
                        append(u*u*u*x0 + 3*u*u*t*cmd.v[0] + 3*u*t*t*cmd.v[2] + t*t*t*cmd.v[4],
                               u*u*u*y0 + 3*u*u*t*cmd.v[1] + 3*u*t*t*cmd.v[3] + t*t*t*cmd.v[5]);
                    }
                    cx = cmd.v[4]; cy = cmd.v[5];
                    break;
                }
                case OpQuad:
                {
                    const int n = 16;
                    const wxDouble x0 = cx, y0 = cy;
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble t = (wxDouble)i / n;
                        const wxDouble u = 1 - t;
                        append(u*u*x0 + 2*u*t*cmd.v[0] + t*t*cmd.v[2],
                               u*u*y0 + 2*u*t*cmd.v[1] + t*t*cmd.v[3]);
                    }
                    cx = cmd.v[2]; cy = cmd.v[3];
                    break;
                }
                case OpArc:
                {
                    const wxDouble r = cmd.v[2];
                    wxDouble sa = cmd.v[3];
                    wxDouble ea = cmd.v[4];
                    const bool ac = cmd.v[5] != 0.0;
                    // canvas arc() sweep normalization
                    wxDouble sweep;
                    if ( fabs(ea - sa) >= 2 * M_PI )
                        sweep = ac ? -2 * M_PI : 2 * M_PI;
                    else if ( ac )
                    {
                        sweep = ea - sa;
                        while ( sweep > 0 ) sweep -= 2 * M_PI;
                    }
                    else
                    {
                        sweep = ea - sa;
                        while ( sweep < 0 ) sweep += 2 * M_PI;
                    }
                    if ( !hasCur )
                    {
                        append(cmd.v[0] + r * cos(sa), cmd.v[1] + r * sin(sa));
                        hasCur = true;
                        sx = cmd.v[0] + r * cos(sa);
                        sy = cmd.v[1] + r * sin(sa);
                    }
                    else
                    {
                        // arc() implicitly connects the current point with
                        // the start of the arc by a straight line.
                        append(cmd.v[0] + r * cos(sa), cmd.v[1] + r * sin(sa));
                    }
                    const int n = wxMax(4, (int)(fabs(sweep) / (M_PI / 18)));
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble ang = sa + sweep * i / n;
                        append(cmd.v[0] + r * cos(ang), cmd.v[1] + r * sin(ang));
                    }
                    cx = cmd.v[0] + r * cos(sa + sweep);
                    cy = cmd.v[1] + r * sin(sa + sweep);
                    break;
                }
                case OpClose:
                    if ( hasCur )
                    {
                        append(sx, sy);
                        cur.closed = true;
                        cx = sx; cy = sy;
                    }
                    flush();
                    hasCur = false;
                    break;
                case OpRect:
                    flush();
                    append(cmd.v[0], cmd.v[1]);
                    append(cmd.v[0] + cmd.v[2], cmd.v[1]);
                    append(cmd.v[0] + cmd.v[2], cmd.v[1] + cmd.v[3]);
                    append(cmd.v[0], cmd.v[1] + cmd.v[3]);
                    append(cmd.v[0], cmd.v[1]);
                    cur.closed = true;
                    hasCur = true;
                    cx = sx = cmd.v[0]; cy = sy = cmd.v[1];
                    break;
                case OpEllipse:
                {
                    flush();
                    const wxDouble rx = cmd.v[2] / 2;
                    const wxDouble ry = cmd.v[3] / 2;
                    const wxDouble ex = cmd.v[0] + rx;
                    const wxDouble ey = cmd.v[1] + ry;
                    const int n = 48;
                    for ( int i = 0; i <= n; ++i )
                    {
                        const wxDouble ang = 2 * M_PI * i / n;
                        append(ex + rx * cos(ang), ey + ry * sin(ang));
                    }
                    cur.closed = true;
                    hasCur = true;
                    cx = sx = ex + rx; cy = sy = ey;
                    break;
                }
                case OpRoundRect:
                {
                    flush();
                    wxDouble rr = cmd.v[4];
                    const wxDouble maxR = wxMin(cmd.v[2], cmd.v[3]) / 2;
                    if ( rr > maxR ) rr = maxR;
                    const wxDouble x = cmd.v[0], y = cmd.v[1];
                    const wxDouble w = cmd.v[2], h = cmd.v[3];
                    const int n = 8;
                    // top edge left-to-right, then corners clockwise
                    append(x + rr, y);
                    append(x + w - rr, y);
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble ang = -M_PI / 2 + (M_PI / 2) * i / n;
                        append(x + w - rr + rr * cos(ang), y + rr + rr * sin(ang));
                    }
                    append(x + w, y + h - rr);
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble ang = (M_PI / 2) * i / n;
                        append(x + w - rr + rr * cos(ang), y + h - rr + rr * sin(ang));
                    }
                    append(x + rr, y + h);
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble ang = M_PI / 2 + (M_PI / 2) * i / n;
                        append(x + rr + rr * cos(ang), y + h - rr + rr * sin(ang));
                    }
                    append(x, y + rr);
                    for ( int i = 1; i <= n; ++i )
                    {
                        const wxDouble ang = M_PI + (M_PI / 2) * i / n;
                        append(x + rr + rr * cos(ang), y + rr + rr * sin(ang));
                    }
                    append(x + rr, y);
                    cur.closed = true;
                    hasCur = true;
                    cx = sx = x + rr; cy = sy = y;
                    break;
                }
            }
        }
        flush();
    }

    std::vector<Cmd> m_cmds;
    bool m_hasCurrent;
    wxDouble m_curX, m_curY;
    wxDouble m_subStartX, m_subStartY;

    friend class wxWasmGraphicsContext;
};

// ----------------------------------------------------------------------------
// Paint data classes: they simply store the full creation parameters; the
// actual canvas styles/gradients are created per drawing operation (canvas
// gradients live in the current user space, so they must be created after
// the transform is set, i.e. at fill/stroke time).
// ----------------------------------------------------------------------------

class wxWasmGraphicsPenData : public wxGraphicsObjectRefData
{
public:
    wxWasmGraphicsPenData(wxGraphicsRenderer* renderer,
                          const wxGraphicsPenInfo& info)
        : wxGraphicsObjectRefData(renderer),
          m_colour(info.GetColour()),
          m_width(info.GetWidth()),
          m_style(info.GetStyle()),
          m_join(info.GetJoin()),
          m_cap(info.GetCap()),
          m_gradientType(info.GetGradientType()),
          m_x1(info.GetX1()), m_y1(info.GetY1()),
          m_x2(info.GetX2()), m_y2(info.GetY2()),
          m_radius(info.GetRadius()),
          m_stops(info.GetStops()),
          m_stipple(info.GetStipple())
    {
        if ( m_width < 0 )
            m_width = 0;

        // Copy the dash pattern: wxGraphicsPenInfo only stores a pointer to
        // the caller-owned array.
        wxDash* dashes = nullptr;
        const int count = info.GetDashes(&dashes);
        for ( int i = 0; i < count; ++i )
            m_dashes.push_back((wxDouble)dashes[i]);
    }

    // wxObjectRefData is not copyable: see the matrix data class.
    wxWasmGraphicsPenData(const wxWasmGraphicsPenData& other)
        : wxGraphicsObjectRefData(other.GetRenderer()),
          m_colour(other.m_colour),
          m_width(other.m_width),
          m_style(other.m_style),
          m_join(other.m_join),
          m_cap(other.m_cap),
          m_dashes(other.m_dashes),
          m_gradientType(other.m_gradientType),
          m_x1(other.m_x1), m_y1(other.m_y1),
          m_x2(other.m_x2), m_y2(other.m_y2),
          m_radius(other.m_radius),
          m_stops(other.m_stops),
          m_stipple(other.m_stipple)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsPenData(*this);
    }

    bool IsTransparent() const
    {
        return m_style == wxPENSTYLE_TRANSPARENT ||
               m_style == wxPENSTYLE_INVALID;
    }

    wxColour m_colour;
    wxDouble m_width;
    wxPenStyle m_style;
    wxPenJoin m_join;
    wxPenCap m_cap;
    std::vector<wxDouble> m_dashes;
    wxGradientType m_gradientType;
    wxDouble m_x1, m_y1, m_x2, m_y2, m_radius;
    wxGraphicsGradientStops m_stops;
    wxBitmap m_stipple;
};

class wxWasmGraphicsBrushData : public wxGraphicsObjectRefData
{
public:
    // Simple brush from a wxBrush.
    wxWasmGraphicsBrushData(wxGraphicsRenderer* renderer, const wxBrush& brush)
        : wxGraphicsObjectRefData(renderer),
          m_kind(brush.GetStyle() == wxBRUSHSTYLE_TRANSPARENT ||
                 brush.GetStyle() == wxBRUSHSTYLE_INVALID
                     ? KindTransparent : KindSolid),
          m_colour(brush.GetColour()),
          m_x1(0), m_y1(0), m_x2(0), m_y2(0), m_radius(0),
          m_stops(brush.GetColour(), brush.GetColour()),
          m_stipple(brush.GetStipple() ? *brush.GetStipple() : wxBitmap())
    {
        // Hatched and stippled brushes are approximated with their solid
        // colour (documented limitation of the Canvas 2D backend).
    }

    // Gradient brush.
    wxWasmGraphicsBrushData(wxGraphicsRenderer* renderer,
                            wxGradientType type,
                            wxDouble x1, wxDouble y1,
                            wxDouble x2, wxDouble y2, wxDouble radius,
                            const wxGraphicsGradientStops& stops)
        : wxGraphicsObjectRefData(renderer),
          m_kind(type == wxGRADIENT_LINEAR ? KindLinear : KindRadial),
          m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2), m_radius(radius),
          m_stops(stops)
    {
    }

    // wxObjectRefData is not copyable: see the matrix data class.
    wxWasmGraphicsBrushData(const wxWasmGraphicsBrushData& other)
        : wxGraphicsObjectRefData(other.GetRenderer()),
          m_kind(other.m_kind),
          m_colour(other.m_colour),
          m_x1(other.m_x1), m_y1(other.m_y1),
          m_x2(other.m_x2), m_y2(other.m_y2),
          m_radius(other.m_radius),
          m_stops(other.m_stops),
          m_stipple(other.m_stipple)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsBrushData(*this);
    }

    bool IsTransparent() const { return m_kind == KindTransparent; }

    enum Kind
    {
        KindTransparent,
        KindSolid,
        KindLinear,
        KindRadial
    };

    Kind m_kind;
    wxColour m_colour;
    wxDouble m_x1, m_y1, m_x2, m_y2, m_radius;
    wxGraphicsGradientStops m_stops;
    wxBitmap m_stipple;
};

class wxWasmGraphicsFontData : public wxGraphicsObjectRefData
{
public:
    wxWasmGraphicsFontData(wxGraphicsRenderer* renderer,
                           const wxString& cssSpec, const wxColour& col)
        : wxGraphicsObjectRefData(renderer),
          m_cssSpec(cssSpec),
          m_colour(col)
    {
    }

    // wxObjectRefData is not copyable: see the matrix data class.
    wxWasmGraphicsFontData(const wxWasmGraphicsFontData& other)
        : wxGraphicsObjectRefData(other.GetRenderer()),
          m_cssSpec(other.m_cssSpec),
          m_colour(other.m_colour)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsFontData(*this);
    }

    wxString m_cssSpec;
    wxColour m_colour;
};

class wxWasmGraphicsBitmapData : public wxGraphicsBitmapData
{
public:
    wxWasmGraphicsBitmapData(wxGraphicsRenderer* renderer, const wxBitmap& bmp)
        : wxGraphicsBitmapData(renderer),
          m_bitmap(bmp)
    {
    }

    // wxObjectRefData is not copyable: see the matrix data class.
    wxWasmGraphicsBitmapData(const wxWasmGraphicsBitmapData& other)
        : wxGraphicsBitmapData(other.GetRenderer()),
          m_bitmap(other.m_bitmap)
    {
    }

    virtual wxGraphicsObjectRefData* Clone() const override
    {
        return new wxWasmGraphicsBitmapData(*this);
    }

    virtual void* GetNativeBitmap() const override
    {
        return nullptr;
    }

    wxBitmap m_bitmap;
};

// ----------------------------------------------------------------------------
// wxWasmGraphicsContext
//
// Drawing model: the canvas 2D context state is not kept between operations
// (other DCs may share the same canvas element), so every operation is
// bracketed by save()/restore() in JS and re-applies the full state: the
// clip (the DC-level canvas._wxClip plus the graphics clip, both in device
// coordinates), the CTM (setTransform with our own matrix), the composition
// mode and the current layer alpha.
// ----------------------------------------------------------------------------

class wxWasmGraphicsContext : public wxGraphicsContext
{
public:
    wxWasmGraphicsContext(wxGraphicsRenderer* renderer,
                          const std::string& canvasId,
                          wxWindow* window,
                          wxDouble width, wxDouble height)
        : wxGraphicsContext(renderer, window),
          m_canvasId(canvasId),
          m_hasClip(false),
          m_layerAlpha(1.0)
    {
        m_width = width;
        m_height = height;
        m_matrix[0] = 1.0; m_matrix[1] = 0.0;
        m_matrix[2] = 0.0; m_matrix[3] = 1.0;
        m_matrix[4] = 0.0; m_matrix[5] = 0.0;
    }

    // State stack

    virtual void PushState() override
    {
        State st;
        for ( int i = 0; i < 6; ++i )
            st.matrix[i] = m_matrix[i];
        st.hasClip = m_hasClip;
        st.clipDev = m_clipDev;
        st.clipUser = m_clipUser;
        st.antialias = m_antialias;
        st.composition = m_composition;
        st.interpolation = m_interpolation;
        m_stateStack.push_back(st);
    }

    virtual void PopState() override
    {
        if ( m_stateStack.empty() )
            return;
        const State& st = m_stateStack.back();
        for ( int i = 0; i < 6; ++i )
            m_matrix[i] = st.matrix[i];
        m_hasClip = st.hasClip;
        m_clipDev = st.clipDev;
        m_clipUser = st.clipUser;
        m_antialias = st.antialias;
        m_composition = st.composition;
        m_interpolation = st.interpolation;
        m_stateStack.pop_back();
    }

    // Clipping. The clip is stored both in user coordinates (for GetClipBox)
    // and as a device-space bounding box (what is actually applied). A
    // complex region is approximated by its box, as in the DC clip.

    virtual void Clip( const wxRegion &region ) override
    {
        const wxRect r = region.GetBox();
        Clip((wxDouble)r.x, (wxDouble)r.y, (wxDouble)r.width, (wxDouble)r.height);
    }

    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h ) override
    {
        // Device-space bounding box of the transformed rect.
        wxDouble x0, y0, x1, y1;
        TransformPt(x, y, x0, y0);
        TransformPt(x + w, y + h, x1, y1);
        wxRect2DDouble dev(wxMin(x0, x1), wxMin(y0, y1),
                           fabs(x1 - x0), fabs(y1 - y0));

        wxRect2DDouble usr(x, y, w, h);
        if ( m_hasClip )
        {
            dev.Intersect(m_clipDev);
            usr.Intersect(m_clipUser);
        }
        m_hasClip = true;
        m_clipDev = dev;
        m_clipUser = usr;
    }

    virtual void ResetClip() override
    {
        m_hasClip = false;
        m_clipDev = wxRect2DDouble();
        m_clipUser = wxRect2DDouble();
    }

    virtual void GetClipBox(wxDouble* x, wxDouble* y, wxDouble* w, wxDouble* h) override
    {
        if ( m_hasClip )
        {
            if (x) *x = m_clipUser.m_x;
            if (y) *y = m_clipUser.m_y;
            if (w) *w = m_clipUser.m_width;
            if (h) *h = m_clipUser.m_height;
        }
        else
        {
            if (x) *x = 0;
            if (y) *y = 0;
            if (w) *w = m_width;
            if (h) *h = m_height;
        }
    }

    virtual void * GetNativeContext() override
    {
        return nullptr;
    }

    virtual bool SetAntialiasMode(wxAntialiasMode antialias) override
    {
        // Canvas 2D always antialiases shapes; the mode only controls
        // imageSmoothingEnabled for DrawBitmap here.
        m_antialias = antialias;
        return true;
    }

    virtual bool SetInterpolationQuality(wxInterpolationQuality interpolation) override
    {
        m_interpolation = interpolation;
        return true;
    }

    virtual bool SetCompositionMode(wxCompositionMode op) override
    {
        if ( op == wxCOMPOSITION_INVALID )
            return false;
        m_composition = op;
        return true;
    }

    // Approximation: real layers composite the whole group with the given
    // opacity; here the opacity is simply multiplied into the globalAlpha of
    // every primitive drawn between BeginLayer/EndLayer.
    virtual void BeginLayer(wxDouble opacity) override
    {
        m_layerStack.push_back(m_layerAlpha);
        m_layerAlpha *= opacity;
    }

    virtual void EndLayer() override
    {
        if ( m_layerStack.empty() )
            return;
        m_layerAlpha = m_layerStack.back();
        m_layerStack.pop_back();
    }

    // Transforms (post-multiplied onto the CTM, as in cairo)

    virtual void Translate( wxDouble dx , wxDouble dy ) override
    {
        PostMultiply(1.0, 0.0, 0.0, 1.0, dx, dy);
    }

    virtual void Scale( wxDouble xScale , wxDouble yScale ) override
    {
        PostMultiply(xScale, 0.0, 0.0, yScale, 0.0, 0.0);
    }

    virtual void Rotate( wxDouble angle ) override
    {
        const wxDouble s = sin(angle);
        const wxDouble c = cos(angle);
        PostMultiply(c, s, -s, c, 0.0, 0.0);
    }

    virtual void ConcatTransform( const wxGraphicsMatrix& matrix ) override
    {
        wxDouble a, b, c, d, tx, ty;
        matrix.Get(&a, &b, &c, &d, &tx, &ty);
        PostMultiply(a, b, c, d, tx, ty);
    }

    virtual void SetTransform( const wxGraphicsMatrix& matrix ) override
    {
        matrix.Get(&m_matrix[0], &m_matrix[1], &m_matrix[2],
                   &m_matrix[3], &m_matrix[4], &m_matrix[5]);
    }

    virtual wxGraphicsMatrix GetTransform() const override
    {
        return GetRenderer()->CreateMatrix(m_matrix[0], m_matrix[1],
                                           m_matrix[2], m_matrix[3],
                                           m_matrix[4], m_matrix[5]);
    }

    // Path drawing

    virtual void StrokePath( const wxGraphicsPath& path ) override
    {
        if ( m_canvasId.empty() || path.IsNull() )
            return;

        const wxWasmGraphicsPenData* pen = PenData();
        if ( !pen || pen->IsTransparent() )
            return;

        const wxWasmGraphicsPathData* pathData =
            static_cast<const wxWasmGraphicsPathData*>(path.GetPathData());

        std::vector<wxDouble> pathBuf;
        pathData->Serialize(pathBuf);

        // Pack the scalars into a double buffer to stay well below the
        // 16-argument EM_ASM limit:
        //   [0]=paintKind [1]=lineWidth [2]=dashCount [3]=cmdCount
        //   [4]=globalAlpha [5]=stopsCount
        std::vector<wxDouble> params;
        std::vector<wxDouble> coords;
        std::vector<wxDouble> stops;
        std::vector<wxDouble> dashes;
        wxString colStr;

        const int kind = PreparePaint(pen->m_gradientType, pen->m_colour,
                                      pen->m_x1, pen->m_y1, pen->m_x2, pen->m_y2,
                                      pen->m_radius, pen->m_stops,
                                      colStr, coords, stops);
        PrepareDashes(*pen, dashes);

        params.push_back((wxDouble)kind);
        params.push_back(pen->m_width);
        params.push_back((wxDouble)dashes.size());
        params.push_back((wxDouble)pathData->GetCmdCount());
        params.push_back(m_layerAlpha);
        params.push_back((wxDouble)(stops.size() / 5));

        const char* join = "round";
        if ( pen->m_join == wxJOIN_BEVEL ) join = "bevel";
        else if ( pen->m_join == wxJOIN_MITER ) join = "miter";

        const char* cap = "round";
        if ( pen->m_cap == wxCAP_PROJECTING ) cap = "square";
        else if ( pen->m_cap == wxCAP_BUTT ) cap = "butt";

        wxCharBuffer colCStr = colStr.ToUTF8();

        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            // Drop any clip state left by a wxDC sharing this canvas; the DC
            // clip itself is re-applied below from canvas._wxClip.
            if (canvas._wxClipSaved)
            {
                ctx.restore();
                canvas._wxClipSaved = false;
            }
            ctx.save();
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            if (canvas._wxClip)
            {
                ctx.beginPath();
                ctx.rect(canvas._wxClip.x, canvas._wxClip.y,
                         canvas._wxClip.w, canvas._wxClip.h);
                ctx.clip();
            }
            if ($2)
            {
                var qb = $2 >> 3;
                ctx.beginPath();
                ctx.rect(HEAPF64[qb], HEAPF64[qb + 1],
                         HEAPF64[qb + 2], HEAPF64[qb + 3]);
                ctx.clip();
            }
            var mb = $1 >> 3;
            ctx.setTransform(HEAPF64[mb], HEAPF64[mb + 1], HEAPF64[mb + 2],
                             HEAPF64[mb + 3], HEAPF64[mb + 4], HEAPF64[mb + 5]);
            ctx.globalCompositeOperation = UTF8ToString($3);
            var pb = $4 >> 3;
            var kind = HEAPF64[pb];
            ctx.globalAlpha = HEAPF64[pb + 4];
            if (kind === 0)
            {
                ctx.strokeStyle = UTF8ToString($5);
            }
            else
            {
                var cb = $9 >> 3;
                var grad;
                if (kind === 1)
                    grad = ctx.createLinearGradient(HEAPF64[cb], HEAPF64[cb + 1],
                                                    HEAPF64[cb + 2], HEAPF64[cb + 3]);
                else
                    grad = ctx.createRadialGradient(HEAPF64[cb], HEAPF64[cb + 1], 0,
                                                    HEAPF64[cb + 2], HEAPF64[cb + 3],
                                                    HEAPF64[cb + 4]);
                var ns = HEAPF64[pb + 5];
                var sb = $10 >> 3;
                for (var i = 0; i < ns; i++)
                {
                    grad.addColorStop(HEAPF64[sb + i * 5],
                        'rgba(' + HEAPF64[sb + i * 5 + 1] + ',' +
                                  HEAPF64[sb + i * 5 + 2] + ',' +
                                  HEAPF64[sb + i * 5 + 3] + ',' +
                                  (HEAPF64[sb + i * 5 + 4] / 255) + ')');
                }
                ctx.strokeStyle = grad;
            }
            ctx.lineWidth = HEAPF64[pb + 1];
            ctx.lineJoin = UTF8ToString($6);
            ctx.lineCap = UTF8ToString($7);
            var dashes = [];
            var nd = HEAPF64[pb + 2];
            if (nd > 0)
            {
                var db = $8 >> 3;
                for (var i = 0; i < nd; i++)
                    dashes.push(HEAPF64[db + i]);
            }
            ctx.setLineDash(dashes);
            var path = new Path2D();
            var nc = HEAPF64[pb + 3];
            var nb = $11 >> 3;
            for (var i = 0; i < nc; i++)
            {
                var ob = nb + i * 7;
                var op = HEAPF64[ob];
                if (op === 1) path.moveTo(HEAPF64[ob + 1], HEAPF64[ob + 2]);
                else if (op === 2) path.lineTo(HEAPF64[ob + 1], HEAPF64[ob + 2]);
                else if (op === 3) path.bezierCurveTo(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                       HEAPF64[ob + 3], HEAPF64[ob + 4],
                                                       HEAPF64[ob + 5], HEAPF64[ob + 6]);
                else if (op === 4) path.quadraticCurveTo(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                          HEAPF64[ob + 3], HEAPF64[ob + 4]);
                else if (op === 5) path.arc(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                            HEAPF64[ob + 3], HEAPF64[ob + 4],
                                            HEAPF64[ob + 5], HEAPF64[ob + 6] !== 0);
                else if (op === 6) path.closePath();
                else if (op === 7) path.rect(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                             HEAPF64[ob + 3], HEAPF64[ob + 4]);
                else if (op === 8) path.ellipse(HEAPF64[ob + 1] + HEAPF64[ob + 3] / 2,
                                                HEAPF64[ob + 2] + HEAPF64[ob + 4] / 2,
                                                HEAPF64[ob + 3] / 2, HEAPF64[ob + 4] / 2,
                                                0, 0, 2 * Math.PI);
                else if (op === 9) path.roundRect(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                  HEAPF64[ob + 3], HEAPF64[ob + 4],
                                                  HEAPF64[ob + 5]);
            }
            ctx.stroke(path);
            ctx.restore();
        }, m_canvasId.c_str(), m_matrix,
           m_hasClip ? ClipDevBuf() : nullptr,
           wxWasmCompositionToCanvas(m_composition),
           params.data(), colCStr.data(), join, cap,
           dashes.empty() ? nullptr : dashes.data(),
           coords.empty() ? nullptr : coords.data(),
           stops.empty() ? nullptr : stops.data(),
           pathBuf.empty() ? nullptr : pathBuf.data());
    }

    virtual void FillPath( const wxGraphicsPath& path,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE ) override
    {
        if ( m_canvasId.empty() || path.IsNull() )
            return;

        const wxWasmGraphicsBrushData* brush = BrushData();
        if ( !brush || brush->IsTransparent() )
            return;

        const wxWasmGraphicsPathData* pathData =
            static_cast<const wxWasmGraphicsPathData*>(path.GetPathData());

        std::vector<wxDouble> pathBuf;
        pathData->Serialize(pathBuf);

        // [0]=paintKind [1]=fillRule(0 evenodd,1 nonzero) [2]=cmdCount
        // [3]=globalAlpha [4]=stopsCount
        std::vector<wxDouble> params;
        std::vector<wxDouble> coords;
        std::vector<wxDouble> stops;
        wxString colStr;

        const int kind = brush->m_kind == wxWasmGraphicsBrushData::KindSolid ? 0
            : brush->m_kind == wxWasmGraphicsBrushData::KindLinear ? 1 : 2;
        if ( kind == 0 )
            colStr = brush->m_colour.GetAsString(wxC2S_CSS_SYNTAX);
        else
            PrepareGradient(brush->m_x1, brush->m_y1, brush->m_x2, brush->m_y2,
                            brush->m_radius, brush->m_stops, coords, stops);

        params.push_back((wxDouble)kind);
        params.push_back(fillStyle == wxWINDING_RULE ? 1.0 : 0.0);
        params.push_back((wxDouble)pathData->GetCmdCount());
        params.push_back(m_layerAlpha);
        params.push_back((wxDouble)(stops.size() / 5));

        wxCharBuffer colCStr = colStr.ToUTF8();

        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            if (canvas._wxClipSaved)
            {
                ctx.restore();
                canvas._wxClipSaved = false;
            }
            ctx.save();
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            if (canvas._wxClip)
            {
                ctx.beginPath();
                ctx.rect(canvas._wxClip.x, canvas._wxClip.y,
                         canvas._wxClip.w, canvas._wxClip.h);
                ctx.clip();
            }
            if ($2)
            {
                var qb = $2 >> 3;
                ctx.beginPath();
                ctx.rect(HEAPF64[qb], HEAPF64[qb + 1],
                         HEAPF64[qb + 2], HEAPF64[qb + 3]);
                ctx.clip();
            }
            var mb = $1 >> 3;
            ctx.setTransform(HEAPF64[mb], HEAPF64[mb + 1], HEAPF64[mb + 2],
                             HEAPF64[mb + 3], HEAPF64[mb + 4], HEAPF64[mb + 5]);
            ctx.globalCompositeOperation = UTF8ToString($3);
            var pb = $4 >> 3;
            var kind = HEAPF64[pb];
            ctx.globalAlpha = HEAPF64[pb + 3];
            if (kind === 0)
            {
                ctx.fillStyle = UTF8ToString($5);
            }
            else
            {
                var cb = $6 >> 3;
                var grad;
                if (kind === 1)
                    grad = ctx.createLinearGradient(HEAPF64[cb], HEAPF64[cb + 1],
                                                    HEAPF64[cb + 2], HEAPF64[cb + 3]);
                else
                    grad = ctx.createRadialGradient(HEAPF64[cb], HEAPF64[cb + 1], 0,
                                                    HEAPF64[cb + 2], HEAPF64[cb + 3],
                                                    HEAPF64[cb + 4]);
                var ns = HEAPF64[pb + 4];
                var sb = $7 >> 3;
                for (var i = 0; i < ns; i++)
                {
                    grad.addColorStop(HEAPF64[sb + i * 5],
                        'rgba(' + HEAPF64[sb + i * 5 + 1] + ',' +
                                  HEAPF64[sb + i * 5 + 2] + ',' +
                                  HEAPF64[sb + i * 5 + 3] + ',' +
                                  (HEAPF64[sb + i * 5 + 4] / 255) + ')');
                }
                ctx.fillStyle = grad;
            }
            var path = new Path2D();
            var nc = HEAPF64[pb + 2];
            var nb = $8 >> 3;
            for (var i = 0; i < nc; i++)
            {
                var ob = nb + i * 7;
                var op = HEAPF64[ob];
                if (op === 1) path.moveTo(HEAPF64[ob + 1], HEAPF64[ob + 2]);
                else if (op === 2) path.lineTo(HEAPF64[ob + 1], HEAPF64[ob + 2]);
                else if (op === 3) path.bezierCurveTo(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                       HEAPF64[ob + 3], HEAPF64[ob + 4],
                                                       HEAPF64[ob + 5], HEAPF64[ob + 6]);
                else if (op === 4) path.quadraticCurveTo(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                          HEAPF64[ob + 3], HEAPF64[ob + 4]);
                else if (op === 5) path.arc(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                            HEAPF64[ob + 3], HEAPF64[ob + 4],
                                            HEAPF64[ob + 5], HEAPF64[ob + 6] !== 0);
                else if (op === 6) path.closePath();
                else if (op === 7) path.rect(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                             HEAPF64[ob + 3], HEAPF64[ob + 4]);
                else if (op === 8) path.ellipse(HEAPF64[ob + 1] + HEAPF64[ob + 3] / 2,
                                                HEAPF64[ob + 2] + HEAPF64[ob + 4] / 2,
                                                HEAPF64[ob + 3] / 2, HEAPF64[ob + 4] / 2,
                                                0, 0, 2 * Math.PI);
                else if (op === 9) path.roundRect(HEAPF64[ob + 1], HEAPF64[ob + 2],
                                                  HEAPF64[ob + 3], HEAPF64[ob + 4],
                                                  HEAPF64[ob + 5]);
            }
            ctx.fill(path, HEAPF64[pb + 1] === 0 ? 'evenodd' : 'nonzero');
            ctx.restore();
        }, m_canvasId.c_str(), m_matrix,
           m_hasClip ? ClipDevBuf() : nullptr,
           wxWasmCompositionToCanvas(m_composition),
           params.data(), colCStr.data(),
           coords.empty() ? nullptr : coords.data(),
           stops.empty() ? nullptr : stops.data(),
           pathBuf.empty() ? nullptr : pathBuf.data());
    }

    // Text

    virtual void DoDrawText(const wxString& str, wxDouble x, wxDouble y) override
    {
        if ( m_canvasId.empty() )
            return;

        const wxString fontSpec = CurrentFontSpec();
        wxCharBuffer fontCStr = fontSpec.ToUTF8();
        // wx passes the top of the text, canvas the alphabetic baseline.
        const double ascent = wxWasmMeasureAscent(fontCStr.data());

        const wxColour col = FontData() ? FontData()->m_colour : *wxBLACK;
        wxString colStr = col.GetAsString(wxC2S_CSS_SYNTAX);
        wxCharBuffer colCStr = colStr.ToUTF8();

        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            if (canvas._wxClipSaved)
            {
                ctx.restore();
                canvas._wxClipSaved = false;
            }
            ctx.save();
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            if (canvas._wxClip)
            {
                ctx.beginPath();
                ctx.rect(canvas._wxClip.x, canvas._wxClip.y,
                         canvas._wxClip.w, canvas._wxClip.h);
                ctx.clip();
            }
            if ($2)
            {
                var qb = $2 >> 3;
                ctx.beginPath();
                ctx.rect(HEAPF64[qb], HEAPF64[qb + 1],
                         HEAPF64[qb + 2], HEAPF64[qb + 3]);
                ctx.clip();
            }
            var mb = $1 >> 3;
            ctx.setTransform(HEAPF64[mb], HEAPF64[mb + 1], HEAPF64[mb + 2],
                             HEAPF64[mb + 3], HEAPF64[mb + 4], HEAPF64[mb + 5]);
            ctx.globalCompositeOperation = UTF8ToString($3);
            ctx.globalAlpha = $4;
            ctx.font = UTF8ToString($5);
            ctx.fillStyle = UTF8ToString($6);
            ctx.fillText(UTF8ToString($7), $8, $9 + $10);
            ctx.restore();
        }, m_canvasId.c_str(), m_matrix,
           m_hasClip ? ClipDevBuf() : nullptr,
           wxWasmCompositionToCanvas(m_composition),
           m_layerAlpha, fontCStr.data(), colCStr.data(),
           str.ToUTF8().data(), x, y, ascent);
    }

    virtual void GetTextExtent( const wxString &text, wxDouble *width, wxDouble *height,
        wxDouble *descent = nullptr, wxDouble *externalLeading = nullptr ) const override
    {
        const wxString fontSpec = CurrentFontSpec();
        wxCharBuffer fontCStr = fontSpec.ToUTF8();

        if ( width )
            *width = wxWasmMeasureTextWidth(fontCStr.data(), text.ToUTF8().data());
        if ( height )
            *height = wxWasmMeasureAscent(fontCStr.data()) +
                      wxWasmMeasureDescent(fontCStr.data());
        if ( descent )
            *descent = wxWasmMeasureDescent(fontCStr.data());
        if ( externalLeading )
            *externalLeading = 0;
    }

    virtual void GetPartialTextExtents(const wxString& text,
                                       wxArrayDouble& widths) const override
    {
        widths.Empty();
        const wxString fontSpec = CurrentFontSpec();
        wxCharBuffer fontCStr = fontSpec.ToUTF8();
        // Per-character approximation: the canvas measureText() result for a
        // prefix is not the sum of the single characters when kerning is
        // involved, but generic CSS families do not kern.
        double total = 0;
        for ( size_t i = 0; i < text.length(); ++i )
        {
            const wxString ch = text.Mid(i, 1);
            total += wxWasmMeasureTextWidth(fontCStr.data(), ch.ToUTF8().data());
            widths.Add(total);
        }
    }

    // Bitmaps

    virtual void DrawBitmap( const wxGraphicsBitmap &bmp, wxDouble x, wxDouble y,
                             wxDouble w, wxDouble h ) override
    {
        if ( bmp.IsNull() )
            return;
        const wxWasmGraphicsBitmapData* data =
            static_cast<const wxWasmGraphicsBitmapData*>(bmp.GetBitmapData());
        if ( !data || !data->m_bitmap.IsOk() )
            return;
        DrawBitmapData(data->m_bitmap, x, y, w, h);
    }

    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y,
                             wxDouble w, wxDouble h ) override
    {
        if ( !bmp.IsOk() )
            return;
        DrawBitmapData(bmp, x, y, w, h);
    }

    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y,
                           wxDouble w, wxDouble h ) override
    {
        // wxIcon is the generic class deriving from wxBitmap in this port.
        DrawBitmap((const wxBitmap&)icon, x, y, w, h);
    }

private:
    struct State
    {
        wxDouble matrix[6];
        bool hasClip;
        wxRect2DDouble clipDev;
        wxRect2DDouble clipUser;
        wxAntialiasMode antialias;
        wxCompositionMode composition;
        wxInterpolationQuality interpolation;
    };

    const wxWasmGraphicsPenData* PenData() const
    {
        return m_pen.IsNull() ? nullptr
            : static_cast<const wxWasmGraphicsPenData*>(m_pen.GetGraphicsData());
    }

    const wxWasmGraphicsBrushData* BrushData() const
    {
        return m_brush.IsNull() ? nullptr
            : static_cast<const wxWasmGraphicsBrushData*>(m_brush.GetGraphicsData());
    }

    const wxWasmGraphicsFontData* FontData() const
    {
        return m_font.IsNull() ? nullptr
            : static_cast<const wxWasmGraphicsFontData*>(m_font.GetGraphicsData());
    }

    wxString CurrentFontSpec() const
    {
        const wxWasmGraphicsFontData* font = FontData();
        return font ? font->m_cssSpec : wxString("12px sans-serif");
    }

    void PostMultiply(wxDouble a, wxDouble b, wxDouble c, wxDouble d,
                      wxDouble tx, wxDouble ty)
    {
        const wxDouble na = m_matrix[0] * a + m_matrix[2] * b;
        const wxDouble nb = m_matrix[1] * a + m_matrix[3] * b;
        const wxDouble nc = m_matrix[0] * c + m_matrix[2] * d;
        const wxDouble nd = m_matrix[1] * c + m_matrix[3] * d;
        const wxDouble ntx = m_matrix[0] * tx + m_matrix[2] * ty + m_matrix[4];
        const wxDouble nty = m_matrix[1] * tx + m_matrix[3] * ty + m_matrix[5];
        m_matrix[0] = na; m_matrix[1] = nb;
        m_matrix[2] = nc; m_matrix[3] = nd;
        m_matrix[4] = ntx; m_matrix[5] = nty;
    }

    void TransformPt(wxDouble x, wxDouble y, wxDouble& ox, wxDouble& oy) const
    {
        ox = m_matrix[0] * x + m_matrix[2] * y + m_matrix[4];
        oy = m_matrix[1] * x + m_matrix[3] * y + m_matrix[5];
    }

    // Device-space clip as a 4-double buffer (valid until the next call).
    const wxDouble* ClipDevBuf() const
    {
        m_clipBuf[0] = m_clipDev.m_x;
        m_clipBuf[1] = m_clipDev.m_y;
        m_clipBuf[2] = m_clipDev.m_width;
        m_clipBuf[3] = m_clipDev.m_height;
        return m_clipBuf;
    }

    // Fills the gradient coordinate and stop buffers (5 doubles per stop:
    // pos r g b a) from a pen or a brush. Returns the paint kind
    // (0 solid colour, 1 linear gradient, 2 radial gradient).
    static int PreparePaint(wxGradientType gradType, const wxColour& col,
                            wxDouble x1, wxDouble y1,
                            wxDouble x2, wxDouble y2, wxDouble radius,
                            const wxGraphicsGradientStops& stops,
                            wxString& colStr,
                            std::vector<wxDouble>& coords,
                            std::vector<wxDouble>& stopBuf)
    {
        if ( gradType == wxGRADIENT_NONE )
        {
            colStr = col.GetAsString(wxC2S_CSS_SYNTAX);
            return 0;
        }
        PrepareGradient(x1, y1, x2, y2, radius, stops, coords, stopBuf);
        return gradType == wxGRADIENT_LINEAR ? 1 : 2;
    }

    static void PrepareGradient(wxDouble x1, wxDouble y1,
                                wxDouble x2, wxDouble y2, wxDouble radius,
                                const wxGraphicsGradientStops& stops,
                                std::vector<wxDouble>& coords,
                                std::vector<wxDouble>& stopBuf)
    {
        coords.push_back(x1);
        coords.push_back(y1);
        coords.push_back(x2);
        coords.push_back(y2);
        coords.push_back(radius);
        for ( size_t i = 0; i < stops.GetCount(); ++i )
        {
            const wxGraphicsGradientStop& stop = stops.Item((unsigned)i);
            stopBuf.push_back((wxDouble)stop.GetPosition());
            stopBuf.push_back(stop.GetColour().Red());
            stopBuf.push_back(stop.GetColour().Green());
            stopBuf.push_back(stop.GetColour().Blue());
            stopBuf.push_back(stop.GetColour().Alpha());
        }
    }

    // User dashes are used as-is; the preset styles get a pattern in units
    // of the line width.
    static void PrepareDashes(const wxWasmGraphicsPenData& pen,
                              std::vector<wxDouble>& dashes)
    {
        const wxDouble w = pen.m_width > 0 ? pen.m_width : 1;
        switch ( pen.m_style )
        {
            case wxPENSTYLE_USER_DASH:
                dashes = pen.m_dashes;
                break;
            case wxPENSTYLE_DOT:
                dashes.push_back(w);
                dashes.push_back(2 * w);
                break;
            case wxPENSTYLE_SHORT_DASH:
                dashes.push_back(4 * w);
                dashes.push_back(2 * w);
                break;
            case wxPENSTYLE_LONG_DASH:
                dashes.push_back(8 * w);
                dashes.push_back(4 * w);
                break;
            case wxPENSTYLE_DOT_DASH:
                dashes.push_back(w);
                dashes.push_back(2 * w);
                dashes.push_back(4 * w);
                dashes.push_back(2 * w);
                break;
            default:
                break;
        }
        // Canvas requires an even number of dash entries (it duplicates the
        // pattern otherwise); duplicate it here to keep the wx semantics.
        if ( dashes.size() % 2 != 0 )
        {
            const size_t n = dashes.size();
            for ( size_t i = 0; i < n; ++i )
                dashes.push_back(dashes[i]);
        }
    }

    void DrawBitmapData(const wxBitmap& bmp, wxDouble x, wxDouble y,
                        wxDouble w, wxDouble h)
    {
        if ( m_canvasId.empty() )
            return;

        wxImage image = bmp.ConvertToImage();
        if ( !image.IsOk() )
            return;

        const int sw = image.GetWidth();
        const int sh = image.GetHeight();
        unsigned char* rgb = image.GetData();
        // ConvertToImage() folds the mask into the image alpha channel.
        const bool useAlpha = image.HasAlpha() && (bmp.HasAlpha() || bmp.GetMask());
        unsigned char* alpha = useAlpha ? image.GetAlpha() : nullptr;

        const char* quality = "low";
        if ( m_interpolation == wxINTERPOLATION_GOOD ) quality = "medium";
        else if ( m_interpolation == wxINTERPOLATION_BEST ) quality = "high";

        // [0]=globalAlpha [1]=imageSmoothingEnabled
        wxDouble params[2];
        params[0] = m_layerAlpha;
        params[1] = m_antialias == wxANTIALIAS_NONE ? 0.0 : 1.0;

        EM_ASM_({
            var canvas = document.getElementById(UTF8ToString($0));
            if (!canvas) return;
            var ctx = canvas.getContext('2d');
            if (canvas._wxClipSaved)
            {
                ctx.restore();
                canvas._wxClipSaved = false;
            }
            ctx.save();
            ctx.setTransform(1, 0, 0, 1, 0, 0);
            if (canvas._wxClip)
            {
                ctx.beginPath();
                ctx.rect(canvas._wxClip.x, canvas._wxClip.y,
                         canvas._wxClip.w, canvas._wxClip.h);
                ctx.clip();
            }
            if ($2)
            {
                var qb = $2 >> 3;
                ctx.beginPath();
                ctx.rect(HEAPF64[qb], HEAPF64[qb + 1],
                         HEAPF64[qb + 2], HEAPF64[qb + 3]);
                ctx.clip();
            }
            var mb = $1 >> 3;
            ctx.setTransform(HEAPF64[mb], HEAPF64[mb + 1], HEAPF64[mb + 2],
                             HEAPF64[mb + 3], HEAPF64[mb + 4], HEAPF64[mb + 5]);
            ctx.globalCompositeOperation = UTF8ToString($3);
            var pb = $4 >> 3;
            ctx.globalAlpha = HEAPF64[pb];
            ctx.imageSmoothingEnabled = HEAPF64[pb + 1] !== 0;
            ctx.imageSmoothingQuality = UTF8ToString($13);
            var w = $7;
            var h = $8;
            var imgData = ctx.createImageData(w, h);
            var rgb = $5;
            var alpha = $6;
            for (var i = 0; i < w * h; i++)
            {
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
            ctx.drawImage(tmpCanvas, $9, $10, $11, $12);
            ctx.restore();
        }, m_canvasId.c_str(), m_matrix,
           m_hasClip ? ClipDevBuf() : nullptr,
           wxWasmCompositionToCanvas(m_composition),
           params, rgb, alpha, sw, sh, x, y, w, h, quality);
    }

    std::string m_canvasId;
    wxDouble m_matrix[6];
    bool m_hasClip;
    wxRect2DDouble m_clipDev;   // device-space clip actually applied
    wxRect2DDouble m_clipUser;  // user-space clip returned by GetClipBox
    std::vector<State> m_stateStack;
    std::vector<wxDouble> m_layerStack;
    wxDouble m_layerAlpha;
    mutable wxDouble m_clipBuf[4];

    wxDECLARE_NO_COPY_CLASS(wxWasmGraphicsContext);
};

// ----------------------------------------------------------------------------
// wxWasmGraphicsRenderer
// ----------------------------------------------------------------------------

class wxWasmGraphicsRenderer : public wxGraphicsRenderer
{
public:
    wxWasmGraphicsRenderer() = default;
    virtual ~wxWasmGraphicsRenderer() = default;

    // Context

    virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc) override
    {
        const wxWasmDCImpl* impl =
            dynamic_cast<const wxWasmDCImpl*>(dc.GetImpl());
        if ( !impl || impl->GetCanvasId().empty() )
            return nullptr;
        const wxSize size = dc.GetSize();
        return new wxWasmGraphicsContext(this, impl->GetCanvasId(),
                                         dc.GetWindow(), size.x, size.y);
    }

    virtual wxGraphicsContext * CreateContext( const wxMemoryDC& dc) override
    {
        const wxWasmDCImpl* impl =
            dynamic_cast<const wxWasmDCImpl*>(dc.GetImpl());
        if ( !impl || impl->GetCanvasId().empty() )
            return nullptr;
        const wxSize size = dc.GetSize();
        return new wxWasmGraphicsContext(this, impl->GetCanvasId(),
                                         nullptr, size.x, size.y);
    }

#if wxUSE_PRINTING_ARCHITECTURE
    virtual wxGraphicsContext * CreateContext( const wxPrinterDC& WXUNUSED(dc)) override
    {
        return nullptr;
    }
#endif

    virtual wxGraphicsContext * CreateContextFromNativeContext( void * WXUNUSED(context) ) override
    {
        return nullptr;
    }

    virtual wxGraphicsContext * CreateContextFromNativeWindow( void * WXUNUSED(window) ) override
    {
        return nullptr;
    }

    virtual wxGraphicsContext * CreateContext( wxWindow* window ) override
    {
        if ( !window )
            return nullptr;
        // Reuse the window DC canvas (shared, owned by the window): creating
        // a temporary wxClientDC ensures the canvas element exists and has
        // the right size.
        wxClientDC dc(window);
        return CreateContext((const wxWindowDC&)dc);
    }

#if wxUSE_IMAGE
    virtual wxGraphicsContext * CreateContextFromImage(wxImage& WXUNUSED(image)) override
    {
        return nullptr;
    }
#endif // wxUSE_IMAGE

    virtual wxGraphicsContext * CreateMeasuringContext() override
    {
        // No canvas: only the text measuring methods do anything.
        return new wxWasmGraphicsContext(this, std::string(), nullptr, 0, 0);
    }

    // Path

    virtual wxGraphicsPath CreatePath() override
    {
        wxGraphicsPath path;
        path.SetRefData( new wxWasmGraphicsPathData(this) );
        return path;
    }

    // Matrix

    virtual wxGraphicsMatrix CreateMatrix( wxDouble a=1.0, wxDouble b=0.0,
        wxDouble c=0.0, wxDouble d=1.0,
        wxDouble tx=0.0, wxDouble ty=0.0) override
    {
        wxGraphicsMatrix m;
        wxWasmGraphicsMatrixData* data = new wxWasmGraphicsMatrixData( this );
        data->Set( a,b,c,d,tx,ty );
        m.SetRefData(data);
        return m;
    }

    // Paints

    virtual wxGraphicsPen CreatePen(const wxGraphicsPenInfo& info) override
    {
        wxGraphicsPen p;
        p.SetRefData( new wxWasmGraphicsPenData(this, info) );
        return p;
    }

    virtual wxGraphicsBrush CreateBrush(const wxBrush& brush ) override
    {
        wxGraphicsBrush b;
        if ( brush.IsOk() && brush.GetStyle() != wxBRUSHSTYLE_TRANSPARENT )
            b.SetRefData( new wxWasmGraphicsBrushData(this, brush) );
        return b;
    }

    virtual wxGraphicsBrush
    CreateLinearGradientBrush(wxDouble x1, wxDouble y1,
                              wxDouble x2, wxDouble y2,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& WXUNUSED(matrix) = wxNullGraphicsMatrix) override
    {
        // The gradient matrix is not honoured (documented limitation): the
        // gradient coordinates are used directly in the current user space.
        wxGraphicsBrush b;
        b.SetRefData( new wxWasmGraphicsBrushData(this, wxGRADIENT_LINEAR,
                                                  x1, y1, x2, y2, 0, stops) );
        return b;
    }

    virtual wxGraphicsBrush
    CreateRadialGradientBrush(wxDouble startX, wxDouble startY,
                              wxDouble endX, wxDouble endY,
                              wxDouble radius,
                              const wxGraphicsGradientStops& stops,
                              const wxGraphicsMatrix& WXUNUSED(matrix) = wxNullGraphicsMatrix) override
    {
        wxGraphicsBrush b;
        b.SetRefData( new wxWasmGraphicsBrushData(this, wxGRADIENT_RADIAL,
                                                  startX, startY,
                                                  endX, endY, radius, stops) );
        return b;
    }

    // Fonts

    virtual wxGraphicsFont CreateFont( const wxFont &font,
                                       const wxColour &col = *wxBLACK ) override
    {
        wxGraphicsFont f;
        if ( font.IsOk() )
            f.SetRefData( new wxWasmGraphicsFontData(this,
                              wxWasmGraphicsFontSpec(font), col) );
        return f;
    }

    virtual wxGraphicsFont CreateFont(double sizeInPixels,
                                      const wxString& facename,
                                      int flags = wxFONTFLAG_DEFAULT,
                                      const wxColour& col = *wxBLACK) override
    {
        wxString spec;
        if ( flags & wxFONTFLAG_ITALIC )
            spec += "italic ";
        spec += (flags & wxFONTFLAG_BOLD) ? "700 " : "400 ";
        if ( sizeInPixels <= 0 )
            sizeInPixels = 12;
        spec += wxString::Format("%.1fpx ", sizeInPixels);
        wxString face = facename;
        if ( face.empty() )
            face = "sans-serif";
        else
        {
            face.Replace("\"", "\\\"");
            face = wxString::Format("\"%s\"", face);
        }
        spec += face;

        wxGraphicsFont f;
        f.SetRefData( new wxWasmGraphicsFontData(this, spec, col) );
        return f;
    }

    virtual wxGraphicsFont CreateFontAtDPI(const wxFont& font,
                                           const wxRealPoint& WXUNUSED(dpi),
                                           const wxColour& col = *wxBLACK) override
    {
        // The port assumes a 96 DPI display, so the DPI is ignored.
        return CreateFont(font, col);
    }

    // Bitmaps: a graphics bitmap is just a wrapper around a wxBitmap (which
    // owns RGBA pixels in this port).

    virtual wxGraphicsBitmap CreateBitmap( const wxBitmap &bitmap ) override
    {
        wxGraphicsBitmap bmp;
        if ( bitmap.IsOk() )
            bmp.SetRefData( new wxWasmGraphicsBitmapData(this, bitmap) );
        return bmp;
    }

#if wxUSE_IMAGE
    virtual wxGraphicsBitmap CreateBitmapFromImage(const wxImage& image) override
    {
        wxGraphicsBitmap bmp;
        if ( image.IsOk() )
            bmp.SetRefData( new wxWasmGraphicsBitmapData(this, wxBitmap(image)) );
        return bmp;
    }

    virtual wxImage CreateImageFromBitmap(const wxGraphicsBitmap& bmp) override
    {
        if ( bmp.IsNull() )
            return wxNullImage;
        const wxWasmGraphicsBitmapData* data =
            static_cast<const wxWasmGraphicsBitmapData*>(bmp.GetBitmapData());
        return data ? data->m_bitmap.ConvertToImage() : wxNullImage;
    }
#endif // wxUSE_IMAGE

    virtual wxGraphicsBitmap CreateBitmapFromNativeBitmap( void* WXUNUSED(bitmap) ) override
    {
        return wxNullGraphicsBitmap;
    }

    virtual wxGraphicsBitmap CreateSubBitmap( const wxGraphicsBitmap &bitmap,
        wxDouble x, wxDouble y, wxDouble w, wxDouble h ) override
    {
        if ( bitmap.IsNull() )
            return wxNullGraphicsBitmap;
        const wxWasmGraphicsBitmapData* data =
            static_cast<const wxWasmGraphicsBitmapData*>(bitmap.GetBitmapData());
        if ( !data || !data->m_bitmap.IsOk() )
            return wxNullGraphicsBitmap;

        wxGraphicsBitmap bmp;
        const wxBitmap sub = data->m_bitmap.GetSubBitmap(
            wxRect((int)x, (int)y, (int)w, (int)h));
        if ( sub.IsOk() )
            bmp.SetRefData( new wxWasmGraphicsBitmapData(this, sub) );
        return bmp;
    }

    virtual wxString GetName() const override
    {
        return "wasm-canvas2d";
    }

    virtual void GetVersion(int *major, int *minor = nullptr, int *micro = nullptr) const override
    {
        if (major) *major = wxMAJOR_VERSION;
        if (minor) *minor = wxMINOR_VERSION;
        if (micro) *micro = wxRELEASE_NUMBER;
    }

    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxWasmGraphicsRenderer);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWasmGraphicsRenderer, wxGraphicsRenderer);

static wxWasmGraphicsRenderer gs_wasmGraphicsRenderer;

// The default renderer of this port (declared in wx/graphics.h); the generic
// fallback in src/generic/graphicc.cpp is compiled out for __WXWASM__.
wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
    return &gs_wasmGraphicsRenderer;
}

#endif // wxUSE_GRAPHICS_CONTEXT
