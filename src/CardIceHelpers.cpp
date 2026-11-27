#include "CardIceHelpers.hpp"

#include <string.h>

inline void CardTimerMini::EnsureInit()
{
    if ((flags & 1U) == 0)
    {
        currentF = 0.0f;
        current = 0;
        previous = -999999;
        gameSpeed = &g_CardDefaultTimeScale;
        flags |= 1U;
    }
}

inline void CardTimerMini::ResetToMinus1()
{
    EnsureInit();
    currentF = 0.0f;
    current = 0;
    previous = -1;
}

void CardScalarInterp::SetStart(const float &v) { start = v; }
void CardScalarInterp::SetEnd(const float &v)   { end = v; }
void CardScalarInterp::SetVel0(const float &v)  { vel0 = v; }
void CardScalarInterp::SetVel1(const float &v)  { vel1 = v; }
void CardScalarInterp::SetDuration(int d)       { duration = d; }
int CardScalarInterp::GetDuration() const       { return duration; }

float CardVec2::LengthSq() const
{
    return x * x + y * y;
}

float CardVec2::Dot(const CardVec2 &rhs) const
{
    return rhs.y * y + rhs.x * x;
}

float CardVec2::Cross(const CardVec2 &rhs) const
{
    return x * rhs.y - y * rhs.x;
}

int CardVec2::IsOutOfBoundsPad(float px, float py) const
{
    if (x + px <= -192.0 || x - px >= 192.0 ||
        y + py <= 0.0    || y - py >= 448.0)
    {
        return 1;
    }
    return 0;
}

float CardVec2::DistSq(const CardVec2 &rhs) const
{
    float dy = y - rhs.y;
    float dx = x - rhs.x;
    return dx * dx + dy * dy;
}

void CardVec2::Set(float px, float py)
{
    x = px;
    y = py;
}

CardVec2 CardVec2::operator*(float s) const
{
    CardVec2 r;
    r.x = x * s;
    r.y = y * s;
    return r;
}

