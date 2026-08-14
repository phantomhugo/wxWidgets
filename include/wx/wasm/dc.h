/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wasm/dc.h
// Purpose:     wxDcImpl class
// Author:      Hugo Armando Castellanos Morales
// Created:     18.07.21
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows Library Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WASM_DC_H_
#define _WX_WASM_DC_H_

class WXDLLIMPEXP_FWD_CORE wxRegion;

std::string GenerateCanvasId();

class WXDLLIMPEXP_CORE wxWasmDCImpl : public wxDCImpl
{
public:
    wxWasmDCImpl( wxDC *owner );
    virtual ~wxWasmDCImpl();

    virtual bool CanDrawBitmap() const override { return true; }
    virtual bool CanGetTextExtent() const override { return true; }

    virtual void DoGetSize(int *width, int *height) const override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    virtual int GetDepth() const override { return 32; }
    virtual wxSize GetPPI() const override { return wxSize(96, 96); }

    virtual void SetFont(const wxFont& font) override;
    virtual void SetPen(const wxPen& pen) override;
    virtual void SetBrush(const wxBrush& brush) override;
    virtual void SetBackground(const wxBrush& brush) override;
    virtual void SetBackgroundMode(int mode) override;

#if wxUSE_PALETTE
    virtual void SetPalette(const wxPalette& palette) override;
#endif // wxUSE_PALETTE

    virtual void SetLogicalFunction(wxRasterOperationMode function) override;

    virtual wxCoord GetCharHeight() const override;
    virtual wxCoord GetCharWidth() const override;
    virtual void DoGetTextExtent(const wxString& string,
                                 wxCoord *x, wxCoord *y,
                                 wxCoord *descent = NULL,
                                 wxCoord *externalLeading = NULL,
                                 const wxFont *theFont = NULL) const override;

    virtual void Clear() override;

    virtual void DoSetClippingRegion(wxCoord x, wxCoord y,
                                     wxCoord width, wxCoord height) override;

    virtual void DoSetDeviceClippingRegion(const wxRegion& region) override;
    virtual void DestroyClippingRegion() override;
    virtual bool DoGetClippingRect(wxRect& rect) const override;

    virtual bool DoFloodFill(wxCoord x, wxCoord y, const wxColour& col,
                             wxFloodFillStyle style = wxFLOOD_SURFACE) override;
    virtual bool DoGetPixel(wxCoord x, wxCoord y, wxColour *col) const override;

    virtual void DoDrawPoint(wxCoord x, wxCoord y) override;
    virtual void DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2) override;

    virtual void DoDrawArc(wxCoord x1, wxCoord y1,
                           wxCoord x2, wxCoord y2,
                           wxCoord xc, wxCoord yc) override;

    virtual void DoDrawEllipticArc(wxCoord x, wxCoord y, wxCoord w, wxCoord h,
                                   double sa, double ea) override;

    virtual void DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoDrawRoundedRectangle(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height,
                                        double radius) override;
    virtual void DoDrawEllipse(wxCoord x, wxCoord y,
                               wxCoord width, wxCoord height) override;

    virtual void DoCrossHair(wxCoord x, wxCoord y) override;

    virtual void DoDrawIcon(const wxIcon& icon, wxCoord x, wxCoord y) override;
    virtual void DoDrawBitmap(const wxBitmap &bmp, wxCoord x, wxCoord y,
                              bool useMask = false) override;

    virtual void DoDrawText(const wxString& text, wxCoord x, wxCoord y) override;
    virtual void DoDrawRotatedText(const wxString& text,
                                   wxCoord x, wxCoord y, double angle) override;

    virtual bool DoBlit(wxCoord xdest, wxCoord ydest,
                        wxCoord width, wxCoord height,
                        wxDC *source,
                        wxCoord xsrc, wxCoord ysrc,
                        wxRasterOperationMode rop = wxCOPY,
                        bool useMask = false,
                        wxCoord xsrcMask = wxDefaultCoord,
                        wxCoord ysrcMask = wxDefaultCoord) override;

    virtual bool DoStretchBlit(wxCoord xdest, wxCoord ydest,
                               wxCoord dstWidth, wxCoord dstHeight,
                               wxDC *source,
                               wxCoord xsrc, wxCoord ysrc,
                               wxCoord srcWidth, wxCoord srcHeight,
                               wxRasterOperationMode rop = wxCOPY,
                               bool useMask = false,
                               wxCoord xsrcMask = wxDefaultCoord,
                               wxCoord ysrcMask = wxDefaultCoord) override;

    virtual void DoGradientFillLinear(const wxRect& rect,
                                      const wxColour& initialColour,
                                      const wxColour& destColour,
                                      wxDirection nDirection = wxEAST) override;
    virtual void DoGradientFillConcentric(const wxRect& rect,
                                          const wxColour& initialColour,
                                          const wxColour& destColour,
                                          const wxPoint& circleCenter) override;

    virtual void DoDrawLines(int n, const wxPoint points[],
                             wxCoord xoffset, wxCoord yoffset ) override;

    virtual void DoDrawPolygon(int n, const wxPoint points[],
                           wxCoord xoffset, wxCoord yoffset,
                           wxPolygonFillMode fillStyle = wxODDEVEN_RULE) override;

    virtual void ComputeScaleAndOrigin() override;

    const std::string& GetCanvasId() const { return m_canvasId; }

