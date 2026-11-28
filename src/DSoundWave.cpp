#include "DSoundWave.hpp"

#include <new>
#include <string.h>

namespace th128 {

HRESULT DswWaveFile::ReopenTrack(DswTrackInfo *info, DWORD seekPos) {
    if (fromMemory != 0) {
        return E_FAIL;
    }
    if (handle == INVALID_HANDLE_VALUE) {
        mode = 1;
        fromMemory = 0;
        OpenFile(filename, info);
        if (handle == INVALID_HANDLE_VALUE) {
            return E_FAIL;
        }
    }
    trackInfo = info;
    DswWaveDebugLog("%s %d %d\n", info, info->loopStart, info->dataSize);
    ResetFile(false, seekPos);
    size = remainingBytes;
    return 0;
}

DswSound::DswSound(IDswBuffer **apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, DswWaveFile *pWaveFile) {
    buffers = new IDswBuffer *[dwNumBuffers];
    for (DWORD i = 0; i < dwNumBuffers; ++i) {
        buffers[i] = apDSBuffer[i];
    }
    bufferSize = dwDSBufferSize;
    numBuffers = dwNumBuffers;
    waveFile = pWaveFile;

    FillBufferWithSound(buffers[0], FALSE);
    buffers[0]->SetCurrentPosition(0);
    isPlayingFlag = 0;
    isPausedFlag = 0;
}

DswSound::~DswSound() {
}

HRESULT DswSound::FillBufferWithSound(IDswBuffer *pDSB, BOOL bRepeatWavIfBufferLarger) {
    void *lockedBuffer = NULL;
    DWORD lockedSize = 0;
    DWORD wavDataRead = 0;
    if (pDSB == NULL) {
        return (HRESULT)0x800401F0;
    }
    DWORD status;
    HRESULT hr = pDSB->GetStatus(&status);
    if (hr < 0) {
        return hr;
    }
    if ((status & 2) != 0) {
        do {
            if (pDSB->Restore() == (HRESULT)0x88780096) {
                Sleep(10);
            }
        } while (pDSB->Restore() != 0);
    }
    hr = pDSB->Lock(0, bufferSize, &lockedBuffer, &lockedSize, NULL, NULL, 0);
    if (hr < 0) {
        return hr;
    }
    if (waveFile->fromMemory != 0) {
        waveFile->memCur = waveFile->memBase;
        if ((int)waveFile->trackInfo->dataSize > 0) {
            waveFile->memSize = waveFile->trackInfo->dataSize;
        }
    } else if (waveFile->handle != INVALID_HANDLE_VALUE && waveFile->handle != NULL) {
        SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
        waveFile->remainingBytes = waveFile->trackInfo->dataSize;
    }
    hr = waveFile->Read((unsigned char *)lockedBuffer, lockedSize, &wavDataRead);
    if (hr < 0) {
        return hr;
    }
    if (wavDataRead == 0) {
        memset(lockedBuffer, waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0, lockedSize);
    } else if (wavDataRead < lockedSize) {
        if (bRepeatWavIfBufferLarger) {
            DWORD readSoFar = wavDataRead;
            while (readSoFar < lockedSize) {
                if (waveFile->fromMemory != 0) {
                    waveFile->memCur = waveFile->memBase;
                    if ((int)waveFile->trackInfo->dataSize > 0) {
                        waveFile->memSize = waveFile->trackInfo->dataSize;
                    }
                } else {
                    if (waveFile->handle == INVALID_HANDLE_VALUE || waveFile->handle == NULL) {
                        return (HRESULT)0x800401F0;
                    }
                    SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
                    waveFile->remainingBytes = waveFile->trackInfo->dataSize;
                }
                hr = waveFile->Read((unsigned char *)lockedBuffer + readSoFar, lockedSize - readSoFar, &wavDataRead);
                if (hr < 0) {
                    return hr;
                }
                readSoFar += wavDataRead;
            }
        } else {
            memset((unsigned char *)lockedBuffer + wavDataRead,
                waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0,
                lockedSize - wavDataRead);
        }
    }
    pDSB->Unlock(lockedBuffer, lockedSize, NULL, 0);
    return 0;
}

HRESULT DswSound::Play() {
    if (buffers == NULL) {
        return (HRESULT)0x800401F0;
    }
    IDswBuffer *buf = GetFreeBuffer();
    if (buf == NULL) {
        return E_FAIL;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0) {
        return hr;
    }
    if (wasRestored != 0) {
        hr = FillBufferWithSound(buf, FALSE);
        if (hr < 0) {
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

HRESULT DswSound::Resume() {
    if (buffers == NULL) {
        return (HRESULT)0x800401F0;
    }
    if (isPausedFlag == 0) {
        return (HRESULT)0x800401F0;
    }
    isPausedFlag = 0;
    waveFile->ReopenTrack(waveFile->trackInfo, waveFile->savedFilePos);
    IDswBuffer *buf = buffers[0];
    DWORD flags = playFlags;
    DWORD prio = playPriority;
    isPlayingFlag = 1;
    return buf->Play(0, prio, flags);
}

DswStreamingSound::DswStreamingSound(
    IDswBuffer *pDSBuffer,
    DWORD dwDSBufferSize,
    DswWaveFile *pWaveFile,
    DWORD dwNotifySize)
    : DswSound(&pDSBuffer, dwDSBufferSize, 1, pWaveFile) {
    lastPlayPos = 0;
    playProgress = 0;
    notifySize = dwNotifySize;
    nextWriteOffset = 0;
    fillNextNotificationWithSilence = FALSE;
}

DswStreamingSound::~DswStreamingSound() {
}

HRESULT DswStreamingSound::HandleWaveStreamNotification(BOOL bLoopedPlay) {
    if (buffers == NULL) {
        return (HRESULT)0x800401F0;
    }
    if (waveFile == NULL) {
        return (HRESULT)0x800401F0;
    }
    DWORD playPos, writePos;
    buffers[0]->GetCurrentPosition(&playPos, &writePos);
    if (nextWriteOffset >= writePos - notifySize && nextWriteOffset < writePos) {
        return (HRESULT)0x800401F0;
    }
    IDswBuffer *buf = buffers[0];
    if (buf == NULL) {
        return (HRESULT)0x800401F0;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0) {
        return hr;
    }
    if (wasRestored != 0) {
        hr = FillBufferWithSound(buffers[0], FALSE);
        if (hr < 0) {
            return hr;
        }
        return 0;
    }
    void *lockedBuffer = NULL;
    void *lockedBuffer2 = NULL;
    DWORD lockedSize = 0;
    DWORD lockedSize2 = 0;
    hr = buffers[0]->Lock(nextWriteOffset, notifySize, &lockedBuffer, &lockedSize, &lockedBuffer2, &lockedSize2, 0);
    if (hr < 0) {
        return hr;
    }
    if (lockedBuffer2 != NULL) {
        return E_UNEXPECTED;
    }
    if (!fillNextNotificationWithSilence) {
        DWORD bytesWritten = 0;
        hr = waveFile->Read((unsigned char *)lockedBuffer, lockedSize, &bytesWritten);
        if (hr < 0) {
            return hr;
        }
        if (bytesWritten < lockedSize) {
            DWORD totalWritten = bytesWritten;
            while (totalWritten < lockedSize) {
                hr = waveFile->ResetFile(true, 0);
                if (hr < 0) {
                    return hr;
                }
                hr = waveFile->Read((unsigned char *)lockedBuffer + totalWritten, lockedSize - totalWritten, &bytesWritten);
                if (hr < 0) {
                    return hr;
                }
                totalWritten += bytesWritten;
            }
        }
    } else {
        memset(lockedBuffer, waveFile->trackInfo->format.wBitsPerSample == 8 ? 128 : 0, lockedSize);
    }
    buffers[0]->Unlock(lockedBuffer, lockedSize, NULL, 0);
    DWORD currentPlayPos;
    hr = buffers[0]->GetCurrentPosition(&currentPlayPos, NULL);
    if (hr < 0) {
        return hr;
    }
    DWORD playDelta;
    if (currentPlayPos < lastPlayPos) {
        playDelta = bufferSize - lastPlayPos + currentPlayPos;
    } else {
        playDelta = currentPlayPos - lastPlayPos;
    }
    playProgress += playDelta;
    lastPlayPos = currentPlayPos;
    if (fillNextNotificationWithSilence && playProgress >= waveFile->size) {
        buffers[0]->Stop();
    }
    nextWriteOffset += lockedSize;
    nextWriteOffset %= bufferSize;
    return 0;
}

HRESULT DswStreamingSound::ResetStream() {
    if (buffers[0] == NULL || waveFile == NULL) {
        return (HRESULT)0x800401F0;
    }
    lastPlayPos = 0;
    playProgress = 0;
    nextWriteOffset = 0;
    fillNextNotificationWithSilence = FALSE;
    IDswBuffer *buf = buffers[0];
    if (buf == NULL) {
        return (HRESULT)0x800401F0;
    }
    int wasRestored;
    HRESULT hr = RestoreBuffer(buf, &wasRestored);
    if (hr < 0) {
        return hr;
    }
    if (wasRestored != 0) {
        hr = FillBufferWithSound(buffers[0], FALSE);
        if (hr < 0) {
            return hr;
        }
    }
    if (waveFile->fromMemory != 0) {
        waveFile->memCur = waveFile->memBase;
        if ((int)waveFile->trackInfo->dataSize > 0) {
            waveFile->memSize = waveFile->trackInfo->dataSize;
        }
    } else if (waveFile->handle != INVALID_HANDLE_VALUE && waveFile->handle != NULL) {
        SetFilePointer(waveFile->handle, waveFile->trackInfo->dataOffset + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
        waveFile->remainingBytes = waveFile->trackInfo->dataSize;
    }
    return buffers[0]->SetCurrentPosition(0);
}

HRESULT DswSoundManager::CreateStreamingFromFile(
    DswStreamingSound **ppStreamingSound,
    const char *strWaveFileName,
    DWORD dwCreationFlags,
    GUID guid3DAlgorithm,
    DWORD dwNotifyCount,
    DWORD dwNotifySize,
    HANDLE hNotifyEvent,
    DswTrackInfo *info) {
    if (ds == NULL) {
        return (HRESULT)0x800401F0;
    }
    IDswBuffer *pDSBuffer = NULL;
    IDswNotify *pDSNotify = NULL;
    DswWaveFile *pWaveFile = new DswWaveFile();
    pWaveFile->fromMemory = 0;
    pWaveFile->mode = 1;
    if (pWaveFile->OpenFile(strWaveFileName, info) != 0) {
        delete pWaveFile;
        return E_FAIL;
    }

    DWORD dwDSBufferSize = dwNotifySize * dwNotifyCount;
    DswBufferDesc dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = dwCreationFlags;
    dsbd.dwBufferBytes = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat = &pWaveFile->trackInfo->format;

    if (ds->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL) < 0) {
        return E_FAIL;
    }
    if (pDSBuffer->QueryInterface(g_DswIID_IDirectSoundNotify, (void **)&pDSNotify) < 0) {
        return E_FAIL;
    }
    DswPositionNotify *aPosNotify = new DswPositionNotify[dwNotifyCount];
    if (aPosNotify == NULL) {
        return E_OUTOFMEMORY;
    }
    for (DWORD i = 0; i < dwNotifyCount; ++i) {
        aPosNotify[i].dwOffset = dwNotifySize * (i + 1) - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;
    }
    if (pDSNotify->SetNotificationPositions(dwNotifyCount, aPosNotify) < 0) {
        if (pDSNotify != NULL) {
            pDSNotify->Release();
            pDSNotify = NULL;
        }
        delete aPosNotify;
        return E_FAIL;
    }
    if (pDSNotify != NULL) {
        pDSNotify->Release();
        pDSNotify = NULL;
    }
    delete aPosNotify;

    *ppStreamingSound = new DswStreamingSound(pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize);
    (*ppStreamingSound)->dsbd = dsbd;
    (*ppStreamingSound)->soundMgr = this;
    (*ppStreamingSound)->notifyEvent = hNotifyEvent;
    (*ppStreamingSound)->unused78 = 0;
    return 0;
}

HRESULT DswSoundManager::CreateStreamingFromMemory(
    DswStreamingSound **ppStreamingSound,
    unsigned char *data,
    DWORD dataSize,
    DswTrackInfo *info,
    DWORD dwCreationFlags,
    GUID guid3DAlgorithm,
    DWORD dwNotifyCount,
    DWORD dwNotifySize,
    HANDLE hNotifyEvent) {
    if (ds == NULL) {
        return (HRESULT)0x800401F0;
    }
    IDswBuffer *pDSBuffer = NULL;
    IDswNotify *pDSNotify = NULL;
    DswWaveFile *pWaveFile = new DswWaveFile();
    pWaveFile->trackInfo = info;
    pWaveFile->memSize = dataSize;
    pWaveFile->memBase = data;
    pWaveFile->memCur = data;
    pWaveFile->fromMemory = 1;

    DWORD dwDSBufferSize = dwNotifySize * dwNotifyCount;
    DswBufferDesc dsbd;
    memset(&dsbd, 0, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = dwCreationFlags;
    dsbd.dwBufferBytes = dwDSBufferSize;
    dsbd.guid3DAlgorithm = guid3DAlgorithm;
    dsbd.lpwfxFormat = &pWaveFile->trackInfo->format;

    if (ds->CreateSoundBuffer(&dsbd, &pDSBuffer, NULL) < 0) {
        return E_FAIL;
    }
    if (pDSBuffer->QueryInterface(g_DswIID_IDirectSoundNotify, (void **)&pDSNotify) < 0) {
        return E_FAIL;
    }
    DswPositionNotify *aPosNotify = new DswPositionNotify[dwNotifyCount];
    if (aPosNotify == NULL) {
        return E_OUTOFMEMORY;
    }
    for (DWORD i = 0; i < dwNotifyCount; ++i) {
        aPosNotify[i].dwOffset = dwNotifySize * (i + 1) - 1;
        aPosNotify[i].hEventNotify = hNotifyEvent;
    }
    if (pDSNotify->SetNotificationPositions(dwNotifyCount, aPosNotify) < 0) {
        if (pDSNotify != NULL) {
            pDSNotify->Release();
            pDSNotify = NULL;
        }
        delete aPosNotify;
        return E_FAIL;
    }
    if (pDSNotify != NULL) {
        pDSNotify->Release();
        pDSNotify = NULL;
    }
    delete aPosNotify;

    *ppStreamingSound = new DswStreamingSound(pDSBuffer, dwDSBufferSize, pWaveFile, dwNotifySize);
    (*ppStreamingSound)->dsbd = dsbd;
    (*ppStreamingSound)->soundMgr = this;
    (*ppStreamingSound)->notifyEvent = hNotifyEvent;
    (*ppStreamingSound)->unused78 = 0;
    return 0;
}

} // namespace th128
