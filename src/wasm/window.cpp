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
#include <emscripten.h>

#define VERT_SCROLLBAR_POSITION 0, 1
#define HORZ_SCROLLBAR_POSITION 1, 0

#if wxUSE_ACCEL
#endif // wxUSE_ACCEL

//##############################################################################

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

    if (m_parent)
        parent->AddChild( this );

    wxPoint p;
    if ( pos != wxDefaultPosition )
        p = pos;

    PostCreation();

    return true;
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

    // Show can be called before the underlying window is created:
    bool result=false;
    if(show!=IsShown())
    {
        if(show)
        {
            result=EM_ASM_INT(
                {
                    document.getElementById($0).style.display="block";
                    return 1;
                },
                GetId()
            );
        }
        else
        {
            result=EM_ASM_INT(
                {
                    document.getElementById($0).style.display="none";
                    return 1;
                },
                GetId()
            );
        }

    }
    if ( !wxWindowBase::Show( show ))
        return false;

    wxSizeEvent event(GetSize(), GetId());
    event.SetEventObject(this);
    HandleWindowEvent(event);

    return result;
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
    //return ( GetHandle()->fontMetrics().height() );
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
    //return scrollBar->maximum();
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
    //GetHandle()->releaseMouse();
    //s_capturedWindow = nullptr;
}

wxWindowWasm *wxWindowBase::GetCapture()
{
    //return s_capturedWindow;
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
    // if (m_qtWindow && !m_qtWindow->isVisible() && IsShown())
    //     m_qtWindow->show();
}


void wxWindowWasm::DoGetClientSize(int *width, int *height) const
{

}


void wxWindowWasm::DoSetClientSize(int width, int height)
{

}

void wxWindowWasm::DoMoveWindow(int x, int y, int width, int height)
{
    EM_ASM_INT(
        {
            const currentWindow=document.getElementById($0);
            currentWindow.style.position="absolute";
            currentWindow.style.width=$3.toString()+"px";
            currentWindow.style.height=$4.toString()+"px";
            currentWindow.style.top=$2.toString()+"px";
            currentWindow.style.left=$1.toString()+"px";
            return 1;
        },
        GetId(),
        x,y,width,height
    );
}

#if wxUSE_TOOLTIPS


void wxWindowWasm::DoSetToolTip( wxToolTip *tip )
{

}
#endif // wxUSE_TOOLTIPS


#if wxUSE_MENUS
bool wxWindowWasm::DoPopupMenu(wxMenu *menu, int x, int y)
{
    menu->UpdateUI();
    //menu->GetHandle()->exec( GetHandle()->mapToGlobal( QPoint( x, y ) ) );

    return true;
}
#endif // wxUSE_MENUS

#if wxUSE_ACCEL
void wxWindowWasm::SetAcceleratorTable( const wxAcceleratorTable& accel )
{
    wxCHECK_RET(GetHandle(), "Window has not been created");

    wxWindowBase::SetAcceleratorTable( accel );

}
#endif // wxUSE_ACCEL

bool wxWindowWasm::SetBackgroundStyle(wxBackgroundStyle style)
{
    if (!wxWindowBase::SetBackgroundStyle(style))
        return false;
    // NOTE this could be called before creation, so it will be delayed:
    //return QtSetBackgroundStyle();
}


bool wxWindowWasm::IsTransparentBackgroundSupported(wxString* WXUNUSED(reason)) const
{
    return true;
}

bool wxWindowWasm::SetTransparent(wxByte alpha)
{
    // For Qt, range is between 1 (opaque) and 0 (transparent)
    //GetHandle()->setWindowOpacity(alpha/255.0);
    return true;
}


bool wxWindowWasm::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxWindowBase::SetBackgroundColour(colour) )
        return false;

//    WXWidget *widget = GetHandle();

    return true;
}

bool wxWindowWasm::SetForegroundColour(const wxColour& colour)
{
    if (!wxWindowBase::SetForegroundColour(colour))
        return false;

    return true;
}

WXWidget wxWindowWasm::GetHandle() const
{
//    return m_qtWindow;
}

bool wxWindowWasm::EnableTouchEvents(int eventsMask)
{
    wxCHECK_MSG( GetHandle(), false, "can't be called before creating the window" );

    if ( eventsMask == wxTOUCH_NONE )
    {

        return true;
    }

    if ( eventsMask & wxTOUCH_PRESS_GESTURES )
    {
    }
    if ( eventsMask & wxTOUCH_PAN_GESTURES )
    {
    }
    if ( eventsMask & wxTOUCH_ZOOM_GESTURE )
    {
    }

    return true;
}

wxWindow *wxWindowBase::DoFindFocus()
{

}

wxWindow *wxWindowWasm::DoFindFocus()
{

}




void wxWindowWasm::WasmNotifyEvent(const wxWasmEvent& event)
{
    if(event.id==m_windowId)
    {
        if(event.eventType=="click")
        {
            wxCommandEvent generatedEvent;
            HandleWindowEvent(generatedEvent);
        }
        else if(event.eventType=="dblclick")
        {

        }
    }
}
