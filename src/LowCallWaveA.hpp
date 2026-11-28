#pragma once

#include <windows.h>

namespace th128 {

struct LcwChainElem {
    int pad0;
    unsigned int flags4;
    int (__fastcall *callback8)(void *arg);
    void *padC;
    void (__fastcall *pausedCb10)(void *arg);
    int pad14;
    int pad18;
    int pad1C;
    void *arg20;
};

struct LcwChainNode {
    LcwChainElem *elem0;
    LcwChainNode *next4;
};

struct LcwChain {
    unsigned char pad0[0x18];
    LcwChainNode *calcHead18;
    unsigned char pad1c[0x48 - 0x1c];
    LcwChainNode *next48;
    int paused4c;

    int RunCalcChain();
};

struct LcwAnmVm;
struct LcwAnmNode {
    LcwAnmVm *vm0;
    LcwAnmNode *next4;
};

struct LcwAnmVm {
    unsigned int id0;
    unsigned char pad4[0x10];
    LcwAnmNode *children14;
    void *parent18;
    unsigned char pad1c[0x4b0 - 0x1c];
    unsigned int flags4b0;
};

struct LcwAnmMgr {
    unsigned char pad0[0x8b9704];
    LcwAnmNode *world8b9704;
    int pad8b9708;
    LcwAnmNode *ui8b970c;

    LcwAnmVm *FindVmInline(unsigned int id) {
        if (id == 0) return NULL;
        for (LcwAnmNode *n = world8b9704; n != NULL; n = n->next4) {
            if (n->vm0->id0 == id) return n->vm0;
        }
        for (LcwAnmNode *n = ui8b970c; n != NULL; n = n->next4) {
            if (n->vm0->id0 == id) return n->vm0;
        }
        return NULL;
    }
};

struct LcwMsgObj {
    int dummy;
    void Cleanup();
};

struct LcwGui {
    unsigned char pad0[8];
    LcwChainElem *chain8;
    unsigned char padC[0x5860 - 0xc];
    unsigned int anmIds5860[10];
    unsigned int id5888;
    unsigned int id588c;
    unsigned char pad5890[8];
    unsigned int id5898;
    unsigned int arr589c[10];
    unsigned char pad58c4[0x58fc - 0x58c4];
    int dispScore58fc;
    int scoreStep5900;
    unsigned char pad5904[0x5914 - 0x5904];
    int f5914;
    unsigned char pad5918[0x5954 - 0x5918];
    int f5954;
    unsigned char pad5958[0x5968 - 0x5958];
    int f5968;
    unsigned char pad596c[0x59a8 - 0x596c];
    int f59a8;
    unsigned char pad59ac[0x59bc - 0x59ac];
    int f59bc;
    unsigned char pad59c0[0x59fc - 0x59c0];
    int f59fc;
    int pad5a00;
    unsigned int flags5a04;
    unsigned char pad5a08[0x5a1c - 0x5a08];
    LcwMsgObj *msg5a1c;
    void *buf5a20;

    void ShowPopup(int kind, int param1, int param2);
    static void UpdateScoreDisplay();
    void PreCleanup();
};

struct LcwMenuCursor {
    int cur;
    int prev;
    int count;
    unsigned char padC[0x90 - 0xc];
    int disabled[16];
    int wraps;
    int disabledCount;

    void SetIndex(int idx) {
        if (count == 0) {
            cur = idx;
            return;
        }
        if (idx >= count) {
            idx = count - 1;
        }
        cur = idx;
    }

    int Move(int delta);
};

struct LcwSoundTest {
    unsigned char pad0[0x30];
    int state30;
    LcwMenuCursor cursor34;
    LcwMenuCursor cursor10c;

    int Update();
};

struct LcwScoreFile {
    unsigned char pad0[8];
    unsigned char routes8[8][0x66c];
    unsigned char pad3368[0x3374 - (8 + 8 * 0x66c)];
    char lastName3374[16];
};

struct LcwNameEntry {
    unsigned char pad0[0x38];
    LcwMenuCursor cursor38;
    LcwMenuCursor cursor110;
    unsigned char pad1e8[0x1f0 - 0x1e8];
    int namePos1f0;
    int flag1f4;
    int failed1f8;
    unsigned char pad1fc[0x2cc - 0x1fc];
    char name2cc[16];

    void Init();
};

struct LcwReplayStageHdr {
    int pad0;
    int frameCount4;
};

struct LcwReplayStageSlot {
    unsigned char *frameData0;
    unsigned short *curFrame4;
    unsigned char *frameEnd8;
    unsigned char *slowPtrC;
    LcwReplayStageHdr *hdr10;
    int frameIdx14;
    int pad18;
    int pad1c;
    int pad20;
};

struct LcwReplayMgr {
    unsigned char pad0[0x138];
    LcwReplayStageSlot stages138[10];
    unsigned char pad2a0[0x3a0 - 0x2a0];
    unsigned char slowRate3a0;
    unsigned char pad3a1[3];
    int totalFrames3a4;
    int pad3a8;
    int curStage3ac;

    int StepInput();
};

struct LcwInputState {
    unsigned char pad0[0x218];
    unsigned int cur218;
    unsigned int prev21c;
    unsigned int pad220;
    unsigned int ext1_224;
    unsigned int ext2_228;

    void UpdateRepeatMasks();
};

extern unsigned int g_LcwSupFlags;
extern CRITICAL_SECTION g_LcwChainCs;
extern unsigned char g_LcwChainLockDepth;
extern int g_LcwSceneNext;

extern LcwGui *g_LcwGui;
extern LcwAnmMgr *g_LcwAnmMgr;
extern int g_LcwHiScore;
extern int g_LcwScore;
extern int g_LcwDifficulty;
extern int g_LcwContinueCount;
extern int g_LcwHiScoreContinue;
extern int g_LcwExtendIdx;
struct LcwStageFlagsBits {
    unsigned int b0 : 1;
    unsigned int b1 : 1;
    unsigned int hiScore : 1;
    unsigned int b3 : 1;
};

extern LcwStageFlagsBits g_LcwStageFlagsBits;
extern unsigned int g_LcwStageFlags;
extern int g_LcwExtendNormal[16];
extern int g_LcwExtendExtra[16];
extern void *g_LcwSavedMsgBuf;

extern unsigned int g_LcwInputRepeat;
extern unsigned int g_LcwInputPressed;

extern int g_LcwCurScene;
extern int g_LcwNextScene;
extern int g_LcwRouteIdx;
extern LcwScoreFile *g_LcwScoreFile;
extern void *g_LcwActiveTable;
extern unsigned char g_LcwTable0[16];
extern unsigned char g_LcwTable1[16];

extern int g_LcwReplayActive;
extern LcwInputState g_LcwInputState;

void __fastcall LcwChainCutImpl(LcwChain *chain, LcwChainElem *elem);
void LcwGrantExtend(int *hiScorePtr);
void LcwPlaySfx(int id);
void LcwStopSfx(int id);
int __stdcall LcwInsertScoreEntry(void *routeBlock);
void LcwDeleteVmByIdPtr(unsigned int *idPtr);
void LcwOnReplayEnd();

} // namespace th128
