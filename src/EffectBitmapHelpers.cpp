#include "EffectBitmapHelpers.hpp"

#include <stdlib.h>
#include <string.h>

void EffClear0A::Clear() { v = 0; }
void EffClear0B::Clear() { v = 0; }
void EffSet0::Set(int x) { v = x; }
void EffCopyVec3::CopyFrom(const EffVec3 *src) { v = *src; }
void EffZero34::Zero() { memset(this, 0, sizeof(EffZero34)); }
void EffSetAC::Set(int x) { v = x; }
int EffGet4B4D6C() { return g_Eff4B4D6C; }
int EffGet0A::Get() { return v; }
int EffGet0B::Get() { return v; }
int EffGet04::Get() { return v; }
int EffGet08::Get() { return v; }
int EffTable8C::Get(int idx) { return rows[idx][0]; }

void EffBitmapMgr::OrMask(const EffVec2 *pos, const unsigned int *mask, int h, int w)
{
    int shift = (int)(pos->x + 384.0 - (w << 4)) % 32;
    int wordX = (int)((pos->x + 192.0 - (double)w * 16.0) * 0.03125);
    int startY = (int)(pos->y - (double)h * 0.5);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            if (wordX + x >= 0 && wordX + x < 12 && startY + y >= 0 && startY + y < 448)
            {
                bitmap[(startY + y) * 12 + wordX + x] |= mask[y * w + x] >> shift;
            }
            if (wordX + x + 1 >= 0 && wordX + x + 1 < 12 && startY + y >= 0 && startY + y < 448 && shift > 0)
            {
                bitmap[(startY + y) * 12 + wordX + x + 1] |= mask[y * w + x] << (32 - shift);
            }
        }
    }
}

void EffBitmapMgr::ClearMask(const EffVec2 *pos, const unsigned int *mask, int h, int w)
{
    int shift = (int)(pos->x + 384.0 - (w << 4)) % 32;
    int wordX = (int)((pos->x + 192.0 - (double)w * 16.0) * 0.03125);
    int startY = (int)(pos->y - (double)h * 0.5);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            if (wordX + x >= 0 && wordX + x < 12 && startY + y >= 0 && startY + y < 448)
            {
                bitmap[(startY + y) * 12 + wordX + x] &= ~(mask[y * w + x] >> shift);
            }
            if (wordX + x + 1 >= 0 && wordX + x + 1 < 12 && startY + y >= 0 && startY + y < 448 && shift > 0)
            {
                bitmap[(startY + y) * 12 + wordX + x + 1] &= ~(mask[y * w + x] << (32 - shift));
            }
        }
    }
}

unsigned int EffPopCount32(unsigned int x)
{
    x = x - ((x >> 1) & 0x55555555U);
    x = (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
    x = (x + (x >> 4)) & 0x0F0F0F0FU;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x3FU;
}

int EffBitmapCountBits(EffBitmapMgr *bm)
{
    int total = 0;
    unsigned int *p = bm->bitmap;
    for (int i = 0; i < 448 * 12; ++i)
    {
        unsigned int x = *p++;
        x = x - ((x >> 1) & 0x55555555U);
        x = (x & 0x33333333U) + ((x >> 2) & 0x33333333U);
        x = (x + (x >> 4)) & 0x0F0F0F0FU;
        x = x + (x >> 8);
        x = x + (x >> 16);
        total += (int)(x & 0x3FU);
    }
    return total;
}

void EffListNode::InsertAfter(EffListNode *node)
{
    if (next != NULL)
    {
        node->next = next;
        next->prev = node;
    }
    next = node;
    node->prev = this;
}

void EffListNode::Unlink()
{
    if (next != NULL)
    {
        next->prev = prev;
    }
    if (prev != NULL)
    {
        prev->next = next;
    }
    next = NULL;
    prev = NULL;
}

int __stdcall EffCirclesOverlap2D(float x1, float y1, float r1, float x2, float y2, float r2)
{
    if ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) <= (r1 + r2) * (r1 + r2))
    {
        return 1;
    }
    return 0;
}

void __stdcall EffAdd4B4D60(float delta)
{
    g_Eff4B4D60 += delta;
}

void EffNoOp410D70() {}
void EffNoOp411D60() {}

void EffPtrArray::FreeAll()
{
    if (items != NULL)
    {
        for (int i = 0; i < count; ++i)
        {
            void *p = items[i];
            if (p != NULL)
            {
                free(p);
                items[i] = NULL;
            }
        }
        if (items != NULL)
        {
            free(items);
            items = NULL;
        }
        items = NULL;
    }
}

void EffItemMgrMini::ResetPool()
{
    EffItemMgrMini *m = g_EffItemMgr;
    memset(m->pool, 0, sizeof(m->pool));
    m->field1CAB18 = 0;
    m->field1CAB20 = 0;
}

int EffGuiMini::GetFieldC0() { return g_EffGuiMgr->fieldC0; }
int EffGuiMini::GetSubB4Field08() { return g_EffGuiMgr->subB4->field08; }
EffGuiSub *EffGuiMini::GetSubB4() { return g_EffGuiMgr->subB4; }

