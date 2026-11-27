#include "../src/CardIceHelpers.hpp"
#include <new>

CardStatus60 *g_CardStatus60;
CardVec3 g_CardDefaultVec;
float g_CardDefaultTimeScale;
CardGuiMini *g_CardGuiMini;
CardStageMini *g_CardStageMini;
CardReplayMgrMini *g_CardReplayMgr;
signed char g_CardByteTable[250];
CardFreezeMini *g_CardFreezeMini;
CardBombMini *g_CardBombMini;
int g_CardGlobal4D33F0;
CardPauseMini *g_CardPauseMini;
CardAsciiMini *g_CardAsciiMini;
CardPlayerMini *g_CardPlayerMini;
CardBulletMgrMini *g_CardBulletMgr;
CardLaserMgrMini *g_CardLaserMgr;
CardEffectMgrMini *g_CardEffectMgr;

typedef int (CardVec2::*PadBoundsFn)(float, float) const;
typedef void (CardEffectSub54::*Sub54CtorFn)();

extern "C" int __stdcall CardIceHelpersProbe(
    CardVec2 *v2a, CardVec2 *v2b, CardVec3 *v3a, CardVec3 *v3b,
    CardScalarInterp *si, CardAnmVmMini *vm, CardMotionMini *cm,
    CardReplayTarget *rt, CardGuiMini *gui, CardFreezeMini *fr,
    CardChainPair *cp, CardAsciiMini *asc, CardBulletIter *bi,
    CardEnemyMini *en, CardEffectMgrMini *em, CardEffectSub54 *s54, CardEffectSub68 *s68,
    CardPtr11FC *p11fc, CardTable148 *t148, CardField74 *f74, CardField04Sub *f04s,
    CardField4C *f4c, CardPtr08 *p08, CardField10 *f10, CardZeroCheck *zc, CardPtr10 *p10,
    int n, float fa, float fb)
{
    int acc = 0;
    si->SetStart(fa); si->SetEnd(fb); si->SetVel0(fa); si->SetVel1(fb);
    si->SetDuration(n); acc += si->GetDuration();

    acc += (int)v2a->LengthSq() + (int)v2a->Dot(*v2b) + (int)v2a->Cross(*v2b);
    PadBoundsFn pPad = &CardVec2::IsOutOfBoundsPad;
    acc += (v2a->*pPad)(fa, fb);
    acc += (int)v2a->DistSq(*v2b);
    v2a->Set(fa, fb);
    *v2b = (*v2a) * fa;
    v3a->SubInPlace(*v3b);

    vm->ClearBlendBits();
    vm->SetBlendMode1();
    vm->SetInterrupt2();
    vm->SetInterrupt((short)n);
    acc += (int)vm->GetFlag0();
    acc += (int)vm->GetField3F0Ptr();

    acc += (int)CardGetStatusBit2() + (int)CardGetStatusBit0Or2();
    cm->Init(v3a, v3b, n, (unsigned char)n);
    CardNoOp40D300();
    CardEncodeReplaySeed(n, n + 1, rt);

    acc += (int)p11fc->Ptr();
    acc += CardGuiMini::GetArr80(n) + CardGuiMini::GetArr5C(n);
    CardStageMini::ClearFlag0();
    CardStageMini::SetFlag0();
    gui->IncCap80(); gui->IncCap84(); gui->IncCap88(); gui->IncCap8C();
    acc += (int)CardReplayMgrMini::GetEntry(n);
    acc += CardCountByteMatches250(n);
    new (fr) CardFreezeMini();
    CardFreezeMini::CheckEndNoClear80();
    acc += CardRet0_40D660() + CardRet0_40D670() + CardGet4D33F0();

    acc += (int)asc->GetAnmRes();
    CardAsciiMini::SetAlphaByte((unsigned char)n);
    acc += CardPauseMini::IsState1() ? 1 : 0;
    acc += CardPauseMini::GetArr20(n);
    acc += t148->Get(n) + f74->Get() + f04s->Sub300();
    cp->EnableBoth();
    CardNoOp40E5D0();
    acc += f4c->Get();
    CardNoOp40E5F0();
    acc += CardPlayerMini::GetField10();
    acc += (int)bi->GetSub9D8() + (int)bi->GetF9F8() + (int)bi->GetFA00() + (int)bi->GetFlag0();
    acc += (int)p08->Ptr();
    en->SetMode12();
    en->SetTargetPos(fa, fb);
    acc += (int)en->GetF68() + (int)en->GetF6C();
    acc += (int)CardBulletMgrMini::GetPool() + f10->Get();
    acc += (int)CardLaserMgrMini::BeginIter() + (int)CardLaserMgrMini::NextIter();
    acc += zc->IsZero() ? 1 : 0;
    acc += (int)p10->Ptr();

    new (s54) CardEffectSub54[2];
    new (s68) CardEffectSub68[2];
    new (em) CardEffectMgrMini();
    acc += em->GetField6B200();
    CardEffectMgrMini::EnableChainElems();
    CardNoOp40E810();
    return acc;
}
