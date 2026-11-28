#pragma once

#include <windows.h>
#include <stdlib.h>

struct EbbChainElem {
    int dummy;
};

struct EbbChain {
    int dummy;
    inline void Cut(EbbChainElem *elem);
};

void __fastcall EbbChainCutInternal(EbbChain *chain, EbbChainElem *elem);

struct EbbBufHolder {
    void *buf;
    ~EbbBufHolder() {
        if (buf != 0) {
            free(buf);
            buf = 0;
        }
    }
};

struct EbbSup {
    unsigned int flags; // +0x00 (at 0x4d33f8)
    CRITICAL_SECTION cs; // at 0x4d3678
    unsigned char lockCount; // at 0x4d3798
};

struct EbbAnmSlot {
    int dummy;
    ~EbbAnmSlot();
};

struct EbbAnmMgr {
    unsigned char pad000[0x4e90c0];
    EbbAnmSlot *slots[0x26]; // +0x4e90c0 (slot 0 = +0x4e90c0, 1 = +0x4e90c4, ..., 5 = +0x4e90d4, 6 = +0x4e90d8, 7 = +0x4e90dc)
    void KeepSlot(void *anm);
};

struct EbbAnmVm {
    unsigned char pad[0x4e8];
    ~EbbAnmVm();
};

struct EbbAnmVmInline {
    unsigned char pad000[0x4ac];
    void *buf4ac; // +0x4ac
    unsigned char pad4b0[0x38]; // total 0x4e8
    ~EbbAnmVmInline() {
        if (buf4ac != 0) {
            free(buf4ac);
            buf4ac = 0;
        }
    }
};

struct EbbThreadBase {
    virtual void V0() {}
    void Reset();
    ~EbbThreadBase() {
        Reset();
    }
};

struct EbbSub3818 {
    int dummy;
    ~EbbSub3818();
};

struct EbbSub2d8 {
    virtual void V0() {}
    unsigned char pad04[0x88];
    void *buf8c; // +0x8c
    ~EbbSub2d8() {
        if (buf8c != 0) {
            free(buf8c);
            buf8c = 0;
        }
    }
};

struct EbbVObj2dc {
    virtual void V0() {}
    virtual void V1() {}
    virtual void V2() {}
    virtual void V3() {}
    virtual void V4() {}
    virtual ~EbbVObj2dc() {} // vtable +0x14
};

struct EbbScoreMgr {
    void *buf00;
    void *buf04;
    ~EbbScoreMgr() {
        if (buf00 != 0) {
            free(buf00);
            buf00 = 0;
        }
        if (buf04 != 0) {
            free(buf04);
            buf04 = 0;
        }
    }
};

struct EbbExtGui { int dummy; ~EbbExtGui(); };
struct EbbExtPlayer { int dummy; ~EbbExtPlayer(); };
struct EbbExtBullet { int dummy; ~EbbExtBullet(); };
struct EbbExtEnemy { int dummy; ~EbbExtEnemy(); };
struct EbbExtLaser { int dummy; ~EbbExtLaser(); };
struct EbbExt4b8a90 { int dummy; ~EbbExt4b8a90(); };
struct EbbExtMain { int dummy; ~EbbExtMain(); };

// 1. 0x00403710 (601B)
struct EbbStageObj {
    int pad00[2];
    EbbChainElem *calcElem;  // +0x08
    EbbChainElem *drawElem;  // +0x0c
    void *buf10;             // +0x10
    unsigned char pad14[0x1b4];
    void *buf1c8;            // +0x1c8
    EbbAnmVm vms1cc[8];      // +0x1cc (0x2740 -> 0x290c)
    unsigned char pad290c[0x28];
    EbbAnmVm vms2934[3];     // +0x2934 (0xeb8 -> 0x37ec)
    unsigned char pad37ec[0x24];
    unsigned int flags3810;  // +0x3810
    int pad3814;
    EbbSub3818 *sub3818;     // +0x3818
    unsigned char pad381c[0x20];
    EbbChainElem *elem383c;  // +0x383c
    void *buf3840;           // +0x3840
    ~EbbStageObj();
};

// 2. 0x0043acd0 (463B)
struct EbbPlayerObj {
    int pad00[2];
    EbbChainElem *calcElem;  // +0x08
    EbbChainElem *drawElem;  // +0x0c
    void *anm10;             // +0x10
    EbbAnmVmInline vm14;     // +0x14 (buf at +0x4c0)
    EbbAnmVmInline vm4fc;    // +0x4fc (buf at +0x9a8)
    EbbAnmVmInline vm9e4;    // +0x9e4 (buf at +0xe90)
    unsigned char padecC[0xb0];
    void *buf_f7c;           // +0xf7c
    ~EbbPlayerObj();
};

// 3. 0x00444300 (470B)
struct EbbTitleSub {
    int pad00[2];
    EbbChainElem *calcElem;  // +0x08
    EbbChainElem *drawElem;  // +0x0c
    EbbThreadBase thread10;  // +0x10
    unsigned char pad14[0x2c4];
    EbbSub2d8 *obj2d8;       // +0x2d8
    EbbVObj2dc *vobj2dc;     // +0x2dc
    void *buf2e0;            // +0x2e0
    ~EbbTitleSub();
};

// 4. 0x00411100 (437B)
struct EbbGameMgr {
    int pad00[2];
    EbbChainElem *calcElem;  // +0x08
    EbbChainElem *drawElem;  // +0x0c
    EbbThreadBase thread10;  // +0x10
    unsigned char pad14[0x760];
    EbbBufHolder buf774;     // +0x774
    ~EbbGameMgr();
};

// 5. 0x00433210 (478B)
struct EbbLoadingMgr {
    int pad00[2];
    EbbChainElem *calcElem;  // +0x08
    EbbChainElem *drawElem;  // +0x0c
    EbbThreadBase thread10;  // +0x10
    unsigned char pad14[0x4c8];
    EbbBufHolder buf4dc;     // +0x4dc
    ~EbbLoadingMgr();
};

// Globals & helpers
extern EbbChain *g_EbbChain;
extern unsigned int g_EbbSupFlags;
extern CRITICAL_SECTION g_EbbCs;
extern unsigned char g_EbbCsCount;
extern EbbAnmMgr *g_EbbAnmMgr;
extern unsigned int g_EbbFlags4b4d50;
extern EbbStageObj *g_EbbStageA;
extern EbbStageObj *g_EbbStageB;
extern EbbPlayerObj *g_EbbPlayer;
extern void *g_EbbSaved4d2e28;
extern EbbTitleSub *g_EbbTitleSub;
extern EbbGameMgr *g_EbbGameMgr;
extern EbbLoadingMgr *g_EbbLoadingMgr;
extern EbbExtGui *g_EbbGui;
extern EbbExtPlayer *g_EbbExtPlayer;
extern EbbExtBullet *g_EbbBullet;
extern EbbExtEnemy *g_EbbEnemy;
extern EbbExtLaser *g_EbbLaser;
extern EbbExt4b8a90 *g_Ebb4b8a90;
extern EbbExtMain *g_EbbMain;
extern EbbScoreMgr *g_EbbScoreMgr;

void EbbPreDtor444150(EbbTitleSub *self);
void EbbPreDtor410dc0(EbbGameMgr *self);
void EbbHelper432ee0();
void EbbHelper443040();
