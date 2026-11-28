#pragma once

#include <windows.h>
#include <stdlib.h>

namespace th128 {

struct EbfChainElem {
    int pad0;
};

struct EbfChainMgr {
    void Cut(EbfChainElem* elem);
};

void __fastcall EbfChainCutInternal(EbfChainMgr* chain, EbfChainElem* elem);

struct EbfAnmVm {
    unsigned char pad0[0x4e8];
    ~EbfAnmVm();
};

struct EbfAnmVmInline {
    unsigned char pad0[0x4ac];
    void* buf4ac;
    unsigned char pad4b0[0x4e8 - 0x4b0];

    ~EbfAnmVmInline() {
        if (buf4ac != 0) {
            free(buf4ac);
        }
        buf4ac = 0;
    }
};

struct EbfAnmListNode {
    void* vm;
    EbfAnmListNode* next;
};

struct EbfAnmMgr {
    unsigned char pad0[0xbc];
    EbfAnmVm vmsA[0x1000];
    unsigned char pad4e80bc[0x4e90dc - 0x4e80bc];
    void* anmSlot;
    unsigned char pad4e90e0[0x4e9198 - 0x4e90e0];
    EbfAnmVmInline vmB;
    unsigned char pad4e9680[0x8b9704 - 0x4e9680];
    EbfAnmListNode* listA;
    int pad8b9708;
    EbfAnmListNode* listB;
    int pad8b9710;
    EbfAnmVm vmsC[0x21];

    ~EbfAnmMgr();
    void ReleaseSlot();
    void KeepPlayerAnm(int id);
};

struct EbfPlayer {
    unsigned char pad0[8];
    EbfChainElem* chain8;
    EbfChainElem* chainC;
    int field10;
    EbfAnmVmInline vm0;
    EbfAnmVmInline vm1;
    EbfAnmVmInline vm2;
    unsigned char padEcc[0xf7c - 0xecc];
    void* bufF7c;

    ~EbfPlayer();
};

extern EbfChainMgr* g_EbfChainMgr;
extern EbfAnmMgr* g_EbfAnmMgr;
extern EbfPlayer* g_EbfPlayer;
extern int g_EbfLockFlags;
extern CRITICAL_SECTION g_EbfChainCs;
extern unsigned char g_EbfChainLockCount;
extern unsigned char g_EbfStageFlags;
extern void* g_EbfSavedBuf;

void EbfReleaseAnm(void* anm);
void EbfDestroyVm(void* vm);

} // namespace th128
