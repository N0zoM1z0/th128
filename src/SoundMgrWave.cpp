#include "SoundMgrWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

int SmwSoundMgr::Initialize(HWND hwnd) {
    for (int i = 0; i < 0x4c; ++i) {
        sfxSlots1988[i].field4 = -1;
        SmwSfxTableEntry *t = g_SmwSfxTable;
        while (t->id != i) {
            ++t;
        }
        sfxSlots1988[i].indexC = i;
        sfxSlots1988[i].tbl8 = t;
    }
    for (int i = 0; i < 12; ++i) {
        playQueue20[i] = -1;
    }

    csoundMgr10 = new SmwCSoundManager();
    if (csoundMgr10->ds != NULL) {
        csoundMgr10->ds->Release();
        csoundMgr10->ds = NULL;
    }
    if (FAILED(g_SmwDirectSoundCreate8(NULL, &csoundMgr10->ds, NULL)) ||
        FAILED(csoundMgr10->ds->SetCooperativeLevel(hwnd, 2))) {
        g_SmwErrorCtx.Log("DirectSound failed\n");
        if (csoundMgr10 != NULL) {
            delete csoundMgr10;
            csoundMgr10 = NULL;
        }
        return -1;
    }
    csoundMgr10->SetPrimaryBufferFormat();
    ds0 = csoundMgr10->ds;

    SmwBufferDesc dsbd;
    WAVEFORMATEX wfx;
    memset(&dsbd, 0, sizeof(dsbd));
    memset(&wfx, 0, sizeof(wfx));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwFlags = 0x8008;
    dsbd.dwBufferBytes = 0x8000;
    dsbd.lpwfxFormat = &wfx;

    wfx.wFormatTag = 1;
    wfx.nChannels = 2;
    wfx.nSamplesPerSec = 44100;
    wfx.nAvgBytesPerSec = 44100 * 4;
    wfx.nBlockAlign = 4;
    wfx.wBitsPerSample = 16;
    wfx.cbSize = 0;
    bgmThread18 = NULL;

    if (FAILED(ds0->CreateSoundBuffer(&dsbd, &silentBuf8, NULL))) {
        return -1;
    }
    void *p1, *p2;
    DWORD b1, b2;
    if (FAILED(silentBuf8->Lock(0, 0x8000, &p1, &b1, &p2, &b2, 0))) {
        return -1;
    }
    memset(p1, 0, 0x8000);
    silentBuf8->Unlock(p1, b1, p2, b2);
    silentBuf8->Play(0, 0, 1);

    bgmVol5454 = 100;
    sfxVol5458 = 100;
    SetTimer(hwnd, 0, 250, NULL);
    hwndC = hwnd;

    for (int i = 0; i < 0x4c; ++i) {
        if (g_SmwAudioMode == 2) {
            return -1;
        }
        if (LoadWav(i, g_SmwWavNames[g_SmwSfxTable[i].wavIdx]) != 0) {
            g_SmwErrorCtx.Log("wav load failed %s\n", g_SmwWavNames[g_SmwSfxTable[i].wavIdx]);
            return -1;
        }
    }
    g_SmwErrorCtx.Log("DirectSound OK\n");
    return 0;
}

