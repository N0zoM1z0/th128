#include "CSoundManager.hpp"

#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

struct IDirectSoundBufferFullView
{
    virtual HRESULT __stdcall QueryInterface(const GUID &iid, void **out) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                       // +0x04
    virtual ULONG __stdcall Release() = 0;                                      // +0x08
    virtual HRESULT __stdcall Slot03() = 0;                                     // +0x0C
    virtual HRESULT __stdcall Slot04() = 0;                                     // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                                     // +0x14
    virtual HRESULT __stdcall Slot06() = 0;                                     // +0x18
    virtual HRESULT __stdcall Slot07() = 0;                                     // +0x1C
    virtual HRESULT __stdcall Slot08() = 0;                                     // +0x20
    virtual HRESULT __stdcall GetStatus(LPDWORD status) = 0;                    // +0x24
    virtual HRESULT __stdcall Slot10() = 0;                                     // +0x28
    virtual HRESULT __stdcall Slot11() = 0;                                     // +0x2C
    virtual HRESULT __stdcall Play(DWORD r1, DWORD prio, DWORD flags) = 0;      // +0x30
    virtual HRESULT __stdcall SetCurrentPosition(DWORD pos) = 0;                // +0x34
    virtual HRESULT __stdcall SetFormat(const WAVEFORMATEX *fmt) = 0;           // +0x38
    virtual HRESULT __stdcall SetVolume(LONG vol) = 0;                          // +0x3C
    virtual HRESULT __stdcall SetPan(LONG pan) = 0;                             // +0x40
    virtual HRESULT __stdcall Slot17() = 0;                                     // +0x44
    virtual HRESULT __stdcall Stop() = 0;                                       // +0x48
    virtual HRESULT __stdcall Slot19() = 0;                                     // +0x4C
    virtual HRESULT __stdcall Restore() = 0;                                    // +0x50
};

struct IDirectSoundBufferPrimaryView
{
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                // +0x04
    virtual ULONG __stdcall Release() = 0;                               // +0x08
    virtual HRESULT __stdcall Slot03() = 0;                              // +0x0C
    virtual HRESULT __stdcall Slot04() = 0;                              // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                              // +0x14
    virtual HRESULT __stdcall Slot06() = 0;                              // +0x18
    virtual HRESULT __stdcall Slot07() = 0;                              // +0x1C
    virtual HRESULT __stdcall Slot08() = 0;                              // +0x20
    virtual HRESULT __stdcall Slot09() = 0;                              // +0x24
    virtual HRESULT __stdcall Slot10() = 0;                              // +0x28
    virtual HRESULT __stdcall Slot11() = 0;                              // +0x2C
    virtual HRESULT __stdcall Slot12() = 0;                              // +0x30
    virtual HRESULT __stdcall Slot13() = 0;                              // +0x34
    virtual HRESULT __stdcall SetFormat(const WAVEFORMATEX *fmt) = 0;    // +0x38
};

struct DSBufferDescView
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX *lpwfxFormat;
    GUID guid3DAlgorithm;
};

struct IDirectSound8View
{
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                // +0x04
    virtual ULONG __stdcall Release() = 0;                               // +0x08
    virtual HRESULT __stdcall CreateSoundBuffer(const DSBufferDescView *desc, IDirectSoundBufferPrimaryView **out, void *outer) = 0; // +0x0C
    virtual HRESULT __stdcall Slot04() = 0;                              // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                              // +0x14
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD level) = 0; // +0x18
};

extern "C" __declspec(dllimport) HRESULT __stdcall DirectSoundCreate8(const void *guid, IDirectSound8View **out, void *outer);

CSoundManager::CSoundManager()
{
    ds = NULL;
}

CSoundManager::~CSoundManager()
{
    if (ds != NULL)
    {
        ds->Release();
        ds = NULL;
    }
}

HRESULT CSoundManager::Initialize(HWND hwnd)
{
    if (ds != NULL)
    {
        ds->Release();
        ds = NULL;
    }
    HRESULT hr = DirectSoundCreate8(NULL, &ds, NULL);
    if (hr < 0)
    {
        return hr;
    }
    hr = ds->SetCooperativeLevel(hwnd, 2);
    if (hr < 0)
    {
        return hr;
    }
    SetPrimaryBufferFormat();
    return 0;
}

