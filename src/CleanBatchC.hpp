#pragma once

#include <windows.h>

struct CbcListNode {
    void *data;
    CbcListNode *next;
    CbcListNode *prev;
};

struct CbcAnmVm {
    int id;               // +0x00
    CbcListNode listNode; // +0x04 (+0x04 data, +0x08 next, +0x0c prev)
    int pad10;            // +0x10
    CbcListNode *childHead; // +0x14
    int parentFlag;       // +0x18
    unsigned char pad01c[0x494];
    unsigned int flags;   // +0x4b0
    unsigned char pad4b4[0x1c];
    short field4d0;       // +0x4d0
    unsigned char pad4d2[0x16]; // total 0x4e8

    void RegisterWorldTail(int *outId);
    void RegisterWorldHead(int *outId);
    void RegisterUiTail(int *outId);
    void RegisterUiHead(int *outId);
};

struct CbcAnmMgr {
    unsigned char pad000[0x8b9704];
    CbcListNode *worldHead; // +0x8b9704
    CbcListNode *worldTail; // +0x8b9708
    CbcListNode *uiHead;    // +0x8b970c
    CbcListNode *uiTail;    // +0x8b9710
    unsigned char pad8b9714[0xa1e8];
    int nextId;             // +0x8c38fc

    CbcAnmVm *FindVm(int id);
};

// 5. 0x00406350 (241B)
struct CbcChainElem {
    int f00;
    unsigned int flags; // +0x04
    void *cb;           // +0x08
    void *onReg;        // +0x0c
    void *onCut;        // +0x10
    CbcListNode node;   // +0x14 (+0x14 data, +0x18 next, +0x1c prev)
    void *arg;          // +0x20 (total 0x24)

    CbcChainElem() {
        flags &= ~1U;
        cb = 0;
        onReg = 0;
        onCut = 0;
        f00 = 0;
        node.data = this;
        node.next = 0;
        node.prev = 0;
    }
};

struct CbcAnimSubEntry {
    short valid;   // +0x00
    short stride;  // +0x02
    short script;  // +0x04
    short vmIdx;   // +0x06
};

struct CbcAnimEntry {
    unsigned char pad00[3];
    unsigned char flag3; // +0x03
    unsigned char pad04[0x18];
    CbcAnimSubEntry firstSub; // +0x1c
};

struct CbcStageAnimMgr {
    int pad00[2];
    CbcChainElem *ce8;     // +0x08
    CbcChainElem *ceC;     // +0x0c
    short *hdrCount;       // +0x10
    CbcAnimEntry **entries; // +0x14
    int pad18;             // +0x18
    int val1c;             // +0x1c
    unsigned char pad20[0x2c];
    int val4c;             // +0x4c
    unsigned char pad50[0x174];
    void *anmFile;         // +0x1c4
    CbcAnmVm *vms;         // +0x1c8
    unsigned char pad1cc[0x3670];
    CbcChainElem *ce383c;  // +0x383c
};

// 6. 0x00412fc0 (178B)
struct CbcSubD0 {
    void *vtable;
    void Cleanup();
};
struct CbcEffectSub {
    unsigned char pad00[0x40];
    int anmIds[5]; // +0x40
    unsigned char pad54[0x7c];
    CbcSubD0 subD0; // +0xd0

    void Cleanup();
};

// 7. 0x004062c0 (134B)
struct CbcTwoVmOwner {
    unsigned char pad0000[0x2910];
    int field2910; // +0x2910
    unsigned char pad2914[0xf20];
    int anm3834;   // +0x3834
    int anm3838;   // +0x3838

    void ClearTwoVms();
};

// 8. 0x004686d0 (125B)
struct CbcChain {
    unsigned char pad00[0x14];
    CbcListNode calcHead; // +0x14..+0x1f
    unsigned char pad20[0x18];
    CbcListNode drawHead; // +0x38..+0x43
    int pad44;            // +0x44
    CbcListNode *curNode; // +0x48

    void Cut(CbcChainElem *elem);
    void RegisterDraw(CbcChainElem *elem, int prio);
};

// 9. 0x00413650 (116B)
struct CbcEndingInner {
    int pad00;
    int sub04;
    unsigned char pad08[0x6c];
    unsigned int flags74; // +0x74
};
struct CbcEndingObj {
    unsigned char pad00[0x18];
    CbcEndingInner *inner; // +0x18
    int pad1c;
    unsigned int flags20;  // +0x20
    int frame24;           // +0x24

    int OnCalc();
};

// 10. 0x0043df20 (108B)
struct CbcGamePtr {
    unsigned char pad0000[0x43a88];
    int paused; // +0x43a88
};
struct CbcPauseSync {
    unsigned char pad00[0x54];
    int a54, a58; // +0x54, +0x58
    int b5c, b60; // +0x5c, +0x60
    int pad64, pad68;
    int c6c, c70; // +0x6c, +0x70
    unsigned char pad74[0x3c];
    int anmB0;    // +0xb0
    unsigned char padb4[0x18];
    int stateCC;  // +0xcc

    void Sync();
};

// 13. 0x004334b0 (95B)
struct CbcMainMgr {
    int pad00[3];
    CbcChainElem *ceC;     // +0x0c
    CbcChainElem *ce10;    // +0x10
    unsigned char pad14[0x1c88c];
    CbcChainElem *ce1c8a0; // +0x1c8a0
    CbcChainElem *ce1c8a4; // +0x1c8a4
};
struct CbcPauseObj {
    unsigned int flags;
    int Close();
};

// 14. 0x0041f800 (86B)
struct CbcDrawOwner {
    int pad00[3];
    CbcChainElem *drawElem; // +0x0c
    int RegisterDraw43();
};

// Globals & externs
extern CbcAnmMgr *g_CbcAnmMgr;
extern CbcStageAnimMgr *g_CbcStageAnimMgr;
extern unsigned int g_CbcInputFlags;
extern unsigned int g_CbcSupFlags;
extern int g_CbcFrameSkip;
extern CbcGamePtr *g_CbcGamePtr;
extern int g_CbcFreezeGauge;
extern CbcMainMgr *g_CbcMainMgr;
extern int g_CbcFlag4d59e8;
extern int g_CbcGlobal4d2e68;
extern void *g_CbcSubD0Vtbl;
extern CbcChain g_CbcChain;

void CbcStartStageAnim();
void __fastcall CbcChainCut(CbcChain *chain, CbcChainElem *elem);
void CbcResetVm(CbcAnmVm *vm);
void CbcSetVmScript(CbcAnmVm *vm, void *file, int script);
int __stdcall CbcEndingCheck(CbcEndingInner *p);
void CbcEndingStep(int *p);
void __stdcall CbcSetVmScriptById(int id, int script);
char *CbcReadShiftJisLine(const char *src, char *dst, int maxLen);
struct CbcSoundMgr {
    int dummy;
    void PlaySfx(int id);
};
extern CbcSoundMgr g_CbcSoundMgr;
void __stdcall CbcAddFreezeGauge(int delta);
void __stdcall CbcCloseHelper(void *p);
int __fastcall CbcDraw43Cb(void *arg);
