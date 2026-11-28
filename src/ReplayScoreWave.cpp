#include "ReplayScoreWave.hpp"

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

int RswReplayMgr::PlaybackStep() {
    if (g_RswReplayActive == NULL) {
        return 1;
    }
    if (subs[curStage3ac].frame14 < 0) {
        g_RswInput.curInput218 = 0;
        g_RswInput.auxA224 = 0;
        g_RswInput.auxB228 = 0;
        return 1;
    }
    if (curStage3ac >= 0) {
        g_RswInput.prevInput21c = g_RswInput.curInput218;
        if (subs[curStage3ac].frame14 < subs[curStage3ac].hdr10->frameCount4) {
            unsigned short *p = (unsigned short *)subs[curStage3ac].inputPtr4;
            if (p[0] == 0xFFFF && p[1] == 0xFFFF && p[2] == 0xFFFF) {
                g_RswInput.curInput218 = 0;
                g_RswInput.auxA224 = 0;
                g_RswInput.auxB228 = 0;
                RswReplayEnd();
                return 1;
            }
            g_RswInput.curInput218 = p[0];
            g_RswInput.auxA224 = *(unsigned short *)(subs[curStage3ac].inputPtr4 + 2);
            g_RswInput.auxB228 = *(unsigned short *)(subs[curStage3ac].inputPtr4 + 4);
            g_RswInput.UpdateRepeatMasks();
            curFps3a0 = *subs[curStage3ac].fpsPtrC;
            subs[curStage3ac].inputPtr4 += 6;
            if (totalFrames3a4 % 30 == 0) {
                ++subs[curStage3ac].fpsPtrC;
            }
        } else {
            g_RswInput.curInput218 = 0;
            g_RswInput.auxA224 = 0;
            g_RswInput.auxB228 = 0;
        }
        ++subs[curStage3ac].frame14;
        ++totalFrames3a4;
        return 1;
    }
    g_RswInput.curInput218 = 0;
    g_RswInput.auxA224 = 0;
    g_RswInput.auxB228 = 0;
    ++totalFrames3a4;
    return 1;
}

static inline unsigned short RswNextRngU16() {
    if (g_RswGameFlags & 0x8000) {
        EnterCriticalSection(&g_RswRngCs);
        ++g_RswRngLockDepth;
    }
    unsigned int x = g_RswRngSeed;
    ++g_RswRngCount;
    x = (x ^ 0x9630u) - 0x6553u;
    unsigned short v = (unsigned short)x;
    v = (unsigned short)((v >> 14) + (v << 2));
    g_RswRngSeed = v;
    if (g_RswGameFlags & 0x8000) {
        LeaveCriticalSection(&g_RswRngCs);
        --g_RswRngLockDepth;
    }
    return g_RswRngSeed;
}

void RswStatusBlock::Init() {
    memset(this, 0, sizeof(RswStatusBlock));
    magicVer0 = 0x00025453u;
    size8 = sizeof(RswStatusBlock);
    strcpy(nameC, "        ");
    for (int i = 0; i < 0x1ee; ++i) {
        rngPad50[i] = RswNextRngU16();
    }
}

RswScoreFile::RswScoreFile() {
    memset(this, 0, sizeof(RswScoreFile));
    _chdir(g_RswAppDataDir);
    DWORD sz;
    rawBuf0 = (RswScoreHeader *)RswLoadFile("scoreth128.dat", &sz, 1);
    _chdir(g_RswGameDir);
    status3368.Init();
    RswInitCardBlock(&cards3794);
    for (RswRouteBlock *p = routes8; (unsigned int)(p - routes8) < 8; ++p) {
        RswInitRouteBlock(p);
    }
    LoadAndParse();
}

