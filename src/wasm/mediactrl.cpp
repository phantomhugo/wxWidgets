/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/mediactrl.cpp
// Purpose:     wxMediaCtrl backend using the HTML5 <video> element
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MEDIACTRL

#include "wx/mediactrl.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
#endif

#include <emscripten.h>

// Force link into main library so this backend can be loaded
#include "wx/html/forcelnk.h"
FORCE_LINK_ME(basewxmediabackends)

// ----------------------------------------------------------------------------
// wxWasmMediaBackend: plays media with a HTML5 <video> element appended to
// the control's container <div>. The DOM events of the element are reported
// through the generic 'addEvent' queue and reach us via
// wxMediaCtrl::WasmNotifyEvent() (see the bottom of this file).
// ----------------------------------------------------------------------------

class wxWasmMediaBackend : public wxMediaBackendCommonBase
{
public:
    wxWasmMediaBackend() : m_state(wxMEDIASTATE_STOPPED) { }
    virtual ~wxWasmMediaBackend();

    virtual bool CreateControl(wxControl* ctrl, wxWindow* parent,
                               wxWindowID id,
                               const wxPoint& pos,
                               const wxSize& size,
                               long style,
                               const wxValidator& validator,
                               const wxString& name) override;

    virtual bool Play() override;
    virtual bool Pause() override;
    virtual bool Stop() override;

    virtual bool Load(const wxString& fileName) override;
    virtual bool Load(const wxURI& location) override;

    virtual bool SetPosition(wxLongLong where) override;
    virtual wxLongLong GetPosition() override;
    virtual wxLongLong GetDuration() override;

    virtual void Move(int x, int y, int w, int h) override;
    virtual wxSize GetVideoSize() const override;

    virtual double GetPlaybackRate() override;
    virtual bool SetPlaybackRate(double dRate) override;

    virtual wxMediaState GetState() override { return m_state; }

    virtual double GetVolume() override;
    virtual bool SetVolume(double dVolume) override;

    virtual bool ShowPlayerControls(wxMediaCtrlPlayerControls flags) override;
    virtual bool IsInterfaceShown() override;

    // Handles the DOM events of the <video> element, forwarded by
    // wxMediaCtrl::WasmNotifyEvent().
    void NotifyDomEvent(const std::string& eventType);

protected:
    bool DoLoad(const wxString& location, bool isUri);

    wxMediaState m_state;

    wxDECLARE_DYNAMIC_CLASS(wxWasmMediaBackend);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxWasmMediaBackend, wxMediaBackend);

// ----------------------------------------------------------------------------
// wxWasmMediaBackend dtor: release the Blob URL created for MEMFS files
// ----------------------------------------------------------------------------

wxWasmMediaBackend::~wxWasmMediaBackend()
{
    if ( m_ctrl )
    {
        EM_ASM_({
            var container = document.getElementById($0);
            if (!container) return;
            var video = container.querySelector('video.wxMediaCtrl');
            if (video && video.dataset.blobUrl)
                URL.revokeObjectURL(video.dataset.blobUrl);
        }, m_ctrl->GetId());
    }
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::CreateControl
// ----------------------------------------------------------------------------

bool wxWasmMediaBackend::CreateControl(wxControl* ctrl, wxWindow* parent,
                            wxWindowID id,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxValidator& validator,
                            const wxString& name)
{
    if ( !ctrl->wxControl::Create(parent, id, pos, size, style,
                                  validator, name) )
        return false;

    m_ctrl = static_cast<wxMediaCtrl*>(ctrl);

    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;

        var video = document.createElement('video');
        video.className = 'wxMediaCtrl';
        video.style.width = '100%';
        video.style.height = '100%';
        video.preload = 'auto';

        function notify(eventType) {
            if (typeof Module !== 'undefined' && Module.ccall) {
                Module.ccall('addEvent', null,
                    ['number', 'string', 'number', 'number'],
                    [$0, eventType, 0, 0]);
            }
        }

        video.addEventListener('loadeddata',
            function() { notify('medialoaded'); });
        video.addEventListener('play',
            function() { notify('mediaplay'); });
        video.addEventListener('pause',
            function() { notify('mediapause'); });
        video.addEventListener('ended',
            function() { notify('mediaended'); });

        container.appendChild(video);
    }, m_ctrl->GetId());

    return true;
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::NotifyDomEvent
//
// Called from wxMediaCtrl::WasmNotifyEvent() with the DOM events of the
// <video> element. State changes initiated by our own Play()/Pause()/Stop()
// are already handled there, so here we only react to events that do not
// match the current state, i.e. the ones coming from the native <video>
// controls or from the playback itself.
// ----------------------------------------------------------------------------

