#include "../src/LowFpuWaveB.hpp"

namespace th128 {

volatile int g_LfbSink = 0;

float g_LfbTimeScale = 1.0f;
LfbGameState *g_LfbGameState = NULL;
double g_LfbPauseStartTime = 0.0;
LfbGui *g_LfbGui = NULL;
LfbLaserMgr *g_LfbLaserMgr = NULL;
LfbBgMgr *g_LfbBgMgr = NULL;
int g_LfbSkipFade = 0;
int g_LfbSceneState = 0;
LfbScoreMgr *g_LfbScoreMgr = NULL;

__declspec(noinline) void LfbTimer::Increment() {
    this->prev = this->cur;
    this->curF += *this->scale;
    this->cur = (int)this->curF;
}

__declspec(noinline) LfbLaserBase::~LfbLaserBase() {
    g_LfbSink += f5b4;
}

__declspec(noinline) static void LfbClobber(int x) {
    Sleep((DWORD)x);
}

__declspec(noinline) void LfbDeleteVmByIdPtr(int *idPtr) {
    if (*idPtr != 0) {
        LfbClobber(*idPtr);
        *idPtr = 0;
        g_LfbSink += (int)idPtr;
    }
}

__declspec(noinline) void __stdcall LfbStopVmById(int id, int flag) {
    LfbClobber(id);
    g_LfbSink += flag;
    LfbClobber(flag);
    g_LfbSink += id;
}

__declspec(noinline) double LfbGetElapsedSeconds() {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)li.QuadPart;
}

} // namespace th128

static int (th128::LfbScreenFade::*volatile g_LfbFadeCalcFn)() = NULL;

extern "C" int __stdcall LowFpuWaveBProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    LfbEffectNode *en = (LfbEffectNode *)(int)k;
    en->Release();
    (en + 1)->Release();

    LfbNameEntry *ne = (LfbNameEntry *)(int)k;
    ne->EnterMode15();
    (ne + 1)->EnterMode15();
    ne->EnterMode1C();
    (ne + 1)->EnterMode1C();

    LfbHideBossBar();
    LfbHideBossBar();

    LfbLaserBase *lb = new LfbLaserBase();
    g_LfbSink += (int)lb;

    g_LfbSink += LfbCancelAllLasers();
    g_LfbSink += LfbCancelAllLasers();

    LfbBgJumpToLabel(a);
    LfbBgJumpToLabel(b);

    g_LfbFadeCalcFn = &LfbScreenFade::OnCalc;
    LfbScreenFade *sf = (LfbScreenFade *)(int)k;
    g_LfbSink += (sf->*g_LfbFadeCalcFn)();

    LfbPlayTimeObj *pt = (LfbPlayTimeObj *)(int)k;
    pt->AccumulatePlayTime();
    (pt + 1)->AccumulatePlayTime();

    LfbStopVmById(a, b);
    LfbStopVmById(b, a);

    return g_LfbSink;
}
