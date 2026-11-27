#include "../src/EnemyMgrMathHelpers.hpp"
#include <new>

EmItemMgrMini *g_EmItemMgr;
EmEnemyMgr *g_EmEnemyMgr;
void *g_EmVtbl4A2720;
void *g_EmVtbl4A26DC;
float g_EmDefaultTimeScale;
EmVec2 g_EmGlobalVel;
int g_Em4D33CC;
int g_Em4B4D30;

typedef float (__stdcall *AngleDiffFn)(float, float);
typedef void (__stdcall *WrapSetFn)(EmFloat28 *, float);

extern "C" int __stdcall EnemyMgrMathHelpersProbe(
    EmEnemyObj *eo, EmEnemyMgr *em, EmSub1098B *sb, EmSlotsObj *sl,
    EmInterp2D *ip, EmShort41E *s41e, EmField5C *f5c, EmFloat28 *f28,
    EmVec2 *v2a, const EmVec2 *v2b, const EmVec2 *v2c,
    const char *arg, int a, int b, float fa, float fb)
{
    int acc = 0;
    acc += (int)EmItemMgrMini::FindById(a);
    eo->UnlinkFromMgr();
    new (em) EmEnemyMgr();
    new (sb) EmSub1098B();
    acc += EmRet0_415610() + EmRet0_415620() + EmRet1_415A10() + EmRet1_415A60();
    EmEnemyMgr::EnableBoth();
    sb->DestroySelf();
    sb->~EmSub1098B();
    acc += (int)EmEnemyMgr::Create(arg);
    em->DestroySelf();
    EmEnemyMgr::DestroyGlobal();
    em->DamageInCircle(v2b, fa);
    em->DamageInColumn(v2b, fa);
    em->DamageInCircle2(v2b, fa);
    sl->SetSlotF0C(a, b);
    acc += (int)EmEnemyMgr::FindNearest(v2b, fa);
    EmNoOp41E450();

    ip->SetMode(a); ip->SetStart(v2b); ip->SetEnd(v2b);
    ip->SetVel0Global(); ip->SetVel1Global();
    ip->SetDuration(a); ip->ResetTimer(); acc += ip->GetDuration(); ip->ClearDuration();

    EmVec2Sub(v2a, v2b, v2c);
    EmVec2Add(v2a, v2b, v2c);
    AngleDiffFn pDiff = &EmAngleDiff;
    acc += (int)pDiff(fa, fb);
    acc += (int)EmVec2DistSq(v2b, v2c);
    acc += EmGet4D33CC() + s41e->Get() + EmGet4B4D30();
    f5c->Set5C(a);
    acc += (int)f28->Get28() + (int)f28->Get2C();
    f28->SetFlag1(); f28->Set2C(fa);
    WrapSetFn pWrap = &EmSetWrapped28;
    pWrap(f28, fa);
    return acc;
}
