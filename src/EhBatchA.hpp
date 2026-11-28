#pragma once

#include <windows.h>
#include <stdlib.h>

// 1. 0x00464240 (223B)
struct EbaListNode {
    void *data;
    EbaListNode *next;
};

struct EbaAnmVm {
    unsigned char pad[0x4e8];
    ~EbaAnmVm();
};

struct EbaBufHolder {
    void *buf;
    ~EbaBufHolder() {
        if (buf != 0) {
            free(buf);
            buf = 0;
        }
    }
};

struct EbaAnmMgr {
    unsigned char pad000[0xbc];
    EbaAnmVm vms1[0x1000];       // +0xbc (0x1000 * 0x4e8 = 0x4e8000 -> 0x4e80bc)
    unsigned char pad4e80bc[0x1588];
    EbaBufHolder holder4e9644;   // +0x4e9644
    unsigned char pad4e9648[0x3d00bc];
    EbaListNode *list1;          // +0x8b9704
    int pad8b9708;
    EbaListNode *list2;          // +0x8b970c
    int pad8b9710;
    EbaAnmVm vms2[0x21];         // +0x8b9714
    ~EbaAnmMgr();
};

// 2. 0x00408450 (197B)
struct EbaBulletEntry {
    unsigned char pad[0x11b8];
    EbaBulletEntry();
    ~EbaBulletEntry();
};
struct EbaBulletMgr {
    unsigned char pad00[0x64];
    EbaBulletEntry entries[0x7d1]; // +0x64 (0x7d1 * 0x11b8 = 0x8a7f38 -> 0x8a7f9c)
    unsigned char padEnd[4];       // total 0x8a7fa0
    EbaBulletMgr();
    ~EbaBulletMgr();
};

// 3. 0x00441000 (196B)
struct EbaReplaySlot {
    unsigned char pad[0x24];
    EbaReplaySlot();
    ~EbaReplaySlot();
};
struct EbaReplayObj {
    unsigned char pad00[0x138];
    EbaReplaySlot slots[0x11];   // +0x138 (0x11 * 0x24 = 0x264 -> 0x39c)
    unsigned char padEnd[0x118]; // total 0x4b4
    EbaReplayObj();
    ~EbaReplayObj();
};

// 4. 0x00428500 (196B)
struct EbaLaserEntry {
    unsigned char pad[0xa40];
    EbaLaserEntry();
    ~EbaLaserEntry();
};
struct EbaLaserMgr {
    unsigned int flags;            // +0x00
    unsigned char pad04[0x10];
    EbaLaserEntry entries[0x2cc];  // +0x14 (0x2cc * 0xa40 = 0x1cab00 -> 0x1cab14)
    unsigned char padEnd[0x10];    // total 0x1cab24
    EbaLaserMgr();
    ~EbaLaserMgr();
};

// 5. 0x00403990 (155B)
struct EbaStageObj {
    unsigned char pad[0x3960];
    EbaStageObj();
    ~EbaStageObj();
};

// 6. 0x0043aea0 (147B)
struct EbaPlayerObj {
    unsigned char pad[0x43a94];
    EbaPlayerObj();
    ~EbaPlayerObj();
};

// 7. 0x00420fd0 (146B)
struct EbaGuiObj {
    unsigned char pad[0x5a40];
    EbaGuiObj();
    ~EbaGuiObj();
};

// 8. 0x00442d90 (95B)
struct EbaScoreMgr {
    unsigned char pad[0xcc10];
    EbaScoreMgr();
};

// Globals & helpers
extern EbaBulletMgr *g_EbaBulletMgr;
extern EbaLaserMgr *g_EbaLaserMgr;
extern EbaScoreMgr *g_EbaScoreMgr;
extern int g_EbaReplayMgr;

void EbaReleaseVm(void *vm);
int EbaInitBulletMgr(EbaBulletMgr *p);
int __stdcall EbaInitReplayA(EbaReplayObj *p, int *mgr, int arg);
int __stdcall EbaInitLaserMgr(EbaLaserMgr *p);
int __stdcall EbaInitStage(EbaStageObj *p, int arg);
int __stdcall EbaInitPlayer(EbaPlayerObj *p);
int EbaInitGui();

// Target functions (8 functions, 1355B)
EbaBulletMgr *EbaCreateBulletMgr();
EbaReplayObj *__stdcall EbaCreateReplayA(int arg);
EbaLaserMgr *EbaCreateLaserMgr();
EbaStageObj *__stdcall EbaCreateStage(int arg);
EbaPlayerObj *EbaCreatePlayer();
EbaGuiObj *EbaCreateGui();
void EbaCreateScoreMgr();
