#include "SupervisorPauseHelpers.hpp"

#include <stdlib.h>
#include <string.h>

SpAnmVmDel *SpAnmVmDel::DeleteSelf()
{
    if (extraData != NULL)
    {
        free(extraData);
    }
    extraData = NULL;
    operator delete(this);
    return this;
}

int SpCheck4D304B()
{
    unsigned char v = g_Sp4D304B;
    if (v == 2 || v == 1)
    {
        return 0;
    }
    return -1;
}

void SpSupervisor::InitCritSecs()
{
    flags590 |= 0x8000U;
    for (int i = 0; i < 12; ++i)
    {
        g_SpInitCritSec(&critSecs810[i]);
    }
}

void SpSupervisor::DeleteCritSecs()
{
    flags590 &= ~0x8000U;
    for (int i = 0; i < 12; ++i)
    {
        g_SpDelCritSec(&critSecs810[i]);
    }
}

void SpReleaseAllSurfaces()
{
    if (g_SpSurf3014 != NULL)
    {
        g_SpSurf3014->vtbl->release(g_SpSurf3014);
        g_SpSurf3014 = NULL;
    }
    if (g_SpSurf3018 != NULL)
    {
        g_SpSurf3018->vtbl->release(g_SpSurf3018);
        g_SpSurf3018 = NULL;
    }
    if (g_SpSurf301C != NULL)
    {
        g_SpSurf301C->vtbl->release(g_SpSurf301C);
        g_SpSurf301C = NULL;
    }
    g_SpSurf3014 = NULL;
}

void SpSyncStageInfoPtr()
{
    int idx = g_Sp4B4D18;
    g_Sp4B4D14 = idx;
    g_SpStageInfoPtr = &g_SpStageInfos[idx];
}

void SpVec3Cross(SpVec3 *out, const SpVec3 *a, const SpVec3 *b)
{
    SpVec3 tmp;
    tmp.x = a->y * b->z - a->z * b->y;
    tmp.y = a->z * b->x - a->x * b->z;
    tmp.z = a->x * b->y - a->y * b->x;
    *out = tmp;
}

void SpSupervisor::UnlockCritSec(int idx)
{
    if ((flags590 & 0x8000U) != 0)
    {
        g_SpLeaveCritSec(&critSecs810[idx]);
        --lockDepth930[idx];
    }
}

void SpSupervisor::LockCritSec(int idx)
{
    if ((flags590 & 0x8000U) != 0)
    {
        g_SpEnterCritSec(&critSecs810[idx]);
        ++lockDepth930[idx];
    }
}

const char *SpStrChrWrap(const char *s, int ch) { return strchr(s, ch); }
int SpSupervisor::GetFlag200Bit6() { return (int)((flags200 >> 6) & 1U); }
int SpSupervisor::GetFlag200Bit5() { return (int)((flags200 >> 5) & 1U); }
int SpSupervisor::GetFlag200Bit3() { return (int)((flags200 >> 3) & 1U); }
int SpSupervisor::GetFlag200Bit1() { return (int)((flags200 >> 1) & 1U); }
int SpSupervisor::GetField114() { return field114; }
int SpSupervisor::GetFlag200Bit0() { return (int)(flags200 & 1U); }
int SpSupervisor::GetFlag200Bit2() { return (int)((flags200 >> 2) & 1U); }

void SpReset4D33BC()
{
    g_Sp4D33BC = -2;
    g_Sp4D33C0 = 0;
}

void SpComHolder::Release0C()
{
    if (obj0C != NULL)
    {
        obj0C->vtbl->release(obj0C);
        obj0C = NULL;
    }
}

void SpAnmMgr::ReleaseSurf()
{
    SpAnmMgr *m = g_SpAnmMgr;
    if (m->surf4E9698 != NULL)
    {
        m->surf4E9698->vtbl->release(m->surf4E9698);
        m->surf4E9698 = NULL;
    }
}

void SpWriteWord(unsigned short val, unsigned short *dst) { *dst = val; }
void SpAnmMgr::ResetByte8E() { g_SpAnmMgr->byte4E968E = 0xFF; }
void SpAnmMgr::ResetByte93() { g_SpAnmMgr->byte4E9693 = 0xFF; }
void SpAnmMgr::ResetByte92() { g_SpAnmMgr->byte4E9692 = 0xFF; }
void SpAnmMgr::ResetByte90() { g_SpAnmMgr->byte4E9690 = 0xFF; }
void SpAnmMgr::ResetByte8F() { g_SpAnmMgr->byte4E968F = 0xFF; }
void SpAnmMgr::ResetByte8C() { g_SpAnmMgr->byte4E968C = 7; }
void SpAnmMgr::ResetByte8D() { g_SpAnmMgr->byte4E968D = 0xFF; }
void SpAnmMgr::ResetField88() { g_SpAnmMgr->field4E9688 = 0; }
void SpAnmMgr::ResetField94() { g_SpAnmMgr->field4E9694 = 0; }

int SpObj10::GetField10() { return field10; }

