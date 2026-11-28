#include "CleanCoreWave.hpp"

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

unsigned int __fastcall CcwMergeJoyInput(unsigned int flags) {
    if ((g_CcwSupFlags & 0x800u) == 0) {
        JOYINFOEX ji;
        memset(&ji, 0, sizeof(ji));
        ji.dwSize = sizeof(ji);
        ji.dwFlags = JOY_RETURNALL;
        if (joyGetPosEx(0, &ji) != 0) {
            return flags;
        }
        if (g_CcwJoyKeyCfg.shot0 >= 0 && (ji.dwButtons & (1u << g_CcwJoyKeyCfg.shot0)) != 0) {
            flags |= 1u;
        }
        if (g_CcwJoyKeyCfg.bomb2 >= 0 && (ji.dwButtons & (1u << g_CcwJoyKeyCfg.bomb2)) != 0) {
            flags |= 2u;
        }
        if (g_CcwJoyKeyCfg.pause8 >= 0 && (ji.dwButtons & (1u << g_CcwJoyKeyCfg.pause8)) != 0) {
            flags |= 0x100u;
        }
        if (g_CcwJoyKeyCfg.skip6 >= 0 && (ji.dwButtons & (1u << g_CcwJoyKeyCfg.skip6)) != 0) {
            flags |= 8u;
        }
        if (g_CcwJoyKeyCfg.slow4 >= 0 && (ji.dwButtons & (1u << g_CcwJoyKeyCfg.slow4)) != 0) {
            flags |= 0x200u;
        }
        unsigned int xCenter = (g_CcwJoyCaps.xMin0 + g_CcwJoyCaps.xMax4) / 2;
        unsigned int xDead = (g_CcwJoyCaps.xMax4 - g_CcwJoyCaps.xMin0) / 4;
        if (ji.dwXpos > xCenter + xDead) {
            flags |= 0x80u;
        }
        if (ji.dwXpos < xCenter - xDead) {
            flags |= 0x40u;
        }
        unsigned int yCenter = (g_CcwJoyCaps.yMin8 + g_CcwJoyCaps.yMaxC) / 2;
        unsigned int yDead = (g_CcwJoyCaps.yMaxC - g_CcwJoyCaps.yMin8) / 4;
        if (ji.dwYpos > yCenter + yDead) {
            flags |= 0x20u;
        }
        if (ji.dwYpos < yCenter - yDead) {
            flags |= 0x10u;
        }
        return flags;
    }

    if (g_CcwDiJoy->Poll() < 0) {
        HRESULT hr = g_CcwDiJoy->Acquire();
        int tries = 0;
        while (hr == (HRESULT)0x8007001e) {
            hr = g_CcwDiJoy->Acquire();
            CcwDebugLog("error : DIERR_INPUTLOST %d\r\n", tries);
            ++tries;
            if (tries >= 400) {
                break;
            }
        }
        return flags;
    }

    CcwDiJoyState2 js;
    memset(&js, 0, sizeof(js));
    if (g_CcwDiJoy->GetDeviceState(sizeof(js), &js) < 0) {
        return flags;
    }
    if (g_CcwJoyKeyCfg.shot0 >= 0 && (js.rgbButtons[g_CcwJoyKeyCfg.shot0] & 0x80) != 0) {
        flags |= 1u;
    }
    if (g_CcwJoyKeyCfg.bomb2 >= 0 && (js.rgbButtons[g_CcwJoyKeyCfg.bomb2] & 0x80) != 0) {
        flags |= 2u;
    }
    if (g_CcwJoyKeyCfg.pause8 >= 0 && (js.rgbButtons[g_CcwJoyKeyCfg.pause8] & 0x80) != 0) {
        flags |= 0x100u;
    }
    if (g_CcwJoyKeyCfg.skip6 >= 0 && (js.rgbButtons[g_CcwJoyKeyCfg.skip6] & 0x80) != 0) {
        flags |= 8u;
    }
    if (g_CcwJoyKeyCfg.slow4 >= 0 && (js.rgbButtons[g_CcwJoyKeyCfg.slow4] & 0x80) != 0) {
        flags |= 0x200u;
    }
    if (js.lX < -g_CcwPadDeadX) {
        flags |= 0x40u;
    }
    if (js.lY < -g_CcwPadDeadY) {
        flags |= 0x10u;
    }
    if (js.lX > g_CcwPadDeadX) {
        flags |= 0x80u;
    }
    if (js.lX > g_CcwPadDeadY) {
        flags |= 0x20u;
    }
    return flags;
}

