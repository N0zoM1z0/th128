#include "SoundIoWave.hpp"

#include <string.h>

namespace th128 {

unsigned int SiwPollGamepad(unsigned int mask) {
    if (!(g_SiwFlags & 0x800)) {
        JOYINFOEX ji;
        memset(&ji, 0, sizeof(ji));
        ji.dwSize = sizeof(ji);
        ji.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(0, &ji) != 0) {
            return mask;
        }
        if (g_SiwBtn0 >= 0) {
            if (ji.dwButtons & (1u << g_SiwBtn0)) {
                mask |= 1;
            }
        }
        if (g_SiwBtn1 >= 0) {
            if (ji.dwButtons & (1u << g_SiwBtn1)) {
                mask |= 2;
            }
        }
        if (g_SiwBtn2 >= 0) {
            if (ji.dwButtons & (1u << g_SiwBtn2)) {
                mask |= 0x100;
            }
        }
        if (g_SiwBtn3 >= 0) {
            if (ji.dwButtons & (1u << g_SiwBtn3)) {
                mask |= 8;
            }
        }
        if (g_SiwBtn4 >= 0) {
            if (ji.dwButtons & (1u << g_SiwBtn4)) {
                mask |= 0x200;
            }
        }
        unsigned int dzX = (g_SiwJoyMaxX - g_SiwJoyMinX) / 4;
        unsigned int midX = (g_SiwJoyMaxX + g_SiwJoyMinX) / 2;
        if (ji.dwXpos > midX + dzX) {
            mask |= 0x80;
        }
        if (ji.dwXpos < midX - dzX) {
            mask |= 0x40;
        }
        unsigned int dzY = (g_SiwJoyMaxY - g_SiwJoyMinY) / 4;
        unsigned int midY = (g_SiwJoyMinY + g_SiwJoyMaxY) / 2;
        if (ji.dwYpos > midY + dzY) {
            mask |= 0x20;
        }
        if (ji.dwYpos < midY - dzY) {
            mask |= 0x10;
        }
        return mask;
    }

    if (FAILED(g_SiwPad->Poll())) {
        int tries = 0;
        HRESULT hr = g_SiwPad->Acquire();
        while (hr == (HRESULT)0x8007001e) {
            hr = g_SiwPad->Acquire();
            SiwDebugLog("error : DIERR_INPUTLOST %d\r\n", tries);
            ++tries;
            if (tries >= 400) {
                return mask;
            }
        }
        return mask;
    }

    SiwDiJoyState2 js;
    memset(&js, 0, sizeof(js));
    if (FAILED(g_SiwPad->GetDeviceState(sizeof(js), &js))) {
        return mask;
    }
    if (g_SiwBtn0 >= 0) {
        if (js.rgbButtons[g_SiwBtn0] & 0x80) {
            mask |= 1;
        }
    }
    if (g_SiwBtn1 >= 0) {
        if (js.rgbButtons[g_SiwBtn1] & 0x80) {
            mask |= 2;
        }
    }
    if (g_SiwBtn2 >= 0) {
        if (js.rgbButtons[g_SiwBtn2] & 0x80) {
            mask |= 0x100;
        }
    }
    if (g_SiwBtn3 >= 0) {
        if (js.rgbButtons[g_SiwBtn3] & 0x80) {
            mask |= 8;
        }
    }
    if (g_SiwBtn4 >= 0) {
        if (js.rgbButtons[g_SiwBtn4] & 0x80) {
            mask |= 0x200;
        }
    }
    if (js.lX < -g_SiwDeadzoneX) {
        mask |= 0x40;
    }
    if (js.lY < -g_SiwDeadzoneY) {
        mask |= 0x10;
    }
    if (js.lX > g_SiwDeadzoneX) {
        mask |= 0x80;
    }
    if (js.lX > g_SiwDeadzoneY) {
        mask |= 0x20;
    }
    return mask;
}

static __forceinline HRESULT SiwResetWaveFileInline(SiwWaveFile *wf) {
    if (wf->isMemory != 0) {
        wf->memCur = wf->memBase;
        if (wf->desc->dataSize > 0) {
            wf->memRemaining = wf->desc->dataSize;
        }
        return 0;
    }
    if (wf->fileHandle == INVALID_HANDLE_VALUE || wf->fileHandle == NULL) {
        return (HRESULT)0x800401f0;
    }
    SetFilePointer(wf->fileHandle, wf->desc->dataOffset + g_SiwBgmBaseOffset, NULL, FILE_BEGIN);
    wf->remaining = wf->desc->dataSize;
    return 0;
}