HRESULT CSoundManager::SetPrimaryBufferFormat()
{
    IDirectSoundBufferPrimaryView *primary = NULL;
    if (ds == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    DSBufferDescView desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = 1;
    desc.dwBufferBytes = 0;
    desc.lpwfxFormat = NULL;
    HRESULT hr = ds->CreateSoundBuffer(&desc, &primary, NULL);
    if (hr < 0)
    {
        return hr;
    }
    WAVEFORMATEX wfx;
    memset(&wfx, 0, sizeof(wfx));
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = (wfx.wBitsPerSample / 8) * wfx.nChannels;
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    hr = primary->SetFormat(&wfx);
    if (hr < 0)
    {
        return hr;
    }
    if (primary != NULL)
    {
        primary->Release();
    }
    return 0;
}

CWaveFileView::CWaveFileView()
{
    trackInfo = NULL;
    hmmio = NULL;
    size = 0;
    fromMemory = 0;
}

CWaveFileView::~CWaveFileView()
{
    if (mode == 1)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
}

void CWaveFileView::CloseAndDelete()
{
    if (mode == 1)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
    delete this;
}

HRESULT CWaveFileView::OpenFromMemory(unsigned char *data, DWORD dataSize, void *info)
{
    trackInfo = info;
    memSize = dataSize;
    memBase = data;
    memCur = data;
    fromMemory = 1;
    return E_NOTIMPL;
}

DWORD CWaveFileView::GetSize()
{
    return size;
}

HRESULT CWaveFileView::ResetFile(bool loop, DWORD seekPos)
{
    if (fromMemory != 0)
    {
        memCur = memBase;
        BgmTrackInfoView *info = (BgmTrackInfoView *)trackInfo;
        if ((int)info->dataSize > 0)
        {
            memSize = info->dataSize;
        }
        if (loop)
        {
            if ((int)info->loopStart > 0)
            {
                memCur = memBase + info->loopStart;
            }
        }
        return 0;
    }
    if (handle == INVALID_HANDLE_VALUE || handle == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    BgmTrackInfoView *info = (BgmTrackInfoView *)trackInfo;
    if (loop && (int)info->loopStart > 0)
    {
        SetFilePointer(handle, info->dataOffset + info->loopStart + g_BgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = ((BgmTrackInfoView *)trackInfo)->dataSize - ((BgmTrackInfoView *)trackInfo)->loopStart;
        return 0;
    }
    if (seekPos == 0)
    {
        SetFilePointer(handle, info->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = ((BgmTrackInfoView *)trackInfo)->dataSize;
        return 0;
    }
    SetFilePointer(handle, seekPos + g_BgmBaseOffset, NULL, FILE_BEGIN);
    remainingBytes = ((BgmTrackInfoView *)trackInfo)->dataSize + ((BgmTrackInfoView *)trackInfo)->dataOffset - seekPos;
    return 0;
}

HRESULT CWaveFileView::Read(unsigned char *dst, DWORD sizeToRead, DWORD *outRead)
{
    if (fromMemory != 0)
    {
        if (memCur == NULL)
        {
            return (HRESULT)0x800401F0;
        }
        if (outRead != NULL)
        {
            *outRead = 0;
        }
        if (memCur + sizeToRead > memBase + memSize)
        {
            sizeToRead = (DWORD)(memBase - memCur) + memSize;
        }
        memcpy(dst, memCur, sizeToRead);
        memCur += sizeToRead;
        if (outRead != NULL)
        {
            *outRead = sizeToRead;
        }
        return 0;
    }
    if (handle == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    if (dst == NULL || outRead == NULL)
    {
        return E_INVALIDARG;
    }
    DWORD toRead = sizeToRead;
    if (toRead > remainingBytes)
    {
        toRead = remainingBytes;
    }
    remainingBytes -= toRead;
    DWORD bytesRead;
    ReadFile(handle, dst, toRead, &bytesRead, NULL);
    *outRead = bytesRead;
    return 0;
}

HRESULT CWaveFileView::Close()
{
    if (mode == 1)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
    return 0;
}

DWORD CWaveFileView::GetSavedPos()
{
    return savedFilePos;
}

void CWaveFileView::SaveFilePos()
{
    savedFilePos = SetFilePointer(handle, 0, NULL, FILE_CURRENT);
}

HRESULT CSoundView::RestoreBuffer(IDirectSoundBufferFullView *buf, int *wasRestored)
{
    if (wasRestored != NULL)
    {
        *wasRestored = 0;
    }
    DWORD status;
    HRESULT hr = buf->GetStatus(&status);
    if (hr < 0)
    {
        return hr;
    }
    if ((status & 2) != 0)
    {
        do
        {
            if (buf->Restore() == (HRESULT)0x88780096)
            {
                Sleep(10);
            }
        } while (buf->Restore() != 0);
        if (wasRestored != NULL)
        {
            *wasRestored = 1;
        }
        return 0;
    }
    return 1;
}

IDirectSoundBufferFullView *CSoundView::GetFreeBuffer()
{
    if (buffers == NULL)
    {
        return NULL;
    }
    DWORD i;
    for (i = 0; i < numBuffers; ++i)
    {
        if (buffers[i] != NULL)
        {
            DWORD status = 0;
            buffers[i]->GetStatus(&status);
            if ((status & 1) == 0)
            {
                break;
            }
        }
    }
    if (i != numBuffers)
    {
        return buffers[i];
    }
    return buffers[rand() % numBuffers];
}

IDirectSoundBufferFullView *CSoundView::GetBuffer0()
{
    if (buffers == NULL)
    {
        return NULL;
    }
    if (numBuffers <= 0)
    {
        return NULL;
    }
    return buffers[0];
}

HRESULT CSoundView::Get3DBufferInterface(DWORD index, void **out3d)
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    if (index >= numBuffers)
    {
        return E_INVALIDARG;
    }
    *out3d = NULL;
    return buffers[index]->QueryInterface(g_IID_IDirectSound3DBuffer, out3d);
}

HRESULT CSoundView::SetVolume(int volume)
{
    if (g_BgmVolume != 0)
    {
        float v = 1.0f - (float)((double)g_BgmVolume / 100.0);
        float v2 = v * v;
        float scale = 1.0f - v2;
        int db = (int)(scale * (volume + 5000)) - 5000;
        return buffers[0]->SetVolume(db);
    }
    return buffers[0]->SetVolume(-10000);
}

HRESULT CSoundView::Stop(int closeFile)
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    HRESULT hr = 0;
    isPlayingFlag = 0;
    isPausedFlag = 0;
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        hr |= buffers[i]->Stop();
        hr |= buffers[i]->SetCurrentPosition(0);
    }
    fadeMode = 0;
    if (closeFile != 0)
    {
        if (waveFile->mode == 1)
        {
            CloseHandle(waveFile->handle);
            waveFile->handle = INVALID_HANDLE_VALUE;
        }
    }
    return hr;
}

HRESULT CSoundView::Pause()
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    if (isPlayingFlag == 0)
    {
        isPausedFlag = 0;
        return (HRESULT)0x800401F0;
    }
    isPlayingFlag = 0;
    isPausedFlag = 1;
    HRESULT hr = buffers[0]->Stop();
    waveFile->SaveFilePos();
    waveFile->Close();
    return hr;
}

HRESULT CSoundView::Reset()
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    HRESULT hr = 0;
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        hr |= buffers[i]->SetCurrentPosition(0);
    }
    return hr;
}

