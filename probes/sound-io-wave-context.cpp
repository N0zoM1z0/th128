#include "../src/SoundIoWave.hpp"

#include <string.h>

namespace th128 {

volatile int g_SiwSink = 0;

unsigned int g_SiwFlags = 0;
SiwDiPad *g_SiwPad = NULL;
short g_SiwBtn0 = 0;
short g_SiwBtn1 = 0;
short g_SiwBtn4 = 0;
short g_SiwBtn3 = 0;
short g_SiwBtn2 = 0;
unsigned int g_SiwJoyMinX = 0;
unsigned int g_SiwJoyMaxX = 0;
unsigned int g_SiwJoyMinY = 0;
unsigned int g_SiwJoyMaxY = 0;
short g_SiwDeadzoneX = 0;
short g_SiwDeadzoneY = 0;
int g_SiwBgmBaseOffset = 0;

__declspec(noinline) void SiwDebugLog(const char *fmt, ...) {
    g_SiwSink += (int)fmt;
}

__declspec(noinline) HRESULT SiwWaveFile::Read(unsigned char *buf, DWORD sizeToRead,
                                               DWORD *sizeRead) {
    if (isMemory != 0) {
        if (memCur == 0) {
            return (HRESULT)0x800401f0;
        }
        if (sizeRead != 0) {
            *sizeRead = 0;
        }
        if (memCur + sizeToRead > memBase + memRemaining) {
            sizeToRead = (DWORD)(memBase - memCur + memRemaining);
        }
        memcpy(buf, memCur, sizeToRead);
        memCur += sizeToRead;
        if (sizeRead != 0) {
            *sizeRead = sizeToRead;
        }
        return S_OK;
    } else {
        if (fileHandle == 0) {
            return (HRESULT)0x800401f0;
        }
        if (buf == 0 || sizeRead == 0) {
            return E_INVALIDARG;
        }
        DWORD n = sizeToRead;
        if ((int)sizeToRead > remaining) {
            n = (DWORD)remaining;
        }
        remaining -= (int)n;
        DWORD got;
        ReadFile(fileHandle, buf, n, &got, 0);
        *sizeRead = got;
        return S_OK;
    }
}

__declspec(noinline) HRESULT SiwWaveFile::ResetFile(bool loop, DWORD seekPos) {
    if (isMemory != 0) {
        memCur = memBase + seekPos;
        if (loop) {
            memRemaining = desc->dataSize;
        }
        return S_OK;
    }
    if (fileHandle == INVALID_HANDLE_VALUE || fileHandle == NULL) {
        return (HRESULT)0x800401f0;
    }
    SetFilePointer(fileHandle, desc->dataOffset + (int)seekPos + g_SiwBgmBaseOffset, NULL, FILE_BEGIN);
    remaining = desc->dataSize;
    return S_OK;
}

__declspec(noinline) HRESULT SiwSound::RestoreBuffer(SiwDsBuffer *pDSB, int *pbWasRestored) {
    if (pDSB == NULL) {
        return (HRESULT)0x800401f0;
    }
    if (pbWasRestored != NULL) {
        *pbWasRestored = 0;
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
        if (pbWasRestored != NULL) {
            *pbWasRestored = 1;
        }
        return S_OK;
    }
    return S_FALSE;
}

} // namespace th128

static void *volatile g_SiwKeep[8];

extern "C" int __stdcall SoundIoWaveProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    g_SiwSink += (int)SiwPollGamepad((unsigned int)a);
    g_SiwSink += (int)SiwPollGamepad((unsigned int)b);

    SiwSound *snd = (SiwSound *)(int)k;
    SiwDsBuffer *dsb = (SiwDsBuffer *)(int)a;
    g_SiwSink += (int)snd->FillBufferWithSound(dsb, b);
    g_SiwSink += (int)(snd + 1)->FillBufferWithSound(dsb + 1, a);

    g_SiwSink += (int)snd->HandleWaveStreamNotification(1);
    g_SiwSink += (int)(snd + 1)->HandleWaveStreamNotification(1);

    SiwWaveFile *wf = (SiwWaveFile *)(int)k;
    DWORD got = 0;
    g_SiwSink += (int)wf->Read((unsigned char *)(int)a, (DWORD)b, &got);
    g_SiwSink += (int)wf->ResetFile((a & 1) != 0, (DWORD)b);

    g_SiwKeep[0] = (void *)&SiwDebugLog;
    return g_SiwSink;
}
