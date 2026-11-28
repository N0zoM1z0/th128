#include "../src/DSoundWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

DWORD g_DswBgmBaseOffset = 0;
int g_DswBgmVolume = 100;
const GUID g_DswIID_IDirectSoundNotify = { 0xb0210783, 0x89cd, 0x11d0, { 0xaf, 0x08, 0x00, 0xa0, 0xc9, 0x25, 0xcd, 0x16 } };

static volatile int g_Sink = 0;

__declspec(noinline) void __cdecl DswWaveDebugLog(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)fmt + (int)t;
}

__declspec(noinline) HRESULT DswWaveFile::OpenFile(const char *path, DswTrackInfo *info) {
    if (path == NULL) {
        return E_INVALIDARG;
    }
    DswWaveDebugLog("Streaming File Open %s\r\n", path);
    handle = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        LPSTR msgBuf;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&msgBuf, 0, NULL);
        DswWaveDebugLog(msgBuf);
        LocalFree(msgBuf);
        return E_FAIL;
    }
    trackInfo = info;
    filename = path;
    if (fromMemory != 0) {
        memCur = memBase;
        if ((int)info->dataSize > 0) {
            memSize = info->dataSize;
        }
    } else if (handle != NULL) {
        SetFilePointer(handle, info->dataOffset + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = trackInfo->dataSize;
    }
    size = remainingBytes;
    return 0;
}

