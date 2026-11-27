#include "../src/GuiMathHelpers.hpp"
#include <new>

Gm4B8954 *g_Gm4B8954;
GmFpsCounter *g_GmFpsCounter;
GmGuiMini *g_GmGui;
GmVec3 *(__stdcall *g_GmVec3Normalize)(GmVec3 *, const GmVec3 *);

typedef int (__stdcall *RectsFn)(float, float, float, float, float, float, float, float);
typedef void (__stdcall *DblWrapFn)(GmMoveObj *, float);
typedef int (GmFpsCounter::*DrawHookFn)();

extern "C" int __stdcall GuiMathHelpersProbe(
    GmMoveObj *mo, GmCopyVec3 *cv3, GmSet3F4 *s3f4, GmOff1C *o1c, GmOff458 *o458,
    GmOff0C *o0c, GmGet08 *g08, GmGet558 *g558, GmScaleObj *sc,
    GmFpsCounter *fps, GmGuiMini *gm, GmVec3 *v3a, const GmVec3 *v3b,
    const GmVec2 *v2, float *fp1, float *fp2, int n, float fa)
{
    int acc = 0;
    mo->Set10(fa); mo->Set0C(fa); acc += (int)mo->Get20() + (int)mo->Get24();
    mo->SetFlag0(); acc += (int)mo->Get18(); mo->ClearFlag1(); mo->ClearFlags01();
    acc += (int)mo->Get08() + (int)mo->Get2C();
    mo->Set24(fa); mo->Set20(fa); mo->Set18(fa);
    acc += mo->GetFlag1() + mo->GetFlag0();
    mo->Set04(fa); acc += (int)mo->Get04(); mo->Set00(fa); acc += (int)mo->Get00();
    mo->SetVec3At0C(v3b);

    cv3->CopyFrom(v3b); s3f4->Set(n);
    acc += (int)o1c->Get() + (int)o458->Get() + (int)o0c->Get() + g08->Get() + g558->Get();
    acc += (int)sc->Mul60x40() + (int)sc->Mul64x44() + Gm4B8954::GetFlag1();
    GmNoOp41F360(); GmNoOp41F730(); GmNoOp41FB90();

    RectsFn pRect = &GmRectsOverlap2D;
    acc += pRect(fa, fa, fa, fa, fa, fa, fa, fa);
    GmPosToUv(v2, fp1, fp2);
    GmVec3NormalizeScale(v3a, v3b, fa);
    acc += (int)GmVec2LenSq(v2);
    DblWrapFn pWrap = &GmSetDoubleWrapped1C;
    pWrap(mo, fa);
    acc += (int)GmIdentity(mo);

    new (fps) GmFpsCounter();
    fps->DestroySelf();
    GmFpsCounter::DestroyGlobal();
    DrawHookFn pDraw = &GmFpsCounter::DrawHook;
    acc += (fps->*pDraw)();

    acc += GmGuiMini::GetField5904() + gm->GetFlag3();
    gm->ClearRow5914(n);
    GmGuiMini::SetFlag3AndClear5A3C();
    GmGuiMini::SetFlag4();
    return acc;
}
