#pragma once

#include <windows.h>

namespace th128 {

struct RswReplayStageHeader {
    unsigned int field0;
    int frameCount4;
};

struct RswReplaySub {
    unsigned int field0;
    unsigned char *inputPtr4;           // +0x04 (+0x13C in mgr)
    unsigned char pad8[8];
    RswReplayStageHeader *hdr10;        // +0x10 (+0x148 in mgr)
    int frame14;                        // +0x14 (+0x14C in mgr)
    unsigned char *fpsPtr18;            // +0x18 (+0x150? Wait: 0x51 * 4 = 0x144!)
};

struct RswReplaySubExact {
    unsigned int field0;                // +0x00 (+0x138)
    unsigned char *inputPtr4;           // +0x04 (+0x13C)
    unsigned int field8;                // +0x08 (+0x140)
    unsigned char *fpsPtrC;             // +0x0C (+0x144 = 0x51*4!)
    RswReplayStageHeader *hdr10;        // +0x10 (+0x148)
    int frame14;                        // +0x14 (+0x14C)
    unsigned char pad18[0x24 - 0x18];
};

struct RswReplayMgr {
    unsigned char pad0[0x138];
    RswReplaySubExact subs[17];         // +0x138 (stride 0x24)
    unsigned char pad39c[4];
    unsigned char curFps3a0;            // +0x3A0
    unsigned char pad3a1[3];
    int totalFrames3a4;                 // +0x3A4
    int pad3a8;
    int curStage3ac;                    // +0x3AC

    int PlaybackStep();
};

struct RswInputState {
    unsigned char pad0[0x218];
    unsigned int curInput218;           // 0x4DB210 - 0x4DAFF8 = 0x218
    unsigned int prevInput21c;          // 0x4DB214
    unsigned int pad220;
    unsigned int auxA224;               // 0x4DB21C
    unsigned int auxB228;               // 0x4DB220

    void UpdateRepeatMasks();
};

struct RswStatusBlock {
    unsigned int magicVer0;             // +0x00 (0x00025453 = "ST\x02\x00")
    unsigned int checksum4;             // +0x04
    unsigned int size8;                 // +0x08 (0x42C)
    char nameC[0x44];                   // +0x0C ("        ")
    unsigned short rngPad50[0x1ee];     // +0x50..+0x42B

    void Init();
};

struct RswRouteBlock {
    unsigned char data[0x66c];
};

struct RswCardBlock {
    unsigned char data[0x947c];
};

struct RswScoreHeader {
    unsigned int magic0;                // "T821" = 0x31323854
    unsigned int checksum4;
    unsigned short version8;            // 0
    unsigned short padA;
    unsigned int fieldC;                // 0x100
    unsigned int packedSize10;
    unsigned int unpackedSize14;
};

struct RswScoreFile {
    RswScoreHeader *rawBuf0;            // +0x00
    unsigned char *unpackedBuf4;        // +0x04
    RswRouteBlock routes8[8];           // +0x08..+0x3367 (8 * 0x66C = 0x3360)
    RswStatusBlock status3368;          // +0x3368..+0x3793 (0x42C)
    RswCardBlock cards3794;             // +0x3794..+0xCC0F (0x947C)

    RswScoreFile();
    int LoadAndParse();
};

struct RswChunkHeader {
    unsigned short tag0;
    unsigned short ver2;
    unsigned int checksum4;
    unsigned int size8;
    int idxC;

    unsigned int Checksum(int size) const;
};

struct RswMenuCursor {
    int cur;                            // +0x00
    int prev;                           // +0x04
    int count;                          // +0x08
    unsigned char padC[0xd0 - 0x0c];
    int fieldD0;                        // +0xD0

    void SetCountReset(int n) {
        if (count == 0) {
            cur = 0;
        } else if (count > 0) {
            cur = 0;
        } else {
            cur = count - 1;
        }
        count = n;
        fieldD0 = 1;
    }

    void SetCount(int n) {
        count = n;
        cur = (n == 0 || n > 0) ? 0 : (n - 1);
    }

    void Move(int delta);
};

struct RswSoundTest {
    unsigned char pad0[0x30];
    int state30;                        // +0x30
    RswMenuCursor mainCursor34;         // +0x34 (count at +0x3C, fieldD0 at +0x104)
    unsigned char pad108[4];
    RswMenuCursor sfxCursor10c;         // +0x10C (count at +0x114, fieldD0 at +0x1DC)

    int Update();
};

struct RswSptBase {
    unsigned char pad4[0x04];
    int count8;                         // +0x08
    unsigned char padC[0x1090 - 0x0c];
    int f1090;
    int f1094;

    virtual void Load(void *buf) = 0;   // +0x00
    virtual void V1() = 0;
    virtual void V2() = 0;
    virtual void V3() = 0;
    virtual void V4() = 0;
    virtual void DeleteSelf(int flags) = 0; // +0x14
};

struct RswSptConcrete : public RswSptBase {
    RswSptConcrete() {
        f1090 = 0;
        f1094 = 0;
        memset(this, 0, sizeof(RswSptConcrete));
    }
    virtual void Load(void *buf) {}
    virtual void V1() {}
    virtual void V2() {}
    virtual void V3() {}
    virtual void V4() {}
    virtual void DeleteSelf(int flags) {}
};

struct RswEclViewer {
    unsigned char pad0[0x30];
    int state30;                        // +0x30
    const char **names34;               // +0x34
    unsigned char pad38[0x114 - 0x38];
    int selectedIdx114;                 // +0x114
    unsigned char pad118[0x1ec - 0x118];
    RswMenuCursor cursor1ec;            // +0x1EC (cur=+0x1EC, prev=+0x1F0, count=+0x1F4)
    unsigned char pad2c0[0x2d8 - (0x1ec + sizeof(RswMenuCursor))];
    RswSptBase *spt2d8;                 // +0x2D8
    RswSptBase *oldSpt2dc;              // +0x2DC
    void *fileBuf2e0;                   // +0x2E0
};

extern void *g_RswReplayActive;
extern RswInputState g_RswInput;
extern unsigned int g_RswInputHeld;
extern unsigned int g_RswInputPressed;
extern unsigned int g_RswGameFlags;
extern CRITICAL_SECTION g_RswRngCs;
extern unsigned char g_RswRngLockDepth;
extern unsigned short g_RswRngSeed;
extern unsigned int g_RswRngCount;
extern char g_RswAppDataDir[0x1000];
extern char g_RswGameDir[0x1000];
extern int g_RswSupMode3c0;
extern RswEclViewer *g_RswEclViewer;

void RswReplayEnd();
void *__stdcall RswLoadFile(const char *path, DWORD *outSize, int fromDisk);
void RswInitCardBlock(RswCardBlock *cards);
void RswInitRouteBlock(RswRouteBlock *route);
void __stdcall RswDecryptBlocks(unsigned char *buf, int size, unsigned char key, unsigned char step, int block, int limit);
void __stdcall RswDecompressLzss(unsigned char *src, int srcSize, unsigned char *dst, int dstSize);
void RswPlaySfx(int id);
void RswStopSfx(int id);
int RswLoadSelectedEcl();

} // namespace th128
