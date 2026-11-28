#pragma once

#include <windows.h>

// 1. 0x00434bc0 (233B)
struct CbgReplayMgr {
    unsigned char pad000[0x9a0];
    int textLen9a0;   // +0x9a0
    char *text9a4;    // +0x9a4
};

// 2. 0x00457c40 (223B)
struct CbgBgmEntry {
    char name[0x34];
};
struct CbgSoundMgr {
    unsigned char pad000[0x1984];
    CbgBgmEntry *entries; // +0x1984
};

// 3. 0x00459820 (212B)
struct CbgMapOwner {
    unsigned char pad000[0x110];
    int count110;       // +0x110
    int count114;       // +0x114
    unsigned char pad118[0x1c];
    unsigned int *map134; // +0x134
};

// 4. 0x004033d0 (185B) & 5. 0x004200e0 (184B)
struct CbgTimer {
    unsigned int flags;
    CbgTimer() { flags &= ~1U; }
};
struct CbgAnmVm {
    unsigned char pad[0x4e8];
    CbgAnmVm();
    ~CbgAnmVm();
};

struct CbgStageObj {
    unsigned int flags;          // +0x00
    unsigned char pad04[0x30];
    CbgTimer t34;                // +0x34
    unsigned char pad38[0x10];
    CbgTimer t48;                // +0x48
    unsigned char pad4c[0x44];
    CbgTimer t90;                // +0x90
    unsigned char pad94[0x48];
    CbgTimer tdc;                // +0xdc
    unsigned char pade0[0x48];
    CbgTimer t128;               // +0x128
    unsigned char pad12c[0x88];
    CbgTimer t1b4;               // +0x1b4
    unsigned char pad1b8[0x14];
    CbgAnmVm vms1cc[8];          // +0x1cc (8 * 0x4e8 = 0x2740 -> 0x290c)
    unsigned char pad290c[0x28];
    CbgAnmVm vms2934[3];         // +0x2934 (3 * 0x4e8 = 0xeb8 -> 0x37ec)
    unsigned char pad37ec[0x20];
    CbgTimer t380c;              // +0x380c
    unsigned char pad3810[0x150]; // total 0x3960
    CbgStageObj();
};

struct CbgGuiObj {
    unsigned int flags;          // +0x00
    unsigned char pad04[0x0c];
    CbgAnmVm vms10[8];           // +0x10 (0x2740 -> 0x2750)
    CbgAnmVm vms2750[8];         // +0x2750 (0x2740 -> 0x4e90)
    CbgAnmVm vm4e90;             // +0x4e90 (0x4e8 -> 0x5378)
    CbgAnmVm vm5378;             // +0x5378 (0x4e8 -> 0x5860)
    unsigned char pad5860[0x90];
    CbgTimer t58f0;              // +0x58f0
    unsigned char pad58f4[0x124];
    CbgTimer t5a18;              // +0x5a18
    unsigned char pad5a1c[0x24]; // total 0x5a40
    CbgGuiObj();
};

// Globals & helpers
extern CbgGuiObj *g_CbgGui;
extern int g_CbgMotivation;
extern int g_CbgMotivationLock;
extern int g_CbgFreezeGauge;

int __stdcall CbgFileExists(const char *path);
void *__stdcall CbgOpenFile(const char *path, int *outSize, int mode);
void CbgPlaySound(int id);
void __stdcall CbgShowPopup(CbgGuiObj *gui, int type, int a, int b);
void __stdcall CbgUpdateFreezeUi(CbgGuiObj *gui);

// Target functions (7 functions, 1339B)
int __stdcall CbgCheckReplayVersion(CbgReplayMgr *mgr, const char *ver, int expA, int expB);
int __stdcall CbgFindBgmEntry(const char *path, CbgSoundMgr *mgr);
void __stdcall CbgLoadMapFile(CbgMapOwner *owner, const char *path);
void __stdcall CbgAddMotivation(int delta);
void __stdcall CbgAddFreezeGauge(int delta);
