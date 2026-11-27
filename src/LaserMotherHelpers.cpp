#include "LaserMotherHelpers.hpp"

#include <math.h>
#include <stdlib.h>
#include <string.h>

int LmLaserMgr::CancelInBoxA(const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b)
{
    LmLaserNode *cur = head18;
    boxMin5DC = *minP;
    boxMax5E8 = *maxP;
    int total = 0;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if (cur->state0C != 1 && cur->byte7D != 0)
        {
            total += cur->vtbl->cancelBoxA(cur, minP, maxP, f, a, b);
        }
        cur = next;
    }
    return total;
}

int LmLaserMgr::CancelInBoxB(const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b, int c)
{
    LmLaserNode *cur = head18;
    int total = 0;
    flags5F8 = 0;
    boxMin5DC = *minP;
    boxMax5E8 = *maxP;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if (cur->state0C != 1 && cur->byte7D != 0)
        {
            total += cur->vtbl->cancelBoxB(cur, minP, maxP, f, a, b, c);
        }
        cur = next;
    }
    return total;
}

int LmLaserMgr::ResetAllTimersAndCancel()
{
    LmLaserNode *cur = g_LmLaserMgr->head18;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if ((cur->timer5A0.flags & 1U) == 0)
        {
            cur->timer5A0.currentF = 0.0f;
            cur->timer5A0.current = 0;
            cur->timer5A0.previous = -999999;
            cur->timer5A0.gameSpeed = &g_LmDefaultTimeScale;
            cur->timer5A0.flags |= 1U;
        }
        cur->timer5A0.currentF = 0.0f;
        cur->timer5A0.current = 0;
        cur->timer5A0.previous = -1;
        cur->field5B4 = 0;
        cur->vtbl->cancel(cur, 1, 0);
        cur = next;
    }
    return 0;
}

int __stdcall LmLaserMgr::CancelInCircle(const LmVec3 *pos, float f, int a, int b)
{
    LmLaserMgr *mgr = g_LmLaserMgr;
    LmLaserNode *cur = mgr->head18;
    mgr->boxMin5DC = *pos;
    int total = 0;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if (cur->state0C != 1)
        {
            total += cur->vtbl->cancelCircle(cur, pos, f, a, b);
        }
        cur = next;
    }
    return total;
}

int LmLaserMgr::CancelAll(int a, int b)
{
    LmLaserNode *cur = g_LmLaserMgr->head18;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if (cur->state0C != 1)
        {
            cur->vtbl->cancel(cur, a, b);
        }
        cur = next;
    }
    return 1;
}

int LmLaserMgr::CheckAll30(int a, float f)
{
    int total = 0;
    LmLaserNode *cur = head18;
    while (cur != NULL)
    {
        LmLaserNode *next = cur->next;
        if (cur->state0C != 1)
        {
            total += cur->vtbl->check30(cur, a, f);
        }
        cur = next;
    }
    return total;
}

int LmLaserLine::GetTableVal(int idx)
{
    LmLaserOwner *o = owner4E4;
    if (g_LmLaserTable[o->type5B8][0][0] < 0)
    {
        return idx;
    }
    return g_LmLaserTable[o->type5B8][o->sub5BC][idx];
}

int LmRet0_42C170() { return 0; }
int LmRet0_42DB90() { return 0; }

void LmLaserInf::ProcessCmds()
{
    while (cmdIdx594 < 18)
    {
        LmLaserCmd *cmd = &cmds61C[cmdIdx594];
        int type = cmd->type20;
        if (type == 0)
        {
            break;
        }
        if (cmd->flag24 == 0 && field598 != 0)
        {
            break;
        }
        if (type == 0x80)
        {
            field5B4 = cmd->arg10;
        }
        else if (type == 0x400)
        {
            state0C = 3;
        }
        else if (type == 0x100000)
        {
            if (cmd->arg10 != 0)
            {
                flagsDE8 = (flagsDE8 & ~0xC0U) | 0x20U;
            }
            else
            {
                flagsDE8 &= ~0xE0U;
            }
            ++cmdIdx594;
            continue;
        }
        ++cmdIdx594;
    }
}

void LmCopyCurveNode(const LmCurveNode *src, LmCurveNode *dst)
{
    dst->next = src->next;
    dst->prev = src->prev;
    dst->f08 = src->f08;
    dst->f0C = src->f0C;
    dst->i10 = src->i10;
    dst->i14 = src->i14;
    dst->i18 = src->i18;
    dst->i1C = src->i1C;
    dst->i20 = src->i20;
    dst->i24 = src->i24;
    dst->i28 = src->i28;
    dst->f2C = src->f2C;
    dst->f30 = src->f30;
    dst->f34 = src->f34;
    dst->f38 = src->f38;
}

