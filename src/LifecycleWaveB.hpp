#pragma once

#include <windows.h>

namespace th128 {

struct LwbChainElem;

struct LwbChainMgr {
    int dummy;
    void Cut(LwbChainElem *elem);
};

void LwbReleaseAnm(void *anm);
void LwbMarkResourceVmsForDelete(void *anmMgr, void *res);

struct LwbAnmMgr {
    unsigned char pad0[0x4e90c0];
    void *slots[38]; // +0x4E90C0: slot 1=+0x4E90C4, slot 31=+0x4E913C, slot 36=+0x4E9150

    void ReleaseSlotPtr(void *&slot) {
        if (slot != NULL) {
            LwbReleaseAnm(slot);
            ::operator delete(slot);
            slot = NULL;
        }
    }
};

struct LwbHelpInf {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;

    void Shutdown();
};

struct LwbIceObj {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;

    void Shutdown();
};

struct LwbBulletEntry {
    unsigned char pad0[0x11b8];
    ~LwbBulletEntry();
};

struct LwbBulletMgr {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;
    unsigned char pad10[0x64 - 0x10];
    LwbBulletEntry entries[0x7d1]; // +0x64 (0x7D1 * 0x11B8 = 0x8A7F38 -> ends at 0x8A7F9C!)
    void *anmRes8a7f9c;

    ~LwbBulletMgr();
};

struct LwbReplayEntry {
    int dummy;
    ~LwbReplayEntry();
};

struct LwbPauseOrReplay {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;
    unsigned char pad10[0x204 - 0x10];
    LwbReplayEntry *entries[0x19];

    ~LwbPauseOrReplay();
};

struct LwbScoreBoard {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;
    unsigned char pad10[0x4c4 - 0x10];
    void *buf4c4;

    ~LwbScoreBoard();
};

struct LwbItemEntry {
    unsigned char pad0[0xa40];
    ~LwbItemEntry();
};

struct LwbItemMgr {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;
    int pad10;
    LwbItemEntry items[0x2cc]; // +0x14

    ~LwbItemMgr();
};

struct LwbEffectMgr {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;
    void *anmRes10;

    ~LwbEffectMgr();
};

struct LwbSimpleChainOwner {
    unsigned char pad0[8];
    LwbChainElem *chain8;
    LwbChainElem *chainC;

    ~LwbSimpleChainOwner();
};

struct LwbSplash {
    unsigned char pad0[0x528];
    int timer528;
};

struct LwbTitleSub {
    unsigned char pad0[0xc];
    struct LwbChainElemReal {
        int priority;
        unsigned int flags;
    } *chainC;
};

extern unsigned int g_LwbGameFlags;
extern CRITICAL_SECTION g_LwbChainCs;
extern unsigned char g_LwbChainLockDepth;
extern LwbChainMgr *g_LwbChainMgr;
extern LwbAnmMgr *g_LwbAnmMgr;
extern LwbHelpInf *g_LwbHelpInf;
extern LwbIceObj *g_LwbIceObj;
extern LwbBulletMgr *g_LwbBulletMgr;
extern LwbPauseOrReplay *g_LwbPauseOrReplay;
extern LwbScoreBoard *g_LwbScoreBoard;
extern LwbItemMgr *g_LwbItemMgr;
extern LwbEffectMgr *g_LwbEffectMgr;
extern int g_LwbMode3c0;
extern LwbSplash *g_LwbSplash;
extern LwbTitleSub *g_LwbTitleSub;
extern int g_LwbFlag59e8;

void __fastcall LwbChainCutInternal(LwbChainMgr *mgr, LwbChainElem *elem);
int LwbInit445330();
int LwbTitleBootWait();

} // namespace th128