protected:
    std::string m_canvasId;
    wxSize m_size;
    wxColour m_penColour;
    wxColour m_brushColour;
    wxColour m_backgroundColour;
    int m_backgroundMode;
    wxRasterOperationMode m_logicalFunction;
    int m_penWidth;
    wxPenStyle m_penStyle;
    wxBrushStyle m_brushStyle;

    // Copy of the current pen dash pattern: wxPen does not own the dash
    // array it is given (it just stores the pointer), so the values are
    // copied here in SetPen() to keep them valid after the pen is gone.
    std::vector<int> m_penDashes;

    // Copy of the current pen stipple bitmap (wxPENSTYLE_STIPPLE), applied
    // as a repeating canvas pattern in ApplyPen().
    wxBitmap m_penStipple;

    // Copy of the current brush stipple bitmap (wxBRUSHSTYLE_STIPPLE),
    // applied as a repeating canvas pattern in ApplyBrush().
    wxBitmap m_brushStipple;

    wxFont m_font;

    // True if this DC owns its canvas and must remove it from the DOM when
    // destroyed (memory DCs and anonymous canvases). False for window DCs:
    // their canvas is shared by all DCs of the same window and persists
    // after the DC is destroyed, so what was drawn stays visible.
    bool m_canvasOwnedByDC;

    void EnsureCanvasCreated();
    void ApplyPen();
    void ApplyBrush();
    void ApplyFont();
    void ApplyTextColour();

    // True when the current pen/brush must not be drawn with: either it is
    // transparent or its style is invalid (an invalid pen/brush draws
    // nothing, as in the other ports).
    bool IsPenTransparent() const
    {
        return m_penStyle == wxPENSTYLE_TRANSPARENT ||
               m_penStyle == wxPENSTYLE_INVALID;
    }
    bool IsBrushTransparent() const
    {
        return m_brushStyle == wxBRUSHSTYLE_TRANSPARENT ||
               m_brushStyle == wxBRUSHSTYLE_INVALID;
    }

    // Called by EnsureCanvasCreated() (i.e. before every drawing operation)
    // to give derived classes a chance to re-synchronize the canvas pixel
    // buffer with its display size. Does nothing by default; wxWindowDCImpl
    // overrides it to keep the buffer sharp when the window is resized.
    virtual void SyncCanvasBuffer() {}

    // Records the device-space clip box on the canvas element; the clip is
    // actually applied to the 2D context by EnsureCanvasCreated() before
    // each drawing operation.
    void SetCanvasClip(int x, int y, int w, int h);

    // Clipping state, in device coordinates. Kept here (and not in the base
    // class members, which are private) so that successive clips can be
    // intersected and DoGetClippingRect() can return them.
    bool m_hasClip;
    wxRect m_clipRectDev;
private:

    void ApplyRasterColourOp();

    wxDECLARE_NO_COPY_CLASS(wxWasmDCImpl);

};

#endif // _WX_WASM_DC_H_
