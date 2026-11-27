#include "ZeroCallMedium12.hpp"

#include <string.h>
#include <windows.h>

int g_Zc12DatCount;
Zc12DatEntry g_Zc12DatTable[32];
Zc12PopupMgr *g_Zc12PopupMgr;
float g_Zc12TimeScale;
int g_Zc12SfxVol;
int g_Zc12TexFmt;
int g_Zc12TexPitch;
unsigned char *g_Zc12TexPixels;

Zc12DatEntry *Zc12FindDatEntry(const char *path) {
    char buf[260];
    const char *p = strchr(path, '/');
    int idx = (int)((p != 0 ? p + 1 : path) - path) - 1;
    strcpy(buf, path);
    if (idx >= 0) {
        strcpy(&buf[idx], ".dat");
    }
    Zc12DatEntry *e = g_Zc12DatTable;
    for (int i = 0; i < g_Zc12DatCount; ++i, ++e) {
        if (strcmp(e->name, buf) == 0) {
            return e;
        }
    }
    return 0;
}

void __stdcall Zc12SpawnScorePopup(const Zc12Float3 *pos, int score, int color) {
    Zc12PopupMgr *mgr = g_Zc12PopupMgr;
    if (mgr->cursor >= 10) {
        mgr->cursor = 0;
    }
    Zc12PopupSlot *slot = &mgr->slots[mgr->cursor];
    slot->active = 1;
    int n = 0;
    if (score >= 0) {
        while (score != 0) {
            slot->digits[n] = (unsigned char)(score % 10);
            score /= 10;
            ++n;
        }
        if (n == 0) {
            slot->digits[0] = 0;
            n = 1;
        }
    } else {
        slot->digits[0] = 10;
        n = 1;
    }
    slot->digitCount = (unsigned char)n;
    slot->color = color;
    if ((slot->timerFlags & 1) == 0) {
        slot->timerCurF = 0.0f;
        slot->timerCur = 0;
        slot->timerPrev = -999999;
        slot->timerScale = &g_Zc12TimeScale;
        slot->timerFlags |= 1;
    }
    slot->timerCurF = 0.0f;
    slot->timerCur = 0;
    slot->timerPrev = -1;
    slot->pos = *pos;
    ++mgr->cursor;
}

void __stdcall Zc12SetMeterAnmsA(Zc12GuiMeters *gui, int full, int sub) {
    int i;
    for (i = 0; i < full; ++i) {
        if (gui->metersA[i].cb != 0) {
            gui->metersA[i].cb(&gui->metersA[i], 2);
        }
        gui->metersA[i].val = 2;
    }
    if ((unsigned int)i < 8) {
        short v = (short)(sub + 7);
        if (gui->metersA[i].cb != 0) {
            gui->metersA[i].cb(&gui->metersA[i], v);
        }
        gui->metersA[i].val = v;
        ++i;
        for (; (unsigned int)i < 8; ++i) {
            if (gui->metersA[i].cb != 0) {
                gui->metersA[i].cb(&gui->metersA[i], 3);
            }
            gui->metersA[i].val = 3;
        }
    }
}

void __stdcall Zc12SetMeterAnmsB(Zc12GuiMeters *gui, int full, int sub) {
    int i;
    for (i = 0; i < full; ++i) {
        if (gui->metersB[i].cb != 0) {
            gui->metersB[i].cb(&gui->metersB[i], 2);
        }
        gui->metersB[i].val = 2;
    }
    if ((unsigned int)i < 8) {
        short v = (short)(sub + 7);
        if (gui->metersB[i].cb != 0) {
            gui->metersB[i].cb(&gui->metersB[i], v);
        }
        gui->metersB[i].val = v;
        ++i;
        for (; (unsigned int)i < 8; ++i) {
            if (gui->metersB[i].cb != 0) {
                gui->metersB[i].cb(&gui->metersB[i], 3);
            }
            gui->metersB[i].val = 3;
        }
    }
}

