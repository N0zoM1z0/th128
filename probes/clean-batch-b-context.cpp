#include "CleanBatchB.hpp"
#include <new>

volatile int g_CbbSink = 0;

__declspec(noinline) void CbbEnemyItem::Destroy(int flags) {
    g_CbbSink += flags;
}

__declspec(noinline) void *__stdcall CbbOpenFile(const char *path, int *outSize, int isBinary) {
    const char *volatile *p1 = &path;
    int *volatile *p2 = &outSize;
    volatile int *p3 = &isBinary;
    DWORD t = GetTickCount();
    g_CbbSink += (int)*p1 + (int)*p2 + *p3 + (int)t;
    return (void *)g_CbbSink;
}

__declspec(noinline) CbbSub2c8::CbbSub2c8() {
    DWORD t = GetTickCount();
    x = g_CbbSink + (int)t;
}

__declspec(noinline) void CbbPlaySfx(int id) {
    DWORD t = GetTickCount();
    g_CbbSink += id + (int)t;
}

__declspec(noinline) void CbbGui::ShowBanner(int type, int a, int b) {
    volatile int *pa = &a;
    volatile int *pb = &b;
    DWORD t = GetTickCount();
    switch (type) {
    case 0: g_CbbSink += dummy; break;
    case 1: g_CbbSink += dummy + 1; break;
    default: g_CbbSink += type + dummy; break;
    }
    g_CbbSink += (int)t + (int)this + *pa + *pb;
}

__declspec(noinline) CbbAnmVm *CbbAnmMgr::FindVm(int id) {
    volatile int *p = &id;
    DWORD t = GetTickCount();
    g_CbbSink += (int)this + *p + (int)t;
    return (CbbAnmVm *)worldHead;
}

typedef void (__stdcall *CbbCleanFn)(CbbEnemyMgr *);
volatile CbbCleanFn g_CbbCleanPtr = &CbbCleanupEnemies;
typedef void (__stdcall *CbbAddMotFn)(int);
volatile CbbAddMotFn g_CbbAddMotPtr = &CbbAddMotivation;

extern "C" int __stdcall CleanBatchBProbe(void **args, int mode) {
    int r = 0;
    DWORD got = 0;
    if (mode & 1) g_CbbCleanPtr((CbbEnemyMgr *)args[0]);
    if (mode & 2) r += (int)((CbbWaveFile *)args[1])->Read((unsigned char *)args[2], (DWORD)mode, &got);
    if (mode & 4) r += ((CbbStageLoader *)args[3])->Load((const char *)args[4]);
    if (mode & 8) r += (int)(new (args[5]) CbbEndingMgr());
    if (mode & 16) g_CbbAddMotPtr(mode);
    if (mode & 32) r += (int)(new (args[6]) CbbScoreMgr());
    if (mode & 64) r += (int)((CbbReplayMgr *)args[7])->AllocStageBlock(mode & 15);
    if (mode & 128) ((CbbSubObj *)args[8])->Init();
    if (mode & 256) r += (int)(new (args[9]) CbbReplayMenu());
    if (mode & 512) ((CbbTitleScreen *)args[10])->ClearChildFlag(mode & 7, mode >> 3);
    if (mode & 1024) ((CbbTitleScreen *)args[10])->SetChildFlag(mode);
    if (mode & 2048) r += (int)CbbOpenFile((const char *)args[4], &r, mode);
    return r + (int)got + g_CbbSink;
}
