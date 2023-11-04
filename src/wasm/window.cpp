/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/window.cpp
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/frame.h"
    #include "wx/log.h"
    #include "wx/menu.h"
    #include "wx/scrolbar.h"
#endif // WX_PRECOMP

#include "wx/window.h"
#include "wx/dnd.h"
#include "wx/tooltip.h"

#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

#if wxUSE_ACCEL
#endif // wxUSE_ACCEL

//##############################################################################

#ifdef __WXUNIVERSAL__
    wxIMPLEMENT_ABSTRACT_CLASS(wxWindow, wxWindowBase);
#endif // __WXUNIVERSAL__


void wxWindowWasm::Init()
{
    m_mouseInside = false;

#if wxUSE_ACCEL

    m_processingShortcut = false;
#endif

}

wxWindowWasm::wxWindowWasm()
{
    Init();
}


wxWindowWasm::wxWindowWasm(wxWindowWasm *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
    long style, const wxString& name)
{
    Init();

    Create( parent, id, pos, size, style, name );
}


wxWindowWasm::~wxWindowWasm()
{
#if wxUSE_DRAG_AND_DROP
    SetDropTarget(nullptr);
#endif

}


bool wxWindowWasm::Create( wxWindowWasm * parent, wxWindowID id, const wxPoint & pos,
        const wxSize & size, long style, const wxString &name )
{


    if ( !wxWindowBase::CreateBase( parent, id, pos, size, style, wxDefaultValidator, name ))
        return false;

    parent->AddChild( this );

    wxPoint p;
    if ( pos != wxDefaultPosition )
        p = pos;

    DoMoveWindow( p.x, p.y, size.GetWidth(), size.GetHeight() );

    PostCreation();

    return ( true );
}

void wxWindowWasm::PostCreation(bool generic)
{

    wxWindowCreateEvent event(this);
    HandleWindowEvent(event);
}

void wxWindowWasm::AddChild( wxWindowBase *child )
{
    // Make sure all children are children of the inner scroll area widget (if any):

    wxWindowBase::AddChild( child );
}

bool wxWindowWasm::Show( bool show )
{
    if ( !wxWindowBase::Show( show ))
        return false;

    // Show can be called before the underlying window is created:

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    return true;
}


void wxWindowWasm::SetLabel(const wxString& label)
{

}


wxString wxWindowWasm::GetLabel() const
{
}

void wxWindowWasm::DoEnable(bool enable)
{
}

void wxWindowWasm::SetFocus()
{
}

bool wxWindowWasm::Reparent( wxWindowBase *parent )
{
    if ( !wxWindowBase::Reparent( parent ))
        return false;

    return true;
}


void wxWindowWasm::Raise()
{
}

void wxWindowWasm::Lower()
{
}

void wxWindowWasm::WarpPointer(int x, int y)
{
    // QCursor::setPos takes global screen coordinates, so translate it:

    ClientToScreen( &x, &y );
}

void wxWindowWasm::Update()
{

}

void wxWindowWasm::Refresh( bool WXUNUSED( eraseBackground ), const wxRect *rect )
{

}

bool wxWindowWasm::SetCursor( const wxCursor &cursor )
{
    return true;
}

bool wxWindowWasm::SetFont( const wxFont &font )
{

    return wxWindowBase::SetFont(font);
}


int wxWindowWasm::GetCharHeight() const
{
    return ( GetHandle()->fontMetrics().height() );
}


int wxWindowWasm::GetCharWidth() const
{

}

void wxWindowWasm::DoGetTextExtent(const wxString& string, int *x, int *y, int *descent,
        int *externalLeading, const wxFont *font ) const
{
    if ( x )
        *x = 0;
    if ( y )
        *y = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    // We can skip computing the string width and height if it is empty, but
    // not its descent and/or external leading, which still needs to be
    // returned even for an empty string.
    if ( string.empty() && !descent && !externalLeading )
        return;


}

void wxWindowWasm::SetScrollbar( int orientation, int pos, int thumbvisible, int range, bool WXUNUSED(refresh) )
{
    wxCHECK_RET(GetHandle(), "Window has not been created");

    //If not exist, create the scrollbar


}

void wxWindowWasm::SetScrollPos( int orientation, int pos, bool WXUNUSED( refresh ))
{

}

int wxWindowWasm::GetScrollPos( int orientation ) const
{

}

