#include "EndingIceWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

static const double kPiD = (double)3.1415927410125732f;

inline void EiwChainMgr::Cut(EiwChainElem *elem) {
    if (elem == NULL) {
        return;
    }
    if (g_EiwGameFlags & 0x8000) {
        EnterCriticalSection(&g_EiwChainCs);
        ++g_EiwChainLockDepth;
    }
    EiwChainCutInternal(this, elem);
    if (g_EiwGameFlags & 0x8000) {
        LeaveCriticalSection(&g_EiwChainCs);
        --g_EiwChainLockDepth;
    }
}

EiwEnding::~EiwEnding() {
    g_EiwChainMgr->Cut(chain8);
    g_EiwChainMgr->Cut(chainC);
    if (sub18 != NULL) {
        delete sub18;
    }
    sub18 = NULL;
    g_EiwAnmMgr->ReleaseSlot(g_EiwAnmMgr->res0);
    g_EiwAnmMgr->ReleaseSlot(g_EiwAnmMgr->res1);
    g_EiwAnmMgr->ReleaseSlot(g_EiwAnmMgr->res2);
    g_EiwAnmMgr->ReleaseSlot(g_EiwAnmMgr->res3);
    if (buf14 != NULL) {
        free(buf14);
        buf14 = NULL;
    }
    buf14 = NULL;
    g_EiwEnding = NULL;
    g_EiwGlobalAux = NULL;
}

int __fastcall EiwStartEffectBlue(EiwAnmVm *vm, const EiwVec3 *pos) {
    EiwEffectData *d = (EiwEffectData *)malloc(sizeof(EiwEffectData));
    vm->extraData4ac = d;
    memset(d, 0, sizeof(EiwEffectData));
    d->pts[0][0] = pos->x;
    d->pts[0][1] = pos->y;
    d->angleC0 = (float)(g_EiwRng.GetFloatSigned() * kPiD);
    if ((d->flagsD4 & 1) == 0) {
        d->curFCC = 0.0f;
        d->curC8 = 0;
        d->prevC4 = -999999;
        d->gameSpeedD0 = &g_EiwTimeScale;
        d->flagsD4 |= 1u;
    }
    d->curC8 = 1;
    d->curFCC = 1.0f;
    d->prevC4 = 0;
    int fadeIdx = 0;
    for (int i = 0; i < 16; ++i) {
        d->colors[i] = 0xFF0040FFu;
        unsigned char *c = (unsigned char *)&d->colors[i];
        if (i < 8) {
            c[2] = (unsigned char)(255 - (i << 5));
        } else {
            c[3] = (unsigned char)(255 - (fadeIdx << 4));
            ++fadeIdx;
        }
    }
    vm->pos464 = *pos;
    vm->mode20 = 13;
    return 0;
}

int __fastcall EiwStartEffectGreen(EiwAnmVm *vm, const EiwVec3 *pos) {
    EiwEffectData *d = (EiwEffectData *)malloc(sizeof(EiwEffectData));
    vm->extraData4ac = d;
    memset(d, 0, sizeof(EiwEffectData));
    d->pts[0][0] = pos->x;
    d->pts[0][1] = pos->y;
    d->angleC0 = (float)(g_EiwRng.GetFloatSigned() * kPiD);
    if ((d->flagsD4 & 1) == 0) {
        d->curFCC = 0.0f;
        d->curC8 = 0;
        d->prevC4 = -999999;
        d->gameSpeedD0 = &g_EiwTimeScale;
        d->flagsD4 |= 1u;
    }
    d->curC8 = 1;
    d->curFCC = 1.0f;
    d->prevC4 = 0;
    int fadeIdx = 0;
    for (int i = 0; i < 16; ++i) {
        d->colors[i] = 0xFF40FF00u;
        unsigned char *c = (unsigned char *)&d->colors[i];
        if (i < 8) {
            c[0] = (unsigned char)(255 - (i << 5));
        } else {
            c[3] = (unsigned char)(255 - (fadeIdx << 4));
            ++fadeIdx;
        }
    }
    vm->pos464 = *pos;
    vm->mode20 = 13;
    return 0;
}

void EiwIceGrid::ClearIceMask(const EiwVec3 *pos, const unsigned int *mask, int height, int width) {
    int shift = ((int)(pos->x + 384.0 - (width * 16))) % 32;
    int startCol = (int)((pos->x + 192.0 - width * 16.0) * 0.03125);
    int startRow = (int)(pos->y - height * 0.5);
    for (int r = 0; r < height; ++r) {
        int row = startRow + r;
        for (int c = 0; c < width; ++c) {
            int col = startCol + c;
            if (col >= 0 && col < 12 && (unsigned int)row <= 0x1BFu) {
                bits[row][col] &= ~(mask[r * width + c] >> shift);
            }
            int col2 = col + 1;
            if (col2 >= 0 && col2 < 12 && (unsigned int)row <= 0x1BFu && shift > 0) {
                bits[row][col2] &= ~(mask[r * width + c] << (32 - shift));
            }
        }
    }
}

void EiwIceGrid::SetIceMask(const EiwVec3 *pos, const unsigned int *mask, int height, int width) {
    int shift = ((int)(pos->x + 384.0 - (width * 16))) % 32;
    int startCol = (int)((pos->x + 192.0 - width * 16.0) * 0.03125);
    int startRow = (int)(pos->y - height * 0.5);
    for (int r = 0; r < height; ++r) {
        int row = startRow + r;
        for (int c = 0; c < width; ++c) {
            int col = startCol + c;
            if (col >= 0 && col < 12 && (unsigned int)row <= 0x1BFu) {
                bits[row][col] |= (mask[r * width + c] >> shift);
            }
            int col2 = col + 1;
            if (col2 >= 0 && col2 < 12 && (unsigned int)row <= 0x1BFu && shift > 0) {
                bits[row][col2] |= (mask[r * width + c] << (32 - shift));
            }
        }
    }
}

} // namespace th128
