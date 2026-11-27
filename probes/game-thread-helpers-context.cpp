#include "../src/GameThreadHelpers.hpp"

GtGuiMini *g_GtGui;
GtPairChainA *g_Gt4B8940;
GtPairChainB *g_Gt4B8A90;
GtFpsDouble *g_Gt4B894C;
GtLaserMgr *g_Gt4B8A60;
GtEnemyTimerMgr *g_GtEnemyMgr;
float g_GtDefaultTimeScale;
GtGameThread *g_GtGameThread;
int g_Gt4D59E8;
GtAnmClear *g_GtAnmMgr;
char g_GtPathBuf[260];
int g_Gt4B4CC8;
int g_Gt4B4D34;
int g_Gt4B4D38;
int g_Gt4B4D04;
int g_Gt4B4D08;
int g_GtStageNum;
int g_GtRouteNum;
GtStageInfo40 g_GtStageTable[32];
GtStageInfo40 *g_GtCurStageInfo;
int g_Gt4D5A88;
GtSoundSlot g_GtSoundSlots[76];
GtReplayMgr *g_GtReplayMgr;
int g_Gt4D33CC;

typedef double (__stdcall *SetDblFn)(double);
typedef int (GtGameThread::*DrawHookFn)();
typedef int (GtGameThread::*ClearAnmFn)();
typedef void (__stdcall *AdvStageFn)(int);
typedef void (__stdcall *SaveTsFn)(GtTimestampObj *);
typedef void (__stdcall *Set1CFn)(GtGlobalsObj *, float);

extern "C" int __stdcall GameThreadHelpersProbe(
    GtLaserNode *ln, GtGameThread *gt, GtTimestampObj *ts, GtSoundSlot *ss,
    GtGlobalsObj *go, GtVec3 *v3a, const GtVec3 *v3b, const char *s,
    int n, float fa, double da)
{
    int acc = 0;
    acc += GtGuiMini::IsBannerActive();
    GtPairChainA::EnableBoth();
    SetDblFn p24 = &GtFpsDouble::Set24;
    SetDblFn p2C = &GtFpsDouble::Set2C;
    acc += (int)p24(da) + (int)p2C(da);
    ln->Unlink();
    GtLaserMgr::DeleteAll();
    GtPairChainB::EnableBoth();
    acc += GtEnemyTimerMgr::ResetTimerAndLists();

    GtGameThread::DestroyGlobal();
    gt->DestroySelf();
    acc += GtAnmClear::ClearRet1();
    DrawHookFn pDraw = &GtGameThread::DrawHook;
    ClearAnmFn pClr = &GtGameThread::ClearAnmIfNotFlag2;
    acc += (gt->*pDraw)() + (gt->*pClr)();
    GtPlayfieldToScreen(v3a, v3b);
    acc += (int)GtCopyToPathBuf(s);
    acc += GtAddGrazeOrPoints(n);
    GtDecrementBomb();
    go->Set44AndUpdateGui();

    AdvStageFn pAdv = &GtAdvanceStage;
    pAdv(n);
    SaveTsFn pTs = &GtTimestampObj::Save;
    pTs(ts);
    GtAnmClear::Clear();
    go->Inc60And64();
    ss->StopAndSaveStatus();
    GtStopAllSoundBuffers();
    GtReplayMgr::DisableBoth();

    go->Inc68Clamp9(); acc += go->Get58(); go->Set0C1AndClear10();
    go->SetFlag3(n); go->SetFlag1(n); go->SetFlag0(n);
    acc += go->GetFlag1() + go->Get1C();
    go->Set70(n); go->ClearFlag2(); go->Max08With78();
    go->Set3C(n); go->Clear40(); go->SetB0(n);
    go->SetA4Clamp100000(n); go->SetFA0(fa);
    Set1CFn p1C = &GtSet1CMul1000;
    p1C(go, fa);
    go->Clear48(); go->Set04(n); go->Set80(n); go->Set68(n); go->Set4C(n); go->Clear60And64();
    acc += GtGet4D33CC() + GtIs4D33CCZero();
    GtNoOp4277B0();
    return acc;
}
