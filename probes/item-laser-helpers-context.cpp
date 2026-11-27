#include "../src/ItemLaserHelpers.hpp"
#include <new>

IlGlobals g_IlGlobals;
IlHelpMgr *g_IlHelpMgr;
IlPlayerMgr *g_IlPlayerMgr;
IlItemMgr *g_IlItemMgr;
IlGameThread *g_IlGameThread;
int g_Il4B4D38;
int g_Il4B4CDC;
int g_Il4B4D78;
int g_Il4B4D34;
int g_Il4B4D30;
void *g_IlLaserBaseVtbl;
float g_IlDefaultTimeScale;
Il4B8934 *g_Il4B8934;
IlLaserMgr *g_IlLaserMgr;

void __declspec(noinline) IlVec3::FromAngle(float angle, float radius)
{
    g_Il4B4D38 = (int)this;
    x = angle * radius;
    y = angle + radius;
}

void IlLaserBaseExact::DummyVtbl() {}

typedef int (IlItemMgr::*ItemHookFn)();
typedef int (IlLaserMgr::*LaserHookFn)();
typedef void (IlLaserMgr::*SetFlagFn)(int);
typedef void (IlLaserBaseExact::*OffPosFn)(float, IlVec3 *);
typedef void (IlVec3::*FromAngleFn)(float, float);
typedef void (__stdcall *AddPwrFn)(float);

extern "C" int __stdcall ItemLaserHelpersProbe(
    IlHelpMgr *hm, IlTable20 *t20, IlItemEntry *ie, IlItemMgr *im,
    IlLaserNode *na, IlLaserNode *nb, IlLaserBaseExact *lb,
    IlCopy354 *ca, const IlCopy354 *cb, IlCurveObj *co, IlLaserMgr *lm,
    IlIsNZ0 *nz0, IlVec3 *v3, int n, float fa)
{
    int acc = 0;
    acc += (int)IlResetGlobals();
    new (hm) IlHelpMgr();
    acc += (int)IlHelpMgr::Create();
    hm->DestroySelf();
    acc += t20->GetFirst(n) + (int)t20->GetRow1() + IlPlayerMgr::GetF7C();
    IlNoOp4281C0();

    IlItemEntry *ieArr = new IlItemEntry[n];
    delete[] ieArr;
    new (ie) IlItemEntry();
    ie->~IlItemEntry();
    im->DestroySelf();
    IlItemMgr::DestroyGlobal();
    ItemHookFn pCalc = &IlItemMgr::CalcHook;
    ItemHookFn pDraw = &IlItemMgr::DrawHook;
    acc += (im->*pCalc)() + (im->*pDraw)() + IlRet0_4293C0() + nz0->IsNonZero();
    acc += IlGet4B4D38() + IlGet4B4D34();
    AddPwrFn pPwr = &IlAddPowerClamped;
    pPwr(fa);
    IlClamp4B4D30Min();
    IlNoOp4299B0();
    na->LinkAfter(nb);

    new (lb) IlLaserBaseExact();
    IlCopy354A(cb, ca); IlCopy354B(cb, ca);
    FromAngleFn pFA = &IlVec3::FromAngle;
    (v3->*pFA)(fa, fa);
    OffPosFn pA = &IlLaserBaseExact::OffsetPosA;
    OffPosFn pB = &IlLaserBaseExact::OffsetPosB;
    OffPosFn pC = &IlLaserBaseExact::OffsetPosC;
    OffPosFn pD = &IlLaserBaseExact::OffsetPosD;
    (lb->*pA)(fa, v3); (lb->*pB)(fa, v3); (lb->*pC)(fa, v3); (lb->*pD)(fa, v3);
    acc += (int)co->AppendNode(fa);
    IlNoOp429CD0();

    new (lm) IlLaserMgr();
    SetFlagFn pFlag = &IlLaserMgr::SetFlag5F8Bit0;
    (lm->*pFlag)(n);
    lm->PushTail(na);
    lm->UnlinkAndDec(na);
    acc += IlPlayerMgr::Get43A90() + IlPlayerMgr::IsActive438DCOrFlag1();
    IlPlayerMgr::SetFlag3AndInc();
    IlPlayerMgr::ClearFlag3AndZero();
    Il4B8934::AddField84(n);
    lm->DestroySelf();
    IlLaserMgr::DestroyGlobal();
    LaserHookFn pLDraw = &IlLaserMgr::DrawHook;
    acc += (lm->*pLDraw)();
    return acc;
}
