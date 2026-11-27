#include "ItemLaserHelpers.hpp"

#include <stdlib.h>
#include <string.h>

IlGlobals *IlResetGlobals()
{
    memset(&g_IlGlobals, 0, sizeof(IlGlobals));
    g_IlGlobals.field4C = 1;
    g_IlGlobals.field78 = 1;
    return &g_IlGlobals;
}

IlHelpMgr::IlHelpMgr()
{
    timerFlags24 &= ~1U;
    fieldB4 = 0;
    field28 = 0;
    fieldFC = 0;
    fieldF8 = 1;
    field30 = 999;
    memset(this, 0, sizeof(IlHelpMgr));
    flags00 |= 2U;
    g_IlHelpMgr = this;
}

IlHelpMgr *IlHelpMgr::Create()
{
    IlHelpMgr *h = new IlHelpMgr();
    if (IlHelpMgrInit(h) != 0)
    {
        if (h != NULL)
        {
            IlHelpMgrDtor(h);
            operator delete(h);
        }
        return NULL;
    }
    return h;
}

IlHelpMgr *IlHelpMgr::DestroySelf()
{
    IlHelpMgrDtor(this);
    operator delete(this);
    return this;
}

int IlTable20::GetFirst(int idx) { return rows[idx][0]; }
int *IlTable20::GetRow1() { return rows[1]; }
void IlNoOp4281C0() {}
int IlPlayerMgr::GetF7C() { return g_IlPlayerMgr->fieldF7C; }

IlItemEntry::IlItemEntry()
{
    timerFlA00 &= ~1U;
    timerFlA14 &= ~1U;
}

IlItemEntry::~IlItemEntry()
{
}

IlItemMgr *IlItemMgr::DestroySelf()
{
    IlItemMgrDtor(this);
    operator delete(this);
    return this;
}

void IlItemMgr::DestroyGlobal()
{
    IlItemMgr *m = g_IlItemMgr;
    if (m != NULL)
    {
        IlItemMgrDtor(m);
        operator delete(m);
    }
}

int IlItemMgr::CalcHook()
{
    IlGameThread *gt = g_IlGameThread;
    if (gt != NULL)
    {
        unsigned int fl = gt->flags60;
        if (((fl >> 2) | fl) & 1U)
        {
            return 1;
        }
        if ((fl & 0x400U) != 0)
        {
            return 1;
        }
    }
    return IlItemMgrCalc(this);
}

int IlItemMgr::DrawHook()
{
    IlGameThread *gt = g_IlGameThread;
    if (gt != NULL && (gt->flags60 & 4U) != 0)
    {
        return 1;
    }
    return IlItemMgrDraw(this);
}

int IlRet0_4293C0() { return 0; }
int IlIsNZ0::IsNonZero() { return v != 0; }
int IlGet4B4D38() { return g_Il4B4D38; }

void __stdcall IlAddPowerClamped(float v)
{
    g_Il4B4CDC += (int)(v * 100.0);
    if (g_Il4B4CDC > g_Il4B4D78)
    {
        g_Il4B4CDC = g_Il4B4D78;
    }
}

int IlGet4B4D34() { return g_Il4B4D34; }

void IlClamp4B4D30Min()
{
    if (g_Il4B4D30 < -1024)
    {
        g_Il4B4D30 = -1024;
    }
}

void IlNoOp4299B0() {}

void IlLaserNode::LinkAfter(IlLaserNode *other)
{
    prev = other;
    other->next = this;
}

IlLaserBaseExact::IlLaserBaseExact()
{
    memset(this, 0, sizeof(IlLaserBaseExact));
    if ((timer14.flags & 1U) == 0)
    {
        timer14.currentF = 0.0f;
        timer14.current = 0;
        timer14.previous = -999999;
        timer14.gameSpeed = &g_IlDefaultTimeScale;
        timer14.flags |= 1U;
    }
    timer14.currentF = 0.0f;
    timer14.current = 0;
    timer14.previous = -1;
}

