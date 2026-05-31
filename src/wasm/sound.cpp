/////////////////////////////////////////////////////////////////////////////
// Name:        src/wasm/sound.cpp
// Purpose:     wxSound stub implementation for WASM
// Author:      Hugo Armando Castellanos Morales
// Copyright:   (c) 2022-2026 Hugo Armando Castellanos Morales
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_SOUND

#include "wx/sound.h"
#include "wx/log.h"

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

wxSound::wxSound(const wxString& WXUNUSED(fileName), bool WXUNUSED(isResource))
    : m_data(nullptr)
{
}

wxSound::wxSound(size_t WXUNUSED(size), const void* WXUNUSED(data))
    : m_data(nullptr)
{
}

wxSound::~wxSound()
{
    Free();
}

bool wxSound::Create(const wxString& WXUNUSED(fileName), bool WXUNUSED(isResource))
{
    return false;
}

bool wxSound::Create(size_t WXUNUSED(size), const void* WXUNUSED(data))
{
    return false;
}

void wxSound::Stop()
{
}

bool wxSound::IsPlaying()
{
    return false;
}

void wxSound::UnloadBackend()
{
    ms_backend = nullptr;
}

bool wxSound::DoPlay(unsigned WXUNUSED(flags)) const
{
    return false;
}

void wxSound::EnsureBackend()
{
}

void wxSound::Free()
{
    if (m_data)
    {
        m_data->DecRef();
        m_data = nullptr;
    }
}

bool wxSound::LoadWAV(const void* WXUNUSED(data), size_t WXUNUSED(length), bool WXUNUSED(copyData))
{
    return false;
}

#endif // wxUSE_SOUND
