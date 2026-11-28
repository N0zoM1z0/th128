#include "CleanBatchG.hpp"

volatile int g_CbgSink = 0;

__declspec(noinline) CbgAnmVm::CbgAnmVm() {
    DWORD t = GetTickCount();
    g_CbgSink += (int)this + (int)t;
}
__declspec(noinline) CbgAnmVm::~CbgAnmVm() {
    DWORD t = GetTickCount();
    g_CbgSink += (int)this + (int)t;
}

__declspec(noinline) int __stdcall CbgFileExists(const char *path) {
    const char *volatile *pp = &path;
    DWORD t = GetTickCount();
    g_CbgSink += (int)*pp + (int)t;
    return g_CbgSink & 1;
}

__declspec(noinline) void *__stdcall CbgOpenFile(const char *path, int *outSize, int mode) {
    const char *volatile *pp = &path;
    int *volatile *po = &outSize;
    int volatile *pm = &mode;
    DWORD t = GetTickCount();
    g_CbgSink += (int)*pp + (int)*po + *pm + (int)t;
    return (void *)g_CbgSink;
}

__declspec(noinline) void CbgPlaySound(int id) {
    g_CbgSink += id * 3;
    DWORD t = GetTickCount();
    g_CbgSink += (int)t;
}

__declspec(noinline) void __stdcall CbgShowPopup(CbgGuiObj *gui, int type, int a, int b) {
    int volatile *pa = &a;
    int volatile *pb = &b;
    DWORD t1 = GetTickCount();
    g_CbgSink += (int)t1 + (int)gui + type + *pa + *pb;
    DWORD t2 = GetTickCount();
    g_CbgSink += (int)t1 + (int)t2 + (int)gui;
}

__declspec(noinline) void __stdcall CbgUpdateFreezeUi(CbgGuiObj *gui) {
    CbgGuiObj *volatile *pg = &gui;
    DWORD t = GetTickCount();
    g_CbgSink += (int)*pg + (int)t;
}

typedef void (__stdcall *CbgAddFn)(int);
volatile CbgAddFn g_CbgAdd1 = &CbgAddMotivation;
volatile CbgAddFn g_CbgAdd2 = &CbgAddFreezeGauge;

extern "C" int __stdcall CleanBatchGProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) r += CbgCheckReplayVersion((CbgReplayMgr *)args[0], (const char *)args[1], mode, mode + 1);
    if (mode & 2) r += CbgFindBgmEntry((const char *)args[1], (CbgSoundMgr *)args[2]);
    if (mode & 4) CbgLoadMapFile((CbgMapOwner *)args[3], (const char *)args[1]);
    if (mode & 8) { CbgStageObj *s = new CbgStageObj(); r += (int)s; }
    if (mode & 16) { CbgGuiObj *g = new CbgGuiObj(); r += (int)g; }
    if (mode & 32) g_CbgAdd1(mode);
    if (mode & 64) g_CbgAdd2(mode);
    if (mode & 128) CbgOpenFile((const char *)args[1], (int *)args[4], mode);
    if (mode & 256) CbgPlaySound(mode);
    if (mode & 512) CbgShowPopup((CbgGuiObj *)args[5], mode, mode + 1, mode + 2);
    if (mode & 1024) CbgUpdateFreezeUi((CbgGuiObj *)args[5]);
    return r + g_CbgSink;
}
