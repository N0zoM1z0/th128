#include "../src/LifecycleWaveA.hpp"

namespace th128 {

unsigned int g_LwaGameFlags = 0;
CRITICAL_SECTION g_LwaChainCs;
unsigned char g_LwaChainLockDepth = 0;
LwaChainMgr *g_LwaChainMgr = NULL;
LwaAnmMgr *g_LwaAnmMgr = NULL;
LwaSupervisor g_LwaSupervisor;
int g_LwaStageA = 0;
int g_LwaStageB = 0;
LwaStageBg *g_LwaStageBg = NULL;
LwaEnemyMgr *g_LwaEnemyMgr = NULL;
LwaReplayMgr *g_LwaReplayMgr = NULL;
LwaCardInf *g_LwaCardInf = NULL;
LwaLaserMgr *g_LwaLaserMgr = NULL;
LwaErrorCtx g_LwaErrorCtx;

static volatile int g_Sink = 0;

__declspec(noinline) int LwaRegisterCalc(LwaChainElem *elem, int prio) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += (int)elem + prio + (int)t1;
    g_Sink += (int)elem * 3 + prio * 5 + (int)(t2 + t3);
    return g_Sink;
}

__declspec(noinline) int LwaRegisterDraw(LwaChainElem *elem, int prio) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += (int)elem + prio + (int)t1;
    g_Sink += (int)elem * 3 + prio * 5 + (int)(t2 + t3);
    return g_Sink;
}

__declspec(noinline) void LwaReleaseAnm(void *anm) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1 + (int)anm;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)anm;
}

__declspec(noinline) void LwaDeleteVmByIdPtr(unsigned int *idPtr) {
    if (*idPtr == 0) return;
    DWORD t = GetTickCount();
    g_Sink += (int)*idPtr + (int)t;
    *idPtr = 0;
}

__declspec(noinline) void LwaAddGraze(int amount) {
    DWORD t = GetTickCount();
    g_Sink += amount + (int)t;
}

__declspec(noinline) void LwaStageBgMini::Cleanup() {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += dummy + (int)(t1 + t2);
}

__declspec(noinline) LwaReplaySub::~LwaReplaySub() {
    g_Sink += (int)this;
}

__declspec(noinline) void LwaReplayMgr::FreeStageChunks(int idx) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + idx + (int)t;
}

__declspec(noinline) void *LwaPreloadAnm(int slot, const char *name) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += slot + (int)name + (int)(t1 + t2);
    return (void *)g_Sink;
}

__declspec(noinline) void LwaThreadInf::Stop() {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += (int)this + (int)(t1 + t2);
}

__declspec(noinline) void __cdecl LwaErrorCtx::Log(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)fmt + (int)t;
}

__declspec(noinline) int LwaSupCb0() { return g_Sink; }
__declspec(noinline) int LwaSupCb1() { return g_Sink + 1; }
__declspec(noinline) int LwaSupCb2() { return g_Sink + 2; }
__declspec(noinline) int LwaSupCb3() { return g_Sink + 3; }
__declspec(noinline) int LwaSupCb4() { return g_Sink + 4; }
__declspec(noinline) int LwaSupCb5() { return g_Sink + 5; }
__declspec(noinline) int LwaSupCb6() { return g_Sink + 6; }
__declspec(noinline) int LwaSupCb7() { return g_Sink + 7; }
__declspec(noinline) int LwaSupCb8() { return g_Sink + 8; }
__declspec(noinline) int LwaSupCb9() { return g_Sink + 9; }
__declspec(noinline) int LwaBulletCalc() { return g_Sink + 10; }
__declspec(noinline) int LwaBulletDraw() { return g_Sink + 11; }
__declspec(noinline) int LwaLoadCalc() { return g_Sink + 12; }
__declspec(noinline) int LwaLoadDraw() { return g_Sink + 13; }
__declspec(noinline) unsigned int __stdcall LwaLoadThreadProc(void *arg) { return (unsigned int)arg + g_Sink; }

} // namespace th128

extern "C" int __stdcall LifecycleWaveAProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return LwaRegisterSupervisorChains();
    if (selector == 2) { ((LwaStageBg *)arg)->~LwaStageBg(); return g_Sink; }
    if (selector == 3) { ((LwaEnemyMgr *)arg)->~LwaEnemyMgr(); return g_Sink; }
    if (selector == 4) { ((LwaReplayMgr *)arg)->~LwaReplayMgr(); return g_Sink; }
    if (selector == 5) { ((LwaCardInf *)arg)->Shutdown(); return g_Sink; }
    if (selector == 6) return ((LwaBulletMgr *)arg)->Init();
    if (selector == 7) return LwaLoading::Init((LwaLoading *)arg);
    if (selector == 8) { ((LwaLaserMgr *)arg)->~LwaLaserMgr(); return g_Sink; }
    return 0;
}