int RswScoreFile::LoadAndParse() {
    if (rawBuf0 != NULL) {
        if (rawBuf0->magic0 == 0x31323854u && rawBuf0->version8 == 0) {
            RswDecryptBlocks((unsigned char *)(rawBuf0 + 1), rawBuf0->packedSize10, 0xAC, 0x35, 0x10, rawBuf0->packedSize10);
            unpackedBuf4 = (unsigned char *)malloc(rawBuf0->unpackedSize14 * 4);
            RswDecompressLzss((unsigned char *)(rawBuf0 + 1), rawBuf0->packedSize10, unpackedBuf4, rawBuf0->unpackedSize14);
            int remaining = (int)rawBuf0->unpackedSize14;
            unsigned char *cur = unpackedBuf4;
            while (remaining > 0) {
                RswChunkHeader *hdr = (RswChunkHeader *)cur;
                unsigned short tag = hdr->tag0;
                if (tag == 0x5243) {
                    if (hdr->ver2 == 3 &&
                        hdr->Checksum(sizeof(RswRouteBlock)) == hdr->checksum4 &&
                        hdr->size8 == sizeof(RswRouteBlock)) {
                        int idx = hdr->idxC;
                        routes8[idx] = *(RswRouteBlock *)cur;
                    }
                } else if (tag == 0x4443) {
                    if (hdr->ver2 == 1 &&
                        hdr->Checksum(sizeof(RswCardBlock)) == hdr->checksum4 &&
                        hdr->size8 == sizeof(RswCardBlock)) {
                        memcpy(&cards3794, cur, sizeof(RswCardBlock));
                    }
                } else if (tag == 0x5453) {
                    if (hdr->ver2 == 2 &&
                        hdr->Checksum(sizeof(RswStatusBlock)) == hdr->checksum4 &&
                        hdr->size8 == sizeof(RswStatusBlock)) {
                        status3368 = *(RswStatusBlock *)cur;
                    }
                } else {
                    break;
                }
                int chunkSize = (int)hdr->size8;
                remaining -= chunkSize;
                if (remaining < 0) {
                    break;
                }
                cur += chunkSize;
            }
            return 0;
        }
        free(rawBuf0);
        rawBuf0 = NULL;
    }
    rawBuf0 = (RswScoreHeader *)malloc(sizeof(RswScoreHeader));
    memset(rawBuf0, 0, sizeof(RswScoreHeader));
    rawBuf0->magic0 = 0x31323854u;
    rawBuf0->version8 = 0;
    rawBuf0->fieldC = 0x100;
    return 0;
}

int RswSoundTest::Update() {
    switch (state30) {
    case 0:
        mainCursor34.SetCountReset(2);
        sfxCursor10c.SetCountReset(0x4C);
        state30 = 1;
        // fallthrough
    case 1:
        mainCursor34.prev = mainCursor34.cur;
        if ((g_RswInputPressed & 0x20) || (g_RswInputHeld & 0x20)) {
            mainCursor34.Move(1);
        }
        if ((g_RswInputPressed & 0x10) || (g_RswInputHeld & 0x10)) {
            mainCursor34.Move(-1);
        }
        switch (mainCursor34.cur) {
        case 0:
            sfxCursor10c.prev = sfxCursor10c.cur;
            if ((g_RswInputPressed & 0x80) || (g_RswInputHeld & 0x80)) {
                sfxCursor10c.Move(1);
            }
            if ((g_RswInputPressed & 0x40) || (g_RswInputHeld & 0x40)) {
                sfxCursor10c.Move(-1);
            }
            if (g_RswInputPressed & 0x80201) {
                RswPlaySfx(sfxCursor10c.cur);
            }
            if (g_RswInputPressed & 0x102) {
                RswStopSfx(sfxCursor10c.cur);
            }
            break;
        case 1:
            if (g_RswInputPressed & 0x80201) {
                g_RswSupMode3c0 = (((~(g_RswGameFlags >> 13)) & 1) | 2);
                return 1;
            }
            break;
        }
        break;
    }
    return 1;
}

int RswLoadSelectedEcl() {
    RswEclViewer *v = g_RswEclViewer;
    char path[256];
    sprintf(path, "%s", v->names34[v->selectedIdx114]);
    if (v->oldSpt2dc != NULL) {
        v->oldSpt2dc->DeleteSelf(1);
        v->oldSpt2dc = NULL;
    }
    v->oldSpt2dc = NULL;
    if (v->fileBuf2e0 != NULL) {
        free(v->fileBuf2e0);
        v->fileBuf2e0 = NULL;
    }
    v->fileBuf2e0 = RswLoadFile(path, NULL, 0);
    v->spt2d8 = new RswSptConcrete();
    v->spt2d8->Load(v->fileBuf2e0);
    v->cursor1ec.SetCount(v->spt2d8->count8);
    v->state30 = 1;
    return 0;
}

} // namespace th128
