#include "GameThreadHelpers.hpp"

#include <string.h>

int GtGuiMini::IsBannerActive()
{
    GtGuiMini *g = g_GtGui;
    if ((g->flags5A04 & 0x100U) != 0 && g->field5A0C < 120)
    {
        return 1;
    }
    return 0;
}

void GtPairChainA::EnableBoth()
{
    GtPairChainA *p = g_Gt4B8940;
    p->a->flags |= 2U;
    p->b->flags |= 2U;
}

double __stdcall GtFpsDouble::Set24(double v)
{
    g_Gt4B894C->d24 = v;
    return v;
}

double __stdcall GtFpsDouble::Set2C(double v)
{
    g_Gt4B894C->d2C = v;
    return v;
}

void GtLaserNode::Unlink()
{
    prev->next = next;
    if (next != NULL)
    {
        next->prev = prev;
    }
}

void GtLaserMgr::DeleteAll()
{
    for (GtLaserNode *n = g_Gt4B8A60->head18; n != NULL; )
    {
        GtLaserNode *next = n->next;
        n->vtbl->onDelete(n);
        n->prev->next = n->next;
        if (n->next != NULL)
        {
            n->next->prev = n->prev;
        }
        operator delete(n);
        n = next;
    }
}

void GtPairChainB::EnableBoth()
{
    GtPairChainB *p = g_Gt4B8A90;
    p->a->flags |= 2U;
    p->b->flags |= 2U;
}

int GtEnemyTimerMgr::ResetTimerAndLists()
{
    GtEnemyTimerMgr *m = g_GtEnemyMgr;
    if ((m->timerFlags & 1U) == 0)
    {
        m->timerCurF = 0.0f;
        m->timerCur = 0;
        m->timerPrev = -999999;
        m->timerSpeed = &g_GtDefaultTimeScale;
        m->timerFlags |= 1U;
    }
    m->timerCurF = 0.0f;
    m->timerCur = 0;
    m->timerPrev = -1;
    m->headB8 = NULL;
    m->tailBC = NULL;
    return 0;
}

void GtGameThread::DestroyGlobal()
{
    GtGameThread *gt = g_GtGameThread;
    g_Gt4D59E8 = 0;
    if (gt != NULL)
    {
        GtGameThreadDtor(gt);
        operator delete(gt);
    }
}

GtGameThread *GtGameThread::DestroySelf()
{
    GtGameThreadDtor(this);
    operator delete(this);
    return this;
}

int GtAnmClear::ClearRet1()
{
    GtAnmClear *a = g_GtAnmMgr;
    a->fA4 = 0;
    a->fA8 = 0;
    a->fA0 = 0;
    a->fAC = 0;
    return 1;
}

int GtGameThread::DrawHook()
{
    return GtGameThreadDraw(this);
}

int GtGameThread::ClearAnmIfNotFlag2()
{
    if ((flags60 & 4U) == 0)
    {
        GtAnmClear *a = g_GtAnmMgr;
        a->fA4 = 0;
        a->fA8 = 0;
        a->fA0 = 0;
        a->fAC = 0;
    }
    return 1;
}

void GtPlayfieldToScreen(GtVec3 *out, const GtVec3 *in)
{
    out->x = (float)(in->x + 128.0 + 192.0);
    out->y = (float)(in->y + 16.0);
    out->z = in->z;
}

char *GtCopyToPathBuf(const char *s)
{
    g_GtPathBuf[0] = '\0';
    strcat(g_GtPathBuf, s);
    return g_GtPathBuf;
}

int GtAddGrazeOrPoints(int delta)
{
    if (g_Gt4B4CC8 >= g_Gt4B4D34)
    {
        return 0;
    }
    int prev = g_Gt4B4CC8;
    int next = prev + delta;
    g_Gt4B4CC8 = next;
    if (next > g_Gt4B4D34)
    {
        next = g_Gt4B4D34;
        g_Gt4B4CC8 = next;
    }
    return ((next - delta) / g_Gt4B4D38) != (next / g_Gt4B4D38);
}

void GtDecrementBomb()
{
    int v = --g_Gt4B4D04;
    if (v < 0)
    {
        v = 0;
        g_Gt4B4D04 = 0;
    }
    else if (v > 8)
    {
        v = 8;
        g_Gt4B4D04 = 8;
    }
    if (g_GtGui != NULL)
    {
        GtGuiUpdateBombs(g_GtGui, v, g_Gt4B4D08);
    }
}

void GtGlobalsObj::Set44AndUpdateGui()
{
    f44 = 2;
    if (g_GtGui != NULL)
    {
        GtGuiUpdateBombs(g_GtGui, g_Gt4B4D04, g_Gt4B4D08);
    }
}

