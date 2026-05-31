/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/sound.cpp
// Purpose:     wxSound implementation for WASM using Web Audio API
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SOUND

#include "wx/sound.h"
#include "wx/log.h"
#include "wx/file.h"
#include "wx/module.h"

#include <emscripten.h>
#include <emscripten/em_js.h>
#include <emscripten/wget.h>

// ----------------------------------------------------------------------------
// JavaScript audio helpers using EM_JS
// ----------------------------------------------------------------------------

EM_JS(void, wxWasmPlaySound, (const void* data, size_t length, int loop), {
    if (typeof Module === 'undefined' || !Module.HEAPU8) {
        console.error('wxWasmPlaySound: Module.HEAPU8 not available');
        return;
    }
    // Copy data from WASM heap into a JS Uint8Array
    var arr = new Uint8Array(Module.HEAPU8.buffer, data, length);
    var blob = new Blob([arr], {type: 'audio/wav'});
    var url = URL.createObjectURL(blob);
    var audio = new Audio(url);
    if (loop) audio.loop = true;

    if (!Module.wxWasmSounds) Module.wxWasmSounds = [];
    Module.wxWasmSounds.push(audio);

    audio.onended = function() {
        if (Module.wxWasmSounds) {
            var idx = Module.wxWasmSounds.indexOf(audio);
            if (idx >= 0) Module.wxWasmSounds.splice(idx, 1);
        }
        URL.revokeObjectURL(url);
    };

    audio.play().catch(function(e) {
        console.error('wxSound play failed:', e);
        if (Module.wxWasmSounds) {
            var idx = Module.wxWasmSounds.indexOf(audio);
            if (idx >= 0) Module.wxWasmSounds.splice(idx, 1);
        }
        URL.revokeObjectURL(url);
    });
});

EM_JS(void, wxWasmStopSounds, (), {
    if (Module.wxWasmSounds) {
        Module.wxWasmSounds.forEach(function(audio) {
            audio.pause();
            audio.currentTime = 0;
        });
        Module.wxWasmSounds = [];
    }
});

EM_JS(int, wxWasmIsPlayingSound, (), {
    if (!Module.wxWasmSounds) return 0;
    Module.wxWasmSounds = Module.wxWasmSounds.filter(function(audio) {
        return !audio.ended;
    });
    var playing = Module.wxWasmSounds.filter(function(audio) {
        return !audio.paused;
    });
    return playing.length > 0 ? 1 : 0;
});

// ----------------------------------------------------------------------------
// WAV parsing helpers (adapted from src/unix/sound.cpp)
// ----------------------------------------------------------------------------

typedef struct
{
    wxUint32      uiSize;
    wxUint16      uiFormatTag;
    wxUint16      uiChannels;
    wxUint32      ulSamplesPerSec;
    wxUint32      ulAvgBytesPerSec;
    wxUint16      uiBlockAlign;
    wxUint16      uiBitsPerSample;
} WAVEFORMAT;

#define WAVE_FORMAT_PCM  1
#define WAVE_INDEX       8
#define FMT_INDEX       12

// ----------------------------------------------------------------------------
// wxSoundData
// ----------------------------------------------------------------------------

void wxSoundData::IncRef()
{
    m_refCnt++;
}

void wxSoundData::DecRef()
{
    if (--m_refCnt == 0)
        delete this;
}

wxSoundData::~wxSoundData()
{
    delete[] m_dataWithHeader;
}

// ----------------------------------------------------------------------------
// wxSound
// ----------------------------------------------------------------------------

wxSoundBackend* wxSound::ms_backend = nullptr;

wxSound::wxSound()
    : m_data(nullptr)
{
}

wxSound::wxSound(const wxString& fileName, bool isResource)
    : m_data(nullptr)
{
    Create(fileName, isResource);
}

wxSound::wxSound(size_t size, const void* data)
    : m_data(nullptr)
{
    Create(size, data);
}

wxSound::~wxSound()
{
    Free();
}

bool wxSound::Create(const wxString& fileName, bool isResource)
{
    if (isResource)
    {
        wxLogError(wxT("wxSound: resources not supported in WASM"));
        return false;
    }

    // First, try to read from the virtual file system (MEMFS)
    {
        wxFile file(fileName);
        if (file.IsOpened())
        {
            wxFileOffset len = file.Length();
            if (len != wxInvalidOffset && len > 0)
            {
                wxUint8* buf = new wxUint8[(size_t)len];
                if (file.Read(buf, (size_t)len) == (ssize_t)len)
                {
                    bool ok = LoadWAV(buf, (size_t)len, false);
                    if (!ok)
                        delete[] buf;
                    return ok;
                }
                delete[] buf;
            }
        }
    }

    // Fallback: load via synchronous HTTP request from the server
    void* buffer = nullptr;
    int numBytes = 0;
    int error = 0;
    emscripten_wget_data(fileName.ToUTF8().data(), &buffer, &numBytes, &error);
    if (error != 0 || !buffer || numBytes <= 0)
        return false;

    bool ok = LoadWAV(buffer, (size_t)numBytes, true);
    free(buffer);
    return ok;
}