int LmLaserCurve::Cleanup()
{
    LmCurveNode *cur = head12EC;
    while (cur != NULL)
    {
        LmCurveNode *next = cur->next;
        operator delete(cur);
        cur = next;
    }
    if (buf12E8 != NULL)
    {
        free(buf12E8);
        buf12E8 = NULL;
    }
    if (buf12E4 != NULL)
    {
        free(buf12E4);
        buf12E4 = NULL;
    }
    return 0;
}

int LmLaserLine::GetSubPlus234() { return owner4E4->sub5BC + 0x234; }
int LmRet0_432A70() { return 0; }
int __stdcall LmRet0_S5(int, int, int, int, int) { return 0; }
int __stdcall LmRet0_S6(int, int, int, int, int, int) { return 0; }
int __stdcall LmRet0_S4(int, int, int, int) { return 0; }

int LmLaserNode::SetByte7CIfZero(int, int b)
{
    if (b == 0)
    {
        byte7C = 1;
    }
    return 0;
}

int __stdcall LmRet0_S2(int, int) { return 0; }
void LmNoOp432AE0() {}
void LmNoOp432B30() {}
float LmGetFloat88(int idx, const LmArray88 *a) { return a->arr88[idx].x; }

void LmTimer::SetFloat(float v)
{
    if ((flags & 1U) == 0)
    {
        currentF = 0.0f;
        current = 0;
        previous = -999999;
        gameSpeed = &g_LmDefaultTimeScale;
        flags |= 1U;
    }
    current = (int)v;
    currentF = v;
    previous = current - 1;
}

float LmVec3::DistSq2D(const LmVec3 *other)
{
    return (x - other->x) * (x - other->x) + (y - other->y) * (y - other->y);
}

void LmVec3::ScaleTo(LmVec3 *out, float s)
{
    out->x = x * s;
    out->y = y * s;
    out->z = z * s;
}

int LmVec3::IsOutsidePlayfield(float halfW, float halfH)
{
    if (x + halfW <= -192.0 || x - halfW >= 192.0)
    {
        return 1;
    }
    if (y + halfH <= 0.0 || y - halfH >= 448.0)
    {
        return 1;
    }
    return 0;
}

void LmNoOp432DE0() {}
int LmIsNZ4::IsNonZero4() { return field04 != 0; }

void LmVec3::FromAngle(float angle, float radius)
{
    x = cos(angle) * radius;
    y = sin(angle) * radius;
}

LmMotherSub10::LmMotherSub10()
{
    vtbl = &g_LmSub10Vtbl;
    f04 = 0;
    f08 = 0;
    f0C = 0;
    f10 = 0;
}

LmMother::LmMother()
{
    memset(this, 0, sizeof(LmMother));
    flags00 |= 2U;
    g_LmMother = this;
}

LmMother *LmMother::Create()
{
    LmMother *m = new LmMother();
    if (LmMotherInit(m) != 0)
    {
        if (m != NULL)
        {
            LmMotherDtor(m);
            operator delete(m);
        }
        return NULL;
    }
    return m;
}

LmMother *LmMother::DestroySelf()
{
    LmMotherDtor(this);
    operator delete(this);
    return this;
}

void LmMother::DestroyGlobal()
{
    LmMother *m = g_LmMother;
    if (m != NULL)
    {
        LmMotherDtor(m);
        operator delete(m);
    }
}

int LmMother::OnCalcInner()
{
    if ((flags00 & 2U) != 0)
    {
        Lm435D20(&g_Lm4D2E68);
        g_Lm4D59E8 = 1;
        LmSupervisorChains *s = g_LmSupervisor;
        s->node0C->flags04 |= 2U;
        s->node10->flags04 |= 2U;
        s->node1C8A0->flags04 |= 2U;
        s->node1C8A4->flags04 |= 2U;
        g_Lm4D33F8 &= ~0x2000U;
        g_Lm4D33C0 = 4;
        flags00 &= ~2U;
    }
    return 1;
}

int LmMother::OnCalcHook()
{
    return OnCalcInner();
}

void LmPauseAllChains()
{
    LmSupervisorChains *s = g_LmSupervisor;
    s->node0C->flags04 |= 2U;
    s->node10->flags04 |= 2U;
    s->node1C8A0->flags04 |= 2U;
    s->node1C8A4->flags04 |= 2U;
}

void LmObj558::Set558(int v) { field558 = v; }
int LmObj558::GetFlag200Bit4() { return (int)((flags200 >> 4) & 1U); }
void LmNoOp433620() {}