unsigned char *__stdcall CcwFindRiffChunk(unsigned char *data, unsigned int size, unsigned int *outSize, const char *tag) {
    while (size != 0) {
        *outSize = *(unsigned int *)(data + 4);
        if (strncmp((const char *)data, tag, 4) == 0) {
            return data + 8;
        }
        size -= *outSize + 8;
        data += *outSize + 8;
    }
    return NULL;
}

int CcwSoundBuffer::LoadWave(const char *name) {
    if (g_CcwSoundEnabled == 0) {
        return 0;
    }
    if (buf0 != NULL) {
        buf0->Release();
        buf0 = NULL;
    }
    for (int i = 0; i < countC; ++i) {
        if (g_CcwSfxEntries[i].info8->waveIdx4 == info8->waveIdx4) {
            g_CcwDSound->DuplicateSoundBuffer(g_CcwSfxEntries[i].buf0, &buf0);
            return 0;
        }
    }
    while (g_CcwWaveBufs[info8->waveIdx4] == NULL) {
        Sleep(10);
        if (g_CcwLoadThreadState == 2) {
            return 0;
        }
    }

    unsigned char *raw = (unsigned char *)g_CcwWaveBufs[info8->waveIdx4];
    if (strncmp((const char *)raw, "RIFF", 4) != 0) {
        g_CcwErrorCtx.Log("Wav \x83t\x83@\x83" "C\x83\x8b\x82\xb6\x82\xe1\x82\xc8\x82\xa2 %s\r\n", name);
        goto fail;
    }
    int riffSize = *(int *)(raw + 4);
    raw += 8;
    if (strncmp((const char *)raw, "WAVE", 4) != 0) {
        g_CcwErrorCtx.Log("Wav \x83t\x83@\x83" "C\x83\x8b\x82\xb6\x82\xe1\x82\xc8\x82\xa2? %s\r\n", name);
        goto fail;
    }
    unsigned int chunkSize;
    unsigned char *fmtPtr = CcwFindRiffChunk(raw + 4, riffSize - 12, &chunkSize, "fmt ");
    if (fmtPtr == NULL) {
        g_CcwErrorCtx.Log("Wav \x83t\x83@\x83" "C\x83\x8b\x82\xb6\x82\xe1\x82\xc8\x82\xa2? %s\r\n", name);
        goto fail;
    }
    WAVEFORMATEX wfx = *(WAVEFORMATEX *)fmtPtr;
    unsigned char *dataPtr = CcwFindRiffChunk(raw + 4, riffSize - 12, &chunkSize, "data");
    if (dataPtr == NULL) {
        g_CcwErrorCtx.Log("Wav \x83t\x83@\x83" "C\x83\x8b\x82\xb6\x82\xe1\x82\xc8\x82\xa2? %s\r\n", name);
        goto fail;
    }

    CcwDSBufferDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    desc.dwFlags = 0x80c8;
    desc.dwBufferBytes = chunkSize;
    desc.lpwfxFormat = &wfx;
    if (g_CcwDSound->CreateSoundBuffer(&desc, &buf0, NULL) < 0) {
        goto fail;
    }

    void *ptr1;
    DWORD size1;
    void *ptr2;
    DWORD size2;
    if (buf0->Lock(0, chunkSize, &ptr1, &size1, &ptr2, &size2, 0) < 0) {
        goto fail;
    }
    memcpy(ptr1, dataPtr, size1);
    if (size2 != 0) {
        memcpy(ptr2, dataPtr + size1, size2);
    }
    buf0->Unlock(ptr1, size1, ptr2, size2);

    if (g_CcwWaveBufs[info8->waveIdx4] != NULL) {
        free(g_CcwWaveBufs[info8->waveIdx4]);
        g_CcwWaveBufs[info8->waveIdx4] = NULL;
    }
    CcwSoundDebugLog("Create Sound Buffer %s\n", name);
    return 0;

fail:
    if (g_CcwWaveBufs[info8->waveIdx4] != NULL) {
        free(g_CcwWaveBufs[info8->waveIdx4]);
        g_CcwWaveBufs[info8->waveIdx4] = NULL;
    }
    return -1;
}