int wxWindowWasm::GetScrollThumb( int orientation ) const
{

}

int wxWindowWasm::GetScrollRange( int orientation ) const
{
    return scrollBar->maximum();
}

// scroll window to the specified position
void wxWindowWasm::ScrollWindow( int dx, int dy, const wxRect *rect )
{

}


#if wxUSE_DRAG_AND_DROP
void wxWindowWasm::SetDropTarget( wxDropTarget *dropTarget )
{

}
#endif

void wxWindowWasm::SetWindowStyleFlag( long style )
{
    wxWindowBase::SetWindowStyleFlag( style );

//    wxMISSING_IMPLEMENTATION( "wxWANTS_CHARS, wxTAB_TRAVERSAL" );
//    // wxFULL_REPAINT_ON_RESIZE: Qt::WResizeNoErase (marked obsolete)
//    // wxTRANSPARENT_WINDOW, wxCLIP_CHILDREN: Used in window for
//    //   performance, apparently not needed.
//
//    // wxWANTS_CHARS: Need to reimplement event()
//    //   See: http://doc.qt.nokia.com/latest/qwidget.html#events
//    // wxTAB_TRAVERSAL: reimplement focusNextPrevChild()
//
//    Qt::WindowFlags qtFlags = GetHandle()->windowFlags();
//
//    // For this to work Qt::CustomizeWindowHint must be set (done in Create())
//    if ( HasFlag( wxCAPTION ) )
//    {
//        // Enable caption bar and all buttons. This behavious
//        // is overwritten by subclasses (wxTopLevelWindow).
//        qtFlags |= Qt::WindowTitleHint;
//        qtFlags |= Qt::WindowSystemMenuHint;
//        qtFlags |= Qt::WindowMinMaxButtonsHint;
//        qtFlags |= Qt::WindowCloseButtonHint;
//    }
//    else
//    {
//        // Disable caption bar, include all buttons to be effective
//        qtFlags &= ~Qt::WindowTitleHint;
//        qtFlags &= ~Qt::WindowSystemMenuHint;
//        qtFlags &= ~Qt::WindowMinMaxButtonsHint;
//        qtFlags &= ~Qt::WindowCloseButtonHint;
//    }
//
//    GetHandle()->setWindowFlags( qtFlags );
//
//    // Validate border styles
//    int numberOfBorderStyles = 0;
//    if ( HasFlag( wxBORDER_NONE ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_STATIC ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_SIMPLE ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_RAISED ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_SUNKEN ))
//        numberOfBorderStyles++;
//    if ( HasFlag( wxBORDER_THEME ))
//        numberOfBorderStyles++;
//    wxCHECK_RET( numberOfBorderStyles <= 1, "Only one border style can be specified" );
//
//    // Borders only supported for QFrame's
//    QFrame *qtFrame = qobject_cast< QFrame* >( QtGetContainer() );
//    wxCHECK_RET( numberOfBorderStyles == 0 || qtFrame,
//                 "Borders not supported for this window type (not QFrame)" );
//
//    if ( HasFlag( wxBORDER_NONE ) )
//    {
//        qtFrame->setFrameStyle( QFrame::NoFrame );
//    }
//    else if ( HasFlag( wxBORDER_STATIC ) )
//    {
//        wxMISSING_IMPLEMENTATION( "wxBORDER_STATIC" );
//    }
//    else if ( HasFlag( wxBORDER_SIMPLE ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Plain );
//    }
//    else if ( HasFlag( wxBORDER_RAISED ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Raised );
//    }
//    else if ( HasFlag( wxBORDER_SUNKEN ) )
//    {
//        qtFrame->setFrameStyle( QFrame::Panel );
//        qtFrame->setFrameShadow( QFrame::Sunken );
//    }
//    else if ( HasFlag( wxBORDER_THEME ) )
//    {
//        qtFrame->setFrameStyle( QFrame::StyledPanel );
//        qtFrame->setFrameShadow( QFrame::Plain );
//    }


}

void wxWindowWasm::SetExtraStyle( long exStyle )
{

}



void wxWindowWasm::DoClientToScreen( int *x, int *y ) const
{

}


void wxWindowWasm::DoScreenToClient( int *x, int *y ) const
{

}


void wxWindowWasm::DoCaptureMouse()
{

}


