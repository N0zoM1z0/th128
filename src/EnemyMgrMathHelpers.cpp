#include "EnemyMgrMathHelpers.hpp"

#include <stdlib.h>
#include <string.h>

EmItemNode *EmItemMgrMini::FindById(int id)
{
    EmItemNode *n = g_EmItemMgr->head18;
    if (id != 0)
    {
        while (n != NULL)
        {
            if (n->id == id)
            {
                return n;
            }
            n = n->next;
        }
    }
    return NULL;
}

void EmEnemyObj::UnlinkFromMgr()
{
    EmEnemyMgr *m = g_EmEnemyMgr;
    if (m->headB8 == &node)
    {
        m->headB8 = node.next;
    }
    if (m->tailBC == &node)
    {
        m->tailBC = node.prev;
    }
    if (node.next != NULL)
    {
        node.next->prev = node.prev;
    }
    if (node.prev != NULL)
    {
        node.prev->next = node.next;
    }
    node.next = NULL;
    node.prev = NULL;
    --m->countC0;
}

EmEnemyMgr::EmEnemyMgr()
{
    timerFlagsB0 &= ~1U;
    memset(this, 0, sizeof(EmEnemyMgr));
    flags00 |= 2U;
    g_EmEnemyMgr = this;
}

EmSub1098B::EmSub1098B()
    : field1090(0)
    , field1094(0)
{
    memset(this, 0, sizeof(EmSub1098B));
    vtbl = &g_EmVtbl4A2720;
}

int EmRet0_415610() { return 0; }
int EmRet0_415620() { return 0; }

void EmEnemyMgr::EnableBoth()
{
    EmEnemyMgr *m = g_EmEnemyMgr;
    if (m->calcElem != NULL)
    {
        m->calcElem->flags |= 2U;
    }
    if (m->drawElem != NULL)
    {
        m->drawElem->flags |= 2U;
    }
}

EmSub1098B::~EmSub1098B()
{
    vtbl = &g_EmVtbl4A26DC;
    if (buf8C != NULL)
    {
        free(buf8C);
        buf8C = NULL;
    }
}

EmSub1098B *EmSub1098B::DestroySelf()
{
    this->~EmSub1098B();
    operator delete(this);
    return this;
}

EmEnemyMgr *__stdcall EmEnemyMgr::Create(const char *arg)
{
    EmEnemyMgr *m = new EmEnemyMgr();
    if (EmEnemyMgrInit(m, arg) != 0)
    {
        if (m != NULL)
        {
            EmEnemyMgrDtor(m);
            operator delete(m);
        }
        return NULL;
    }
    return m;
}

EmEnemyMgr *EmEnemyMgr::DestroySelf()
{
    EmEnemyMgrDtor(this);
    operator delete(this);
    return this;
}

void EmEnemyMgr::DestroyGlobal()
{
    EmEnemyMgr *m = g_EmEnemyMgr;
    if (m != NULL)
    {
        EmEnemyMgrDtor(m);
        operator delete(m);
    }
}

int EmRet1_415A10() { return 1; }
int EmRet1_415A60() { return 1; }

void EmEnemyMgr::DamageInCircle(const EmVec2 *pos, float radius)
{
    for (EmListNode *n = headB8; n != NULL; )
    {
        EmEnemyObj *e = (EmEnemyObj *)n->owner;
        EmListNode *next = n->next;
        if ((e->flags50AC & 0x800U) != 0)
        {
            EmVec3 d;
            d.x = e->pos.x - pos->x;
            d.y = e->pos.y - pos->y;
            float r = (float)(radius + 16.0);
            float d2 = d.x * d.x + d.y * d.y;
            if (d2 <= r * r)
            {
                if ((e->hp.flags14 & 1U) != 0)
                {
                    e->hp.rawHp -= 99999;
                    e->hp.hp = (e->hp.rawHp - e->hp.minHp * 7) / 7 + e->hp.minHp;
                }
                else
                {
                    e->hp.hp -= 99999;
                }
            }
        }
        n = next;
    }
}

void EmEnemyMgr::DamageInColumn(const EmVec2 *pos, float halfW)
{
    for (EmListNode *n = headB8; n != NULL; )
    {
        EmEnemyObj *e = (EmEnemyObj *)n->owner;
        EmListNode *next = n->next;
        if ((e->flags50AC & 0x800U) != 0)
        {
            if (e->pos.x >= (double)pos->x - halfW && e->pos.x <= (double)pos->x + halfW && e->pos.y < pos->y)
            {
                if ((e->hp.flags14 & 1U) != 0)
                {
                    e->hp.rawHp -= 99999;
                    e->hp.hp = (e->hp.rawHp - e->hp.minHp * 7) / 7 + e->hp.minHp;
                }
                else
                {
                    e->hp.hp -= 99999;
                }
            }
        }
        n = next;
    }
}

