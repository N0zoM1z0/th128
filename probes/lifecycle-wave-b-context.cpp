#include "../src/LifecycleWaveB.hpp"

namespace th128 {

unsigned int g_LwbGameFlags = 0;
CRITICAL_SECTION g_LwbChainCs;
unsigned char g_LwbChainLockDepth = 0;
LwbChainMgr *g_LwbChainMgr = NULL;
LwbAnmMgr *g_LwbAnmMgr = NULL;
LwbHelpInf *g_LwbHelpInf = NULL;
LwbIceObj *g_LwbIceObj = NULL;
LwbBulletMgr *g_LwbBulletMgr = NULL;
LwbPauseOrReplay *g_LwbPauseOrReplay = NULL;
LwbScoreBoard *g_LwbScoreBoard = NULL;
LwbItemMgr *g_LwbItemMgr = NULL;
LwbEffectMgr *g_LwbEffectMgr = NULL;
int g_LwbMode3c0 = 0;
LwbSplash *g_LwbSplash = NULL;
LwbTitleSub *g_LwbTitleSub = NULL;
int g_LwbFlag59e8 = 0;

static volatile int g_Sink = 0;

__declspec(noinline) void LwbReleaseAnm(void *anm) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1 + (int)anm;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)anm;
}

__declspec(noinline) void LwbMarkResourceVmsForDelete(void *anmMgr, void *res) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)anmMgr + (int)res + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)anmMgr * 3 + (int)res + (int)t2;
}

__declspec(noinline) LwbBulletEntry::~LwbBulletEntry() {
    g_Sink += (int)this;
}

__declspec(noinline) LwbItemEntry::~LwbItemEntry() {
    g_Sink += (int)this;
}

__declspec(noinline) LwbReplayEntry::~LwbReplayEntry() {
    g_Sink += dummy;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) int LwbInit445330() {
    DWORD t = GetTickCount();
    return (int)t + g_Sink;
}

} // namespace th128

extern "C" int __stdcall LifecycleWaveBProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) { ((LwbHelpInf *)arg)->Shutdown(); return g_Sink; }
    if (selector == 2) { ((LwbIceObj *)arg)->Shutdown(); return g_Sink; }
    if (selector == 3) { ((LwbBulletMgr *)arg)->~LwbBulletMgr(); return g_Sink; }
    if (selector == 4) { ((LwbPauseOrReplay *)arg)->~LwbPauseOrReplay(); return g_Sink; }
    if (selector == 5) { ((LwbScoreBoard *)arg)->~LwbScoreBoard(); return g_Sink; }
    if (selector == 6) { ((LwbItemMgr *)arg)->~LwbItemMgr(); return g_Sink; }
    if (selector == 7) { ((LwbEffectMgr *)arg)->~LwbEffectMgr(); return g_Sink; }
    if (selector == 8) { ((LwbSimpleChainOwner *)arg)->~LwbSimpleChainOwner(); return g_Sink; }
    if (selector == 9) return LwbTitleBootWait();
    return 0;
}