void wxWindowWasm::DoReleaseMouse()
{
    wxCHECK_RET( GetHandle() != nullptr, wxT("invalid window") );
    GetHandle()->releaseMouse();
    s_capturedWindow = nullptr;
}

wxWindowWasm *wxWindowBase::GetCapture()
{
    return s_capturedWindow;
}


void wxWindowWasm::DoGetPosition(int *x, int *y) const
{

}


void wxWindowWasm::DoGetSize(int *width, int *height) const
{

}



void wxWindowWasm::DoSetSize(int x, int y, int width, int height, int sizeFlags )
{
    int currentX, currentY;
    GetPosition( &currentX, &currentY );
    if ( x == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        x = currentX;
    if ( y == wxDefaultCoord && !( sizeFlags & wxSIZE_ALLOW_MINUS_ONE ))
        y = currentY;

    // Should we use the best size:

    if (( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH )) ||
        ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT )))
    {
        const wxSize BEST_SIZE = GetBestSize();
        if ( width == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_WIDTH ))
            width = BEST_SIZE.x;
        if ( height == wxDefaultCoord && ( sizeFlags & wxSIZE_AUTO_HEIGHT ))
            height = BEST_SIZE.y;
    }

    int w, h;
    GetSize(&w, &h);
    if (width == -1)
        width = w;
    if (height == -1)
        height = h;

    DoMoveWindow( x, y, width, height );

    // An annoying feature of Qt
    // if a control is created with size of zero, it is set as hidden by qt
    // if it is then resized, in some cases it remains hidden, so it
    // needs to be shown here
    if (m_qtWindow && !m_qtWindow->isVisible() && IsShown())
        m_qtWindow->show();
}


void wxWindowWasm::DoGetClientSize(int *width, int *height) const
{

}


void wxWindowWasm::DoSetClientSize(int width, int height)
{

}

void wxWindowWasm::DoMoveWindow(int x, int y, int width, int height)
{

}

#if wxUSE_TOOLTIPS
void wxWindowWasm::QtApplyToolTip(const wxString& text)
{
    GetHandle()->setToolTip(wxQtConvertString(text));
}

void wxWindowWasm::DoSetToolTip( wxToolTip *tip )
{

}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindowWasm::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->UpdateUI();
    menu->GetHandle()->exec( GetHandle()->mapToGlobal( QPoint( x, y ) ) );

    return true;
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindowWasm::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxCHECK_RET(GetHandle(), "Window has not been created");

    wxWindowBase::SetAcceleratorTable( accel );

    // Disable previously set accelerators
    for ( wxVector<QShortcut*>::const_iterator it = m_qtShortcuts.begin();
          it != m_qtShortcuts.end(); ++it )
    {
        delete *it;
    }

    m_qtShortcuts = accel.ConvertShortcutTable(GetHandle());

    // Connect shortcuts to window
    for ( wxVector<QShortcut*>::const_iterator it = m_qtShortcuts.begin();
          it != m_qtShortcuts.end(); ++it )
    {
        QObject::connect( *it, &QShortcut::activated, m_qtShortcutHandler.get(), &wxQtShortcutHandler::activated );
        QObject::connect( *it, &QShortcut::activatedAmbiguously, m_qtShortcutHandler.get(), &wxQtShortcutHandler::activated );
    }
}
#endif // wxUSE_ACCEL

bool wxWindowWasm::SetBackgroundStyle(wxBackgroundStyle style)
{
    if (!wxWindowBase::SetBackgroundStyle(style))
        return false;
    // NOTE this could be called before creation, so it will be delayed:
    return QtSetBackgroundStyle();
}

