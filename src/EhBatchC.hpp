#pragma once

#include <windows.h>
#include <stdlib.h>

struct EbcChainElem {
    int dummy;
};

struct EbcChain {
    int dummy;
    inline void Cut(EbcChainElem *elem);
};

void __fastcall EbcChainCutInternal(EbcChain *chain, EbcChainElem *elem);

struct EbcAnmSlot {
    int dummy;
    ~EbcAnmSlot();
};

struct EbcListNode {
    void *data;
    EbcListNode *next;
};

typedef void (__fastcall *EbcVmCb)(void *vm, int val);

struct EbcAnmVmNode {
    unsigned int id;           // +0x00
    unsigned char pad04[0x10];
    EbcListNode *children;     // +0x14
    int flag18;                // +0x18
    unsigned char pad1c[0x3dc];
    short state3f8;            // +0x3f8
    unsigned char pad3fa[0xce];
    EbcVmCb cb;                // +0x4c8
};

struct EbcAnmMgr {
    unsigned char pad000[0x4e90c0];
    EbcAnmSlot *slots[0x26]; // +0x4e90c0 (slot 0=+0x4e90c0, 2=+0x4e90c8, 0x18=+0x4e9120, 0x19=+0x4e9124)
    unsigned char pad4e9158[0x3d05ac];
    EbcListNode *list1;      // +0x8b9704
    int pad8b9708;
    EbcListNode *list2;      // +0x8b970c
};

struct EbcAnmVmInline {
    unsigned char pad000[0x4ac];
    void *buf4ac; // +0x4ac
    unsigned char pad4b0[0x38]; // total 0x4e8
    ~EbcAnmVmInline() {
        if (buf4ac != 0) {
            free(buf4ac);
        }
        buf4ac = 0;
    }
};

struct EbcThreadBase {
    virtual void V0() {}
    void Reset();
    ~EbcThreadBase() {
        Reset();
    }
};

struct EbcReplayItem {
    int dummy;
    ~EbcReplayItem();
};

// 1. 0x004012a0 (583B)
struct EbcMainMgr {
    virtual void V0() {}
    int pad04[2];
    EbcChainElem *elem0c;      // +0x0c
    EbcChainElem *elem10;      // +0x10
    EbcAnmVmInline vm14;       // +0x14 (buf at +0x4c0)
    EbcAnmVmInline vm4fc;      // +0x4fc (buf at +0x9a8)
    unsigned char pad9e4[0x1bebc];
    EbcChainElem *elem1c8a0;   // +0x1c8a0
    EbcChainElem *elem1c8a4;   // +0x1c8a4
    ~EbcMainMgr();
};

// 2. 0x004434f0 (259B)
struct EbcScoreMgr {
    int pad00[2];
    EbcChainElem *calcElem;    // +0x08
    EbcChainElem *drawElem;    // +0x0c
    EbcThreadBase thread10;    // +0x10
    ~EbcScoreMgr();
};

// 3. 0x00445450 (609B)
struct EbcTitleObj {
    virtual void V0() {}
    int pad04[2];
    EbcChainElem *calcElem;    // +0x0c
    EbcChainElem *drawElem;    // +0x10
    unsigned char pad14[0x60c];
    unsigned int vmId620;      // +0x620
    unsigned char pad624[0x5410];
    EbcReplayItem *replays[100]; // +0x5a34 (100 * 4 = 0x190 -> 0x5bc4)
    void *buf5bc4;             // +0x5bc4
    int pad5bc8[2];
    EbcThreadBase thread5bd0;  // +0x5bd0
    ~EbcTitleObj();
};

// Globals & helpers
extern EbcChain *g_EbcChain;
extern unsigned int g_EbcSupFlags;
extern CRITICAL_SECTION g_EbcCs;
extern unsigned char g_EbcCsCount;
extern EbcAnmMgr *g_EbcAnmMgr;
extern EbcMainMgr *g_EbcMainMgr;
extern EbcScoreMgr *g_EbcScoreMgr;
extern EbcTitleObj *g_EbcTitleObj;

void EbcStopBgm(int arg);