void EmEnemyMgr::DamageInCircle2(const EmVec2 *pos, float radius)
{
    for (EmListNode *n = headB8; n != NULL; )
    {
        EmEnemyObj *e = (EmEnemyObj *)n->owner;
        EmListNode *next = n->next;
        if ((e->flags50AC & 0x800U) != 0)
        {
            EmVec3 d;
            d.x = e->pos.x - pos->x;
            d.y = e->pos.y - pos->y;
            float r = (float)(radius + 16.0);
            float d2 = d.x * d.x + d.y * d.y;
            if (d2 <= r * r)
            {
                if ((e->hp.flags14 & 1U) != 0)
                {
                    e->hp.rawHp -= 99999;
                    e->hp.hp = (e->hp.rawHp - e->hp.minHp * 7) / 7 + e->hp.minHp;
                }
                else
                {
                    e->hp.hp -= 99999;
                }
            }
        }
        n = next;
    }
}

void EmSlotsObj::SetSlotF0C(int idx, int v)
{
    slots[idx].f0C = v;
}

EmEnemyObj *__stdcall EmEnemyMgr::FindNearest(const EmVec2 *pos, float maxDist)
{
    float bestSq = maxDist * maxDist;
    EmEnemyObj *best = NULL;
    for (EmListNode *n = g_EmEnemyMgr->headB8; n != NULL; )
    {
        EmEnemyObj *e = (EmEnemyObj *)n->owner;
        EmListNode *next = n->next;
        if ((e->flags50AC & 0x21U) == 0 && (e->flags50AC & 0x18000000U) == 0)
        {
            float dx = pos->x - e->pos.x;
            float dy = pos->y - e->pos.y;
            float d2 = dx * dx + dy * dy;
            if (d2 < bestSq)
            {
                bestSq = d2;
                best = e;
            }
        }
        n = next;
    }
    return best;
}

void EmNoOp41E450() {}

void EmInterp2D::SetMode(int m) { mode = m; }
void EmInterp2D::SetStart(const EmVec2 *v) { start = *v; }
void EmInterp2D::SetEnd(const EmVec2 *v) { end = *v; }
void EmInterp2D::SetVel0Global() { vel0 = g_EmGlobalVel; }
void EmInterp2D::SetVel1Global() { vel1 = g_EmGlobalVel; }
void EmInterp2D::SetDuration(int d) { duration = d; }

void EmInterp2D::ResetTimer()
{
    if ((timer.flags & 1U) == 0)
    {
        timer.currentF = 0.0f;
        timer.current = 0;
        timer.previous = -999999;
        timer.gameSpeed = &g_EmDefaultTimeScale;
        timer.flags |= 1U;
    }
    timer.currentF = 0.0f;
    timer.current = 0;
    timer.previous = -1;
}

int EmInterp2D::GetDuration() { return duration; }
void EmInterp2D::ClearDuration() { duration = 0; }

void EmVec2Sub(EmVec2 *out, const EmVec2 *a, const EmVec2 *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void EmVec2Add(EmVec2 *out, const EmVec2 *a, const EmVec2 *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

float __stdcall EmAngleDiff(float a, float b)
{
    if (a - b > 3.1415927410125732)
    {
        return (float)(b + 6.2831854820251465 - a);
    }
    if (b - a > 3.1415927410125732)
    {
        return (float)(b - 6.2831854820251465 - a);
    }
    return b - a;
}

float EmVec2DistSq(const EmVec2 *a, const EmVec2 *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    return dx * dx + dy * dy;
}

int EmGet4D33CC() { return g_Em4D33CC; }
int EmShort41E::Get() { return v; }

void EmField5C::Set5C(int v)
{
    int prev = field5C;
    field5C = v;
    if (prev != v)
    {
        field64 = 0;
    }
}

int EmGet4B4D30() { return g_Em4B4D30; }
float EmFloat28::Get28() { return f28; }
float EmFloat28::Get2C() { return f2C; }
void EmFloat28::SetFlag1() { flags30 |= 2U; }
void EmFloat28::Set2C(float v) { f2C = v; }

void __stdcall EmSetWrapped28(EmFloat28 *obj, float angle)
{
    obj->f28 = EmWrapAngle(angle);
}
