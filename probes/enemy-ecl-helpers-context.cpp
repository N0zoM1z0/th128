#include "../src/EnemyEclHelpers.hpp"
#include <new>

unsigned int g_EclInputMask;
int g_EclKeyHoldTable[32];
EclField60Global *g_Ecl4B8930;
void *g_EclHostVtbl;
void *g_EclSub1098Vtbl;

typedef unsigned int (__stdcall *TestMaskFn)(unsigned int);
typedef void (__stdcall *SetF60Fn)(float);
typedef void (EclHostClean::*DestroyHostFn)();

extern "C" int __stdcall EnemyEclHelpersProbe(
    EclSub1000 *s1000, EclClear1000A *ca, EclClear1000B *cb,
    EclCtxHolder *ch, EclHostClean *hc, EclSub1098 *s1098,
    EclHp *hp, EclGet84 *g84, EclIsZ0 *iz0, EclGet68 *g68, EclArr0C *a0c,
    int n, unsigned int u, float fa)
{
    int acc = 0;
    acc += EclRet1_4136D0() + EclRet1_413720() + EclGetKeyHoldFrames(n);
    TestMaskFn pTest = &EclTestInputMask;
    acc += (int)pTest(u);
    acc += g84->Get() + iz0->IsZero() + g68->Get() + a0c->Get(n);
    EclNoOp414260();
    SetF60Fn pSet60 = &EclSet4B8930Field60;
    pSet60(fa);
    ca->Clear(); cb->Clear(); s1000->ResetSlots();

    acc += (int)ch->GetIntArg0() + (int)ch->GetFloatArg(n);
    ch->Tail46FF70();
    ch->SubTime(fa);

    new (hc) EclHostClean();
    hc->InitFields();
    hc->FreeList();
    DestroyHostFn pDtor = &EclHostClean::DestroyHost;
    (hc->*pDtor)();

    new (s1098) EclSub1098();
    s1098->~EclSub1098();

    hp->SetHpA(n); acc += hp->GetHp(); hp->SetHpB(n);
    acc += hp->GetFlag1();
    hp->SetRawHpMul7(n); hp->SetFlag0(n);
    acc += hp->GetField04(); hp->SetField04(n);
    acc += hp->GetField08(); hp->SetField08(n);
    hp->SetMinHp(n); hp->SetHpC(n);
    acc += hp->ApplyDamage(n);
    return acc;
}
