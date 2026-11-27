#include "../src/LaserMotherHelpers.hpp"
#include <new>

LmLaserMgr *g_LmLaserMgr;
float g_LmDefaultTimeScale;
int g_LmLaserTable[16][18][4];
void *g_LmSub10Vtbl;
LmMother *g_LmMother;
LmSupervisorChains *g_LmSupervisor;
int g_Lm4D2E68;
unsigned int g_Lm4D59E8;
unsigned int g_Lm4D33F8;
int g_Lm4D33C0;

typedef int (__stdcall *Ret0S5Fn)(int, int, int, int, int);
typedef int (__stdcall *Ret0S6Fn)(int, int, int, int, int, int);
typedef int (__stdcall *Ret0S4Fn)(int, int, int, int);
typedef int (__stdcall *Ret0S2Fn)(int, int);

typedef int (LmLaserNode::*SetByteFn)(int, int);
typedef int (LmLaserCurve::*CleanupFn)();
typedef int (LmLaserLine::*Sub234Fn)();
typedef int (LmMother::*MotherHookFn)();
typedef int (LmIsNZ4::*IsNZ4Fn)();
typedef void (LmVec3::*FromAngleFn)(float, float);

extern "C" int __stdcall LaserMotherHelpersProbe(
    LmLaserMgr *mgr, LmVec3 *va, LmVec3 *vb, LmLaserLine *ll, LmLaserInf *li,
    LmCurveNode *cna, const LmCurveNode *cnb, LmLaserCurve *lc, LmLaserNode *ln,
    const LmArray88 *a88, LmTimer *tm, LmIsNZ4 *nz4, LmMother *mo, LmObj558 *o558,
    int a, int b, int c, float fa, float fb)
{
    int acc = 0;
    acc += mgr->CancelInBoxA(va, vb, fa, a, b);
    acc += mgr->CancelInBoxB(va, vb, fa, a, b, c);
    acc += LmLaserMgr::ResetAllTimersAndCancel();
    acc += LmLaserMgr::CancelInCircle(va, fa, a, b);
    acc += LmLaserMgr::CancelAll(a, b);
    acc += mgr->CheckAll30(a, fa);
    acc += ll->GetTableVal(a);
    acc += LmRet0_42C170() + LmRet0_42DB90();
    li->ProcessCmds();
    LmCopyCurveNode(cnb, cna);
    CleanupFn pClean = &LmLaserCurve::Cleanup;
    acc += (lc->*pClean)();
    Sub234Fn pSub = &LmLaserLine::GetSubPlus234;
    acc += (ll->*pSub)();
    Ret0S5Fn pS5 = &LmRet0_S5;
    Ret0S6Fn pS6 = &LmRet0_S6;
    Ret0S4Fn pS4 = &LmRet0_S4;
    Ret0S2Fn pS2 = &LmRet0_S2;
    acc += LmRet0_432A70() + pS5(a, b, c, a, b) + pS6(a, b, c, a, b, c)
        + pS4(a, b, c, a) + pS2(a, b);
    SetByteFn pByte = &LmLaserNode::SetByte7CIfZero;
    acc += (ln->*pByte)(a, b);
    LmNoOp432AE0();
    LmNoOp432B30();
    acc += (int)LmGetFloat88(a, a88);
    tm->SetFloat(fa);
    acc += (int)va->DistSq2D(vb);
    va->ScaleTo(vb, fa);
    acc += va->IsOutsidePlayfield(fa, fb);
    LmNoOp432DE0();
    IsNZ4Fn pNZ4 = &LmIsNZ4::IsNonZero4;
    acc += (nz4->*pNZ4)();
    FromAngleFn pFA = &LmVec3::FromAngle;
    (va->*pFA)(fa, fb);
    new (mo) LmMother();
    acc += (int)LmMother::Create();
    mo->DestroySelf();
    LmMother::DestroyGlobal();
    MotherHookFn pHook = &LmMother::OnCalcHook;
    acc += (mo->*pHook)() + mo->OnCalcInner();
    LmPauseAllChains();
    o558->Set558(a);
    acc += o558->GetFlag200Bit4();
    LmNoOp433620();
    return acc;
}