__declspec(noinline) HRESULT DswWaveFile::ResetFile(bool loop, DWORD seekPos) {
    if (fromMemory != 0) {
        memCur = memBase;
        DswTrackInfo *info = trackInfo;
        if ((int)info->dataSize > 0) {
            memSize = info->dataSize;
        }
        if (loop) {
            if ((int)info->loopStart > 0) {
                memCur = memBase + info->loopStart;
                return 0;
            }
        }
    } else {
        if (handle == INVALID_HANDLE_VALUE || handle == NULL) {
            return (HRESULT)0x800401F0;
        }
        if (loop) {
            DswTrackInfo *info = trackInfo;
            if ((int)info->loopStart > 0) {
                SetFilePointer(handle, info->dataOffset + info->loopStart + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
                remainingBytes = trackInfo->dataSize - trackInfo->loopStart;
                return 0;
            }
        }
        if (seekPos == 0) {
            SetFilePointer(handle, trackInfo->dataOffset + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
            remainingBytes = trackInfo->dataSize;
            return 0;
        }
        SetFilePointer(handle, seekPos + g_DswBgmBaseOffset, NULL, FILE_BEGIN);
        remainingBytes = trackInfo->dataSize + trackInfo->dataOffset - seekPos;
    }
    return 0;
}

__declspec(noinline) HRESULT DswWaveFile::Read(unsigned char *dst, DWORD sizeToRead, DWORD *outRead) {
    if (fromMemory != 0) {
        unsigned char *cur = memCur;
        if (cur == NULL) {
            return (HRESULT)0x800401F0;
        }
        if (outRead != NULL) {
            *outRead = 0;
        }
        if (cur + sizeToRead > memBase + memSize) {
            sizeToRead = (DWORD)(memBase - cur) + memSize;
        }
        memcpy(dst, cur, sizeToRead);
        memCur += sizeToRead;
        if (outRead != NULL) {
            *outRead = sizeToRead;
        }
        return 0;
    }
    if (handle == NULL) {
        return (HRESULT)0x800401F0;
    }
    if (dst == NULL || outRead == NULL) {
        return E_INVALIDARG;
    }
    DWORD toRead = sizeToRead;
    if (toRead > remainingBytes) {
        toRead = remainingBytes;
    }
    remainingBytes -= toRead;
    DWORD bytesRead;
    ReadFile(handle, dst, toRead, &bytesRead, NULL);
    *outRead = bytesRead;
    return 0;
}

__declspec(noinline) HRESULT DswWaveFile::Close() {
    if (mode == 1) {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
    return 0;
}

__declspec(noinline) HRESULT DswSound::RestoreBuffer(IDswBuffer *buf, int *wasRestored) {
    if (wasRestored != NULL) {
        *wasRestored = 0;
    }
    DWORD status;
    HRESULT hr = buf->GetStatus(&status);
    if (hr < 0) {
        return hr;
    }
    if ((status & 2) != 0) {
        do {
            if (buf->Restore() == (HRESULT)0x88780096) {
                Sleep(10);
            }
        } while (buf->Restore() != 0);
        if (wasRestored != NULL) {
            *wasRestored = 1;
        }
        return 0;
    }
    return 1;
}

__declspec(noinline) IDswBuffer *DswSound::GetFreeBuffer() {
    if (buffers == NULL) {
        return NULL;
    }
    DWORD i;
    for (i = 0; i < numBuffers; ++i) {
        if (buffers[i] != NULL) {
            DWORD status = 0;
            buffers[i]->GetStatus(&status);
            if ((status & 1) == 0) {
                break;
            }
        }
    }
    if (i != numBuffers) {
        return buffers[i];
    }
    return buffers[rand() % numBuffers];
}

__declspec(noinline) HRESULT DswSound::SetVolume(int volume) {
    if (g_DswBgmVolume != 0) {
        float v = 1.0f - (float)((double)g_DswBgmVolume / 100.0);
        float v2 = v * v;
        float scale = 1.0f - v2;
        int db = (int)(scale * (volume + 5000)) - 5000;
        return buffers[0]->SetVolume(db);
    }
    return buffers[0]->SetVolume(-10000);
}

__declspec(noinline) HRESULT DswSound::Reset() {
    if (buffers == NULL) {
        return (HRESULT)0x800401F0;
    }
    HRESULT hr = 0;
    for (DWORD i = 0; i < numBuffers; ++i) {
        hr |= buffers[i]->SetCurrentPosition(0);
    }
    return hr;
}

} // namespace th128

extern "C" int __stdcall DSoundWaveProbe(int selector, int arg) {
    using namespace th128;
    DswSoundManager *mgr = (DswSoundManager *)arg;
    DswStreamingSound *stream = (DswStreamingSound *)(arg + 4);
    DswSound *snd = (DswSound *)(arg + 8);
    DswWaveFile *wf = (DswWaveFile *)(arg + 12);
    DswTrackInfo *info = (DswTrackInfo *)(arg + 16);
    IDswBuffer *buf = (IDswBuffer *)(arg + 20);
    GUID guid = { (unsigned long)arg, 1, 2, { 3, 4, 5, 6, 7, 8, 9, 10 } };
    HANDLE hEv = (HANDLE)(arg + 24);
    DWORD nSize = (DWORD)(arg + 28);

    if (selector == 1) {
        return (int)mgr->CreateStreamingFromFile(&stream, "thbgm.dat", 0x18188, guid, 16, nSize, hEv, info);
    }
    if (selector == 2) {
        return (int)mgr->CreateStreamingFromMemory(&stream, (unsigned char *)(arg + 32), (DWORD)(arg + 36), info, 0x18188, guid, 16, nSize, hEv);
    }
    if (selector == 3) {
        return (int)snd->Play();
    }
    if (selector == 4) {
        if (snd->isPlayingFlag == 0) {
            return (int)snd->Resume();
        }
        return (int)snd->Resume() + snd->isPausedFlag;
    }
    if (selector == 5) {
        return (int)stream->HandleWaveStreamNotification(TRUE);
    }
    if (selector == 6) {
        HRESULT r1 = stream->ResetStream();
        HRESULT r2 = stream->ResetStream();
        return (int)(r1 | r2);
    }
    if (selector == 7) {
        HRESULT r1 = snd->FillBufferWithSound(buf, arg & 1);
        HRESULT r2 = snd->FillBufferWithSound(buf, (arg >> 1) & 1);
        return (int)(r1 | r2);
    }
    if (selector == 8) {
        return (int)wf->ReopenTrack(info, nSize);
    }
    if (selector == 9) {
        return (int)mgr->CreateStreamingFromFile(&stream, "thbgm.dat", 0x18188, guid, 16, nSize + 4, hEv, info + 1);
    }
    if (selector == 10) {
        return (int)snd->SetVolume(arg);
    }
    return 0;
}