void wxWasmMediaBackend::NotifyDomEvent(const std::string& eventType)
{
    if ( eventType == "medialoaded" )
    {
        NotifyMovieLoaded();
    }
    else if ( eventType == "mediaplay" )
    {
        if ( m_state != wxMEDIASTATE_PLAYING )
        {
            m_state = wxMEDIASTATE_PLAYING;
            QueuePlayEvent();
        }
    }
    else if ( eventType == "mediapause" )
    {
        if ( m_state == wxMEDIASTATE_PLAYING )
        {
            m_state = wxMEDIASTATE_PAUSED;
            QueuePauseEvent();
        }
    }
    else if ( eventType == "mediaended" )
    {
        m_state = wxMEDIASTATE_STOPPED;
        QueueFinishEvent();
    }
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::Play/Pause/Stop
// ----------------------------------------------------------------------------

bool wxWasmMediaBackend::Play()
{
    const bool ok = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        // play() returns a promise, rejected e.g. when no source is
        // loaded or autoplay is blocked by the browser.
        var promise = video.play();
        if (promise && promise.catch)
            promise.catch(function() {});
        return 1;
    }, m_ctrl->GetId()) != 0;

    if ( !ok )
        return false;

    m_state = wxMEDIASTATE_PLAYING;
    QueuePlayEvent();
    return true;
}

bool wxWasmMediaBackend::Pause()
{
    const bool ok = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        video.pause();
        return 1;
    }, m_ctrl->GetId()) != 0;

    if ( !ok )
        return false;

    m_state = wxMEDIASTATE_PAUSED;
    QueuePauseEvent();
    return true;
}

bool wxWasmMediaBackend::Stop()
{
    if ( !SendStopEvent() )
        return false;

    const bool ok = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        video.pause();
        video.currentTime = 0;
        return 1;
    }, m_ctrl->GetId()) != 0;

    if ( !ok )
        return false;

    m_state = wxMEDIASTATE_STOPPED;
    QueueEvent(wxEVT_MEDIA_STATECHANGED);
    return true;
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::Load
//
// A wxURI is passed to the element untouched. A plain file name refers to
// the Emscripten virtual file system (MEMFS), which the browser cannot
// reach, so the file is copied into a Blob and played through an object URL
// (names that already are URLs are used directly as well).
// ----------------------------------------------------------------------------

bool wxWasmMediaBackend::Load(const wxString& fileName)
{
    return DoLoad(fileName, false);
}

bool wxWasmMediaBackend::Load(const wxURI& location)
{
    return DoLoad(location.BuildURI(), true);
}

bool wxWasmMediaBackend::DoLoad(const wxString& location, bool isUri)
{
    if ( !m_ctrl )
        return false;

    const wxCharBuffer buf = location.ToUTF8();

    const bool ok = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;

        var src = UTF8ToString($1);
        if (!$2 && !/^(https?|blob|data):/i.test(src)) {
            // Local file in MEMFS: wrap it in a Blob URL.
            try {
                var data = FS.readFile(src);
                if (video.dataset.blobUrl)
                    URL.revokeObjectURL(video.dataset.blobUrl);
                src = URL.createObjectURL(new Blob([data]));
                video.dataset.blobUrl = src;
            } catch (e) {
                return 0;
            }
        }

        video.src = src;
        video.load();
        return 1;
    }, m_ctrl->GetId(), buf.data(), isUri ? 1 : 0) != 0;

    if ( ok )
        m_state = wxMEDIASTATE_STOPPED;

    return ok;
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::SetPosition/GetPosition/GetDuration
//
// HTML5 media time is in seconds, wxMediaCtrl uses milliseconds.
// ----------------------------------------------------------------------------

