#include "../src/EndingIceWave.hpp"

namespace th128 {

float g_EiwTimeScale = 1.0f;
EiwRng g_EiwRng;
EiwChainMgr *g_EiwChainMgr = NULL;
EiwAnmMgr *g_EiwAnmMgr = NULL;
EiwEnding *g_EiwEnding = NULL;
void *g_EiwGlobalAux = NULL;
unsigned int g_EiwGameFlags = 0;
CRITICAL_SECTION g_EiwChainCs;
unsigned char g_EiwChainLockDepth = 0;

static volatile int g_Sink = 0;

__declspec(noinline) unsigned int EiwRng::GetU32() {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += (int)seed + (int)count + (int)(t1 + t2);
    return (unsigned int)g_Sink;
}

__declspec(noinline) void EiwReleaseAnm(void *anm) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1 + (int)anm;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)anm;
}

__declspec(noinline) EiwEndingSub::~EiwEndingSub() {
    g_Sink += dummy;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

} // namespace th128

extern "C" int __stdcall EndingIceWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        ((EiwEnding *)arg)->~EiwEnding();
        return g_Sink;
    }
    if (selector == 2) {
        int (__fastcall *f1)(EiwAnmVm *, const EiwVec3 *) = &EiwStartEffectBlue;
        int (__fastcall *f2)(EiwAnmVm *, const EiwVec3 *) = &EiwStartEffectGreen;
        return f1((EiwAnmVm *)arg, (const EiwVec3 *)(arg + 4)) + f2((EiwAnmVm *)arg, (const EiwVec3 *)(arg + 4));
    }
    if (selector == 3) {
        ((EiwIceGrid *)arg)->ClearIceMask((const EiwVec3 *)(arg + 4), (const unsigned int *)(arg + 16), arg & 15, (arg >> 4) & 15);
        ((EiwIceGrid *)arg)->SetIceMask((const EiwVec3 *)(arg + 4), (const unsigned int *)(arg + 16), arg & 15, (arg >> 4) & 15);
        return g_Sink;
    }
    return 0;
}
