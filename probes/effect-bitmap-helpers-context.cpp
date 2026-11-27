#include "../src/EffectBitmapHelpers.hpp"
#include <new>

EffItemMgrMini *g_EffItemMgr;
EffAnmMgrMini *g_EffAnmMgr;
EffGuiMini *g_EffGuiMgr;
EffZero24Global *g_Eff4B8940;
int g_Eff4B4D6C;
float g_Eff4B4D60;

typedef void (EffBitmapMgr::*MaskFn)(const EffVec2 *, const unsigned int *, int, int);
typedef int (__stdcall *CirclesFn)(float, float, float, float, float, float);
typedef void (__stdcall *AddFloatFn)(float);
typedef int (__fastcall *AttachFn)(EffAnmVmMini *, const EffMotion34 *);

extern "C" int __stdcall EffectBitmapHelpersProbe(
    EffBitmapMgr *bm, const EffVec2 *v2, const unsigned int *mask,
    EffListNode *na, EffListNode *nb, EffPtrArray *pa,
    EffEnemyMgrMini *em, EffInputStateMini *inp, EffCursorMini *cur,
    EffMotion34 *m34a, const EffMotion34 *m34b, EffAnmVmMini *vm,
    EffZero20 *z20, EffZero24Global *z24, EffMode558 *m558,
    EffClear0A *c0a, EffClear0B *c0b, EffSet0 *s0, EffCopyVec3 *cv3, const EffVec3 *v3,
    EffZero34 *z34, EffSetAC *sac, EffGet0A *g0a, EffGet0B *g0b,
    EffGet04 *g04, EffGet08 *g08, EffTable8C *t8c, int n, float fa)
{
    int acc = 0;
    c0a->Clear(); c0b->Clear(); s0->Set(n); cv3->CopyFrom(v3); z34->Zero(); sac->Set(n);
    acc += EffGet4B4D6C() + g0a->Get() + g0b->Get() + g04->Get() + g08->Get() + t8c->Get(n);

    bm->OrMask(v2, mask, n, n + 1);
    bm->ClearMask(v2, mask, n, n + 1);
    acc += EffBitmapCountBits(bm);
    acc += (int)EffPopCount32((unsigned int)n);

    na->InsertAfter(nb);
    na->Unlink();

    CirclesFn pCirc = &EffCirclesOverlap2D;
    acc += pCirc(fa, fa, fa, fa, fa, fa);
    AddFloatFn pAdd = &EffAdd4B4D60;
    pAdd(fa);
    EffNoOp410D70(); EffNoOp411D60();

    pa->FreeAll();
    EffItemMgrMini::ResetPool();
    acc += EffGuiMini::GetFieldC0() + EffGuiMini::GetSubB4Field08() + (int)EffGuiMini::GetSubB4();

    em->CleanupAll();
    em->DisableBoth();

    inp->UpdateRepeatMasks();
    m558->SetMode558(n);
    acc += (int)inp->GetConfirmMask() + (int)inp->TestTrigger((unsigned int)n) + inp->TestTriggerOrRepeat((unsigned int)n);

    cur->SavePrevious();
    acc += cur->ClampSet(n);
    cur->SetFieldD0(n); cur->SetCount(n); acc += cur->GetValue();
    cur->ResetMenu();

    EffZero20Init(z20);
    new (z24) EffZero24Global();
    m34b->CopyTo(*m34a);
    AttachFn pAtt = &EffMotionAttach;
    acc += pAtt(vm, m34b);

    acc += EffRet1_4120C0() + EffRet1_4120D0() + EffRet1_4120E0() + EffRet1_4120F0();
    acc += EffRet0_412260() + EffRet0_412390();
    return acc;
}
