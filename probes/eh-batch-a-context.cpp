#include "EhBatchA.hpp"

volatile int g_EbaSink = 0;

__declspec(noinline) EbaAnmVm::~EbaAnmVm() { g_EbaSink += (int)this + (int)::operator new(4); }
__declspec(noinline) EbaBulletEntry::EbaBulletEntry() { g_EbaSink += (int)this + (int)::operator new(4); }
__declspec(noinline) EbaBulletEntry::~EbaBulletEntry() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaBulletMgr::~EbaBulletMgr() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaReplaySlot::EbaReplaySlot() { g_EbaSink += (int)this + (int)::operator new(4); }
__declspec(noinline) EbaReplaySlot::~EbaReplaySlot() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaReplayObj::~EbaReplayObj() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaLaserEntry::EbaLaserEntry() { g_EbaSink += (int)this + (int)::operator new(4); }
__declspec(noinline) EbaLaserEntry::~EbaLaserEntry() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaLaserMgr::~EbaLaserMgr() { g_EbaSink += (int)this + (int)GetTickCount(); }
__declspec(noinline) EbaStageObj::EbaStageObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaStageObj::~EbaStageObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaPlayerObj::EbaPlayerObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaPlayerObj::~EbaPlayerObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaGuiObj::EbaGuiObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaGuiObj::~EbaGuiObj() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4 + (int)::operator new(4);
    g_EbaSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbaScoreMgr::EbaScoreMgr() {
    DWORD t1 = GetTickCount();
    g_EbaSink += (int)t1 + (int)::operator new(4) + (int)this;
    DWORD t2 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 + (int)this;
}

__declspec(noinline) void EbaReleaseVm(void *vm) {
    DWORD t = GetTickCount();
    g_EbaSink += (int)vm + (int)t;
}

__declspec(noinline) int EbaInitBulletMgr(EbaBulletMgr *p) {
    DWORD t1 = GetTickCount();
    g_EbaSink += (int)t1 + (int)p;
    DWORD t2 = GetTickCount();
    g_EbaSink += (int)t1 * 3 + (int)t2 + (int)p;
    return g_EbaSink & 1;
}

__declspec(noinline) int __stdcall EbaInitReplayA(EbaReplayObj *p, int *mgr, int arg) {
    EbaReplayObj *volatile *pp = &p;
    g_EbaSink += arg * 3 + (1 << ((int)mgr)) + (int)*pp;
    DWORD t = GetTickCount();
    g_EbaSink += (int)t + (int)*pp;
    return g_EbaSink & 1;
}

__declspec(noinline) int __stdcall EbaInitLaserMgr(EbaLaserMgr *p) {
    EbaLaserMgr *volatile *pp = &p;
    DWORD t = GetTickCount();
    g_EbaSink += (int)*pp + (int)t;
    return g_EbaSink & 1;
}

__declspec(noinline) int __stdcall EbaInitStage(EbaStageObj *p, int arg) {
    EbaStageObj *volatile *pp = &p;
    int volatile *pa = &arg;
    DWORD t = GetTickCount();
    g_EbaSink += (int)*pp + *pa + (int)t;
    return g_EbaSink & 1;
}

__declspec(noinline) int __stdcall EbaInitPlayer(EbaPlayerObj *p) {
    EbaPlayerObj *volatile *pp = &p;
    DWORD t = GetTickCount();
    g_EbaSink += (int)*pp + (int)t;
    return g_EbaSink & 1;
}

__declspec(noinline) int EbaInitGui() {
    DWORD t = GetTickCount();
    g_EbaSink += (int)t;
    return g_EbaSink & 1;
}

typedef EbaBulletMgr *(*EbaFn2)();
typedef EbaReplayObj *(__stdcall *EbaFn3)(int);
typedef EbaLaserMgr *(*EbaFn4)();
typedef EbaStageObj *(__stdcall *EbaFn5)(int);
typedef EbaPlayerObj *(*EbaFn6)();
typedef EbaGuiObj *(*EbaFn7)();
typedef void (*EbaFn8)();

volatile EbaFn2 g_EbaP2 = &EbaCreateBulletMgr;
volatile EbaFn3 g_EbaP3 = &EbaCreateReplayA;
volatile EbaFn4 g_EbaP4 = &EbaCreateLaserMgr;
volatile EbaFn5 g_EbaP5 = &EbaCreateStage;
volatile EbaFn6 g_EbaP6 = &EbaCreatePlayer;
volatile EbaFn7 g_EbaP7 = &EbaCreateGui;
volatile EbaFn8 g_EbaP8 = &EbaCreateScoreMgr;

extern "C" int __stdcall EhBatchAProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) delete (EbaAnmMgr *)args[0];
    if (mode & 2) r += (int)g_EbaP2();
    if (mode & 4) r += (int)g_EbaP3(mode);
    if (mode & 8) r += (int)g_EbaP4();
    if (mode & 16) r += (int)g_EbaP5(mode);
    if (mode & 32) r += (int)g_EbaP6();
    if (mode & 64) r += (int)g_EbaP7();
    if (mode & 128) g_EbaP8();
    if (mode & 256) EbaInitReplayA((EbaReplayObj *)args[1], (int *)args[2], mode);
    if (mode & 512) { EbaStageObj *s = (EbaStageObj *)args[3]; s->~EbaStageObj(); }
    if (mode & 1024) { EbaPlayerObj *p = (EbaPlayerObj *)args[4]; p->~EbaPlayerObj(); }
    if (mode & 2048) { EbaGuiObj *g = (EbaGuiObj *)args[5]; g->~EbaGuiObj(); }
    return r + g_EbaSink;
}
