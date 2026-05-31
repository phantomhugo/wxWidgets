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
    wxColour m_textColour;
    wxColour m_penColour;
    wxColour m_brushColour;
    int m_penWidth;
    wxFont m_font;
    bool m_hasClipping;

    void EnsureCanvasCreated();
    void ApplyPen();
    void ApplyBrush();
    void ApplyFont();
    void ApplyTextColour();

    wxRegion m_clippingRegion;
private:

    void ApplyRasterColourOp();

    wxDECLARE_NO_COPY_CLASS(wxWasmDCImpl);

};

#endif // _WX_WASM_DC_H_
