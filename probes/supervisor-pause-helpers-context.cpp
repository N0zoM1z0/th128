#include "../src/SupervisorPauseHelpers.hpp"
#include <new>

unsigned char g_Sp4D304B;
SpComObj *g_SpSurf3014;
SpComObj *g_SpSurf3018;
SpComObj *g_SpSurf301C;
int g_Sp4B4D18;
int g_Sp4B4D14;
SpStageInfo g_SpStageInfos[16];
SpStageInfo *g_SpStageInfoPtr;
int g_Sp4D33BC;
int g_Sp4D33C0;
SpAnmMgr *g_SpAnmMgr;
SpSupervisor g_SpSupervisor;
unsigned int g_Sp4D33F8;
SpGuiMgr *g_SpGuiMgr;
int g_Sp4B4CC4;
int g_Sp4B4CC0;
SpFpsCounter *g_SpFpsCounter;
SpHelpMgr *g_SpHelpMgr;
SpPauseMgr *g_SpPauseMgr;
SpSoundSlot g_SpSoundSlots[76];
SpGameThread *g_SpGameThread;
SpPlayer *g_SpPlayer;

void (__stdcall *g_SpInitCritSec)(SpCritSec *);
void (__stdcall *g_SpDelCritSec)(SpCritSec *);
void (__stdcall *g_SpEnterCritSec)(SpCritSec *);
void (__stdcall *g_SpLeaveCritSec)(SpCritSec *);

typedef int (SpCmp0::*CmpFn)(int);
typedef void (SpSoundSlot::*ResumeFn)();

extern "C" int __stdcall SupervisorPauseHelpersProbe(
    SpAnmVmDel *avd, SpSupervisor *sup, SpVec3 *va, const SpVec3 *vb, const SpVec3 *vc,
    const char *str, SpComHolder *ch, unsigned short *u16p, SpObj10 *o10,
    SpPauseMgr *pm, SpCmp0 *c0, SpArrayD4 *ad4, SpSoundSlot *ss, SpCaptureMgr *cm,
    SpVm428 *v428, SpPlayer *pl, SpFlag10 *f10, SpFlagC8 *fc8, SpFlag5C *f5c,
    int a, int b)
{
    int acc = 0;
    acc += (int)avd->DeleteSelf();
    acc += SpCheck4D304B();
    sup->InitCritSecs();
    sup->DeleteCritSecs();
    SpReleaseAllSurfaces();
    SpSyncStageInfoPtr();
    SpVec3Cross(va, vb, vc);
    sup->UnlockCritSec(a);
    sup->LockCritSec(a);
    acc += (int)SpStrChrWrap(str, a);
    acc += sup->GetFlag200Bit6() + sup->GetFlag200Bit5() + sup->GetFlag200Bit3()
        + sup->GetFlag200Bit1() + sup->GetField114() + sup->GetFlag200Bit0()
        + sup->GetFlag200Bit2();
    SpReset4D33BC();
    ch->Release0C();
    SpAnmMgr::ReleaseSurf();
    SpWriteWord((unsigned short)a, u16p);
    SpAnmMgr::ResetByte8E(); SpAnmMgr::ResetByte93(); SpAnmMgr::ResetByte92();
    SpAnmMgr::ResetByte90(); SpAnmMgr::ResetByte8F(); SpAnmMgr::ResetByte8C();
    SpAnmMgr::ResetByte8D(); SpAnmMgr::ResetField88(); SpAnmMgr::ResetField94();
    acc += o10->GetField10() + (int)SpResetSupervisor() + SpGuiMgr::Get5A1C();
    SpGuiMgr::SyncScore();
    acc += (int)SpFpsCounter::GetSlowdownPct() + SpHelpMgr::Get128();
    new (pm) SpPauseMgr();
    acc += SpRet0_436580() + SpRet0_436590();
    SpPauseMgr::PauseBothChains();
    CmpFn pNe = &SpCmp0::NotEqual;
    CmpFn pLt = &SpCmp0::LessThan;
    CmpFn pEq = &SpCmp0::Equal;
    acc += (c0->*pNe)(a) + (c0->*pLt)(a) + (c0->*pEq)(a);
    ad4->Push90(a);
    ResumeFn pRes = &SpSoundSlot::Resume;
    (ss->*pRes)();
    SpResumeAllSoundBuffers();
    acc += cm->AllocSlot(a, b, a, b, a, b);
    acc += v428->GetSub428Field04() + SpGameThread::Get14() + SpGameThread::IsNode08Flag1();
    acc += (int)SpPlayer::GetPosED8() + (int)SpPlayer::GetPosEF0() + (int)SpPlayer::GetPosEFC();
    pl->ResetAllOptFlags();
    pl->SetFlag438ECBit1(a);
    SpFlag10 *a10 = new SpFlag10[a]; delete[] a10;
    SpFlagC8 *ac8 = new SpFlagC8[a]; delete[] ac8;
    SpFlag5C *a5c = new SpFlag5C[a]; delete[] a5c;
    new (f10) SpFlag10(); new (fc8) SpFlagC8(); new (f5c) SpFlag5C();
    acc += SpRet0_43ACB0() + SpRet0_43ACC0();
    acc += (int)pl->DestroySelf();
    SpPlayer::DestroyGlobal();
    pl->SetFlag1AndZeroF64();
    return acc;
}
