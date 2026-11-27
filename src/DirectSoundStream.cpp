#include "DirectSoundStream.hpp"

#include <stdlib.h>
#include <string.h>

CWaveFileStream::CWaveFileStream()
{
    trackInfo = NULL;
    hmmio = NULL;
    size = 0;
    fromMemory = 0;
}

CWaveFileStream::~CWaveFileStream()
{
    if (mode == 1)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
}

HRESULT CWaveFileStream::Close()
{
    if (mode == 1)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
    return 0;
}

HRESULT CWaveFileStream::OpenFile(const char *path, BgmTrackInfoStream *info)
{
    if (path == NULL)
    {
        return E_INVALIDARG;
    }
    WaveDebugLog("Streaming File Open %s\r\n", path);
    handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE)
    {
        LPSTR msgBuf;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msgBuf, 0, NULL);
        WaveDebugLog(msgBuf);
        LocalFree(msgBuf);
        return E_FAIL;
    }
    trackInfo = info;
    filename = path;
    if (fromMemory != 0)
    {
        memCur = memBase;
        if ((int)info->dataSize > 0)
        {
            memSize = info->dataSize;
        }
    }
    else if (handle != NULL)
    {
        SetFilePointer(handle, info->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = trackInfo->dataSize;
    }
    size = remainingBytes;
    return 0;
}

HRESULT CWaveFileStream::ReopenTrack(BgmTrackInfoStream *info, DWORD seekPos)
{
    if (fromMemory != 0)
    {
        return E_FAIL;
    }
    if (handle == INVALID_HANDLE_VALUE)
    {
        mode = 1;
        fromMemory = 0;
        OpenFile(filename, info);
        if (handle == INVALID_HANDLE_VALUE)
        {
            return E_FAIL;
        }
    }
    trackInfo = info;
    WaveDebugLog("%s %d %d\n", info, info->loopStart, info->dataSize);
    ResetFile(false, seekPos);
    size = remainingBytes;
    return 0;
}

