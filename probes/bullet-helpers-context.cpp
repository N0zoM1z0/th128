#include "../src/BulletHelpers.hpp"

BulletAnmManagerMini *g_BulletAnmManager;
BulletManagerMini *g_BulletManagerMini;
BombInfMini *g_BombInfMini;
FreezeInfMini *g_FreezeInfMini;
EnemyMgrMini *g_EnemyMgrMini;
PlayerMini *g_PlayerMini;
float g_BulletDefaultTimeScale;
int g_GameModeA;
int g_GameModeB;
BulletTypeInfo g_BulletTypes[64];

typedef BulletTimerMini *(BulletTimerMini::*ClearFlag0Fn)();
typedef BulletSub380 *(BulletSub380::*Reset380Fn)();
typedef BulletRetThis *(BulletRetThis::*RetThisFn)();
typedef void (BulletItemMini::*DtorFn)();
typedef int (BulletBoundsCheck::*BoundsFn)(float, float);
typedef void (__stdcall *SetInvulnFn)(int);
SetInvulnFn g_SetInvuln = &PlayerMini::SetInvulnTimer;

extern "C" int __stdcall BulletHelpersProbe(
    BulletVec3 *va, BulletVec3 *vb, BulletAngle *ang, BulletTimerMini *tm,
    BulletItemMini *bi, BulletSub380 *s380, BulletZero54 *z54,
    BulletBoundsCheck *bc, FreezeInfMini *fr, PlayerMini *pl, EnemyMgrMini *em,
    BulletField08 *f08, BulletField04 *f04, BulletFlag90 *f90,
    BulletField54 *f54, BulletField5A34 *f5a, BulletRetThis *rt,
    int n, float fa, float fb)
{
    int acc = 0;
    *va = *va + *vb;
    *vb = *va - *vb;
    va->Set(fa, fb, fa);
    va->AddInPlace(*vb);
    va->ScaleInPlace(fa);
    acc += (int)va->LengthSq2D();
    acc += (int)va->GetX() + (int)va->GetX2() + (int)va->GetZ() + (int)va->GetZ2();

    acc += (int)ang->Sub(fa);
    acc += (int)ang->Add(fb);

    tm->SetScaleInt(n);
    tm->SetScaleNegInt(n);
    tm->SetScaleMinusOne();
    ClearFlag0Fn pClear = &BulletTimerMini::ClearFlag0;
    acc += (int)(tm->*pClear)();

    BulletAnmManagerMini::ResetColorMod();
    BulletAnmManagerMini::SetColorMod((unsigned int)n);

    acc += f08->Get() + f04->Get() + (int)f90->GetBit0() + f54->Get() + f5a->Get();
    RetThisFn pRt = &BulletRetThis::Self;
    acc += (int)(rt->*pRt)();

    acc += (int)pl->GetPosPtr();
    acc += fr->IsTimerGe60() ? 1 : 0;
    FreezeInfMini::CheckEnd();
    pl->SetFloat87B0(fa);
    g_SetInvuln(n);
    acc += BombInfMini::IsActive() ? 1 : 0;
    acc += BulletGetModeIndex();
    acc += (int)em->GetField65DDC();
    EnemyMgrMini::SetFlag6B1FCBit2();
    EnemyMgrMini::Trigger6B200();

    bi->ResetTimersAndClearFlag0();
    delete[] bi;
    Reset380Fn p380 = &BulletSub380::Reset;
    acc += (int)(s380->*p380)();
    acc += (int)z54->Zero();
    acc += (int)BulletManagerMini::GetAnm();
    acc += BulletManagerMini::UnloadAnm();
    acc += BulletGetTypeField00(n);
    acc += (int)BulletGetTypeField104(n);
    acc += (int)BulletGetTypeField108(n);

    BoundsFn pTall = &BulletBoundsCheck::IsOutOfBoundsTall;
    BoundsFn pNorm = &BulletBoundsCheck::IsOutOfBounds;
    acc += (bc->*pTall)(fa, fb);
    acc += (bc->*pNorm)(fa, fb);

    BulletNoOp1(); BulletNoOp2(); BulletNoOp3(); BulletNoOp4();
    acc += BulletRet1A() + BulletRet1B() + BulletRet1C();
    acc += BulletRet0A() + BulletRet0B() + BulletRet0C() + BulletRet0D() + BulletRet0E();
    return acc;
}