void CardVec3::SubInPlace(const CardVec3 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

void CardAnmVmMini::ClearBlendBits()
{
    flags4B0 &= ~0xE0U;
}

void CardAnmVmMini::SetBlendMode1()
{
    flags4B0 = (flags4B0 & ~0xC0U) | 0x20U;
}

void CardAnmVmMini::SetInterrupt2()
{
    if (onInterrupt != NULL)
    {
        onInterrupt(this, 2);
    }
    interruptId = 2;
}

void CardAnmVmMini::SetInterrupt(short id)
{
    if (onInterrupt != NULL)
    {
        onInterrupt(this, id);
    }
    interruptId = id;
}

unsigned int CardAnmVmMini::GetFlag0()
{
    return flags4B0 & 1U;
}

int *CardAnmVmMini::GetField3F0Ptr()
{
    return &field3F0;
}

unsigned int CardGetStatusBit2()
{
    return (g_CardStatus60->flags >> 2) & 1U;
}

unsigned int CardGetStatusBit0Or2()
{
    unsigned int f = g_CardStatus60->flags;
    return ((f >> 2) | f) & 1U;
}

void CardMotionMini::Init(const CardVec3 *a, const CardVec3 *b, int dur, unsigned char flag)
{
    duration = dur;
    vecC = g_CardDefaultVec;
    vecD = g_CardDefaultVec;
    flagEC = flag;
    posA = *a;
    posB = *b;
    timer.ResetToMinus1();
}

void CardNoOp40D300() {}

void __stdcall CardEncodeReplaySeed(int a, int b, CardReplayTarget *out)
{
    out->encoded = ((a + b + 22) * 1000 + (a + 66) % 1000) * 100 + (b + 33) % 100;
}

int *CardPtr11FC::Ptr() { return &v; }
int CardGuiMini::GetArr80(int idx) { return g_CardGuiMini->arr80[idx]; }
int CardGuiMini::GetArr5C(int idx) { return g_CardGuiMini->arr5C[idx]; }
void CardStageMini::ClearFlag0() { g_CardStageMini->flags37F8 &= ~1U; }
void CardStageMini::SetFlag0()   { g_CardStageMini->flags37F8 |= 1U; }

void CardGuiMini::IncCap80() { if (arr80[0] < 99999) ++arr80[0]; }
void CardGuiMini::IncCap84() { if (arr80[1] < 99999) ++arr80[1]; }
void CardGuiMini::IncCap88() { if (arr80[2] < 99999) ++arr80[2]; }
void CardGuiMini::IncCap8C() { if (arr80[3] < 99999) ++arr80[3]; }

CardReplayEntry *CardReplayMgrMini::GetEntry(int idx)
{
    return &g_CardReplayMgr->entries[idx];
}

int CardCountByteMatches250(int val)
{
    int count = 0;
    for (int i = 0; i < 250; ++i)
    {
        if (g_CardByteTable[i] == val)
        {
            ++count;
        }
    }
    return count;
}

CardFreezeMini::CardFreezeMini()
{
    timerFlags34 &= ~1U;
    memset(this, 0, sizeof(CardFreezeMini));
    flags00 |= 2U;
    g_CardFreezeMini = this;
}

void CardFreezeMini::CheckEndNoClear80()
{
    CardFreezeMini *fr = g_CardFreezeMini;
    if ((fr->flags7C & 1U) != 0)
    {
        if (fr->timer28 >= 60)
        {
            fr->flags7C &= ~0x44U;
        }
        else if (g_CardBombMini->active != 0)
        {
            fr->flags7C |= 0x40U;
        }
    }
}

int CardRet0_40D660() { return 0; }
int CardRet0_40D670() { return 0; }
int CardGet4D33F0()   { return g_CardGlobal4D33F0; }

void *CardAsciiMini::GetAnmRes() { return anmRes; }
void CardAsciiMini::SetAlphaByte(unsigned char a) { g_CardAsciiMini->alphaByte = a; }

bool CardPauseMini::IsState1() { return g_CardPauseMini->state10 == 1; }
int CardPauseMini::GetArr20(int idx) { return g_CardPauseMini->arr20[idx]; }

int CardTable148::Get(int idx) { return rows[idx][0]; }
int CardField74::Get() { return v; }
int CardField04Sub::Sub300() { return v - 300; }

void CardChainPair::EnableBoth()
{
    if (calcElem != NULL)
    {
        calcElem->flags |= 2U;
    }
    if (drawElem != NULL)
    {
        drawElem->flags |= 2U;
    }
}

void CardNoOp40E5D0() {}
int CardField4C::Get() { return v; }
void CardNoOp40E5F0() {}
int CardPlayerMini::GetField10() { return g_CardPlayerMini->field10; }
void *CardBulletIter::GetSub9D8() { return sub9D8; }
float CardBulletIter::GetF9F8() { return f9F8; }
float CardBulletIter::GetFA00() { return fA00; }
unsigned int CardBulletIter::GetFlag0() { return flags00 & 1U; }
int *CardPtr08::Ptr() { return &v; }

void CardEnemyMini::SetMode12()
{
    field38 = 1;
    field48 = 2;
}

void CardEnemyMini::SetTargetPos(float px, float py)
{
    field78 = 0;
    f54 = px;
    field64 = 60;
    field74 = 4;
    f58 = py;
}

float CardEnemyMini::GetF68() { return f68; }
float CardEnemyMini::GetF6C() { return f6C; }
void *CardBulletMgrMini::GetPool() { return g_CardBulletMgr->pool; }
int CardField10::Get() { return v; }

CardLaserNode *CardLaserMgrMini::BeginIter()
{
    CardLaserMgrMini *m = g_CardLaserMgr;
    m->cur = m->head;
    return m->head;
}

CardLaserNode *CardLaserMgrMini::NextIter()
{
    CardLaserMgrMini *m = g_CardLaserMgr;
    if (m->cur != NULL)
    {
        m->cur = m->cur->next;
        return m->cur;
    }
    return NULL;
}

bool CardZeroCheck::IsZero() { return v == 0; }
int *CardPtr10::Ptr() { return &v; }

CardEffectSub54::CardEffectSub54()
{
    flags4C &= ~1U;
}

CardEffectSub68::CardEffectSub68()
{
    flags30 &= ~1U;
    flags44 &= ~1U;
}

CardEffectMgrMini::CardEffectMgrMini()
{
    flags6B1F8 &= ~1U;
    memset(this, 0, sizeof(CardEffectMgrMini));
    flags00 |= 2U;
    g_CardEffectMgr = this;
}

int CardEffectMgrMini::GetField6B200() { return field6B200; }

void CardEffectMgrMini::EnableChainElems()
{
    CardEffectMgrMini *m = g_CardEffectMgr;
    m->calcElem->flags |= 2U;
    m->drawElem->flags |= 2U;
}

void CardNoOp40E810() {}