bool wxSound::Create(size_t size, const void* data)
{
    return LoadWAV(data, size, true);
}

bool wxSound::LoadWAV(const void* data_, size_t length, bool copyData)
{
    if (length < 44)
        return false;

    const wxUint8* data = static_cast<const wxUint8*>(data_);

    WAVEFORMAT waveformat;
    memcpy(&waveformat, &data[FMT_INDEX + 4], sizeof(WAVEFORMAT));
    waveformat.uiSize = wxUINT32_SWAP_ON_BE(waveformat.uiSize);
    waveformat.uiFormatTag = wxUINT16_SWAP_ON_BE(waveformat.uiFormatTag);
    waveformat.uiChannels = wxUINT16_SWAP_ON_BE(waveformat.uiChannels);
    waveformat.ulSamplesPerSec = wxUINT32_SWAP_ON_BE(waveformat.ulSamplesPerSec);
    waveformat.ulAvgBytesPerSec = wxUINT32_SWAP_ON_BE(waveformat.ulAvgBytesPerSec);
    waveformat.uiBlockAlign = wxUINT16_SWAP_ON_BE(waveformat.uiBlockAlign);
    waveformat.uiBitsPerSample = wxUINT16_SWAP_ON_BE(waveformat.uiBitsPerSample);

    if (memcmp(data, "RIFF", 4) != 0)
        return false;
    if (memcmp(&data[WAVE_INDEX], "WAVE", 4) != 0)
        return false;
    if (memcmp(&data[FMT_INDEX], "fmt ", 4) != 0)
        return false;

    if (waveformat.uiSize != 16)
        return false;

    wxUint32 data_offset = FMT_INDEX + waveformat.uiSize + 8;
    if (memcmp(&data[data_offset], "LIST", 4) == 0)
    {
        wxUint32 list_chunk_length;
        memcpy(&list_chunk_length, &data[data_offset + 4], 4);
        list_chunk_length = wxUINT32_SWAP_ON_BE(list_chunk_length);
        if (length - (data_offset + 8u) < list_chunk_length)
            return false;

        data_offset += (list_chunk_length + 8u);
    }

    if (length - data_offset < 8u)
        return false;

    if (memcmp(&data[data_offset], "data", 4) != 0)
        return false;

    if (waveformat.uiFormatTag != WAVE_FORMAT_PCM)
        return false;

    if (waveformat.ulAvgBytesPerSec !=
        waveformat.ulSamplesPerSec * waveformat.uiBlockAlign)
        return false;

    unsigned tmp = waveformat.uiChannels;
    if (tmp >= 0x10000)
        return false;

    tmp *= waveformat.uiBitsPerSample;

    wxUint32 const sampleSize = tmp / 8;
    if (!sampleSize)
        return false;

    wxUint32 chunkSize;
    memcpy(&chunkSize, &data[4], 4);
    chunkSize = wxUINT32_SWAP_ON_BE(chunkSize);

    if (chunkSize > length - 8)
        return false;

    wxUint32 ul;
    memcpy(&ul, &data[data_offset + 4u], 4);
    ul = wxUINT32_SWAP_ON_BE(ul);

    if (ul > length - data_offset - 8u)
        return false;

    Free();

    m_data = new wxSoundData;
    m_data->m_channels = waveformat.uiChannels;
    m_data->m_samplingRate = waveformat.ulSamplesPerSec;
    m_data->m_bitsPerSample = waveformat.uiBitsPerSample;
    m_data->m_samples = ul / sampleSize;
    m_data->m_dataBytes = ul;

    if (copyData)
    {
        m_data->m_dataWithHeader = new wxUint8[length];
        memcpy(m_data->m_dataWithHeader, data, length);
    }
    else
        m_data->m_dataWithHeader = const_cast<wxUint8*>(data);

    m_data->m_data = (&m_data->m_dataWithHeader[data_offset]);

    return true;
}

bool wxSound::DoPlay(unsigned flags) const
{
    if (!m_data)
        return false;

    // Calculate total buffer size including headers
    size_t headerSize = m_data->m_data - m_data->m_dataWithHeader;
    size_t totalSize = headerSize + m_data->m_dataBytes;

    wxWasmPlaySound(m_data->m_dataWithHeader, totalSize, flags & wxSOUND_LOOP ? 1 : 0);

    return true;
}

void wxSound::Stop()
{
    wxWasmStopSounds();
}

bool wxSound::IsPlaying()
{
    return wxWasmIsPlayingSound() != 0;
}

void wxSound::EnsureBackend()
{
    // No backend needed; we use Web Audio API directly
}

void wxSound::UnloadBackend()
{
    ms_backend = nullptr;
}

void wxSound::Free()
{
    if (m_data)
    {
        m_data->DecRef();
        m_data = nullptr;
    }
}

// ----------------------------------------------------------------------------
// wxSoundCleanupModule
// ----------------------------------------------------------------------------

class wxSoundCleanupModule: public wxModule
{
public:
    bool OnInit() override { return true; }
    void OnExit() override { wxSound::UnloadBackend(); }
    wxDECLARE_DYNAMIC_CLASS(wxSoundCleanupModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxSoundCleanupModule, wxModule);

#endif // wxUSE_SOUND