HRESULT CcwDSound::FillBufferWithSound(CcwIDirectSoundBuffer *pDSB, BOOL bRepeatWavIfBufferLarger) {
    HRESULT hr;
    void *pDSLockedBuffer = NULL;
    DWORD dwDSLockedBufferSize = 0;
    DWORD dwWavDataRead = 0;

    if (pDSB == NULL) {
        return (HRESULT)0x800401f0;
    }

    DWORD dwStatus;
    if (FAILED(hr = pDSB->GetStatus(&dwStatus))) {
        return hr;
    }
    if (dwStatus & 2) {
        hr = pDSB->Restore();
        if (hr == (HRESULT)0x88780096) {
            do {
                Sleep(10);
                hr = pDSB->Restore();
            } while (hr != 0);
        }
    }

    if (FAILED(hr = pDSB->Lock(0, bufferSize8, &pDSLockedBuffer, &dwDSLockedBufferSize, NULL, NULL, 0))) {
        return hr;
    }

    waveFileC->ResetFile();

    if (FAILED(hr = waveFileC->Read((unsigned char *)pDSLockedBuffer, dwDSLockedBufferSize, &dwWavDataRead))) {
        return hr;
    }

    if (dwWavDataRead == 0) {
        memset(pDSLockedBuffer, (waveFileC->hdr90->bitsPerSample2e == 8) ? 128 : 0, dwDSLockedBufferSize);
    } else if (dwWavDataRead < dwDSLockedBufferSize) {
        if (bRepeatWavIfBufferLarger) {
            DWORD dwReadSoFar = dwWavDataRead;
            while (dwReadSoFar < dwDSLockedBufferSize) {
                if (FAILED(hr = waveFileC->ResetFile())) {
                    return hr;
                }
                if (FAILED(hr = waveFileC->Read((unsigned char *)pDSLockedBuffer + dwReadSoFar,
                    dwDSLockedBufferSize - dwReadSoFar, &dwWavDataRead))) {
                    return hr;
                }
                dwReadSoFar += dwWavDataRead;
            }
        } else {
            memset((unsigned char *)pDSLockedBuffer + dwWavDataRead,
                (waveFileC->hdr90->bitsPerSample2e == 8) ? 128 : 0,
                dwDSLockedBufferSize - dwWavDataRead);
        }
    }

    pDSB->Unlock(pDSLockedBuffer, dwDSLockedBufferSize, NULL, 0);
    return S_OK;
}

int CcwReplayMgr::LoadReplay(const char *filename) {
    char path[256];
    strcpy(filename3b4, filename);

    unsigned char *compBuf;
    if ((g_CcwStageFlags & 0x20) == 0) {
        _chdir(g_CcwAppDataDir);
        sprintf(path, "replay/%s", filename);
        if (CcwFileExists(path) == 0 || CcwOpenReadStream(path) != 0) {
            _chdir(g_CcwOrigDir);
            return -1;
        }
        header18 = (CcwReplayHeader *)CcwReadStream(0x24);
        if (header18->magic0 != 0x72383231) {
            CcwCloseStream();
            _chdir(g_CcwOrigDir);
            return -1;
        }
        if (header18->version4 != 2) {
            CcwCloseStream();
            _chdir(g_CcwOrigDir);
            return -1;
        }
        compBuf = (unsigned char *)CcwReadStream(header18->compSize1c);
        CcwCloseStream();
    } else {
        unsigned int packedSize;
        header18 = (CcwReplayHeader *)CcwLoadPackedFile(filename, &packedSize, 0);
        compBuf = (unsigned char *)header18 + 0x24;
    }

    uncompBuf39c = malloc(header18->uncompSize20);
    CcwDecryptBlocks(compBuf, header18->compSize1c, 0x5e, 0xe7, 0x800, header18->compSize1c);
    CcwDecryptBlocks(compBuf, header18->compSize1c, 0x7d, 0x36, 0x80, header18->compSize1c);
    CcwDecompressLzss(compBuf, header18->compSize1c, uncompBuf39c, header18->uncompSize20);

    data1c = (CcwReplayData *)uncompBuf39c;
    unsigned char *cur = (unsigned char *)data1c + 0x70;
    for (int i = 0; i < (data1c->stageCount58 < 0x11 ? data1c->stageCount58 : 6); ++i) {
        CcwReplayStageHdr *sh = (CcwReplayStageHdr *)cur;
        stages138[sh->stageId0].hdr10 = sh;
        stages138[sh->stageId0].frameData0 = cur + 0x90;
        stages138[sh->stageId0].frameEnd8 = stages138[sh->stageId0].frameData0 + sh->frameCount4 * 6;
        cur += sh->dataSize8 + 0x90;
    }

    if ((g_CcwStageFlags & 0x20) == 0 && compBuf != NULL) {
        free(compBuf);
    }
    _chdir(g_CcwOrigDir);
    return 0;
}

} // namespace th128