void __stdcall GtAdvanceStage(int branch)
{
    int s = g_GtStageNum;
    if (g_GtGameThread->isReplay != 0)
    {
        if ((s == 1 || s == 6 || s == 11) && (g_GtRouteNum % 2 == 1))
        {
            s += 3;
        }
        else
        {
            ++s;
        }
    }
    else
    {
        if ((s == 1 || s == 6 || s == 11) && branch != 0)
        {
            ++g_GtRouteNum;
            s += 3;
        }
        else
        {
            ++s;
        }
    }
    g_GtStageNum = s;
    g_GtCurStageInfo = &g_GtStageTable[s];
}

void __stdcall GtTimestampObj::Save(GtTimestampObj *obj)
{
    if (g_GtGameThread->isReplay == 0)
    {
        obj->ts88 = GtGetTimeSeconds();
    }
}

void GtAnmClear::Clear()
{
    GtAnmClear *a = g_GtAnmMgr;
    a->fA4 = 0;
    a->fA8 = 0;
    a->fA0 = 0;
    a->fAC = 0;
}

void GtGlobalsObj::Inc60And64()
{
    ++f60;
    ++f64;
}

void GtSoundSlot::StopAndSaveStatus()
{
    unsigned int status;
    wasPlaying = 0;
    if (buf != NULL)
    {
        buf->vtbl->getStatus(buf, &status);
        wasPlaying = status & 1U;
        buf->vtbl->stop(buf);
    }
}

void GtStopAllSoundBuffers()
{
    g_Gt4D5A88 = -1;
    for (int i = 0; i < 76; ++i)
    {
        unsigned int status;
        g_GtSoundSlots[i].wasPlaying = 0;
        if (g_GtSoundSlots[i].buf != NULL)
        {
            g_GtSoundSlots[i].buf->vtbl->getStatus(g_GtSoundSlots[i].buf, &status);
            g_GtSoundSlots[i].wasPlaying = status & 1U;
            g_GtSoundSlots[i].buf->vtbl->stop(g_GtSoundSlots[i].buf);
        }
    }
}

void GtReplayMgr::DisableBoth()
{
    GtReplayMgr *r = g_GtReplayMgr;
    if (r->elem3A8 != NULL)
    {
        r->elem3A8->flags &= ~2U;
    }
    if (r->elem0C != NULL)
    {
        r->elem0C->flags &= ~2U;
    }
}

void GtGlobalsObj::Inc68Clamp9()
{
    ++f68;
    if (f68 >= 10)
    {
        f68 = 9;
    }
}

int GtGlobalsObj::Get58() { return f58; }

void GtGlobalsObj::Set0C1AndClear10()
{
    f10 = 0;
    f0C = 1;
}

void GtGlobalsObj::SetFlag3(int bit)
{
    flags90 ^= ((unsigned int)(bit << 3) ^ flags90) & 8U;
}

void GtGlobalsObj::SetFlag1(int bit)
{
    flags90 ^= ((unsigned int)(bit << 1) ^ flags90) & 2U;
}

void GtGlobalsObj::SetFlag0(int bit)
{
    flags90 ^= (flags90 ^ (unsigned int)bit) & 1U;
}

int GtGlobalsObj::GetFlag1() { return (flags90 >> 1) & 1U; }
int GtGlobalsObj::Get1C() { return f1C; }
void GtGlobalsObj::Set70(int v) { f70 = v; }
void GtGlobalsObj::ClearFlag2() { flags90 &= ~4U; }

void GtGlobalsObj::Max08With78()
{
    if (f08 < f78)
    {
        f08 = f78;
    }
}

void GtGlobalsObj::Set3C(int v) { f3C = v; }
void GtGlobalsObj::Clear40() { f40 = 0; }
void GtGlobalsObj::SetB0(int v) { fB0 = v; }

void GtGlobalsObj::SetA4Clamp100000(int v)
{
    fA4 = v;
    fA8 = 0;
    if (v > 100000)
    {
        fA4 = 100000;
    }
}

void GtGlobalsObj::SetFA0(float v) { fA0 = v; }

void __stdcall GtSet1CMul1000(GtGlobalsObj *obj, float sec)
{
    obj->f1C = (int)(sec * 1000.0);
}

void GtGlobalsObj::Clear48() { f48 = 0; }
void GtGlobalsObj::Set04(int v) { f04 = v; }
void GtGlobalsObj::Set80(int v) { f80 = v; }
void GtGlobalsObj::Set68(int v) { f68 = v; }
void GtGlobalsObj::Set4C(int v) { f4C = v; }

void GtGlobalsObj::Clear60And64()
{
    f60 = 0;
    f64 = 0;
}

int GtGet4D33CC() { return g_Gt4D33CC; }
int GtIs4D33CCZero() { return g_Gt4D33CC == 0; }
void GtNoOp4277B0() {}