bool wxWindowWasm::QtSetBackgroundStyle()
{
    QWidget *widget;
    // if it is a scroll area, don't make transparent (invisible) scroll bars:
    if ( QtGetScrollBarsContainer() )
        widget = QtGetScrollBarsContainer()->viewport();
    else
        widget = GetHandle();
    // check if the control is created (wxGTK requires calling it before):
    if ( widget != nullptr )
    {
        if (m_backgroundStyle == wxBG_STYLE_PAINT)
        {
            // wx paint handler will draw the invalidated region completely:
            widget->setAttribute(Qt::WA_OpaquePaintEvent);
        }
        else if (m_backgroundStyle == wxBG_STYLE_TRANSPARENT)
        {
            widget->setAttribute(Qt::WA_TranslucentBackground);
            // avoid a default background color (usually black):
            widget->setStyleSheet("background:transparent;");
        }
        else if (m_backgroundStyle == wxBG_STYLE_SYSTEM)
        {
            // let Qt erase the background by default
            // (note that wxClientDC will not work)
            //widget->setAutoFillBackground(true);
            // use system colors for background (default in Qt)
            widget->setAttribute(Qt::WA_NoSystemBackground, false);
        }
        else if (m_backgroundStyle == wxBG_STYLE_ERASE)
        {
            // erase events will be fired, if not handled, wx will clear the DC
            widget->setAttribute(Qt::WA_OpaquePaintEvent);
            // Qt should not clear the background (default):
            widget->setAutoFillBackground(false);
        }
    }
    return true;
}


bool wxWindowWasm::IsTransparentBackgroundSupported(wxString* WXUNUSED(reason)) const
{
    return true;
}

bool wxWindowWasm::SetTransparent(wxByte alpha)
{
    // For Qt, range is between 1 (opaque) and 0 (transparent)
    GetHandle()->setWindowOpacity(alpha/255.0);
    return true;
}


namespace
{

void wxQtChangeRoleColour(QPalette::ColorRole role,
                          QWidget* widget,
                          const wxColour& colour)
{
    QPalette palette = widget->palette();
    palette.setColor(role, colour.GetQColor());
    widget->setPalette(palette);
}

} // anonymous namespace

bool wxWindowWasm::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindowBase::SetBackgroundColour(colour) )
        return false;

    QWidget *widget = GetHandle();
    wxQtChangeRoleColour(widget->backgroundRole(), widget, colour);

    return true;
}

bool wxWindowWasm::SetForegroundColour(const wxColour& colour)
{
    if (!wxWindowBase::SetForegroundColour(colour))
        return false;

    QWidget *widget = GetHandle();
    wxQtChangeRoleColour(widget->foregroundRole(), widget, colour);

    return true;
}

bool wxWindowWasm::QtHandlePaintEvent ( QWidget *handler, QPaintEvent *event )
{
    /* If this window has scrollbars, only let wx handle the event if it is
     * for the client area (the scrolled part). Events for the whole window
     * (including scrollbars and maybe status or menu bars are handled by Qt */

    if ( QtGetScrollBarsContainer() && handler != QtGetScrollBarsContainer() )
    {
        return false;
    }
    else
    {
        // use the Qt event region:
        m_updateRegion.QtSetRegion( event->region() );

        // Prepare the Qt painter for wxWindowDC:
        bool ok = false;
        if ( QtGetScrollBarsContainer() )
        {
            // QScrollArea can only draw in the viewport:
            ok = m_qtPainter->begin( QtGetScrollBarsContainer()->viewport() );
        }
        if ( !ok )
        {
            // Start the paint in the widget itself
            ok =  m_qtPainter->begin( GetHandle() );
        }

        if ( ok )
        {
            bool handled;

            if ( m_qtPicture == nullptr )
            {
                // Real paint event (not for wxClientDC), prepare the background
                switch ( GetBackgroundStyle() )
                {
                    case wxBG_STYLE_TRANSPARENT:
                        if (IsTransparentBackgroundSupported())
                        {
                            // Set a transparent background, so that overlaying in parent
                            // might indeed let see through where this child did not
                            // explicitly paint. See wxBG_STYLE_SYSTEM for more comment
                        }
                        break;
                    case wxBG_STYLE_ERASE:
                        {
                            // the background should be cleared by qt auto fill
                            // send the erase event (properly creating a DC for it)
                            wxPaintDC dc( this );
                            dc.SetDeviceClippingRegion( m_updateRegion );

                            wxEraseEvent erase( GetId(), &dc );
                            erase.SetEventObject(this);
                            if ( ProcessWindowEvent(erase) )
                            {
                                // background erased, don't do it again
                                break;
                            }
                            // Ensure DC is cleared if handler didn't and Qt will not do it
                            if ( UseBgCol() && !GetHandle()->autoFillBackground() )
                            {
                                wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::QtHandlePaintEvent %s clearing DC to %s"),
                                           GetName(), GetBackgroundColour().GetAsString()
                                           );
                                dc.SetBackground(GetBackgroundColour());
                                dc.Clear();
                            }
                        }
                        // fall through
                    case wxBG_STYLE_SYSTEM:
                        if ( GetThemeEnabled() )
                        {
                            // let qt render the background:
                            // commented out as this will cause recursive painting
                            // this should be done outside using setBackgroundRole
                            // setAutoFillBackground or setAttribute
                            //wxWindowDC dc( (wxWindow*)this );
                            //widget->render(m_qtPainter);
                        }
                        break;
                    case wxBG_STYLE_PAINT:
                        // nothing to do: window will be painted over in EVT_PAINT
                        break;

                    case wxBG_STYLE_COLOUR:
                        wxFAIL_MSG( "unsupported background style" );
                }

                // send the paint event (wxWindowDC will draw directly):
                wxPaintEvent paint( this );
                handled = ProcessWindowEvent(paint);
                m_updateRegion.Clear();
            }
            else
            {
                // Data from wxClientDC, paint it
                m_qtPicture->play( m_qtPainter.get() );
                // Reset picture
                m_qtPicture->setData( nullptr, 0 );
                handled = true;
            }

            // commit changes of the painter to the widget
            m_qtPainter->end();

            return handled;
        }
        else
        {
            // Painter didn't begun, not handled by wxWidgets:
            wxLogTrace(TRACE_QT_WINDOW, wxT("wxWindow::QtHandlePaintEvent %s Qt widget painter begin failed"), GetName() );
            return false;
        }
    }
}