HRESULT CWaveFileStream::ResetFile(bool loop, DWORD seekPos)
{
    if (fromMemory != 0)
    {
        memCur = memBase;
        BgmTrackInfoStream *info = trackInfo;
        if ((int)info->dataSize > 0)
        {
            memSize = info->dataSize;
        }
        if (loop)
        {
            if ((int)info->loopStart > 0)
            {
                memCur = memBase + info->loopStart;
                return 0;
            }
        }
    }
    else
    {
        if (handle == INVALID_HANDLE_VALUE || handle == NULL)
        {
            return (HRESULT)0x800401F0;
        }
        if (loop)
        {
            BgmTrackInfoStream *info = trackInfo;
            if ((int)info->loopStart > 0)
            {
                SetFilePointer(handle, info->dataOffset + info->loopStart + g_BgmBaseOffset, NULL, FILE_BEGIN);
                remainingBytes = trackInfo->dataSize - trackInfo->loopStart;
                return 0;
            }
        }
        if (seekPos == 0)
        {
            SetFilePointer(handle, trackInfo->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
            remainingBytes = trackInfo->dataSize;
            return 0;
        }
        SetFilePointer(handle, seekPos + g_BgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = trackInfo->dataSize + trackInfo->dataOffset - seekPos;
    }
    return 0;
}

HRESULT CWaveFileStream::Read(unsigned char *dst, DWORD sizeToRead, DWORD *outRead)
{
    if (fromMemory != 0)
    {
        unsigned char *cur = memCur;
        if (cur == NULL)
        {
            return (HRESULT)0x800401F0;
        }
        if (outRead != NULL)
        {
            *outRead = 0;
        }
        if (cur + sizeToRead > memBase + memSize)
        {
            sizeToRead = (DWORD)(memBase - cur) + memSize;
        }
        memcpy(dst, cur, sizeToRead);
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

CSoundStream::CSoundStream(IDirectSoundBufferStreamView **apDSBuffer, DWORD dwDSBufferSize, CWaveFileStream *pWaveFile)
{
    buffers = new IDirectSoundBufferStreamView *[1];
    buffers[0] = apDSBuffer[0];
    bufferSize = dwDSBufferSize;
    numBuffers = 1;
    waveFile = pWaveFile;

    IDirectSoundBufferStreamView *buf = buffers[0];
    void *lockedBuffer = NULL;
    DWORD lockedSize = 0;
    DWORD wavDataRead = 0;
    if (buf != NULL)
    {
        DWORD status;
        if (buf->GetStatus(&status) >= 0)
        {
            if ((status & 2) != 0)
            {
                do
                {
                    if (buf->Restore() == (HRESULT)0x88780096)
                    {
                        Sleep(10);
                    }
                } while (buf->Restore() != 0);
            }
            if (buf->Lock(0, bufferSize, &lockedBuffer, &lockedSize, NULL, NULL, 0) >= 0)
            {
                if (waveFile->fromMemory != 0)
                {
                    waveFile->memCur = waveFile->memBase;
                    if ((int)waveFile->trackInfo->dataSize > 0)
                    {
                        waveFile->memSize = waveFile->trackInfo->dataSize;
                    }
                }
                else if (waveFile->handle != INVALID_HANDLE_VALUE && waveFile->handle != NULL)
                {
                    SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
                    waveFile->remainingBytes = waveFile->trackInfo->dataSize;
                }
                if (waveFile->Read((unsigned char *)lockedBuffer, lockedSize, &wavDataRead) >= 0)
                {
                    if (wavDataRead == 0)
                    {
                        memset(lockedBuffer, waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0, lockedSize);
                    }
                    else if (wavDataRead < lockedSize)
                    {
                        memset((unsigned char *)lockedBuffer + wavDataRead,
                            waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0,
                            lockedSize - wavDataRead);
                    }
                    buf->Unlock(lockedBuffer, lockedSize, NULL, 0);
                }
            }
        }
    }
    buffers[0]->SetCurrentPosition(0);
    isPlayingFlag = 0;
    isPausedFlag = 0;
}

CSoundStream::~CSoundStream()
{
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        if (buffers[i] != NULL)
        {
            buffers[i]->Release();
            buffers[i] = NULL;
        }
    }
    if (buffers != NULL)
    {
        delete[] buffers;
        buffers = NULL;
    }
    if (waveFile != NULL)
    {
        delete waveFile;
        waveFile = NULL;
    }
}

HRESULT CSoundStream::RecreateBuffers()
{
    isPlayingFlag = 0;
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        if (buffers[i] != NULL)
        {
            buffers[i]->Release();
            buffers[i] = NULL;
        }
    }
    if (buffers != NULL)
    {
        delete[] buffers;
        buffers = NULL;
    }
    IDirectSoundNotifyView *notify = NULL;
    buffers = new IDirectSoundBufferStreamView *[numBuffers];
    for (DWORD i = 0; i < numBuffers; ++i)
    {
        if (soundManager->ds->CreateSoundBuffer(&dsbd, &buffers[i], NULL) < 0)
        {
            return E_FAIL;
        }
        if (buffers[i]->QueryInterface(g_IID_IDirectSoundNotify, (void **)&notify) < 0)
        {
            return E_FAIL;
        }
        DsPositionNotifyView *posNotifies = new DsPositionNotifyView[16];
        if (posNotifies == NULL)
        {
            return E_OUTOFMEMORY;
        }
        CStreamingSound *self = (CStreamingSound *)this;
        for (DWORD j = 0; j < 16; ++j)
        {
            posNotifies[j].dwOffset = self->notifySize * (j + 1) - 1;
            posNotifies[j].hEventNotify = self->notifyEvent;
        }
        if (notify->SetNotificationPositions(16, posNotifies) < 0)
        {
            if (notify != NULL)
            {
                notify->Release();
                notify = NULL;
            }
            delete[] posNotifies;
            return E_FAIL;
        }
        if (notify != NULL)
        {
            notify->Release();
            notify = NULL;
        }
        delete[] posNotifies;
    }
    return 0;
}

HRESULT CSoundStream::FillBufferWithSound(IDirectSoundBufferStreamView *pDSB, BOOL bRepeatWavIfBufferLarger)
{
    void *lockedBuffer = NULL;
    DWORD lockedSize = 0;
    DWORD wavDataRead = 0;
    if (pDSB == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    DWORD status;
    HRESULT hr = pDSB->GetStatus(&status);
    if (hr < 0)
    {
        return hr;
    }
    if ((status & 2) != 0)
    {
        do
        {
            if (pDSB->Restore() == (HRESULT)0x88780096)
            {
                Sleep(10);
            }
        } while (pDSB->Restore() != 0);
    }
    hr = pDSB->Lock(0, bufferSize, &lockedBuffer, &lockedSize, NULL, NULL, 0);
    if (hr < 0)
    {
        return hr;
    }
    if (waveFile->fromMemory != 0)
    {
        waveFile->memCur = waveFile->memBase;
        if ((int)waveFile->trackInfo->dataSize > 0)
        {
            waveFile->memSize = waveFile->trackInfo->dataSize;
        }
    }
    else if (waveFile->handle != INVALID_HANDLE_VALUE && waveFile->handle != NULL)
    {
        SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
        waveFile->remainingBytes = waveFile->trackInfo->dataSize;
    }
    hr = waveFile->Read((unsigned char *)lockedBuffer, lockedSize, &wavDataRead);
    if (hr < 0)
    {
        return hr;
    }
    if (wavDataRead == 0)
    {
        memset(lockedBuffer, waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0, lockedSize);
    }
    else if (wavDataRead < lockedSize)
    {
        if (bRepeatWavIfBufferLarger)
        {
            DWORD readSoFar = wavDataRead;
            while (readSoFar < lockedSize)
            {
                if (waveFile->fromMemory != 0)
                {
                    waveFile->memCur = waveFile->memBase;
                    if ((int)waveFile->trackInfo->dataSize > 0)
                    {
                        waveFile->memSize = waveFile->trackInfo->dataSize;
                    }
                }
                else
                {
                    if (waveFile->handle == INVALID_HANDLE_VALUE || waveFile->handle == NULL)
                    {
                        return (HRESULT)0x800401F0;
                    }
                    SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
                    waveFile->remainingBytes = waveFile->trackInfo->dataSize;
                }
                hr = waveFile->Read((unsigned char *)lockedBuffer + readSoFar, lockedSize - readSoFar, &wavDataRead);
                if (hr < 0)
                {
                    return hr;
                }
                readSoFar += wavDataRead;
            }
        }
        else
        {
            memset((unsigned char *)lockedBuffer + wavDataRead,
                waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0,
                lockedSize - wavDataRead);
        }
    }
    pDSB->Unlock(lockedBuffer, lockedSize, NULL, 0);
    return 0;
}

HRESULT CSoundStream::RestoreBuffer(IDirectSoundBufferStreamView *buf, int *wasRestored)
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

IDirectSoundBufferStreamView *CSoundStream::GetFreeBuffer()
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

HRESULT CSoundStream::SetVolume(int volume)
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

HRESULT CSoundStream::Reset()
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

HRESULT CSoundStream::Play()
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    IDirectSoundBufferStreamView *buf = GetFreeBuffer();
    if (buf == NULL)
    {
        return E_FAIL;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0)
    {
        return hr;
    }
    if (wasRestored != 0)
    {
        hr = FillBufferWithSound(buf, FALSE);
        if (hr < 0)
        {
            return hr;
        }
        Reset();
    }
    fadeMode = 0;
    fadeRemaining = 0;
    fadeTotal = 0;
    SetVolume(0);
    isPlayingFlag = 1;
    playPriority = 0;
    playFlags = 1;
    field2C = 0;
    return buf->Play(0, 0, 1);
}

HRESULT CSoundStream::Stop(int closeFile)
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
        waveFile->Close();
    }
    return hr;
}

