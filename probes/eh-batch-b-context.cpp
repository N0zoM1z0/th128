#include "EhBatchB.hpp"

volatile int g_EbbSink = 0;

__declspec(noinline) void __fastcall EbbChainCutInternal(EbbChain *chain, EbbChainElem *elem) {
    g_EbbSink += (int)elem * 3 + (1 << ((int)chain & 7));
    DWORD t = GetTickCount();
    g_EbbSink += (int)t + (int)elem;
}

typedef void (__fastcall *EbbCutFn)(EbbChain *, EbbChainElem *);
volatile EbbCutFn g_EbbCutPtr = &EbbChainCutInternal;

__declspec(noinline) EbbAnmSlot::~EbbAnmSlot() {
    DWORD t1 = GetTickCount();
    g_EbbSink += (int)t1 + (int)this;
    DWORD t2 = GetTickCount();
    g_EbbSink += (int)t1 * 3 + (int)t2 + (int)this;
}

__declspec(noinline) void EbbAnmMgr::KeepSlot(void *anm) {
    g_EbbSink += (int)anm * 3;
    DWORD t = GetTickCount();
    g_EbbSink += (int)this + (int)t;
}

__declspec(noinline) EbbAnmVm::~EbbAnmVm() {
    g_EbbSink += (int)this + (int)::operator new(4);
}

__declspec(noinline) void EbbThreadBase::Reset() {
    DWORD t = GetTickCount();
    g_EbbSink += (int)this + (int)t;
    DWORD t2 = GetTickCount();
    g_EbbSink += (int)this + (int)t2;
}

__declspec(noinline) EbbSub3818::~EbbSub3818() {
    DWORD t = GetTickCount();
    g_EbbSink += (int)this + (int)t;
    DWORD t2 = GetTickCount();
    g_EbbSink += (int)this + (int)t2;
}

__declspec(noinline) EbbExtGui::~EbbExtGui() {
    g_EbbGui = 0;
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbbSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4;
    g_EbbSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbbExtPlayer::~EbbExtPlayer() {
    g_EbbExtPlayer = 0;
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbbSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4;
    g_EbbSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbbExtMain::~EbbExtMain() {
    g_EbbMain = 0;
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount(); DWORD t3 = GetTickCount(); DWORD t4 = GetTickCount();
    g_EbbSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 * 7 + (int)t4;
    g_EbbSink += (int)t1 + (int)t2 + (int)t3 + (int)this;
}
__declspec(noinline) EbbExtBullet::~EbbExtBullet() { g_EbbSink += (int)this + dummy; g_EbbSink += (int)GetTickCount(); }
__declspec(noinline) EbbExtEnemy::~EbbExtEnemy() { g_EbbSink += (int)this + dummy; g_EbbSink += (int)GetTickCount(); }
__declspec(noinline) EbbExtLaser::~EbbExtLaser() { g_EbbSink += (int)this + dummy; g_EbbSink += (int)GetTickCount(); }
__declspec(noinline) EbbExt4b8a90::~EbbExt4b8a90() {
    DWORD t1 = GetTickCount(); DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_EbbSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 + (int)this + dummy;
    g_EbbSink += (int)t1 * 7 + (int)t2 * 11 + (int)this;
}

__declspec(noinline) void EbbPreDtor444150(EbbTitleSub *self) {
    g_EbbSink += (int)self;
    DWORD t = GetTickCount();
    g_EbbSink += (int)self + (int)t;
}
__declspec(noinline) void EbbPreDtor410dc0(EbbGameMgr *self) {
    g_EbbSink += (int)self;
    DWORD t = GetTickCount();
    g_EbbSink += (int)self + (int)t;
}
__declspec(noinline) void EbbHelper432ee0() {
    DWORD t = GetTickCount();
    g_EbbSink += (int)t;
}
__declspec(noinline) void EbbHelper443040() {
    DWORD t = GetTickCount();
    g_EbbSink += (int)t;
}

extern "C" int __stdcall EhBatchBProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) delete (EbbStageObj *)args[0];
    if (mode & 2) delete (EbbPlayerObj *)args[1];
    if (mode & 4) delete (EbbTitleSub *)args[2];
    if (mode & 8) delete (EbbGameMgr *)args[3];
    if (mode & 16) delete (EbbLoadingMgr *)args[4];
    if (mode & 32) ((EbbStageObj *)args[0])->~EbbStageObj();
    if (mode & 64) ((EbbPlayerObj *)args[1])->~EbbPlayerObj();
    if (mode & 128) ((EbbTitleSub *)args[2])->~EbbTitleSub();
    if (mode & 256) ((EbbGameMgr *)args[3])->~EbbGameMgr();
    if (mode & 512) ((EbbLoadingMgr *)args[4])->~EbbLoadingMgr();
    if (mode & 1024) {
        g_EbbCutPtr((EbbChain *)args[5], (EbbChainElem *)args[6]);
        ((EbbAnmMgr *)args[5])->KeepSlot(args[6]);
    }
    return r + g_EbbSink;
}
