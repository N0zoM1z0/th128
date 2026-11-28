#include "WaveFive.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

// 1. 0x0042EEB0 (170B)
void WfLaserInfinite::Slot01() {
    while (this->index594 < 0x12) {
        WfLaserEntry *e = &this->entries[this->index594];
        int type = e->type20;
        if (type == 0) {
            return;
        }
        if (e->flag24 == 0 && this->blocked598 != 0) {
            return;
        }
        if (type == 0x80) {
            this->param5b4 = e->value10;
            ++this->index594;
        } else if (type == 0x400) {
            this->state0c = 3;
            ++this->index594;
        } else if (type == 0x100000) {
            if (e->value10 != 0) {
                this->flagsDe8 = (this->flagsDe8 & ~0xc0u) | 0x20u;
            } else {
                this->flagsDe8 &= ~0xe0u;
            }
            ++this->index594;
        } else {
            ++this->index594;
        }
    }
}

// 2. 0x00451D00 (166B)
bool __stdcall WfInvertTextImageAlpha(int height) {
    int totalBytes = g_WfTextStride * height;
    unsigned char *bits = g_WfTextBits;
    switch (g_WfTextFmt) {
    case 0x15: // A8R8G8B8
        for (int i = 3; i < totalBytes; i += 4) {
            bits[i] = (unsigned char)~bits[i];
        }
        return true;
    case 0x19: // A1R5G5B5
        {
            unsigned short *p = (unsigned short *)bits;
            for (int i = 0; i < totalBytes; i += 2, ++p) {
                unsigned short v = *p;
                v = (unsigned short)((v & 0x7fff) | (~v & 0x8000));
                *p = v;
                if ((v & 0x8000) == 0) {
                    *p = (unsigned short)(v & 0x8000);
                }
            }
        }
        return true;
    case 0x1a: // A4R4G4B4
        for (int i = 1; i < totalBytes; i += 2) {
            bits[i] ^= 0xf0;
        }
        return true;
    default:
        return false;
    }
}

// 3. 0x00420020 (165B)
void __stdcall WfGuiGetSpellTime(int *outSec, int *outCs) {
    int enc = g_WfGui->encodedSpellTime;
    int sec = (enc / 100 % 1000 + 934) % 1000;
    int cs = (enc % 100 + 67) % 100;
    if (enc / 100000 - 22 != sec + cs) {
        *outSec = 999;
        *outCs = 99;
        return;
    }
    int enc2 = g_WfGui->encodedSpellTime;
    *outSec = (enc2 / 100 % 1000 + 934) % 1000;
    *outCs = (enc2 % 100 + 67) % 100;
}

// 4. 0x00458210 (154B)
void WfSoundManager::StopStreamingBgm() {
    if (this->stream == NULL) {
        return;
    }
    this->stream->Stop(1);
    if (this->threadHandle != NULL) {
        PostThreadMessageA(this->threadId, WM_QUIT, 0, 0);
        while (WaitForSingleObject(this->threadHandle, 0x100) != 0) {
            PostThreadMessageA(this->threadId, WM_QUIT, 0, 0);
        }
        CloseHandle(this->threadHandle);
        CloseHandle(this->notifyEvent);
        this->threadHandle = NULL;
    }
    if (this->stream != NULL) {
        this->stream->Destroy(1);
        this->stream = NULL;
    }
}

// 5. 0x004616D0 (142B)
int WfAnmManager::DrawSprite2D(WfAnmVm *vm, const void *verts) {
    unsigned int f = vm->flags;
    if ((f & 1) == 0 || (f & 2) == 0 || vm->alpha == 0) {
        return -1;
    }
    void **texSlot = vm->sprite->texSlot;
    if (this->curTexSlot != texSlot) {
        this->curTexSlot = texSlot;
        this->FlushSpriteBatch();
        g_WfDevice->SetTexture(0, *this->curTexSlot);
    }
    if (this->fvfState != 1) {
        this->FlushSpriteBatch();
        this->fvfState = 1;
    }
    this->ApplyState(vm);
    this->AppendSprite(verts);
    return 0;
}

// 6. 0x00457EC0 (508B)
int WfPreLoadBgm(const char *path, int slot) {
    if (g_WfPreloadBufs[slot] != NULL) {
        if (strcmp(path, g_WfPreloadNames[slot]) == 0) {
            return 0;
        }
    }
    strcpy(g_WfPreloadNames[slot], path);
    if ((g_WfSoundFlag & 0x10) == 0 || g_WfSoundInited == 0) {
        return 0;
    }
    if (g_WfPreloadBufs[slot] != NULL) {
        free(g_WfPreloadBufs[slot]);
        g_WfPreloadBufs[slot] = NULL;
    }
    WfSoundDebugLog("Streming BGM PreLoad %d\r\n", slot);
    if (g_WfFlags & 0x8000) {
        EnterCriticalSection(&g_WfSoundCs);
        ++g_WfSoundCsDepth;
    }
    HANDLE hFile = CreateFileA(g_WfBgmPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                               OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        WfSoundDebugLog("error : bgmfile is not find %s\r\n", g_WfBgmPath);
        if (g_WfFlags & 0x8000) {
            LeaveCriticalSection(&g_WfSoundCs);
            --g_WfSoundCsDepth;
        }
        return -1;
    }
    int idx = g_WfSoundMgr.FindBgmIndex(path);
    SetFilePointer(hFile, g_WfSoundMgr.bgmTable[idx].offset, NULL, FILE_BEGIN);
    void *buf = malloc(g_WfSoundMgr.bgmTable[idx].size);
    if (buf == NULL) {
        CloseHandle(hFile);
        WfSoundDebugLog("error : bgmfile is not find %s\r\n", g_WfBgmPath);
        g_WfCritSup.UnlockCritSec(2);
        return -1;
    }
    DWORD bytesRead;
    ReadFile(hFile, buf, g_WfSoundMgr.bgmTable[idx].size, &bytesRead, NULL);
    CloseHandle(hFile);
    g_WfCritSup.UnlockCritSec(2);
    WfBgmEntry *entry = &g_WfSoundMgr.bgmTable[idx];
    g_WfPreloadEntries[slot] = entry;
    g_WfPreloadBufs[slot] = buf;
    g_WfPreloadCurs[slot] = buf;
    g_WfPreloadSizes[slot] = entry->size;
    return 0;
}

} // namespace th128