bool wxWindowWasm::QtHandleResizeEvent ( QWidget *WXUNUSED( handler ), QResizeEvent *event )
{
    wxSizeEvent e( wxQtConvertSize( event->size() ) );
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowWasm::QtHandleWheelEvent ( QWidget *WXUNUSED( handler ), QWheelEvent *event )
{
    wxMouseEvent e( wxEVT_MOUSEWHEEL );
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QPoint qPt = event->position().toPoint();
#else
    QPoint qPt = event->pos();
#endif
    e.SetPosition( wxQtConvertPoint( qPt ) );
    e.SetEventObject(this);

    e.m_wheelAxis = ( event->orientation() == Qt::Vertical ) ? wxMOUSE_WHEEL_VERTICAL : wxMOUSE_WHEEL_HORIZONTAL;
    e.m_wheelRotation = event->delta();
    e.m_linesPerAction = 3;
    e.m_wheelDelta = 120;

    return ProcessWindowEvent( e );
}


bool wxWindowWasm::QtHandleKeyEvent ( QWidget *WXUNUSED( handler ), QKeyEvent *event )
{
    // qt sends keyup and keydown events for autorepeat, but this is not
    // normal for wx which only sends repeated keydown events
    // discard repeated keyup events
    if ( event->isAutoRepeat() && event->type() == QEvent::KeyRelease )
        return true;

#if wxUSE_ACCEL
    if ( m_processingShortcut )
    {
        /* Enter here when a shortcut isn't handled by Qt.
         * Return true to avoid Qt-processing of the event
         * Instead, use the flag to indicate that it wasn't processed */
        m_processingShortcut = false;

        return true;
    }
#endif // wxUSE_ACCEL

    bool handled = false;

    // Build the event
    wxKeyEvent e( event->type() == QEvent::KeyPress ? wxEVT_KEY_DOWN : wxEVT_KEY_UP );
    e.SetEventObject(this);
    // TODO: m_x, m_y
    e.m_keyCode = wxQtConvertKeyCode( event->key(), event->modifiers() );

    if ( event->text().isEmpty() )
        e.m_uniChar = 0;
    else
        e.m_uniChar = event->text().at( 0 ).unicode();

    e.m_rawCode = event->nativeVirtualKey();
    e.m_rawFlags = event->nativeModifiers();
    e.m_isRepeat = event->isAutoRepeat();

    // Modifiers
    wxQtFillKeyboardModifiers( event->modifiers(), &e );

    handled = ProcessWindowEvent( e );

    // On key presses, send the EVT_CHAR event
    if ( !handled && event->type() == QEvent::KeyPress )
    {
#if wxUSE_ACCEL
        // Check for accelerators
        if ( !m_processingShortcut )
        {
            /* The call to notify() will try to execute a shortcut. If it fails
             * it will call keyPressEvent() in our wxQtWidget which calls back
             * to this function. We use the m_processingShortcut flag to avoid
             * processing that recursive call and return back to this one. */
            m_processingShortcut = true;

            QApplication::instance()->notify( GetHandle(), event );

            handled = m_processingShortcut;
            m_processingShortcut = false;

            if ( handled )
                return true;
        }
#endif // wxUSE_ACCEL

        e.SetEventType( wxEVT_CHAR );
        e.SetEventObject(this);

        // Translated key code (including control + letter -> 1-26)
        int translated = 0;
        if ( !event->text().isEmpty() )
            translated = event->text().at( 0 ).toLatin1();
        if ( translated )
            e.m_keyCode = translated;

        handled = ProcessWindowEvent( e );
    }

    return handled;
}

bool wxWindowWasm::QtHandleMouseEvent ( QWidget *handler, QMouseEvent *event )
{
    // Convert event type
    wxEventType wxType = 0;
    switch ( event->type() )
    {
        case QEvent::MouseButtonDblClick:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_DCLICK;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_DCLICK;
                    break;
                case Qt::MiddleButton:
                    wxType = wxEVT_MIDDLE_DCLICK;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_DCLICK;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_DCLICK;
                    break;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                default:
                    return false;
            }
            break;
        case QEvent::MouseButtonPress:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_DOWN;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_DOWN;
                    break;
                case Qt::MiddleButton:
                    wxType = wxEVT_MIDDLE_DOWN;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_DOWN;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_DOWN;
                    break;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                default:
                    return false;
            }
            break;
        case QEvent::MouseButtonRelease:
            switch ( event->button() )
            {
                case Qt::LeftButton:
                    wxType = wxEVT_LEFT_UP;
                    break;
                case Qt::RightButton:
                    wxType = wxEVT_RIGHT_UP;
                    break;
                case Qt::MiddleButton:
                    wxType = wxEVT_MIDDLE_UP;
                    break;
                case Qt::XButton1:
                    wxType = wxEVT_AUX1_UP;
                    break;
                case Qt::XButton2:
                    wxType = wxEVT_AUX2_UP;
                    break;
                case Qt::NoButton:
                case Qt::MouseButtonMask: // Not documented ?
                default:
                    return false;
            }
            break;
        case QEvent::MouseMove:
            wxType = wxEVT_MOTION;
            break;
        default:
            // Unknown event type
            wxFAIL_MSG( "Unknown mouse event type" );
    }

    // Fill the event

    // Use screen position as the event might originate from a different
    // Qt window than this one.
    wxPoint mousePos = ScreenToClient(wxQtConvertPoint(event->globalPos()));

    wxMouseEvent e( wxType );
    e.SetEventObject(this);
    e.m_clickCount = -1;
    e.SetPosition(mousePos);

    // Mouse buttons
    wxQtFillMouseButtons( event->buttons(), &e );

    // Keyboard modifiers
    wxQtFillKeyboardModifiers( event->modifiers(), &e );

    bool handled = ProcessWindowEvent( e );

    // Determine if mouse is inside the widget
    bool mouseInside = true;
    if ( mousePos.x < 0 || mousePos.x > handler->width() ||
        mousePos.y < 0 || mousePos.y > handler->height() )
        mouseInside = false;

    if ( e.GetEventType() == wxEVT_MOTION )
    {
        /* Qt doesn't emit leave/enter events while the mouse is grabbed
        * and it automatically grabs the mouse while dragging. In that cases
        * we emulate the enter and leave events */

        // Mouse enter/leaves
        if ( m_mouseInside != mouseInside )
        {
            if ( mouseInside )
                e.SetEventType( wxEVT_ENTER_WINDOW );
            else
                e.SetEventType( wxEVT_LEAVE_WINDOW );

            ProcessWindowEvent( e );
        }

        QtSendSetCursorEvent(this, wxQtConvertPoint( event->globalPos()));
    }

    m_mouseInside = mouseInside;

    return handled;
}

