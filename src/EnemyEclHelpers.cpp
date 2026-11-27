#include "EnemyEclHelpers.hpp"

#include <stdlib.h>
#include <string.h>

int EclRet1_4136D0() { return 1; }
int EclRet1_413720() { return 1; }

int EclGetKeyHoldFrames(int bit)
{
    if ((g_EclInputMask & (1U << bit)) != 0)
    {
        return g_EclKeyHoldTable[bit];
    }
    return 0;
}

unsigned int __stdcall EclTestInputMask(unsigned int mask)
{
    return g_EclInputMask & mask;
}

int EclGet84::Get() { return v; }
int EclIsZ0::IsZero() { return v == 0; }
void EclNoOp414260() {}
int EclGet68::Get() { return v; }

void __stdcall EclSet4B8930Field60(float v)
{
    g_Ecl4B8930->field60 = v;
}

void EclClear1000A::Clear() { a = 0; b = 0; }
void EclClear1000B::Clear() { a = 0; b = 0; }

void EclSub1000::ResetSlots()
{
    field1008 = 0;
    field100C = 0;
    slots[0].flags &= ~1U;
    slots[1].flags &= ~1U;
    slots[2].flags &= ~1U;
    slots[3].flags &= ~1U;
    slots[4].flags &= ~1U;
    slots[5].flags &= ~1U;
    slots[6].flags &= ~1U;
    slots[7].flags &= ~1U;
    slots[0].val = 0;
    slots[1].val = 0;
    slots[2].val = 0;
    slots[3].val = 0;
    slots[4].val = 0;
    slots[5].val = 0;
    slots[6].val = 0;
    slots[7].val = 0;
}

int *EclCtxHolder::GetIntArg0()
{
    EclCtxInner *c = ctx;
    EclInstrHdr *h = c->hdr;
    if ((h->flags & 1) != 0)
    {
        int id = h->intArgs[0];
        if (id >= 0)
        {
            return (int *)(c->stackBuf + c->stackTop + id);
        }
        return c->vm->vtbl->getIntVar(c->vm, id);
    }
    return NULL;
}

void EclCtxHolder::Tail46FF70()
{
    EclTailTarget46FF70(ctx);
}

float *EclCtxHolder::GetFloatArg(int idx)
{
    EclCtxInner *c = ctx;
    EclInstrHdrF *h = (EclInstrHdrF *)c->hdr;
    if ((h->flags & (1 << idx)) != 0)
    {
        float v = h->floatArgs[idx];
        if (v >= 0.0f)
        {
            return (float *)(c->stackBuf + c->stackTop + (int)v);
        }
        return c->vm->vtbl->getFloatVar(c->vm, (int)v);
    }
    return NULL;
}

void EclCtxHolder::SubTime(float dt)
{
    ctx->time -= dt;
}

void EclHostClean::FreeList()
{
    for (EclListNode *n = listHead; n != NULL; )
    {
        EclListNode *next = n->next;
        operator delete(n->data);
        operator delete(n);
        n = next;
    }
}

EclHostClean::EclHostClean()
{
    vtbl = &g_EclHostVtbl;
    sub08.ResetSlots();
    sub08.slots[0].val = 0;
    sub08.slots[1].val = 0;
    sub08.slots[2].val = 0;
    sub08.slots[3].val = 0;
    sub08.slots[4].val = 0;
    sub08.slots[5].val = 0;
    sub08.slots[6].val = 0;
    sub08.slots[7].val = 0;
}

void EclHostClean::InitFields()
{
    flags11A8 &= ~1U;
    *(float *)((char *)&sub08) = 0.0f;
    *(int *)((char *)&sub08 + 4) = 0;
    *(EclHostClean **)((char *)this + 0x101C) = this;
    *(int *)((char *)this + 0x1018) = -1;
    *(int *)((char *)this + 0x1020) = 0;
    sub08.slots[0].val = 0;
    sub08.slots[1].val = 0;
    sub08.slots[2].val = 0;
    sub08.slots[3].val = 0;
    sub08.slots[4].val = 0;
    sub08.slots[5].val = 0;
    sub08.slots[6].val = 0;
    sub08.slots[7].val = 0;
    field04 = &sub08;
    *(int *)((char *)this + 0x1010) = 0;
    *(int *)((char *)this + 0x1014) = 0;
    field11B0 = &sub08;
    listHead = NULL;
    field11B8 = 0;
}

void EclHostClean::DestroyHost()
{
    vtbl = &g_EclHostVtbl;
    FreeList();
}

int EclArr0C::Get(int idx) { return a[idx]; }

EclSub1098::EclSub1098()
{
    vtbl = &g_EclSub1098Vtbl;
    field1090 = 0;
    field1094 = 0;
    memset(this, 0, sizeof(EclSub1098));
}

EclSub1098::~EclSub1098()
{
    vtbl = &g_EclSub1098Vtbl;
    if (buf8C != NULL)
    {
        free(buf8C);
        buf8C = NULL;
    }
}

void EclHp::SetHpA(int v) { hp = v; }
int EclHp::GetHp() { return hp; }
void EclHp::SetHpB(int v) { hp = v; }
int EclHp::GetFlag1() { return (flags14 >> 1) & 1U; }
void EclHp::SetRawHpMul7(int v) { rawHp = v * 7; }

void EclHp::SetFlag0(int bit)
{
    flags14 ^= (flags14 ^ (unsigned int)bit) & 1U;
}

int EclHp::GetField04() { return field04; }
void EclHp::SetField04(int v) { field04 = v; }
int EclHp::GetField08() { return field08; }
void EclHp::SetField08(int v) { field08 = v; }
void EclHp::SetMinHp(int v) { minHp = v; }
void EclHp::SetHpC(int v) { hp = v; }

int EclHp::ApplyDamage(int dmg)
{
    if ((flags14 & 1U) != 0)
    {
        rawHp -= dmg;
        hp = (rawHp - minHp * 7) / 7 + minHp;
        return hp;
    }
    hp -= dmg;
    return hp;
}
