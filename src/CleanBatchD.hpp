#pragma once

#include <windows.h>

struct CbdListNode {
    void *data;
    CbdListNode *next;
    CbdListNode *prev;
};

struct CbdChainElem {
    int priority;       // +0x00
    unsigned int flags; // +0x04
    void *cb;           // +0x08
    void *onReg;        // +0x0c
    void *onCut;        // +0x10
    CbdListNode node;   // +0x14
    void *arg;          // +0x20 (total 0x24)

    CbdChainElem() {
        flags &= ~1U;
        cb = 0;
        onReg = 0;
        onCut = 0;
        priority = 0;
        node.data = this;
        node.next = 0;
        node.prev = 0;
    }
};

struct CbdChain {
    unsigned char pad00[0x14];
    CbdListNode calcHead; // +0x14
    unsigned char pad20[0x18];
    CbdListNode drawHead; // +0x38

    void RegisterCalc(CbdChainElem *elem, int prio);
    void RegisterDraw(CbdChainElem *elem, int prio);
};

struct CbdChainOwner {
    int pad00[2];
    CbdChainElem *calcElem; // +0x08
    CbdChainElem *drawElem; // +0x0c
};

// 6 & 7. 0x0041fc80 (207B) & 0x0041fd50 (208B)
typedef void (__fastcall *CbdVmCb)(void *vm, int val);
struct CbdAnmVm {
    unsigned char pad000[0x3f8];
    short state3f8;    // +0x3f8
    unsigned char pad3fa[0xce];
    CbdVmCb cb;        // +0x4c8
    unsigned char pad4cc[0x1c]; // total 0x4e8
};

struct CbdMeterOwner {
    unsigned char pad000[0x10];
    CbdAnmVm vmsA[8];  // +0x10 .. +0x274f (8 * 0x4e8 = 0x2740)
    CbdAnmVm vmsB[8];  // +0x2750 .. +0x4e8f
};

// 8. 0x00458f30 (181B)
struct CbdBgmPlayer {
    unsigned char pad00[0x30];
    int active30;      // +0x30
    unsigned char pad34[0x44];
    int busy78;        // +0x78
};

// 9. 0x00445280 (167B)
struct CbdLoadingObj {
    unsigned char pad000[0x528];
    int frames528;
};
struct CbdAnmSlot {
    int dummy;
    ~CbdAnmSlot();
};
struct CbdAnmMgr {
    unsigned char pad000[0x4e90c4];
    CbdAnmSlot *loadingSlot; // +0x4e90c4
};

// Globals & callbacks
extern CbdChain g_CbdChain;
extern CbdBgmPlayer *g_CbdBgm;
extern HANDLE g_CbdBgmEvent;
extern unsigned int g_CbdSupFlags;
extern int g_CbdFrameSkip;
extern CbdLoadingObj *g_CbdLoading;
extern CbdAnmMgr *g_CbdAnmMgr;
extern CbdChainOwner *g_CbdTitleObj;
extern int g_CbdFlag4d59e8;

int __fastcall CbdCb407790(void *arg);
int __fastcall CbdCb4077e0(void *arg);
int __fastcall CbdCb411bb0(void *arg);
int __fastcall CbdCb411bc0(void *arg);
int __fastcall CbdCb4438f0(void *arg);
int __fastcall CbdCb443900(void *arg);
int __fastcall CbdCb444c90(void *arg);
int __fastcall CbdCb444ca0(void *arg);
int __fastcall CbdCb429360(void *arg);
int __fastcall CbdCb4293a0(void *arg);

void __stdcall CbdBgmFillBuffer(CbdBgmPlayer *bgm);
int CbdTitleCheck();

// Target functions (9 functions, 1679B)
int __stdcall CbdReg407490(CbdChainOwner *obj);
int __stdcall CbdReg411040(CbdChainOwner *obj);
int __stdcall CbdReg443430(CbdChainOwner *obj);
int __stdcall CbdReg444240(CbdChainOwner *obj);
int __stdcall CbdReg428360(CbdChainOwner *obj);
void __stdcall CbdUpdateMeterA(CbdMeterOwner *obj, int fullCount, int partial);
void __stdcall CbdUpdateMeterB(CbdMeterOwner *obj, int fullCount, int partial);
DWORD __stdcall CbdBgmThreadProc(void *param);
int CbdTitleInitWait();