int Zc12StepBulletMove(Zc12BulletMove *b) {
    if (b->timerCur >= b->limit) {
        b->flags &= ~9u;
        return 1;
    }
    Zc12Float3 d;
    d.x = b->vel.x * g_Zc12TimeScale;
    d.y = b->vel.y * g_Zc12TimeScale;
    d.z = b->vel.z * g_Zc12TimeScale;
    b->pos.x += d.x;
    b->pos.y += d.y;
    b->pos.z += d.z;
    if ((b->timerFlags & 1) == 0) {
        b->timerCurF = 0.0f;
        b->timerCur = 0;
        b->timerPrev = -999999;
        b->timerScale = &g_Zc12TimeScale;
        b->timerFlags |= 1;
    }
    b->timerCurF = 0.0f;
    b->timerCur = 0;
    b->timerPrev = -1;
    return 0;
}

void Zc12LaserObj::RunActions() {
    while (actionIdx < 18) {
        Zc12LaserAction *a = &actions[actionIdx];
        if (a->type == 0) {
            break;
        }
        if (a->cond24 == 0 && flag598 != 0) {
            break;
        }
        if (a->type == 0x80) {
            val5B4 = a->param10;
            ++actionIdx;
        } else if (a->type == 0x400) {
            state0C = 3;
            ++actionIdx;
        } else if (a->type == 0x100000) {
            if (a->param10 != 0) {
                flagsDE8 = (flagsDE8 & ~0xc0u) | 0x20u;
            } else {
                flagsDE8 &= ~0xe0u;
            }
            ++actionIdx;
        } else {
            ++actionIdx;
        }
    }
}

int __stdcall Zc12ReadWaveData(Zc12WaveReader *r, unsigned int size, unsigned char *dst, unsigned int *outRead) {
    if (r->isMemory != 0) {
        unsigned char *cur = r->memCur;
        if (cur == 0) {
            return (int)0x800401f0u;
        }
        if (outRead != 0) {
            *outRead = 0;
        }
        if (cur + size > r->memBase + r->memSize) {
            size = (unsigned int)((r->memBase + r->memSize) - cur);
        }
        memcpy(dst, cur, size);
        r->memCur += size;
        if (outRead != 0) {
            *outRead = size;
        }
        return 0;
    }
    if (r->fileHandle == 0) {
        return (int)0x800401f0u;
    }
    if (dst == 0 || outRead == 0) {
        return (int)0x80070057u;
    }
    unsigned int toRead = size;
    if (toRead > r->remaining) {
        toRead = r->remaining;
    }
    r->remaining -= toRead;
    DWORD readBytes = 0;
    ReadFile(r->fileHandle, dst, toRead, &readBytes, 0);
    *outRead = readBytes;
    return 0;
}

void Zc12PlaySfxChannel(Zc12SfxChan *chan, int pan) {
    if (chan->buf == 0) {
        return;
    }
    chan->buf->vtbl->Stop(chan->buf);
    chan->buf->vtbl->SetCurrentPosition(chan->buf, 0);
    chan->buf->vtbl->SetPan(chan->buf, pan);
    chan->pan = pan;
    if (g_Zc12SfxVol != 0) {
        float f = (float)(g_Zc12SfxVol / 100.0);
        f = 1.0f - f;
        f = f * f * f;
        f = 1.0f - f;
        int vol = (int)(f * (chan->info->baseVol + 5000)) - 5000;
        chan->buf->vtbl->SetVolume(chan->buf, vol);
    } else {
        chan->buf->vtbl->SetVolume(chan->buf, -10000);
    }
    chan->buf->vtbl->Play(chan->buf, 0, 0, chan->info->playFlags);
}

bool __stdcall Zc12InvertTexAlpha(int rows) {
    int total = g_Zc12TexPitch * rows;
    unsigned char *p = g_Zc12TexPixels;
    switch (g_Zc12TexFmt) {
    case 0x15:
        for (int i = 3; i < total; i += 4) {
            p[i] = (unsigned char)~p[i];
        }
        return true;
    case 0x19: {
        unsigned short *w = (unsigned short *)p;
        for (int i = 0; i < total; i += 2, ++w) {
            unsigned short v = *w;
            v = (unsigned short)((v & 0x7fff) | (~v & 0x8000));
            *w = v;
            if ((v & 0x8000) == 0) {
                *w = (unsigned short)(v & 0x8000);
            }
        }
        return true;
    }
    case 0x1a:
        for (int i = 1; i < total; i += 2) {
            p[i] ^= 0xf0;
        }
        return true;
    default:
        return false;
    }
}