bool wxWasmMediaBackend::SetPosition(wxLongLong where)
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        video.currentTime = $1 / 1000;
        return 1;
    }, m_ctrl->GetId(), static_cast<double>(where.ToLong())) != 0;
}

wxLongLong wxWasmMediaBackend::GetPosition()
{
    return wxLongLong(EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return video ? Math.round(video.currentTime * 1000) : 0;
    }, m_ctrl->GetId()));
}

wxLongLong wxWasmMediaBackend::GetDuration()
{
    return wxLongLong(EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video || !isFinite(video.duration)) return 0;
        return Math.round(video.duration * 1000);
    }, m_ctrl->GetId()));
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::Move/GetVideoSize
// ----------------------------------------------------------------------------

void wxWasmMediaBackend::Move(int WXUNUSED(x), int WXUNUSED(y),
                              int WXUNUSED(w), int WXUNUSED(h))
{
    // Nothing to do: the <video> element always fills the container <div>,
    // which is already moved by wxMediaCtrl::DoMoveWindow().
}

wxSize wxWasmMediaBackend::GetVideoSize() const
{
    if ( !m_ctrl )
        return wxSize(0, 0);

    const int w = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return video ? video.videoWidth : 0;
    }, m_ctrl->GetId());

    const int h = EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return video ? video.videoHeight : 0;
    }, m_ctrl->GetId());

    return wxSize(w, h);
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::GetPlaybackRate/SetPlaybackRate
// ----------------------------------------------------------------------------

double wxWasmMediaBackend::GetPlaybackRate()
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return video ? video.playbackRate : 0;
    }, m_ctrl->GetId());
}

bool wxWasmMediaBackend::SetPlaybackRate(double dRate)
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        video.playbackRate = $1;
        return 1;
    }, m_ctrl->GetId(), dRate) != 0;
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::GetVolume/SetVolume
//
// Both wxMediaCtrl and HTML5 use a 0.0-1.0 volume range.
// ----------------------------------------------------------------------------

double wxWasmMediaBackend::GetVolume()
{
    return EM_ASM_DOUBLE({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return video ? video.volume : 0;
    }, m_ctrl->GetId());
}

bool wxWasmMediaBackend::SetVolume(double dVolume)
{
    if ( dVolume < 0.0 )
        dVolume = 0.0;
    else if ( dVolume > 1.0 )
        dVolume = 1.0;

    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        if (!video) return 0;
        video.volume = $1;
        return 1;
    }, m_ctrl->GetId(), dVolume) != 0;
}

// ----------------------------------------------------------------------------
// wxWasmMediaBackend::ShowPlayerControls/IsInterfaceShown
//
// The native <video> controls bundle seek bar, volume and play/pause
// together, so any requested control group enables them all.
// ----------------------------------------------------------------------------

bool wxWasmMediaBackend::ShowPlayerControls(wxMediaCtrlPlayerControls flags)
{
    EM_ASM_({
        var container = document.getElementById($0);
        if (!container) return;
        var video = container.querySelector('video.wxMediaCtrl');
        if (video) video.controls = !!$1;
    }, m_ctrl->GetId(), flags != wxMEDIACTRLPLAYERCONTROLS_NONE ? 1 : 0);

    return true;
}

bool wxWasmMediaBackend::IsInterfaceShown()
{
    return EM_ASM_INT({
        var container = document.getElementById($0);
        if (!container) return 0;
        var video = container.querySelector('video.wxMediaCtrl');
        return (video && video.controls) ? 1 : 0;
    }, m_ctrl->GetId()) != 0;
}

// ----------------------------------------------------------------------------
// wxMediaCtrl::WasmNotifyEvent
//
// Receives the DOM events of the <video> element (reported to 'addEvent'
// with the id of this control) and forwards them to the backend.
// ----------------------------------------------------------------------------

void wxMediaCtrl::WasmNotifyEvent(const wxWasmEvent& event)
{
    if ( event.id != m_windowId )
        return;

    if ( event.eventType.compare(0, 5, "media") == 0 )
    {
        wxWasmMediaBackend* backend = wxDynamicCast(m_imp, wxWasmMediaBackend);
        if ( backend )
            backend->NotifyDomEvent(event.eventType);
        return;
    }

    wxControl::WasmNotifyEvent(event);
}

#endif // wxUSE_MEDIACTRL
