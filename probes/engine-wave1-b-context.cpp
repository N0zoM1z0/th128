#include "EngineWave1B.hpp"

namespace th128 {

Ew1bErrorContext g_Ew1bErrorContext;

static volatile int g_Sink = 0;
static volatile float g_FSink = 0.0f;

__declspec(noinline) float __stdcall Ew1bShortestAngleDiff(float a, float b) {
    Sleep(1);
    g_FSink += a + b;
    return g_FSink;
}

__declspec(noinline) float __stdcall Ew1bNormalizeAngle(float a) {
    Sleep(1);
    g_FSink += a;
    return g_FSink;
}

static void *volatile g_Keep[] = {
    (void *)&Ew1bInitGameErrorContext,
};

} // namespace th128

extern "C" int __stdcall EngineWave1BProbe(int a, int b) {
    using namespace th128;
    int r = (int)g_Keep[0];
    float fa = (float)a;
    float fb = (float)b;

    Ew1bRotateVec2((Ew1bVec2 *)(int)a, (const Ew1bVec2 *)(int)b, fa);
    Ew1bRotateVec2((Ew1bVec2 *)(int)(a + 8), (const Ew1bVec2 *)(int)(b + 8), fb);

    Ew1bTimer *t1 = (Ew1bTimer *)(int)a;
    Ew1bTimer *t2 = (Ew1bTimer *)(int)(a + 4);
    r += t1->Tick() + t2->Tick();

    r += Ew1bPointInRotatedRect(fa, fb, fa + 1.0f, fb + 1.0f, fa + 2.0f, fb + 2.0f, fa + 3.0f);
    r += Ew1bPointInRotatedRect(fb, fa, fb + 1.0f, fa + 1.0f, fb + 2.0f, fa + 2.0f, fb + 3.0f);

    r += Ew1bCircleIntersectsRotatedRect(fa, fb, fa + 1.0f, fb + 1.0f, fa + 2.0f, fb + 2.0f, fa + 3.0f, fb + 3.0f);
    r += Ew1bCircleIntersectsRotatedRect(fb, fa, fb + 1.0f, fa + 1.0f, fb + 2.0f, fa + 2.0f, fb + 3.0f, fa + 3.0f);

    r += Ew1bCircleInsideRotatedRect(fa, fb, fa + 1.0f, fb + 1.0f, fa + 2.0f, fb + 2.0f, fa + 3.0f, fb + 3.0f);
    r += Ew1bCircleInsideRotatedRect(fb, fa, fb + 1.0f, fa + 1.0f, fb + 2.0f, fa + 2.0f, fb + 3.0f, fa + 3.0f);

    float o1 = 0.0f, o2 = 0.0f;
    r += Ew1bRayCircleIntersect((const Ew1bVec2 *)(int)a, (const Ew1bVec2 *)(int)b, &o1, &o2, fa, fb);
    r += Ew1bRayCircleIntersect((const Ew1bVec2 *)(int)(a + 8), (const Ew1bVec2 *)(int)(b + 8), &o2, &o1, fb, fa);

    r += (int)Ew1bAdvanceAngle(&o1, &o2, fa);
    r += (int)Ew1bAdvanceAngle(&o2, &o1, fb);

    r += (int)Ew1bInitGameErrorContext();
    return r + (int)o1 + (int)o2;
}
