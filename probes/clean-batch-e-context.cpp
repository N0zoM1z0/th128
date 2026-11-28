#include "CleanBatchE.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

volatile int g_CbeSink = 0;

__declspec(noinline) void CbeChain::RegisterCalc(CbeChainElem *elem, int prio) {
    CbeChain *chain = &g_CbeChain;
    CbeListNode *cursor = &chain->calcHead;
    if (elem->onReg != 0) {
        g_CbeSink += (int)elem->onReg + (int)elem->arg + (int)elem->cb + (int)elem->flags;
        elem->onReg = 0;
    }
    DWORD t = GetTickCount();
    g_CbeSink += (int)t + (int)cursor + (int)elem->cb + (int)elem->arg + (int)elem->flags + (int)elem->onCut;
    elem->priority = prio;
    while (cursor->next != 0) {
        CbeChainElem *nextElem = (CbeChainElem *)cursor->next->data;
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

__declspec(noinline) void CbeChain::RegisterDraw(CbeChainElem *elem, int prio) {
    CbeChain *chain = &g_CbeChain;
    CbeListNode *cursor = &chain->drawHead;
    if (elem->onReg != 0) {
        g_CbeSink += (int)elem->onReg + (int)elem->arg + (int)elem->cb + (int)elem->flags;
        elem->onReg = 0;
    }
    DWORD t = GetTickCount();
    g_CbeSink += (int)t + (int)cursor + (int)elem->cb + (int)elem->arg + (int)elem->flags + (int)elem->onCut;
    elem->priority = prio;
    while (cursor->next != 0) {
        CbeChainElem *nextElem = (CbeChainElem *)cursor->next->data;
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

__declspec(noinline) void *CbeLoadAnm(int slot, const char *name) {
    g_CbeSink += (1 << slot);
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_CbeSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 + (int)name[0] + (int)name[1];
    g_CbeSink += (int)t1 * 7 + (int)t2 * 11 + (int)name[2];
    return (void *)((int)name + (int)t1 + (int)t2 + (int)t3);
}

__declspec(noinline) const char *__cdecl CbeLog::Print(const char *msg, ...) {
    char local[0x2000];
    va_list args;
    va_start(args, msg);
    vsprintf(local, msg, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    return msg;
}

__declspec(noinline) void CbeThreadObj::Reset() {
    DWORD t = GetTickCount();
    g_CbeSink += (int)this + (int)t + handle + tid;
    handle = 0;
    tid = 0;
}

__declspec(noinline) int __fastcall CbeCb41fb70(void *arg) { return (int)arg + 1; }
__declspec(noinline) int __fastcall CbeCb4101b0(void *arg) { return (int)arg + 2; }
__declspec(noinline) int __fastcall CbeCb4101c0(void *arg) { return (int)arg + 3; }
__declspec(noinline) int __fastcall CbeCb4120e0(void *arg) { return (int)arg + 4; }
__declspec(noinline) int __fastcall CbeCb4120f0(void *arg) { return (int)arg + 5; }
__declspec(noinline) int __fastcall CbeCb4335b0(void *arg) { return (int)arg + 6; }
__declspec(noinline) int __fastcall CbeCb4335c0(void *arg) { return (int)arg + 7; }
__declspec(noinline) unsigned int __stdcall CbeThread432f50(void *arg) { return (unsigned int)arg + 8; }
__declspec(noinline) int __fastcall CbeCb42a370(void *arg) { return (int)arg + 9; }
__declspec(noinline) int __fastcall CbeCb42a3e0(void *arg) { return (int)arg + 10; }
__declspec(noinline) int __fastcall CbeCb445c80(void *arg) { return (int)arg + 11; }
__declspec(noinline) int __fastcall CbeCb445c90(void *arg) { return (int)arg + 12; }

typedef CbeObj41f8c0 *(*CbeFn1)();
typedef int (__stdcall *CbeFn2)(CbeObj40e820 *);
typedef int (__stdcall *CbeFn3)(CbeObj411e50 *);
typedef int (__stdcall *CbeFn4)(CbeObj433110 *);
typedef int (__stdcall *CbeFn5)(CbeObj429ea0 *);
typedef int (*CbeFn6)();
typedef bool (__stdcall *CbeFn7)(int);

volatile CbeFn1 g_CbeP1 = &CbeCreate41f8c0;
volatile CbeFn2 g_CbeP2 = &CbeInit40e820;
volatile CbeFn3 g_CbeP3 = &CbeInit411e50;
volatile CbeFn4 g_CbeP4 = &CbeInit433110;
volatile CbeFn5 g_CbeP5 = &CbeInit429ea0;
volatile CbeFn6 g_CbeP6 = &CbeInit445330;
volatile CbeFn7 g_CbeP7 = &CbeInvertAlpha;

extern "C" int __stdcall CleanBatchEProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) r += (int)g_CbeP1();
    if (mode & 2) r += g_CbeP2((CbeObj40e820 *)args[0]);
    if (mode & 4) r += g_CbeP3((CbeObj411e50 *)args[1]);
    if (mode & 8) r += g_CbeP4((CbeObj433110 *)args[2]);
    if (mode & 16) r += g_CbeP5((CbeObj429ea0 *)args[3]);
    if (mode & 32) r += g_CbeP6();
    if (mode & 64) r += g_CbeP7(mode);
    if (mode & 128) ((CbeLog *)args[4])->Print((const char *)args[5]);
    return r + g_CbeSink;
}