HRESULT SiwSound::FillBufferWithSound(SiwDsBuffer *pDSB, int bRepeatWavIfBufferLarger) {
    void *pDSLockedBuffer = NULL;
    DWORD dwDSLockedBufferSize = 0;
    DWORD dwWavDataRead = 0;

    if (pDSB == NULL) {
        return (HRESULT)0x800401f0;
    }

    DWORD dwStatus;
    HRESULT hr = pDSB->GetStatus(&dwStatus);
    if (FAILED(hr)) {
        return hr;
    }
    if (dwStatus & 2) {
        do {
            hr = pDSB->Restore();
            if (hr == (HRESULT)0x88780096) {
                Sleep(10);
            }
        } while ((hr = pDSB->Restore()) != 0);
    }

    if (FAILED(hr = pDSB->Lock(0, this->bufSize, &pDSLockedBuffer, &dwDSLockedBufferSize,
                               NULL, NULL, 0))) {
        return hr;
    }

    SiwResetWaveFileInline(this->waveFile);

    if (FAILED(hr = this->waveFile->Read((unsigned char *)pDSLockedBuffer, dwDSLockedBufferSize,
                                         &dwWavDataRead))) {
        return hr;
    }

    if (dwWavDataRead == 0) {
        memset(pDSLockedBuffer, (unsigned char)(this->waveFile->desc->bitsPerSample == 8 ? 128 : 0),
               dwDSLockedBufferSize);
    } else if (dwWavDataRead < dwDSLockedBufferSize) {
        if (bRepeatWavIfBufferLarger) {
            DWORD dwReadSoFar = dwWavDataRead;
            while (dwReadSoFar < dwDSLockedBufferSize) {
                if (FAILED(hr = SiwResetWaveFileInline(this->waveFile))) {
                    return hr;
                }
                if (FAILED(hr = this->waveFile->Read((unsigned char *)pDSLockedBuffer + dwReadSoFar,
                                                     dwDSLockedBufferSize - dwReadSoFar,
                                                     &dwWavDataRead))) {
                    return hr;
                }
                dwReadSoFar += dwWavDataRead;
            }
        } else {
            memset((unsigned char *)pDSLockedBuffer + dwWavDataRead,
                   (unsigned char)(this->waveFile->desc->bitsPerSample == 8 ? 128 : 0),
                   dwDSLockedBufferSize - dwWavDataRead);
        }
    }

    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);
    return 0;
}

HRESULT SiwSound::HandleWaveStreamNotification(int bLoopedPlay) {
    if (this->buffers == NULL || this->waveFile == NULL) {
        return (HRESULT)0x800401f0;
    }

    DWORD dwCurrentPlayPos;
    DWORD dwCurrentWritePos;
    this->buffers[0]->GetCurrentPosition(&dwCurrentPlayPos, &dwCurrentWritePos);
    if (this->nextWriteOffset >= dwCurrentWritePos - this->notifySize &&
        this->nextWriteOffset < dwCurrentWritePos) {
        return (HRESULT)0x800401f0;
    }

    if (this->buffers[0] == NULL) {
        return (HRESULT)0x800401f0;
    }

    int bRestored;
    HRESULT hr = this->RestoreBuffer(this->buffers[0], &bRestored);
    if (FAILED(hr)) {
        return hr;
    }
    if (bRestored) {
        if (FAILED(hr = this->FillBufferWithSound(this->buffers[0], 0))) {
            return hr;
        }
        return 0;
    }

    void *pDSLockedBuffer = NULL;
    void *pDSLockedBuffer2 = NULL;
    DWORD dwDSLockedBufferSize;
    DWORD dwDSLockedBufferSize2;
    if (FAILED(hr = this->buffers[0]->Lock(this->nextWriteOffset, this->notifySize,
                                           &pDSLockedBuffer, &dwDSLockedBufferSize,
                                           &pDSLockedBuffer2, &dwDSLockedBufferSize2, 0))) {
        return hr;
    }

    if (pDSLockedBuffer2 != NULL) {
        return (HRESULT)0x8000ffff;
    }

    DWORD dwBytesWrittenToBuffer = 0;
    if (!this->fillNextNotificationWithSilence) {
        if (FAILED(hr = this->waveFile->Read((unsigned char *)pDSLockedBuffer, dwDSLockedBufferSize,
                                             &dwBytesWrittenToBuffer))) {
            return hr;
        }
        if (dwBytesWrittenToBuffer < dwDSLockedBufferSize) {
            DWORD dwReadSoFar = dwBytesWrittenToBuffer;
            while (dwReadSoFar < dwDSLockedBufferSize) {
                if (FAILED(hr = this->waveFile->ResetFile(true, 0))) {
                    return hr;
                }
                if (FAILED(hr = this->waveFile->Read((unsigned char *)pDSLockedBuffer + dwReadSoFar,
                                                     dwDSLockedBufferSize - dwReadSoFar,
                                                     &dwBytesWrittenToBuffer))) {
                    return hr;
                }
                dwReadSoFar += dwBytesWrittenToBuffer;
            }
        }
    } else {
        memset(pDSLockedBuffer, (unsigned char)(this->waveFile->desc->bitsPerSample == 8 ? 128 : 0),
               dwDSLockedBufferSize);
    }

    this->buffers[0]->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);

    DWORD dwPlayPos;
    if (FAILED(hr = this->buffers[0]->GetCurrentPosition(&dwPlayPos, NULL))) {
        return hr;
    }

    DWORD dwPlayDelta;
    if (dwPlayPos < this->lastPlayPos) {
        dwPlayDelta = (this->bufSize - this->lastPlayPos) + dwPlayPos;
    } else {
        dwPlayDelta = dwPlayPos - this->lastPlayPos;
    }
    this->playProgress += dwPlayDelta;
    this->lastPlayPos = dwPlayPos;

    if (this->fillNextNotificationWithSilence) {
        if (this->playProgress >= this->waveFile->totalSize) {
            this->buffers[0]->Stop();
        }
    }

    this->nextWriteOffset = (this->nextWriteOffset + dwDSLockedBufferSize) % this->bufSize;
    return 0;
}

} // namespace th128