int CSoundView::IsSoundPlaying()
{
    int playing = 0;
    if (buffers == NULL)
    {
        return 0;
    }
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        if (buffers[i] != NULL)
        {
            DWORD status = 0;
            buffers[i]->GetStatus(&status);
            playing |= (status & 1) != 0;
        }
    }
    return playing;
}

int CSoundView::UpdateFadeOut()
{
    if (fadeMode == 1)
    {
        --fadeRemaining;
        if (fadeRemaining <= 0)
        {
            fadeMode = 0;
            buffers[0]->Stop();
            return 1;
        }
        SetVolume((fadeRemaining * 5000) / fadeTotal - 5000);
    }
    return 0;
}

int CSoundView::UpdateFadeIn()
{
    if (fadeMode == 2)
    {
        --fadeRemaining;
        if (fadeRemaining <= 0)
        {
            fadeMode = 0;
            return 1;
        }
        SetVolume((fadeRemaining * -5000) / fadeTotal);
    }
    return 0;
}

int CSoundView::UpdateFadeLow()
{
    if (fadeMode == 3)
    {
        --fadeRemaining;
        if (fadeRemaining <= 0)
        {
            fadeMode = 0;
            return 1;
        }
        SetVolume((fadeRemaining * -1000) / fadeTotal);
    }
    return 0;
}

int CSoundView::UpdateFadeHigh()
{
    if (fadeMode == 4)
    {
        --fadeRemaining;
        if (fadeRemaining <= 0)
        {
            fadeMode = 0;
            return 1;
        }
        SetVolume((fadeRemaining * 1000) / fadeTotal - 1000);
    }
    return 0;
}

void __cdecl WaveDebugLog(const char *, ...)
{
}

void __cdecl EclDebugLog(const char *, ...)
{
}