bool wxWindowWasm::QtHandleEnterEvent ( QWidget *handler, QEvent *event )
{
    wxMouseEvent e( event->type() == QEvent::Enter ? wxEVT_ENTER_WINDOW : wxEVT_LEAVE_WINDOW );
    e.m_clickCount = 0;
    e.SetPosition( wxQtConvertPoint( handler->mapFromGlobal( QCursor::pos() ) ) );
    e.SetEventObject(this);

    // Mouse buttons
    wxQtFillMouseButtons( QApplication::mouseButtons(), &e );

    // Keyboard modifiers
    wxQtFillKeyboardModifiers( QApplication::keyboardModifiers(), &e );

    return ProcessWindowEvent( e );
}

bool wxWindowWasm::QtHandleMoveEvent ( QWidget *handler, QMoveEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    wxMoveEvent e( wxQtConvertPoint( event->pos() ), GetId() );
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowWasm::QtHandleShowEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    wxShowEvent e(GetId(), event->type() == QEvent::Show);
    e.SetEventObject(this);

    return ProcessWindowEvent( e );
}

bool wxWindowWasm::QtHandleChangeEvent ( QWidget *handler, QEvent *event )
{
    if ( GetHandle() != handler )
        return false;

    if ( event->type() == QEvent::ActivationChange )
    {
        wxActivateEvent e( wxEVT_ACTIVATE, handler->isActiveWindow(), GetId() );
        e.SetEventObject(this);

        return ProcessWindowEvent( e );
    }
    else
        return false;
}