void EffEnemyMgrMini::CleanupAll()
{
    EffAnmMgrMini *mgr = g_EffAnmMgr;
    for (int i = 0; i < 8; ++i)
    {
        void *res = mgr->enemyAnms[i];
        for (EffAnmNodeMini *n = mgr->listA; n != NULL; )
        {
            EffAnmNodeMini *next = n->next;
            if (n->vm->anmRes == res)
            {
                n->vm->flags4B0 |= 0x20000000U;
            }
            n = next;
        }
        for (EffAnmNodeMini *n = mgr->listB; n != NULL; )
        {
            EffAnmNodeMini *next = n->next;
            if (n->vm->anmRes == res)
            {
                n->vm->flags4B0 |= 0x20000000U;
            }
            n = next;
        }
    }
    for (EffEnemyNode *n = headB8; n != NULL; )
    {
        EffEnemyNode *next = n->next;
        if (n->obj != NULL)
        {
            n->obj->vtbl->scalarDtor(n->obj, 1);
        }
        n = next;
    }
    fieldC4 = 0;
    field5C = 0;
    DisableBoth();
}

void EffEnemyMgrMini::DisableBoth()
{
    if (calcElem != NULL)
    {
        calcElem->flags &= ~2U;
    }
    if (drawElem != NULL)
    {
        drawElem->flags &= ~2U;
    }
}

void EffInputStateMini::UpdateRepeatMasks()
{
    unsigned int bits = curMask;
    unsigned int mask = 1;
    outRepeat = 0;
    outHeld8 = 0;
    unsigned int *p = repeatCounters;
    for (int i = 0; i < 32; ++i)
    {
        if ((bits & 1U) != 0)
        {
            ++(*p);
            ++(*(p + 64));
            if (*p >= 8U)
            {
                outHeld8 |= mask;
            }
            if (*p >= 26U)
            {
                outRepeat |= mask;
                *p -= 8U;
            }
        }
        else
        {
            *p = 0;
            *(p + 64) = 0;
        }
        ++p;
        bits >>= 1;
        mask <<= 1;
    }
    unsigned int cur = curMask;
    unsigned int diff = cur ^ prevMask;
    outTrigger = cur & diff;
    outRelease = ~cur & diff;
}

void EffMode558::SetMode558(int v)
{
    if ((flags590 & 0x2000U) == 0)
    {
        field558 = v;
    }
    else
    {
        field558 = 2;
    }
}

unsigned int EffInputStateMini::GetConfirmMask()
{
    return field10 & 0x80201U;
}

unsigned int EffInputStateMini::TestTrigger(unsigned int mask)
{
    return triggerMask & mask;
}

int EffInputStateMini::TestTriggerOrRepeat(unsigned int mask)
{
    if ((triggerMask & mask) != 0 || (repeatMask & mask) != 0)
    {
        return 1;
    }
    return 0;
}

void EffCursorMini::SavePrevious() { previous = value; }

int EffCursorMini::ClampSet(int v)
{
    if (count == 0)
    {
        value = v;
        return v;
    }
    if (v >= count)
    {
        value = count - 1;
        return value;
    }
    value = (v < 0) ? 0 : v;
    return value;
}

void EffCursorMini::SetFieldD0(int v) { fieldD0 = v; }
void EffCursorMini::SetCount(int c) { count = c; }
int EffCursorMini::GetValue() { return value; }

void EffCursorMini::ResetMenu()
{
    field8C = 0;
    value = 0;
    fieldD4 = 0;
    fieldD0 = 1;
    count = 999;
}

void EffZero20Init(EffZero20 *self)
{
    memset(self, 0, sizeof(EffZero20));
}

EffZero24Global::EffZero24Global()
{
    memset(this, 0, sizeof(EffZero24Global));
    d[0] |= 2;
    g_Eff4B8940 = this;
}

void EffMotion34::CopyTo(EffMotion34 &dst) const
{
    dst.pos[0] = pos[0];
    dst.pos[1] = pos[1];
    dst.pos[2] = pos[2];
    dst.vel[0] = vel[0];
    dst.vel[1] = vel[1];
    dst.vel[2] = vel[2];
    dst.f18 = f18;
    dst.f1C = f1C;
    dst.f20 = f20;
    dst.f24 = f24;
    dst.f28 = f28;
    dst.f2C = f2C;
    dst.field30 = field30;
}

int __fastcall EffMotionAttach(EffAnmVmMini *vm, const EffMotion34 *src)
{
    EffMotion34 *dst = (EffMotion34 *)operator new(sizeof(EffMotion34));
    vm->extraData = dst;
    src->CopyTo(*dst);
    vm->pos464[0] = dst->pos[0];
    vm->pos464[1] = dst->pos[1];
    vm->pos464[2] = dst->pos[2];
    return 0;
}

int EffRet1_4120C0() { return 1; }
int EffRet1_4120D0() { return 1; }
int EffRet1_4120E0() { return 1; }
int EffRet1_4120F0() { return 1; }
int EffRet0_412260() { return 0; }
int EffRet0_412390() { return 0; }
