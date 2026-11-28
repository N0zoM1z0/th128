#include "../src/LowFpuWaveA.hpp"

#include <string.h>

namespace th128 {

volatile int g_LfaSink = 0;

void *g_LfaRenderTargetA = NULL;
void *g_LfaRenderTargetB = NULL;
LfaAnmManager *g_LfaAnmMgr = NULL;
LfaD3dDevice9 *g_LfaDevice = NULL;
int g_LfaViewX = 0;
int g_LfaViewY = 0;
int g_LfaViewW = 0;
int g_LfaViewH = 0;
DWORD g_LfaClearColor = 0;
float g_LfaTimeScale = 1.0f;
LfaAsciiManager *g_LfaAsciiMgr = NULL;
LfaPopupMgr *g_LfaPopupMgr = NULL;

__declspec(noinline) void LfaAnmManager::FlushSpriteBatch() {
    g_LfaDevice->SetRenderTarget(0, this);
    g_LfaSink += (int)this;
}

__declspec(noinline) LfaAnmVmCtor::LfaAnmVmCtor() {
    memset(this, 0, sizeof(data));
    data[0] = 1;
}

__declspec(noinline) LfaAsciiManager::~LfaAsciiManager() {
    g_LfaSink += f1c84c;
}

__declspec(noinline) static LfaEclVm *LfaGetEcl(int k) { return (LfaEclVm *)k; }
__declspec(noinline) static LfaAnmVmEffect *LfaGetEff(int k) { return (LfaAnmVmEffect *)k; }

} // namespace th128

static int (th128::LfaLaserMgr::*volatile g_LfaOnDrawFn)() = NULL;

extern "C" int __stdcall LowFpuWaveAProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    g_LfaSink += LfaClearRenderTargetB();
    g_LfaSink += LfaClearRenderTargetB();
    g_LfaSink += LfaClearRenderTargetA();
    g_LfaSink += LfaClearRenderTargetA();
    ((LfaAnmManager *)(int)k)->FlushSpriteBatch();

    g_LfaOnDrawFn = &LfaLaserMgr::OnDraw;
    LfaLaserMgr *lm = (LfaLaserMgr *)(int)k;
    g_LfaSink += (lm->*g_LfaOnDrawFn)();

    LfaGetEff(k)->SetEffectState(a, (unsigned char)b, (unsigned char)k, (unsigned char)(a + 1));
    LfaGetEff(k + 1)->SetEffectState(b, (unsigned char)a, (unsigned char)(k + 1), (unsigned char)(b + 1));

    LfaGetEcl(k)->Reset();
    LfaGetEcl(k + 1)->Reset();

    LfaAsciiManager *am = new LfaAsciiManager();
    g_LfaSink += (int)am;

    LfaSpawnScorePopup((const LfaVec3 *)(int)a, k, b);
    LfaSpawnScorePopup((const LfaVec3 *)(int)b, k + 1, a);

    return g_LfaSink;
}
