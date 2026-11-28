#include "CleanBatchD.hpp"

volatile int g_CbdSink = 0;

__declspec(noinline) void CbdChain::RegisterCalc(CbdChainElem *elem, int prio) {
    CbdChain *chain = &g_CbdChain;
    CbdListNode *cursor = &chain->calcHead;
    if (elem->onReg != 0) {
        g_CbdSink += (int)elem->onReg + (int)elem->arg + (int)elem->cb + (int)elem->flags;
        elem->onReg = 0;
    }
    DWORD t = GetTickCount();
    g_CbdSink += (int)t + (int)cursor + (int)elem->cb + (int)elem->arg + (int)elem->flags + (int)elem->onCut;
    elem->priority = prio;
    while (cursor->next != 0) {
        CbdChainElem *nextElem = (CbdChainElem *)cursor->next->data;
        if (nextElem->priority > prio) {
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

__declspec(noinline) void CbdChain::RegisterDraw(CbdChainElem *elem, int prio) {
    CbdChain *chain = &g_CbdChain;
    CbdListNode *cursor = &chain->drawHead;
    if (elem->onReg != 0) {
        g_CbdSink += (int)elem->onReg + (int)elem->arg + (int)elem->cb + (int)elem->flags;
        elem->onReg = 0;
    }
    DWORD t = GetTickCount();
    g_CbdSink += (int)t + (int)cursor + (int)elem->cb + (int)elem->arg + (int)elem->flags + (int)elem->onCut;
    elem->priority = prio;
    while (cursor->next != 0) {
        CbdChainElem *nextElem = (CbdChainElem *)cursor->next->data;
        if (nextElem->priority > prio) {
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

__declspec(noinline) int __fastcall CbdCb407790(void *arg) { return (int)arg + 1; }
__declspec(noinline) int __fastcall CbdCb4077e0(void *arg) { return (int)arg + 2; }
__declspec(noinline) int __fastcall CbdCb411bb0(void *arg) { return (int)arg + 3; }
__declspec(noinline) int __fastcall CbdCb411bc0(void *arg) { return (int)arg + 4; }
__declspec(noinline) int __fastcall CbdCb4438f0(void *arg) { return (int)arg + 5; }
__declspec(noinline) int __fastcall CbdCb443900(void *arg) { return (int)arg + 6; }
__declspec(noinline) int __fastcall CbdCb444c90(void *arg) { return (int)arg + 7; }
__declspec(noinline) int __fastcall CbdCb444ca0(void *arg) { return (int)arg + 8; }
__declspec(noinline) int __fastcall CbdCb429360(void *arg) { return (int)arg + 9; }
__declspec(noinline) int __fastcall CbdCb4293a0(void *arg) { return (int)arg + 10; }

__declspec(noinline) void __stdcall CbdBgmFillBuffer(CbdBgmPlayer *bgm) {
    CbdBgmPlayer *volatile *p = &bgm;
    DWORD t = GetTickCount();
    g_CbdSink += (int)*p + (int)t;
}

__declspec(noinline) int CbdTitleCheck() {
    DWORD t = GetTickCount();
    g_CbdSink += (int)t;
    return g_CbdSink & 1;
}

__declspec(noinline) CbdAnmSlot::~CbdAnmSlot() {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_CbdSink += (int)t1 + (int)t2 + (int)this + dummy;
}

typedef int (__stdcall *CbdRegFn)(CbdChainOwner *);
volatile CbdRegFn g_CbdRegPtrs[5] = {
    &CbdReg407490, &CbdReg411040, &CbdReg443430, &CbdReg444240, &CbdReg428360
};
typedef void (__stdcall *CbdMeterFn)(CbdMeterOwner *, int, int);
volatile CbdMeterFn g_CbdMeterPtrs[2] = { &CbdUpdateMeterA, &CbdUpdateMeterB };
typedef DWORD (__stdcall *CbdThreadFn)(void *);
volatile CbdThreadFn g_CbdThreadPtr = &CbdBgmThreadProc;
typedef int (*CbdWaitFn)();
volatile CbdWaitFn g_CbdWaitPtr = &CbdTitleInitWait;

extern "C" int __stdcall CleanBatchDProbe(void **args, int mode) {
    int r = 0;
    OutputDebugStringA((const char *)&g_CbdBgm);
    if (mode & 1) r += g_CbdRegPtrs[mode & 3]((CbdChainOwner *)args[0]);
    if (mode & 2) r += g_CbdRegPtrs[4]((CbdChainOwner *)args[0]);
    if (mode & 4) g_CbdMeterPtrs[mode & 1]((CbdMeterOwner *)args[1], mode, mode + 1);
    if (mode & 8) r += (int)g_CbdThreadPtr(args[2]);
    if (mode & 16) r += g_CbdWaitPtr();
    if (mode & 32) CbdBgmFillBuffer((CbdBgmPlayer *)args[2]);
    if (mode & 64) { CbdAnmSlot *s = (CbdAnmSlot *)args[3]; s->~CbdAnmSlot(); }
    return r + g_CbdSink;
}