SpSupervisor *SpResetSupervisor()
{
    memset(&g_SpSupervisor, 0, sizeof(SpSupervisor));
    g_Sp4D33F8 |= 0x4240U;
    return &g_SpSupervisor;
}

int SpGuiMgr::Get5A1C() { return g_SpGuiMgr->field5A1C; }

void SpGuiMgr::SyncScore()
{
    int s = g_Sp4B4CC4;
    g_SpGuiMgr->score58FC = s;
    if (g_Sp4B4CC0 < s)
    {
        g_Sp4B4CC0 = s;
    }
}

float SpFpsCounter::GetSlowdownPct()
{
    SpFpsCounter *f = g_SpFpsCounter;
    float ratio = (float)(f->d24 / f->d2C);
    return (float)(100.0 - ratio * 100.0);
}

int SpHelpMgr::Get128() { return g_SpHelpMgr->field128; }

SpPauseMgr::SpPauseMgr()
{
    timerFl20 &= ~1U;
    timerFl34 &= ~1U;
    fieldC4 = 0;
    field38 = 0;
    field10C = 0;
    field108 = 1;
    field40 = 999;
    field19C = 0;
    field110 = 0;
    field1E4 = 0;
    field1E0 = 1;
    field118 = 999;
    memset(this, 0, sizeof(SpPauseMgr));
    flags00 |= 2U;
    g_SpPauseMgr = this;
}

int SpRet0_436580() { return 0; }
int SpRet0_436590() { return 0; }

void SpPauseMgr::PauseBothChains()
{
    SpPauseMgr *p = g_SpPauseMgr;
    if (p->node08 != NULL)
    {
        p->node08->flags04 |= 2U;
    }
    if (p->node0C != NULL)
    {
        p->node0C->flags04 |= 2U;
    }
}

int SpCmp0::NotEqual(int x) { return v != x; }
int SpCmp0::LessThan(int x) { return v < x; }
int SpCmp0::Equal(int x) { return v == x; }

void SpArrayD4::Push90(int val)
{
    items90[countD4] = val;
    ++countD4;
}

void SpSoundSlot::Resume()
{
    if (buf00 != NULL && active14 != 0)
    {
        buf00->vtbl->play(buf00, 0, 0, desc08->playFlags0C);
    }
}

void SpResumeAllSoundBuffers()
{
    for (int i = 0; i < 76; ++i)
    {
        if (g_SpSoundSlots[i].buf00 != NULL && g_SpSoundSlots[i].active14 != 0)
        {
            g_SpSoundSlots[i].buf00->vtbl->play(g_SpSoundSlots[i].buf00, 0, 0, g_SpSoundSlots[i].desc08->playFlags0C);
        }
    }
}

int SpCaptureMgr::AllocSlot(int a, int b, int c, int d, int e, int f)
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        if (slots[i].f00 < 0)
        {
            slots[i].f00 = a;
            slots[i].f04 = b;
            slots[i].f18 = c;
            slots[i].f1C = d;
            slots[i].f08 = 0x80;
            slots[i].f0C = 0x10;
            slots[i].f10 = 0x180;
            slots[i].f14 = 0x1C0;
            slots[i].f20 = e;
            slots[i].f24 = f;
            return 0;
        }
    }
    return 0;
}

int SpVm428::GetSub428Field04() { return sub428->field04; }
int SpGameThread::Get14() { return g_SpGameThread->field14; }

int SpGameThread::IsNode08Flag1()
{
    SpChainNode *n = g_SpGameThread->node08;
    if (n != NULL && (n->flags04 & 2U) != 0)
    {
        return 1;
    }
    return 0;
}

SpVec3 *SpPlayer::GetPosED8() { return &g_SpPlayer->posED8; }
SpVec3 *SpPlayer::GetPosEF0() { return &g_SpPlayer->posEF0; }
SpVec3 *SpPlayer::GetPosEFC() { return &g_SpPlayer->posEFC; }

void SpPlayer::ResetAllOptFlags()
{
    for (int i = 0; i < 8; ++i)
    {
        *(int *)((unsigned char *)&optFlag8888 + i * 0xE4) = 1;
    }
}

void SpPlayer::SetFlag438ECBit1(int bit)
{
    flags438EC ^= (flags438EC ^ (unsigned int)(bit << 1)) & 2U;
}

SpFlag10::SpFlag10() { fl &= ~1U; }
SpFlagC8::SpFlagC8() { fl &= ~1U; }
SpFlag5C::SpFlag5C() { fl &= ~1U; }

int SpRet0_43ACB0() { return 0; }
int SpRet0_43ACC0() { return 0; }

SpPlayer *SpPlayer::DestroySelf()
{
    SpPlayerDtor(this);
    operator delete(this);
    return this;
}

void SpPlayer::DestroyGlobal()
{
    SpPlayer *p = g_SpPlayer;
    if (p != NULL)
    {
        SpPlayerDtor(p);
        operator delete(p);
    }
}

void SpPlayer::SetFlag1AndZeroF64()
{
    flags438EC |= 2U;
    fieldF64 = 0;
    fieldF68 = 0;
}
