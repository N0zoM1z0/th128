#include "../src/GuiDialogHelpers.hpp"

GdEnemyMgrMini *g_GdEnemyMgr;
Gd4B8934 *g_Gd4B8934;
GdGuiObj *g_GdGuiObj;
char g_GdDecodeBuf[256];
int g_Gd4B4D3C;
unsigned int g_Gd4B4D50;
GdStageInfo40 g_GdStageTable[32];
GdStageInfo40 *g_GdCurStageInfo;
int g_GdPower;
GdReplayMgr *g_GdReplayMgr;
unsigned int g_GdInput210;
int g_GdHold18C[32];
unsigned int g_GdInput21C;
int g_Gd4B4D2C;
int g_Gd4D33CC;
int g_Gd4B4D04;
int g_Gd4B4D08;
int g_Gd4B4CFC;
int g_Gd4B4D00;
int g_Gd4B4CC0;
Gd4B8924 *g_Gd4B8924;

typedef void (__stdcall *DecTimeFn)(int *, int *);
typedef unsigned int (__stdcall *TestInFn)(unsigned int);
typedef int (GdGuiObj::*CalcHookFn)();

extern "C" int __stdcall GuiDialogHelpersProbe(
    GdGuiFlags *gf, GdEnemyFlags *ef, GdTable66C *t66, GdGuiObj *go,
    GdStageOwner *so, GdPair *pr, GdOff40 *o40, GdOff464 *o464,
    GdGet3F0 *g3f0, GdGetFltA0 *gfa0, GdGetB0 *gb0, GdGetA4 *ga4,
    GdSet0 *s0, GdGet04 *g04, GdVmState *vm,
    int *ip1, int *ip2, const char *sa, const char *sb, int n, unsigned int u)
{
    int acc = 0;
    acc += gf->GetFlag4() + ef->IsNotFlag5() + ef->IsNotFlag0() + ef->IsFlag0Or4OrPos506C();
    acc += GdEnemyMgrMini::GetFlag0() + Gd4B8934::GetField94();
    DecTimeFn pDec = &Gd4B8934::DecodeTime;
    pDec(ip1, ip2);
    acc += (int)t66->GetEntry(n);
    CalcHookFn pCalc = &GdGuiObj::CalcHook;
    acc += (go->*pCalc)();
    go->DestroySelf();
    GdGuiObj::DestroyGlobal();
    acc += (int)GdDecodeDialogString(sa);

    so->Set7C(n); so->Set6C(n); so->SetStageIdx(n);
    acc += GdGet4B4D3C() + GdGet4B4D50Bit2() + GdGetPowerFloor10();
    GdSet4B4D50Bit2();
    acc += GdReplayMgr::GetSubByte0ABit0() + so->GetFlag4() + GdCall473090(sa, sb);
    TestInFn pT210 = &GdTestInput210;
    TestInFn pT21C = &GdTestInput21C;
    acc += (int)pT210(u) + GdGetHold18C(n) + pr->Diff() + (int)pT21C(u);
    acc += (int)o40->Get() + g3f0->Get() + (int)gfa0->Get() + gb0->Get() + ga4->Get();
    acc += GdGet4B4D2C() + (int)o464->Get() + so->GetFlag3() + GdGet4D33CC() + so->GetFlag5();
    acc += GdGet4B4D04() + GdGet4B4D08() + GdGet4B4CFC() + GdGet4B4D00() + GdGet4B4CC0();
    GdNoOp425BD0(); s0->Set(n); acc += g04->Get();
    vm->SetState3(); vm->SetStatePlus7(n);
    acc += Gd4B8924::GetFlag3();
    return acc;
}