// Returns true if the closing should be vetoed and false if the window should be closed.
bool wxWindowWasm::QtHandleCloseEvent ( QWidget *handler, QCloseEvent *WXUNUSED( event ) )
{
    if ( GetHandle() != handler )
        return false;

    // This is required as Qt will still send out close events when the window is disabled.
    if ( !IsEnabled() )
        return true;

    return !Close();
}

bool wxWindowWasm::QtHandleContextMenuEvent ( QWidget *WXUNUSED( handler ), QContextMenuEvent *event )
{
    const wxPoint pos =
        event->reason() == QContextMenuEvent::Keyboard
            ? wxDefaultPosition
            : wxQtConvertPoint( event->globalPos() );
    return WXSendContextMenuEvent(pos);
}

bool wxWindowWasm::QtHandleFocusEvent ( QWidget *WXUNUSED( handler ), QFocusEvent *event )
{
    wxFocusEvent e( event->gotFocus() ? wxEVT_SET_FOCUS : wxEVT_KILL_FOCUS, GetId() );
    e.SetEventObject(this);
    e.SetWindow(event->gotFocus() ? this : FindFocus());

    bool handled = ProcessWindowEvent( e );

    wxWindowWasm *parent = GetParent();
    if ( event->gotFocus() && parent )
    {
        wxChildFocusEvent childEvent( this );
        parent->ProcessWindowEvent( childEvent );
    }

    return handled;
}

#if wxUSE_ACCEL
void wxWindowWasm::QtHandleShortcut ( int command )
{
    if (command != -1)
    {
        wxCommandEvent menu_event( wxEVT_COMMAND_MENU_SELECTED, command );
        bool ret = ProcessWindowEvent( menu_event );

        if ( !ret )
        {
            // if the accelerator wasn't handled as menu event, try
            // it as button click (for compatibility with other
            // platforms):
            wxCommandEvent button_event( wxEVT_COMMAND_BUTTON_CLICKED, command );
            button_event.SetEventObject(this);
            ProcessWindowEvent( button_event );
        }
    }
}
#endif // wxUSE_ACCEL

QWidget *wxWindowWasm::GetHandle() const
{
    return m_qtWindow;
}

QScrollArea *wxWindowWasm::QtGetScrollBarsContainer() const
{
    return m_qtContainer;
}

void wxWindowWasm::QtSetPicture( QPicture* pict )
{
    m_qtPicture = pict;
}

QPainter *wxWindowWasm::QtGetPainter()
{
    return m_qtPainter.get();
}

bool wxWindowWasm::EnableTouchEvents(int eventsMask)
{
    wxCHECK_MSG( GetHandle(), false, "can't be called before creating the window" );

    if ( eventsMask == wxTOUCH_NONE )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, false);
        return true;
    }

    if ( eventsMask & wxTOUCH_PRESS_GESTURES )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::TapAndHoldGesture);
        QTapAndHoldGesture::setTimeout ( 1000 );
    }
    if ( eventsMask & wxTOUCH_PAN_GESTURES )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::PanGesture);
    }
    if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
    {
        m_qtWindow->setAttribute(Qt::WA_AcceptTouchEvents, true);
        m_qtWindow->grabGesture(Qt::PinchGesture);
    }

    return true;
}
