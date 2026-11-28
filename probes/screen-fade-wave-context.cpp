#include "ScreenFadeWave.hpp"

namespace th128 {

float SfwTimer::g_SfwGameSpeed = 1.0f;
SfwAnmManager *g_SfwAnmManager = NULL;
SfwD3dDevice *g_SfwD3dDevice = NULL;
SfwViewport g_SfwViewport = { 0, 0, 0, 0, 0.0f, 0.0f };
int g_SfwPaused = 0;
SfwRng g_SfwRng;
float g_SfwShakeX = 0.0f;
float g_SfwShakeY = 0.0f;
SfwStage *g_SfwStage = NULL;

static volatile int g_Sink = 0;

__declspec(noinline) void SfwFlushAnmSpriteBatch(SfwAnmManager *mgr) {
    g_Sink += *(int *)mgr;
    Sleep(1);
    g_Sink += (int)mgr;
}

__declspec(noinline) void SfwTimer::Increment() {
    g_Sink += this->current;
    Sleep(1);
    g_Sink += (int)this;
}

__declspec(noinline) unsigned int SfwRng::GetU32() {
    g_Sink += *(int *)this;
    Sleep(1);
    g_Sink += (int)this;
    return (unsigned int)g_Sink;
}

} // namespace th128

extern "C" int __stdcall ScreenFadeWaveProbe(int a, int b) {
    using namespace th128;
    int r = 0;
    SfwFlushAnmSpriteBatch((SfwAnmManager *)(int)a);

    SfwDrawSolidRect((const SfwRectF *)(int)a, (DWORD)b);
    SfwDrawSolidRect((const SfwRectF *)(int)(a + 16), (DWORD)(b + 1));

    SfwDrawGradientRect((const SfwRectF *)(int)a, (DWORD)a, (DWORD)b, (DWORD)(a + 1), (DWORD)(b + 1));
    SfwDrawGradientRect((const SfwRectF *)(int)(a + 16), (DWORD)b, (DWORD)a, (DWORD)(b + 2), (DWORD)(a + 2));

    int (SfwScreenEffect::*volatile pDraw)() = &SfwScreenEffect::OnDrawFade;
    int (SfwScreenEffect::*volatile pFlash)() = &SfwScreenEffect::OnCalcFlash;
    int (SfwScreenEffect::*volatile pShakeA)() = &SfwScreenEffect::OnCalcShakeA;
    int (SfwScreenEffect::*volatile pShakeB)() = &SfwScreenEffect::OnCalcShakeB;
    SfwScreenEffect *e = (SfwScreenEffect *)(int)a;
    r += (e->*pDraw)();
    r += (e->*pFlash)();
    r += (e->*pShakeA)();
    r += (e->*pShakeB)();
    r += (int)((SfwRng *)(int)a)->GetU32();
    return r;
}