int SmwSoundMgr::Shutdown() {
    if (g_SmwSoundMgr.trackTable1984 != NULL) {
        free(g_SmwSoundMgr.trackTable1984);
        g_SmwSoundMgr.trackTable1984 = NULL;
    }
    for (int i = 0; i < 0x4c; ++i) {
        if (g_SmwSoundMgr.sfxSlots1988[i].buf0 != NULL) {
            g_SmwSoundMgr.sfxSlots1988[i].buf0->Release();
            g_SmwSoundMgr.sfxSlots1988[i].buf0 = NULL;
        }
    }
    for (int i = 0; i < 65; ++i) {
        if (g_SmwSoundMgr.wavBufs20a8[i] != NULL) {
            free(g_SmwSoundMgr.wavBufs20a8[i]);
            g_SmwSoundMgr.wavBufs20a8[i] = NULL;
        }
    }
    if (g_SmwSoundMgr.csoundMgr10 != NULL) {
        KillTimer(g_SmwSoundMgr.hwndC, 1);
        g_SmwSoundMgr.StopAndDestroyBgm();
        g_SmwSoundMgr.ds0 = NULL;
        g_SmwSoundMgr.silentBuf8->Stop();
        if (g_SmwSoundMgr.silentBuf8 != NULL) {
            g_SmwSoundMgr.silentBuf8->Release();
            g_SmwSoundMgr.silentBuf8 = NULL;
        }
        if (g_SmwSoundMgr.stream542c != NULL) {
            delete g_SmwSoundMgr.stream542c;
            g_SmwSoundMgr.stream542c = NULL;
        }
        if (g_SmwSoundMgr.csoundMgr10 != NULL) {
            delete g_SmwSoundMgr.csoundMgr10;
            g_SmwSoundMgr.csoundMgr10 = NULL;
        }
        for (int i = 0; i < 16; ++i) {
            if (g_SmwSoundMgr.preloadBuf18c0[i] != NULL) {
                free(g_SmwSoundMgr.preloadBuf18c0[i]);
                g_SmwSoundMgr.preloadBuf18c0[i] = NULL;
            }
        }
    }
    return 0;
}

int SmwSoundMgr::FindTrackIndex(const char *path) {
    char buf[128];
    int idx = 0;
    const char *p = strrchr(path, '/');
    if (p == NULL) {
        p = strrchr(path, '\\');
        if (p == NULL) {
            strcpy(buf, path);
        } else {
            strcpy(buf, p + 1);
        }
    } else {
        strcpy(buf, p + 1);
    }
    while (trackTable1984[idx].name[0] != '\0') {
        if (strcmp(trackTable1984[idx].name, buf) == 0) {
            break;
        }
        ++idx;
    }
    if (trackTable1984[idx].name[0] == '\0') {
        return 0;
    }
    return idx;
}

int SmwSoundMgr::StartStreamingBgm() {
    strcpy(bgmFileName532c, "thbgm.dat");
    if (csoundMgr10 == NULL) {
        return -1;
    }
    if (ds0 == NULL) {
        return -1;
    }
    StopAndDestroyBgm();
    SmwTrackInfo *info = trackTable1984;
    DWORD blockAlign = info->format.nBlockAlign;
    DWORD notifySize = (info->format.nSamplesPerSec * blockAlign * 4) >> 4;
    notifyEvent5430 = CreateEventA(NULL, FALSE, FALSE, NULL);
    bgmThread18 = CreateThread(NULL, 0, SmwBgmThreadProc, g_SmwThreadParam, 0, &bgmThreadId14);
    notifySize -= notifySize % blockAlign;
    if (FAILED(csoundMgr10->CreateStreamingFromFile(
            &stream542c, bgmFileName532c, 0x10180, g_SmwGuidNull, 16, notifySize, notifyEvent5430, info))) {
        return -1;
    }
    return 0;
}

