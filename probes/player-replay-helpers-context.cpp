#include "../src/PlayerReplayHelpers.hpp"
#include <new>

int g_Pr4B4D40;
int g_Pr4B4CC8;
int g_Pr4B4D38;
int g_Pr4B4D34;
int g_Pr4B4CF4;
PrPlayerExact *g_PrPlayer;
PrEnemyMgr *g_PrEnemyMgr;
PrFpsCounter *g_PrFpsCounter;
PrGameThread *g_PrGameThread;
unsigned int g_Pr4DAFF8;
char g_PrTagBuf[8];
int g_Pr4D33CC;
int g_Pr4D2AE4;
unsigned short g_Pr4D2AE0;
signed char g_PrScoreTable[250];

typedef float (__stdcall *AngleDiffFn)(float, float);
typedef float (__stdcall *FabsFn)(float);
typedef int (PrTimer::*TickMultFn)(int);
typedef int (PrReplayMgr::*FFStepFn)();
typedef int (PrReplayChunk::*GetFpsFn)();
typedef int (PrReplayChunk::*GetTotalFn)();

extern "C" int __stdcall PlayerReplayHelpersProbe(
    PrVec3 *va, PrVec3 *vb, PrVec3 *vc, PrVec3 *vd,
    PrIVec2 *ia, PrIVec2 *ib, PrIVec2 *ic,
    PrTimer *tm, PrWord418 *w418, PrObj38 *o38, PrObj74 *o74,
    PrPlayerExact *pl, PrInputFlags *ifl, PrBulletHit *bh,
    PrReplayMgr *rm, PrReplayChunk *rc, PrReplayFileHeader *rfh,
    PrReplayList *rl, PrStageEntry24 *se, PrReplayState84 *rs,
    PrBuf90 *b90, PrUserTag *ut, PrScoreFile *sf, const unsigned char *raw,
    int a, int b, float fa, float fb)
{
    int acc = 0;
    PrIncGrazeClamped();
    AngleDiffFn pAdA = &PrAngleDiffA;
    AngleDiffFn pAdB = &PrAngleDiffB;
    FabsFn pFabs = &PrFabsFloat;
    acc += (int)(pAdA(fa, fb) + pAdB(fa, fb) + pFabs(fa));
    PrVec2FloatToFixed128(va, ia);
    acc += PrGet4B4CC8Div4B4D38();
    acc += (int)(va->DistSq2DA(vb) + va->DistSq2DB(vb)) + w418->Get418();
    TickMultFn pTm = &PrTimer::IsTickMultiple;
    acc += (tm->*pTm)(a);
    ia->ToVec3Scaled(va);
    ia->AddAssign(ib, ic);
    acc += ia->IsNonZero();
    ia->SubTo(ib, ic);
    ia->MulTo(ib, a);
    ia->Div100To(ib);
    ia->Set(a, b);
    ia->AddTo(ib, ic);
    PrSet4B4D38(a); PrSet4B4D34(b);
    acc += o38->Get38() + PrGet4B4CF4() + o74->Get74();
    ia->CopyTo(ib, ic);
    acc += (int)PrPlayerExact::GetOptPtr(a) + (int)PrPlayerExact::GetOptFloat4C(a);
    pl->ReadOpt54(a, &b);
    acc += ifl->IsFlag3EE4Set() + ifl->IsFlag50ACSet() + ifl->Get11C0() + PrEnemyMgr::Get6B218();
    PrComputeBoxBounds(va, vb, vc, vd);
    bh->Zero54And64A(); bh->Zero54And64B(); bh->AddField18(fa);
    acc += PrRet0_4404A0();
    pl->ClearTargetId(a);
    PrPlayerExact::SetFixedPos(ia);
    acc += (int)PrFpsCounter::Get34();
    acc += rm->GetFastForwardStepA();
    FFStepFn pFFB = &PrReplayMgr::GetFastForwardStepB;
    acc += (rm->*pFFB)() + (int)rm->GetTagStr();
    se->ResetZero(); se->ResetNeg1();
    ut->SetUserMagic();
    GetFpsFn pGF = &PrReplayChunk::GetFpsCount;
    GetTotalFn pGT = &PrReplayChunk::GetTotalByteCount;
    acc += (rc->*pGF)() + rc->GetInputByteCount() + rc->GetInputCount() + (rc->*pGT)();
    acc += rc->PushInput((unsigned short)a, (unsigned short)b, (unsigned short)a);
    acc += rc->PushFps((unsigned char)a);
    rs->Set1C(a); rs->Set38(b); rs->Set84(a);
    acc += rs->Get80() + rs->Get1C() + PrGet4D33CC() + PrGet4D2AE0();
    PrZero4D2AE4();
    new (b90) PrBuf90();
    rfh->InitHeader();
    new (rc) PrReplayChunk();
    rl->AllocAndAppendChunk(a);
    rc->UnlinkNode();
    rc->DeleteSelf();
    rl->FreeStageChunks(a);
    PrStageEntry24 *seArr = new PrStageEntry24[a];
    delete[] seArr;
    new (se) PrStageEntry24();
    se->~PrStageEntry24();
    acc += (int)PrChecksumBytes(raw, a);
    sf->InitScoreFile();
    return acc;
}
