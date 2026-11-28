#pragma once

#include <windows.h>

namespace th128 {

struct CsaVmNode {
    struct CsaAnmVm *vm;
    CsaVmNode *next;
    CsaVmNode *prev;
};

struct CsaAnmVm {
    int id;
    CsaVmNode node;
    CsaVmNode childNode;
    unsigned char pad1c[0x402];
    short scriptId;
    unsigned char pad420[0x08];
    int *resIdPtr;
    unsigned char pad42c[0x84];
    unsigned int flags;
    unsigned char pad4b4[0x14];
    void (__fastcall *interruptCb)(CsaAnmVm *vm, int interrupt);
    unsigned char pad4cc[0x1c];
};

struct CsaAnmVm2 {
    unsigned char pad00[0x3f8];
    short pendingInterrupt;
    unsigned char pad3fa[0x2e];
    int *resIdPtr;
    unsigned char pad42c[0x9c];
    void (__fastcall *interruptCb)(CsaAnmVm2 *vm, int interrupt);
    unsigned char pad4cc[0x1c];
};

struct CsaAnmRes {
    unsigned char pad00[0x120];
    void *texArray;
};

struct CsaAnmManager {
    unsigned char pad00[0x4e90c0];
    CsaAnmRes *slots[64];
    unsigned char pad4e91c0[0x3d0544];
    CsaVmNode *worldHead;
    CsaVmNode *worldTail;
    CsaVmNode *uiHead;
    CsaVmNode *uiTail;
    unsigned char pad8b9714[0xa1e8];
    int nextId;

    CsaAnmVm *GetVmById(int id);
};

struct CsaThreadInf {
    unsigned char pad00[0x04];
    HANDLE handle;
    unsigned int threadId;
    int f0c;
    int f10;
    unsigned char pad14[0x04];
    void *proc;

    void Stop();
};

struct CsaScoreData {
    unsigned char pad00[0x338e];
    unsigned char bgmUnlocked[32];
};

void __stdcall CsaEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *path);

struct CsaMemFile {
    void *vtable;
    unsigned int size;
    unsigned char *cur;
    unsigned char *base;

    bool Seek(int offset, int origin);
};

struct CsaCardObj {
    unsigned char pad00[0x2c8];
    int vmIds[32];
    unsigned char pad348[0x2c8];
    int mainVmId;

    void ShowVm(int scriptId);
    void HideVmBySlot(int slot, int scriptId);
};

extern CsaAnmManager *g_CsaAnmMgr;
extern unsigned int g_CsaFlags;
extern CRITICAL_SECTION g_CsaThreadCs;
extern unsigned char g_CsaThreadCsDepth;
extern CsaThreadInf g_CsaLoaderThread;
extern CsaScoreData *g_CsaScoreData;
extern int g_CsaSoundMgr;

void CsaRegisterWorldVmAtTail(CsaAnmVm *vm, int *outId);
void CsaRegisterUiVmAtTail(CsaAnmVm *vm, int *outId);
void CsaRegisterWorldVmAtHead(CsaAnmVm *vm, int *outId);
void CsaRegisterUiVmAtHead(CsaAnmVm *vm, int *outId);
int CsaStartLoaderThread(unsigned (__stdcall *proc)(void *));
int CsaPlayStageBgm(const char *path, int bgmIdx);
int CsaPlayBgmWithLoop(int arg, const char *path);
void CsaSetInterruptAll(CsaAnmVm2 *vms, int count, short interrupt);

} // namespace th128