int SmwSoundMgr::PreloadBgm(const char *name, int slot) {
    if (g_SmwSoundMgr.preloadBuf18c0[slot] != NULL &&
        strcmp(name, g_SmwSoundMgr.preloadNames432c[slot]) == 0) {
        return 0;
    }
    strcpy(g_SmwSoundMgr.preloadNames432c[slot], name);
    if ((g_SmwSupervisor.flags200 & 0x10) == 0 || g_SmwSoundMgr.csoundMgr10 == NULL) {
        return 0;
    }
    if (g_SmwSoundMgr.preloadBuf18c0[slot] != NULL) {
        free(g_SmwSoundMgr.preloadBuf18c0[slot]);
        g_SmwSoundMgr.preloadBuf18c0[slot] = NULL;
    }
    SmwSoundDebugLog("Streming BGM PreLoad %d\r\n", slot);
    if (g_SmwSupervisor.gameFlags590 & 0x8000) {
        EnterCriticalSection(&g_SmwSupervisor.critSecs810[2]);
        ++g_SmwSupervisor.lockDepth930[2];
    }
    HANDLE h = CreateFileA(g_SmwSoundMgr.bgmFileName532c, GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        SmwSoundDebugLog("error : bgmfile is not find %s\r\n", g_SmwSoundMgr.bgmFileName532c);
        if (g_SmwSupervisor.gameFlags590 & 0x8000) {
            LeaveCriticalSection(&g_SmwSupervisor.critSecs810[2]);
            --g_SmwSupervisor.lockDepth930[2];
        }
        return -1;
    }
    int idx = g_SmwSoundMgr.FindTrackIndex(name);
    SetFilePointer(h, g_SmwSoundMgr.trackTable1984[idx].dataOffset, NULL, FILE_BEGIN);
    unsigned char *buf = (unsigned char *)malloc(g_SmwSoundMgr.trackTable1984[idx].preloadSize);
    if (buf == NULL) {
        CloseHandle(h);
        SmwSoundDebugLog("error : bgmfile is not find %s\r\n", g_SmwSoundMgr.bgmFileName532c);
        g_SmwSupervisor.UnlockCritSec(2);
        return -1;
    }
    DWORD bytesRead;
    ReadFile(h, buf, g_SmwSoundMgr.trackTable1984[idx].preloadSize, &bytesRead, NULL);
    CloseHandle(h);
    g_SmwSupervisor.UnlockCritSec(2);
    SmwTrackInfo *info = &g_SmwSoundMgr.trackTable1984[idx];
    g_SmwSoundMgr.preloadTrack1880[slot] = info;
    g_SmwSoundMgr.preloadBuf18c0[slot] = buf;
    g_SmwSoundMgr.preloadData1900[slot] = buf;
    g_SmwSoundMgr.preloadSize1940[slot] = info->preloadSize;
    return 0;
}

int SmwSoundMgr::LoadPreloadedBgm(int slot) {
    if (g_SmwSoundMgr.csoundMgr10 == NULL) {
        return -1;
    }
    if (g_SmwSupervisor.soundEnabled1e3 == 0) {
        return -1;
    }
    if (g_SmwSoundMgr.ds0 == NULL) {
        return -1;
    }
    if ((g_SmwSupervisor.flags200 & 0x10) == 0) {
        return g_SmwSoundMgr.OpenBgmByName(g_SmwSoundMgr.preloadNames432c[slot]);
    }
    if (g_SmwSoundMgr.preloadBuf18c0[slot] == NULL) {
        return -1;
    }
    SmwSoundDebugLog("Streming BGM Load no %d\r\n", slot);
    SmwTrackInfo *info = g_SmwSoundMgr.preloadTrack1880[slot];
    DWORD blockAlign = info->format.nBlockAlign;
    DWORD notifySize = (info->format.nSamplesPerSec * blockAlign * 4) >> 4;
    g_SmwSoundMgr.notifyEvent5430 = CreateEventA(NULL, FALSE, FALSE, NULL);
    g_SmwSoundMgr.bgmThread18 = CreateThread(NULL, 0, SmwBgmThreadProc, g_SmwThreadParam, 0, &g_SmwSoundMgr.bgmThreadId14);
    notifySize -= notifySize % blockAlign;
    if (FAILED(g_SmwSoundMgr.csoundMgr10->CreateStreamingFromMemory(
            &g_SmwSoundMgr.stream542c,
            g_SmwSoundMgr.preloadData1900[slot],
            g_SmwSoundMgr.preloadSize1940[slot],
            g_SmwSoundMgr.preloadTrack1880[slot],
            0x10180, g_SmwGuidNull, 16, notifySize, g_SmwSoundMgr.notifyEvent5430))) {
        return -1;
    }
    g_SmwSoundMgr.curPreloadSlot1980 = slot;
    return 0;
}

} // namespace th128
