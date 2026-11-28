#include "CleanBatchC.hpp"

volatile int g_CbcSink = 0;

__declspec(noinline) CbcAnmVm *CbcAnmMgr::FindVm(int id) {
    volatile int *p = &id;
    DWORD t = GetTickCount();
    g_CbcSink += (int)this + *p + (int)t;
    return (CbcAnmVm *)worldHead;
}

__declspec(noinline) void CbcResetVm(CbcAnmVm *vm) {
    DWORD t = GetTickCount();
    g_CbcSink += (int)vm + (int)t;
}

__declspec(noinline) void CbcSetVmScript(CbcAnmVm *vm, void *file, int script) {
    DWORD t = GetTickCount();
    g_CbcSink += (int)vm + (int)file + script + (int)t;
}

__declspec(noinline) void CbcSubD0::Cleanup() {
    DWORD t = GetTickCount();
    g_CbcSink += (int)this + (int)vtable + (int)t;
}

struct CbcCopyBlob { unsigned char b[0x3c]; };
CbcCopyBlob g_CbcBlobA, g_CbcBlobB;

__declspec(noinline) void CbcChain::RegisterDraw(CbcChainElem *elem, int prio) {
    CbcChain *chain = &g_CbcChain;
    if (elem->onReg != 0) {
        g_CbcSink += (int)elem->onReg + (int)elem->arg;
        elem->onReg = 0;
    }
    DWORD t = GetTickCount();
    g_CbcSink += (int)t;
    CbcListNode *cursor = &chain->drawHead;
    elem->f00 = prio;
    while (cursor->next != 0) {
        CbcChainElem *nextElem = (CbcChainElem *)cursor->next->data;
        if (nextElem->f00 > prio) {
            break;
        }
        cursor = cursor->next;
    }
    if (cursor->next != 0) {
        elem->node.next = cursor->next;
        cursor->next->prev = &elem->node;
    }
    cursor->next = &elem->node;
    elem->node.prev = cursor;
}

__declspec(noinline) int __stdcall CbcEndingCheck(CbcEndingInner *p) {
    CbcEndingInner *volatile *pp = &p;
    DWORD t = GetTickCount();
    g_CbcSink += (int)*pp + (int)t;
    return g_CbcSink & 1;
}

__declspec(noinline) void CbcEndingStep(int *p) {
    DWORD t = GetTickCount();
    g_CbcSink += *p + (int)t;
}

__declspec(noinline) void __stdcall CbcSetVmScriptById(int id, int script) {
    volatile int *pid = &id;
    g_CbcBlobA = g_CbcBlobB;
    DWORD t = GetTickCount();
    g_CbcSink += *pid + script + (int)t + g_CbcBlobA.b[0];
}

__declspec(noinline) void CbcSoundMgr::PlaySfx(int id) {
    int a = g_CbcFreezeGauge;
    int c = g_CbcFrameSkip;
    g_CbcSink += a + c + id;
    if (id == 0) {
        g_CbcSink += GetTickCount();
    }
}

__declspec(noinline) void __stdcall CbcCloseHelper(void *p) {
    void *volatile *pp = &p;
    DWORD t = GetTickCount();
    g_CbcSink += (int)*pp + (int)t;
}

__declspec(noinline) int __fastcall CbcDraw43Cb(void *arg) {
    g_CbcSink += (int)arg;
    return 1;
}

typedef void (*CbcVoidFn)();
volatile CbcVoidFn g_CbcStartAnimPtr = &CbcStartStageAnim;
typedef void (__stdcall *CbcFreezeFn)(int);
volatile CbcFreezeFn g_CbcFreezePtr = &CbcAddFreezeGauge;
typedef void (__fastcall *CbcCutFn)(CbcChain *, CbcChainElem *);
volatile CbcCutFn g_CbcCutPtr = &CbcChainCut;

extern "C" int __stdcall CleanBatchCProbe(void **args, int mode) {
    int r = 0, outId = 0;
    g_CbcMainMgr = (CbcMainMgr *)args[8];
    OutputDebugStringA((const char *)&g_CbcFlag4d59e8);
    OutputDebugStringA((const char *)&g_CbcSupFlags);
    OutputDebugStringA((const char *)&g_CbcFrameSkip);
    if (mode & 1) ((CbcAnmVm *)args[0])->RegisterWorldTail(&outId);
    if (mode & 2) ((CbcAnmVm *)args[0])->RegisterWorldHead(&outId);
    if (mode & 4) ((CbcAnmVm *)args[0])->RegisterUiTail(&outId);
    if (mode & 8) ((CbcAnmVm *)args[0])->RegisterUiHead(&outId);
    if (mode & 16) g_CbcStartAnimPtr();
    if (mode & 32) ((CbcEffectSub *)args[1])->Cleanup();
    if (mode & 64) ((CbcTwoVmOwner *)args[2])->ClearTwoVms();
    if (mode & 128) g_CbcCutPtr((CbcChain *)args[0], (CbcChainElem *)args[3]);
    if (mode & 256) r += ((CbcEndingObj *)args[4])->OnCalc();
    if (mode & 512) ((CbcPauseSync *)args[5])->Sync();
    if (mode & 1024) r += (int)CbcReadShiftJisLine((const char *)args[6], (char *)args[7], mode);
    if (mode & 2048) g_CbcFreezePtr(mode);
    if (mode & 4096) r += ((CbcPauseObj *)args[8])->Close();
    if (mode & 8192) r += ((CbcDrawOwner *)args[9])->RegisterDraw43();
    if (mode & 16384) {
        CbcCloseHelper(args[0]);
        g_CbcChain.RegisterDraw((CbcChainElem *)args[3], mode);
        CbcSetVmScriptById(mode, mode + 1);
        g_CbcSoundMgr.PlaySfx(mode);
    }
    return r + outId + g_CbcSink;
}
