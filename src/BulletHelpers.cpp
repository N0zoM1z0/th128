#include "BulletHelpers.hpp"

#include <stdlib.h>
#include <string.h>

inline void BulletTimerMini::EnsureInit()
{
    if ((flags & 1U) == 0)
    {
        currentF = 0.0f;
        current = 0;
        previous = -999999;
        gameSpeed = &g_BulletDefaultTimeScale;
        flags |= 1U;
    }
}

inline void BulletTimerMini::SetInt(int v)
{
    EnsureInit();
    current = v;
    previous = v - 1;
    currentF = (float)v;
}

inline void BulletTimerMini::ResetToMinus1()
{
    EnsureInit();
    currentF = 0.0f;
    current = 0;
    previous = -1;
}

BulletVec3 BulletVec3::operator+(const BulletVec3 &rhs) const
{
    BulletVec3 r;
    r.x = x + rhs.x;
    r.y = y + rhs.y;
    r.z = z + rhs.z;
    return r;
}

BulletVec3 BulletVec3::operator-(const BulletVec3 &rhs) const
{
    BulletVec3 r;
    r.x = x - rhs.x;
    r.y = y - rhs.y;
    r.z = z - rhs.z;
    return r;
}

void BulletVec3::Set(float px, float py, float pz)
{
    x = px;
    y = py;
    z = pz;
}

void BulletVec3::AddInPlace(const BulletVec3 &rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

void BulletVec3::ScaleInPlace(float s)
{
    x *= s;
    y *= s;
    z *= s;
}

float BulletVec3::LengthSq2D() const
{
    return x * x + y * y;
}

float BulletVec3::GetX() const  { return x; }
float BulletVec3::GetX2() const { return x; }
float BulletVec3::GetZ() const  { return z; }
float BulletVec3::GetZ2() const { return z; }

BulletAngle *BulletAngle::Sub(float a)
{
    value = BulletNormalizeAngle(value - a);
    return this;
}

BulletAngle *BulletAngle::Add(float a)
{
    value = BulletNormalizeAngle(value + a);
    return this;
}

void BulletTimerMini::SetScaleInt(int v)
{
    SetScaleFloat((float)v);
}

void BulletTimerMini::SetScaleNegInt(int v)
{
    SetScaleFloat((float)(-v));
}

void BulletTimerMini::SetScaleMinusOne()
{
    SetScaleFloat(-1.0f);
}

BulletTimerMini *BulletTimerMini::ClearFlag0()
{
    flags &= ~1U;
    return this;
}

void BulletAnmManagerMini::ResetColorMod()
{
    BulletAnmManagerMini *m = g_BulletAnmManager;
    m->colorModEnabled = 0;
    m->colorMod = 0x80808080U;
}

void BulletAnmManagerMini::SetColorMod(unsigned int c)
{
    BulletAnmManagerMini *m = g_BulletAnmManager;
    m->colorModEnabled = 1;
    m->colorMod = c;
}

int BulletField08::Get()          { return v; }
int BulletField04::Get()          { return v; }
unsigned int BulletFlag90::GetBit0() { return f & 1U; }
int BulletField54::Get()          { return v; }
int BulletField5A34::Get()        { return v; }
BulletRetThis *BulletRetThis::Self() { return this; }

BulletVec3 *PlayerMini::GetPosPtr() { return &pos; }

bool FreezeInfMini::IsTimerGe60()
{
    return timer28 >= 60;
}

void FreezeInfMini::CheckEnd()
{
    FreezeInfMini *fr = g_FreezeInfMini;
    if ((fr->flags7C & 1U) != 0)
    {
        if (fr->timer28 >= 60)
        {
            fr->flags7C &= ~0x46U;
            fr->field80 = 0;
        }
        else if (g_BombInfMini->active != 0)
        {
            fr->flags7C |= 0x40U;
        }
    }
}

void PlayerMini::SetFloat87B0(float v)
{
    float87B0 = v;
}

void __stdcall PlayerMini::SetInvulnTimer(int v)
{
    g_PlayerMini->invulnTimer.SetInt(v);
}

bool BombInfMini::IsActive()
{
    return g_BombInfMini->active != 0;
}

int BulletGetModeIndex()
{
    return g_GameModeA * 2 + g_GameModeB;
}

void *EnemyMgrMini::GetField65DDC()
{
    return field65DDC;
}

void EnemyMgrMini::SetFlag6B1FCBit2()
{
    g_EnemyMgrMini->flags6B1FC |= 4U;
}

void EnemyMgrMini::Trigger6B200()
{
    EnemyMgrMini *e = g_EnemyMgrMini;
    e->field6B200 = 12000;
    e->field6B218 = 1;
}

void BulletItemMini::ResetTimersAndClearFlag0()
{
    stateA2A = 0;
    timer1.ResetToMinus1();
    timer2.ResetToMinus1();
    flags &= ~1U;
}

BulletItemMini::~BulletItemMini()
{
    if (extraB != NULL)
    {
        free(extraB);
    }
    extraB = NULL;
    if (extraA != NULL)
    {
        free(extraA);
    }
    extraA = NULL;
}

BulletSub380 *BulletSub380::Reset()
{
    memset(this, 0, sizeof(BulletSub380));
    field374 = -1;
    return this;
}

BulletZero54 *BulletZero54::Zero()
{
    memset(this, 0, sizeof(BulletZero54));
    return this;
}

BulletAnmResMini *BulletManagerMini::GetAnm()
{
    return g_BulletManagerMini->anm;
}

int BulletManagerMini::UnloadAnm()
{
    BulletAnmManagerMini *m = g_BulletAnmManager;
    if (m->bulletAnm != NULL)
    {
        m->bulletAnm->Release();
        operator delete(m->bulletAnm);
        m->bulletAnm = NULL;
    }
    return 0;
}

int BulletGetTypeField00(int idx)
{
    return g_BulletTypes[idx].field00;
}

float BulletGetTypeField104(int idx)
{
    return g_BulletTypes[idx].field104;
}

float BulletGetTypeField108(int idx)
{
    return g_BulletTypes[idx].field108;
}

int BulletBoundsCheck::IsOutOfBoundsTall(float w, float h)
{
    if (x + w * 0.5 <= -192.0 || x - w * 0.5 >= 192.0 ||
        y + h * 0.5 <= -64.0  || y - h * 0.5 >= 448.0)
    {
        return 1;
    }
    return 0;
}

int BulletBoundsCheck::IsOutOfBounds(float w, float h)
{
    if (x + w * 0.5 <= -192.0 || x - w * 0.5 >= 192.0 ||
        y + h * 0.5 <= 0.0    || y - h * 0.5 >= 448.0)
    {
        return 1;
    }
    return 0;
}

void BulletNoOp1() {}
void BulletNoOp2() {}
int BulletRet1A() { return 1; }
int BulletRet1B() { return 1; }
int BulletRet0A() { return 0; }
int BulletRet0B() { return 0; }
void BulletNoOp3() {}
void BulletNoOp4() {}
int BulletRet1C() { return 1; }
int BulletRet0C() { return 0; }
int BulletRet0D() { return 0; }
int BulletRet0E() { return 0; }