void IlCopy354A(const IlCopy354 *src, IlCopy354 *dst) { *dst = *src; }
void IlCopy354B(const IlCopy354 *src, IlCopy354 *dst) { *dst = *src; }

void IlLaserBaseExact::OffsetPosA(float radius, IlVec3 *out)
{
    out->FromAngle(angle68, radius);
    out->x += pos50.x;
    out->y += pos50.y;
    out->z += pos50.z;
}

void IlLaserBaseExact::OffsetPosB(float radius, IlVec3 *out)
{
    out->FromAngle(angle68, radius);
    out->x += pos50.x;
    out->y += pos50.y;
    out->z += pos50.z;
}

void IlLaserBaseExact::OffsetPosC(float radius, IlVec3 *out)
{
    out->FromAngle(angle68, radius);
    out->x += pos50.x;
    out->y += pos50.y;
    out->z += pos50.z;
}

void IlLaserBaseExact::OffsetPosD(float radius, IlVec3 *out)
{
    out->FromAngle(angle68, radius);
    out->x += pos50.x;
    out->y += pos50.y;
    out->z += pos50.z;
}

IlCurveNode *IlCurveObj::AppendNode(float f)
{
    IlCurveNode *cur = (IlCurveNode *)&head12EC;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    IlCurveNode *n = (IlCurveNode *)operator new(sizeof(IlCurveNode));
    cur->next = n;
    cur->val0C = f;
    n->val08 = f;
    cur->next->next = NULL;
    cur->next->prev = cur;
    return cur->next;
}

void IlNoOp429CD0() {}

void IlLaserMgr::SetFlag5F8Bit0(int bit)
{
    flags5F8 ^= (flags5F8 ^ (unsigned int)bit) & 1U;
}

void IlLaserMgr::PushTail(IlLaserNode *n)
{
    IlLaserNode *tail = tail5D0;
    n->prev = tail;
    tail->next = n;
    ++count5D4;
    tail5D0 = n;
}

void IlLaserMgr::UnlinkAndDec(IlLaserNode *n)
{
    --count5D4;
    n->prev->next = n->next;
    if (n->next != NULL)
    {
        n->next->prev = n->prev;
    }
    if (tail5D0 == n)
    {
        tail5D0 = n->prev;
    }
}

int IlPlayerMgr::Get43A90() { return g_IlPlayerMgr->field43A90; }

void IlPlayerMgr::SetFlag3AndInc()
{
    IlPlayerMgr *p = g_IlPlayerMgr;
    p->flags438EC |= 8U;
    ++p->field43A90;
}

void IlPlayerMgr::ClearFlag3AndZero()
{
    IlPlayerMgr *p = g_IlPlayerMgr;
    p->flags438EC &= ~8U;
    p->field43A90 = 0;
}

int IlPlayerMgr::IsActive438DCOrFlag1()
{
    IlPlayerMgr *p = g_IlPlayerMgr;
    if (p->field438DC != 0 || (p->flags438EC & 2U) != 0)
    {
        return 1;
    }
    return 0;
}

void Il4B8934::AddField84(int delta)
{
    Il4B8934 *m = g_Il4B8934;
    m->field84 += delta;
    if (m->field84 < 10)
    {
        m->field84 = 10;
    }
    if (delta < 0)
    {
        m->flags7C |= 0x100U;
    }
}

IlLaserMgr::IlLaserMgr()
{
    memset(this, 0, sizeof(IlLaserMgr));
    nextId5D8 = 0x10000;
    g_IlLaserMgr = this;
}

IlLaserMgr *IlLaserMgr::DestroySelf()
{
    IlLaserMgrDtor(this);
    operator delete(this);
    return this;
}

void IlLaserMgr::DestroyGlobal()
{
    IlLaserMgr *m = g_IlLaserMgr;
    if (m != NULL)
    {
        IlLaserMgrDtor(m);
        operator delete(m);
    }
}

int IlLaserMgr::DrawHook()
{
    if ((g_IlGameThread->flags60 & 4U) != 0)
    {
        return 1;
    }
    return Draw();
}