HRESULT CSoundStream::Resume()
{
    if (buffers == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    if (isPausedFlag == 0)
    {
        return (HRESULT)0x800401F0;
    }
    isPausedFlag = 0;
    waveFile->ReopenTrack(waveFile->trackInfo, waveFile->savedFilePos);
    IDirectSoundBufferStreamView *buf = buffers[0];
    DWORD flags = playFlags;
    DWORD prio = playPriority;
    isPlayingFlag = 1;
    return buf->Play(0, prio, flags);
}

HRESULT CSoundStream::ReopenTrack(BgmTrackInfoStream *info, DWORD seekPos)
{
    return waveFile->ReopenTrack(info, seekPos);
}

CStreamingSound::CStreamingSound(
    IDirectSoundBufferStreamView *pDSBuffer,
    DWORD dwDSBufferSize,
    CWaveFileStream *pWaveFile,
    DWORD dwNotifySize)
    : CSoundStream(&pDSBuffer, dwDSBufferSize, pWaveFile)
{
    lastPlayPos = 0;
    playProgress = 0;
    nextWriteOffset = 0;
    fillNextNotificationWithSilence = FALSE;
    notifySize = dwNotifySize;
}

CStreamingSound::~CStreamingSound()
{
}

HRESULT CStreamingSound::HandleWaveStreamNotification(BOOL bLoopedPlay)
{
    if (buffers == NULL || waveFile == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    DWORD playPos, writePos;
    buffers[0]->GetCurrentPosition(&playPos, &writePos);
    if (nextWriteOffset >= writePos - notifySize && nextWriteOffset < writePos)
    {
        return (HRESULT)0x800401F0;
    }
    IDirectSoundBufferStreamView *buf = buffers[0];
    if (buf == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0)
    {
        return hr;
    }
    if (wasRestored != 0)
    {
        hr = FillBufferWithSound(buffers[0], FALSE);
        if (hr < 0)
        {
            return hr;
        }
        return 0;
    }
    void *lockedBuffer = NULL;
    void *lockedBuffer2 = NULL;
    DWORD lockedSize = 0;
    DWORD lockedSize2 = 0;
    hr = buffers[0]->Lock(nextWriteOffset, notifySize, &lockedBuffer, &lockedSize, &lockedBuffer2, &lockedSize2, 0);
    if (hr < 0)
    {
        return hr;
    }
    if (lockedBuffer2 != NULL)
    {
        return E_UNEXPECTED;
    }
    if (!fillNextNotificationWithSilence)
    {
        DWORD bytesWritten = 0;
        hr = waveFile->Read((unsigned char *)lockedBuffer, lockedSize, &bytesWritten);
        if (hr < 0)
        {
            return hr;
        }
        if (bytesWritten < lockedSize)
        {
            DWORD totalWritten = bytesWritten;
            while (totalWritten < lockedSize)
            {
                hr = waveFile->ResetFile(true, 0);
                if (hr < 0)
                {
                    return hr;
                }
                hr = waveFile->Read((unsigned char *)lockedBuffer + totalWritten, lockedSize - totalWritten, &bytesWritten);
                if (hr < 0)
                {
                    return hr;
                }
                totalWritten += bytesWritten;
            }
        }
    }
    else
    {
        memset(lockedBuffer, waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0, lockedSize);
    }
    buffers[0]->Unlock(lockedBuffer, lockedSize, NULL, 0);
    DWORD currentPlayPos;
    hr = buffers[0]->GetCurrentPosition(&currentPlayPos, NULL);
    if (hr < 0)
    {
        return hr;
    }
    DWORD playDelta;
    if (currentPlayPos < lastPlayPos)
    {
        playDelta = bufferSize - lastPlayPos + currentPlayPos;
    }
    else
    {
        playDelta = currentPlayPos - lastPlayPos;
    }
    playProgress += playDelta;
    lastPlayPos = currentPlayPos;
    if (fillNextNotificationWithSilence && playProgress >= waveFile->size)
    {
        buffers[0]->Stop();
    }
    nextWriteOffset += lockedSize;
    nextWriteOffset %= bufferSize;
    return 0;
}

HRESULT CStreamingSound::ResetStream()
{
    if (buffers[0] == NULL || waveFile == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    lastPlayPos = 0;
    playProgress = 0;
    nextWriteOffset = 0;
    fillNextNotificationWithSilence = FALSE;
    IDirectSoundBufferStreamView *buf = buffers[0];
    if (buf == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0)
    {
        return hr;
    }
    if (wasRestored != 0)
    {
        hr = FillBufferWithSound(buffers[0], FALSE);
        if (hr < 0)
        {
            return hr;
        }
    }
    if (waveFile->fromMemory != 0)
    {
        waveFile->memCur = waveFile->memBase;
        if ((int)waveFile->trackInfo->dataSize > 0)
        {
            waveFile->memSize = waveFile->trackInfo->dataSize;
        }
    }
    else if (waveFile->handle != INVALID_HANDLE_VALUE && waveFile->handle != NULL)
    {
        SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_BgmBaseOffset, NULL, FILE_BEGIN);
        waveFile->remainingBytes = waveFile->trackInfo->dataSize;
    }
    return buffers[0]->SetCurrentPosition(0);
}

HRESULT CSoundManagerStream::CreateStreamingFromMemory(
    CStreamingSound **ppStreamingSound,
    unsigned char *data,
    DWORD dataSize,
    BgmTrackInfoStream *info,
    GUID guid3DAlgorithm,
    DWORD dwNotifySize,
    HANDLE hNotifyEvent)
{
    if (ds == NULL)
    {
        return (HRESULT)0x800401F0;
    }
    IDirectSoundBufferStreamView *pDSBuffer = NULL;
    IDirectSoundNotifyView *pDSNotify = NULL;
    CWaveFileStream *pWaveFile = new CWaveFileStream();
    pWaveFile->memSize = dataSize;
    pWaveFile->trackInfo = info;
    pWaveFile->memBase = data;
    pWaveFile->memCur = data;
    pWaveFile->fromMemory = 1;

    DWORD dwDSBufferSize = dwNotifySize * 16;
    DsBufferDescStreamView dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = 0x18188;
    dsbd.dwBufferBytes = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat = &pWaveFile->trackInfo->format;

    if (ds->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL) < 0)
    {
        return E_FAIL;
    }
    if (pDSBuffer->QueryInterface(g_IID_IDirectSoundNotify, (void **)&pDSNotify) < 0)
    {
        return E_FAIL;
    }
    DsPositionNotifyView *aPosNotify = new DsPositionNotifyView[16];
    if (aPosNotify == NULL)
    {
        return E_OUTOFMEMORY;
    }
    for (DWORD i = 0; i < 16; ++i)
    {
        aPosNotify[i].dwOffset = dwNotifySize * (i + 1) - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;
    }
    if (pDSNotify->SetNotificationPositions(16, aPosNotify) < 0)
    {
        if (pDSNotify != NULL)
        {
            pDSNotify->Release();
            pDSNotify = NULL;
        }
        delete aPosNotify;
        return E_FAIL;
    }
    if (pDSNotify != NULL)
    {
        pDSNotify->Release();
        pDSNotify = NULL;
    }
    delete aPosNotify;

    *ppStreamingSound = new CStreamingSound(pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize);
    (*ppStreamingSound)->dsbd = dsbd;
    (*ppStreamingSound)->soundManager = this;
    (*ppStreamingSound)->notifyEvent = hNotifyEvent;
    (*ppStreamingSound)->unused78 = 0;
    return 0;
}
