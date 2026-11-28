#pragma once

#include <windows.h>

namespace th128 {

struct LwaChainElem;

struct LwaChainNode {
    LwaChainElem *owner;
    LwaChainNode *next;
    LwaChainNode *prev;
};

struct LwaChainElem {
    int priority;           // +0x00
    unsigned int flags;     // +0x04
    void *cb;               // +0x08
    void *onReg;            // +0x0C
    void *onUnreg;          // +0x10
    LwaChainNode node;      // +0x14
    void *arg;              // +0x20

    LwaChainElem() {
        flags &= ~1u;
        cb = NULL;
        onReg = NULL;
        onUnreg = NULL;
        priority = 0;
        node.owner = this;
        node.next = NULL;
        node.prev = NULL;
    }
};

struct LwaChainMgr {
    int dummy;
    void Cut(LwaChainElem *elem);
};

struct LwaAnmMgr {
    unsigned char pad0[0x4e90c0];
    void *slots[38];        // +0x4E90C0 (slot 8 = +0x4E90E0, slot 37 = +0x4E9154!)

    void ReleaseSlotAt(int idx);
    void ReleaseSlotPtr(void *&slot);
};

struct LwaStageBgMini {
    int dummy;
    void Cleanup();
};

struct LwaStageBg {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    unsigned char pad10[0x3c - 0x10];
    int flag3c;
    unsigned int anm40;
    unsigned int anm44;
    unsigned int anm48;
    unsigned char pad4c[0x4f8 - 0x4c];
    void *buf4f8;
    unsigned char pad4fc[0x534 - 0x4fc];
    void *buf534;
    LwaStageBgMini *mini538;
    unsigned char pad53c[0x554 - 0x53c];
    LwaChainElem *chain554;

    ~LwaStageBg();
};

struct LwaSptRes {
    unsigned char pad4[0x0c - 4];
    void *files[32];        // +0x0C..+0x8B
    void *subs8c;           // +0x8C

    virtual void DummyVFunc() {}
    ~LwaSptRes() {
        if (subs8c != NULL) {
            free(subs8c);
            subs8c = NULL;
        }
    }
};

struct LwaEnemyMgr {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    unsigned char pad10[0xb4 - 0x10];
    LwaSptRes *resB4;

    ~LwaEnemyMgr();
};

struct LwaReplaySub {
    unsigned char pad0[0x24];
    ~LwaReplaySub();
};

struct LwaReplayMgr {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    unsigned char pad10[0x18 - 0x10];
    void *buf18;
    void *buf1c;
    void *stagePtrs[17];    // +0x20..+0x63
    unsigned char pad64[0x138 - 0x64];
    LwaReplaySub subs[17];  // +0x138 (17 * 0x24 = 0x264 -> ends at 0x39C)
    unsigned char pad39c[0x3a8 - 0x39c];
    LwaChainElem *chain3a8;

    ~LwaReplayMgr();
    void FreeStageChunks(int idx);
};

struct LwaCardInf {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    int pad10;
    unsigned int anm14;
    unsigned int anm18;
    unsigned int anm1c;

    void Shutdown();
};

struct LwaLaserNode {
    LwaLaserNode *prev;     // +0x04
    LwaLaserNode *next;     // +0x08

    virtual void V0() = 0;
    virtual void V1() = 0;
    virtual void V2() = 0;
    virtual void V3() = 0;
    virtual void V4() = 0;
    virtual void V5() = 0;
    virtual void Unlink() = 0; // +0x18 (slot 6)
};

struct LwaLaserMgr {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    unsigned char pad10[0x18 - 0x10];
    LwaLaserNode *head18;

    ~LwaLaserMgr();
};

struct LwaSupervisor {
    unsigned char pad0[0x554];
    int field554;           // 0x4D33BC - 0x4D2E68 = 0x554
    int field558;           // 0x4D33C0
    int field55c;
    int field560;           // 0x4D33C8
};

struct LwaBulletMgr {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    void *ptr10;
    unsigned char pad14[0x64 - 0x14];
    unsigned char pool64[0x8a780e - 0x64];
    short count8a780e;
    unsigned char pad8a7810[0x8a7f9c - 0x8a7810];
    void *anm8a7f9c;

    int Init();
};

struct LwaThreadInf {
    void *vtbl0;
    void *handle4;
    unsigned int threadId8;
    int fieldC;
    int running10;
    int field14;
    unsigned int (__stdcall *proc18)(void *);

    void Stop();
};

struct LwaLoading {
    unsigned char pad0[8];
    LwaChainElem *chain8;
    LwaChainElem *chainC;
    LwaThreadInf thread10;

    static int __stdcall Init(LwaLoading *self);
};

struct LwaErrorCtx {
    int dummy;
    void __cdecl Log(const char *fmt, ...);
};

extern unsigned int g_LwaGameFlags;
extern CRITICAL_SECTION g_LwaChainCs;
extern unsigned char g_LwaChainLockDepth;
extern LwaChainMgr *g_LwaChainMgr;
extern LwaAnmMgr *g_LwaAnmMgr;
extern LwaSupervisor g_LwaSupervisor;
extern int g_LwaStageA;
extern int g_LwaStageB;
extern LwaStageBg *g_LwaStageBg;
extern LwaEnemyMgr *g_LwaEnemyMgr;
extern LwaReplayMgr *g_LwaReplayMgr;
extern LwaCardInf *g_LwaCardInf;
extern LwaLaserMgr *g_LwaLaserMgr;
extern LwaErrorCtx g_LwaErrorCtx;

void __fastcall LwaChainCutInternal(LwaChainMgr *mgr, LwaChainElem *elem);
int LwaRegisterCalc(LwaChainElem *elem, int prio);
int LwaRegisterDraw(LwaChainElem *elem, int prio);
void LwaReleaseAnm(void *anm);
void LwaDeleteVmByIdPtr(unsigned int *idPtr);
void LwaAddGraze(int amount);
void __stdcall LwaEnemyPreCleanup(LwaEnemyMgr *mgr);
void *LwaPreloadAnm(int slot, const char *name);

int LwaSupCb0(); int LwaSupCb1(); int LwaSupCb2(); int LwaSupCb3();
int LwaSupCb4(); int LwaSupCb5(); int LwaSupCb6(); int LwaSupCb7();
int LwaSupCb8(); int LwaSupCb9();
int LwaBulletCalc(); int LwaBulletDraw();
int LwaLoadCalc(); int LwaLoadDraw();
unsigned int __stdcall LwaLoadThreadProc(void *arg);

int LwaRegisterSupervisorChains();

} // namespace th128
