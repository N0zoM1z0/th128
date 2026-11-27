#include "PlayerReplayHelpers.hpp"

#include <math.h>
#include <string.h>

void PrIncGrazeClamped()
{
    if (g_Pr4B4D40 < 99999999)
    {
        ++g_Pr4B4D40;
    }
}

float __stdcall PrAngleDiffA(float a, float b)
{
    if (a - b > 3.1415927410125732)
    {
        return (float)(a - (b + 6.2831854820251465));
    }
    if (b - a > 3.1415927410125732)
    {
        return (float)(a - (b - 6.2831854820251465));
    }
    return a - b;
}

float __stdcall PrAngleDiffB(float a, float b)
{
    if (a - b > 3.1415927410125732)
    {
        return (float)(a - (b + 6.2831854820251465));
    }
    if (b - a > 3.1415927410125732)
    {
        return (float)(a - (b - 6.2831854820251465));
    }
    return a - b;
}

float __stdcall PrFabsFloat(float v) { return (float)fabs(v); }

void PrVec2FloatToFixed128(const PrVec3 *in, PrIVec2 *out)
{
    out->x = (int)(in->x * 128.0);
    out->y = (int)(in->y * 128.0);
}

int PrGet4B4CC8Div4B4D38() { return g_Pr4B4CC8 / g_Pr4B4D38; }

float PrVec3::DistSq2DA(const PrVec3 *other)
{
    return (x - other->x) * (x - other->x) + (y - other->y) * (y - other->y);
}

float PrVec3::DistSq2DB(const PrVec3 *other)
{
    return (x - other->x) * (x - other->x) + (y - other->y) * (y - other->y);
}

int PrWord418::Get418() { return field418; }

int PrTimer::IsTickMultiple(int mod)
{
    if (current != previous && (current % mod) == 0)
    {
        return 1;
    }
    return 0;
}

void PrIVec2::ToVec3Scaled(PrVec3 *out)
{
    out->x = (float)(x * 0.0078125);
    out->y = (float)(y * 0.0078125);
    out->z = 0.0f;
}

PrIVec2 *PrIVec2::AddAssign(PrIVec2 *out, const PrIVec2 *rhs)
{
    x += rhs->x;
    y += rhs->y;
    out->x = x;
    out->y = y;
    return out;
}

int PrIVec2::IsNonZero()
{
    if (x == 0 && y == 0)
    {
        return 0;
    }
    return 1;
}

void PrIVec2::SubTo(PrIVec2 *out, const PrIVec2 *rhs)
{
    out->x = x - rhs->x;
    out->y = y - rhs->y;
}

void PrIVec2::MulTo(PrIVec2 *out, int scale)
{
    out->x = x * scale;
    out->y = y * scale;
}

void PrIVec2::Div100To(PrIVec2 *out)
{
    out->x = x / 100;
    out->y = y / 100;
}

void PrIVec2::Set(int a, int b)
{
    x = a;
    y = b;
}

void PrIVec2::AddTo(PrIVec2 *out, const PrIVec2 *rhs)
{
    out->x = x + rhs->x;
    out->y = y + rhs->y;
}

void PrSet4B4D38(int v) { g_Pr4B4D38 = v; }
void PrSet4B4D34(int v) { g_Pr4B4D34 = v; }
int PrObj38::Get38() { return field38; }
int PrGet4B4CF4() { return g_Pr4B4CF4; }

void PrIVec2::CopyTo(PrIVec2 *out, const PrIVec2 *rhs)
{
    x = rhs->x;
    y = rhs->y;
    out->x = x;
    out->y = y;
}

int PrObj74::Get74() { return field74; }
PrPlayerOpt *PrPlayerExact::GetOptPtr(int idx) { return &g_PrPlayer->opts8810[idx]; }
float PrPlayerExact::GetOptFloat4C(int idx) { return g_PrPlayer->opts8810[idx].field4C; }
void PrPlayerExact::ReadOpt54(int idx, int *out) { *out = opts8810[idx].field54; }

int PrInputFlags::IsFlag3EE4Set()
{
    if ((flags3EE4 & 0x21U) != 0 || (flags3EE4 & 0x18000000U) != 0)
    {
        return 1;
    }
    return 0;
}

int PrInputFlags::IsFlag50ACSet()
{
    if ((flags50AC & 0x21U) != 0 || (flags50AC & 0x18000000U) != 0)
    {
        return 1;
    }
    return 0;
}

int PrInputFlags::Get11C0() { return field11C0; }
int PrEnemyMgr::Get6B218() { return g_PrEnemyMgr->field6B218; }

void PrComputeBoxBounds(const PrVec3 *center, const PrVec3 *size, PrVec3 *outMin, PrVec3 *outMax)
{
    outMin->x = (float)(center->x - size->x * 0.5);
    outMin->y = (float)(center->y - size->y * 0.5);
    outMax->x = (float)(center->x + size->x * 0.5);
    outMax->y = (float)(center->y + size->y * 0.5);
}

void PrBulletHit::Zero54And64A()
{
    field54 = 0;
    field64 = 0;
}

void PrBulletHit::Zero54And64B()
{
    field54 = 0;
    field64 = 0;
}

int PrRet0_4404A0() { return 0; }

void PrPlayerExact::ClearTargetId(int id)
{
    if (targetId8ED4 == id)
    {
        targetId8ED4 = 0;
        targetByte8ED8 = 0;
    }
    for (int i = 0; i < 256; ++i)
    {
        int *slot = (int *)(bulletsFFC + i * 0x78);
        if (*slot == id)
        {
            *slot = 0;
        }
    }
}

void PrBulletHit::AddField18(float v) { field18 += v; }

void PrPlayerExact::SetFixedPos(const PrIVec2 *p)
{
    PrPlayerExact *pl = g_PrPlayer;
    pl->fixedPosED8.x = p->x;
    pl->fixedPosED8.y = p->y;
    pl->posECC.x = (float)(pl->fixedPosED8.x * 0.0078125);
    pl->posECC.y = (float)(pl->fixedPosED8.y * 0.0078125);
    for (int i = 0; i < 8; ++i)
    {
        *(int *)((unsigned char *)&pl->opts8810[0].flag78 + i * 0xE4) = 1;
    }
}

float PrFpsCounter::Get34() { return g_PrFpsCounter->field34; }

int PrReplayMgr::GetFastForwardStepA()
{
    if (g_PrGameThread != NULL && mode10 == 1 && (g_Pr4DAFF8 & 0x200U) != 0 && (frame3A4 % 6) != 0)
    {
        return 6;
    }
    return 1;
}

int PrReplayMgr::GetFastForwardStepB()
{
    if (g_PrGameThread != NULL && (g_PrGameThread->flags60 & 4U) == 0 && mode10 == 1
        && (g_Pr4DAFF8 & 0x200U) != 0 && (frame3A4 % 6) != 0)
    {
        return 6;
    }
    return 1;
}

char *PrReplayMgr::GetTagStr()
{
    *(unsigned int *)g_PrTagBuf = tag3BC;
    g_PrTagBuf[4] = '\0';
    return g_PrTagBuf;
}

void PrStageEntry24::ResetZero()
{
    f04 = f00;
    f0C = f08;
    f14 = 0;
}

void PrUserTag::SetUserMagic() { tag = 0x52455355U; }
int PrReplayChunk::GetFpsCount() { return (int)(fpsPtr18A0 - fpsBytes151C); }
int PrReplayChunk::GetInputByteCount() { return (int)((inputPtr1518 - inputs) * 6); }
int PrReplayChunk::GetInputCount() { return (int)(inputPtr1518 - inputs); }
int PrReplayChunk::GetTotalByteCount() { return (int)((inputPtr1518 - inputs) * 6 + (fpsPtr18A0 - fpsBytes151C)); }

int PrReplayChunk::PushInput(unsigned short a, unsigned short b, unsigned short c)
{
    inputPtr1518->a = a;
    inputPtr1518->b = b;
    inputPtr1518->c = c;
    ++inputPtr1518;
    return (inputPtr1518 - inputs) >= 900;
}

int PrReplayChunk::PushFps(unsigned char v)
{
    *fpsPtr18A0 = v;
    ++fpsPtr18A0;
    return (fpsPtr18A0 - fpsBytes151C) >= 30;
}

void PrReplayState84::Set1C(int v) { field1C = v; }
void PrReplayState84::Set38(int v) { field38 = v; }
void PrReplayState84::Set84(int v) { field84 = v; }

void PrStageEntry24::ResetNeg1()
{
    f04 = f00;
    f0C = f08;
    f14 = -1;
}

int PrReplayState84::Get80() { return field80; }
int PrReplayState84::Get1C() { return field1C; }
int PrGet4D33CC() { return g_Pr4D33CC; }
void PrZero4D2AE4() { g_Pr4D2AE4 = 0; }
unsigned short PrGet4D2AE0() { return g_Pr4D2AE0; }
PrBuf90::PrBuf90() { memset(this, 0, sizeof(PrBuf90)); }

void PrReplayFileHeader::InitHeader()
{
    ver04 = 0;
    pad06 = 0;
    f08 = 0;
    f0C = 0;
    f14 = 0;
    f18 = 0;
    f1C = 0;
    f20 = 0;
    magic00 = 0x72383231U;
    ver04 = 2;
    f10 = 0x100;
}

PrReplayChunk::PrReplayChunk()
{
    memset(this, 0, sizeof(PrReplayChunk));
    fpsPtr18A0 = fpsBytes151C;
    inputPtr1518 = inputs;
    node18A4.owner = this;
    node18A4.next = NULL;
    node18A4.prev = NULL;
}

void PrReplayList::AllocAndAppendChunk(int stageIdx)
{
    PrReplayChunk *chunk = new PrReplayChunk();
    PrReplayChunkNode *cur = &heads64[stageIdx];
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    PrReplayChunkNode *n = &chunk->node18A4;
    if (cur->next != NULL)
    {
        n->next = cur->next;
        cur->next->prev = n;
    }
    cur->next = n;
    n->prev = cur;
    ++chunkCount134;
}

void PrReplayChunk::UnlinkNode()
{
    if (node18A4.next != NULL)
    {
        node18A4.next->prev = node18A4.prev;
    }
    if (node18A4.prev != NULL)
    {
        node18A4.prev->next = node18A4.next;
    }
    node18A4.next = NULL;
    node18A4.prev = NULL;
}

PrReplayChunk *PrReplayChunk::DeleteSelf()
{
    if (node18A4.next != NULL)
    {
        node18A4.next->prev = node18A4.prev;
    }
    if (node18A4.prev != NULL)
    {
        node18A4.prev->next = node18A4.next;
    }
    node18A4.next = NULL;
    node18A4.prev = NULL;
    operator delete(this);
    return this;
}

void PrReplayList::FreeStageChunks(int stageIdx)
{
    PrReplayChunkNode *cur = heads64[stageIdx].next;
    while (cur != NULL)
    {
        PrReplayChunkNode *next = cur->next;
        PrReplayChunk *owner = (PrReplayChunk *)cur->owner;
        if (owner != NULL)
        {
            if (owner->node18A4.next != NULL)
            {
                owner->node18A4.next->prev = owner->node18A4.prev;
            }
            if (owner->node18A4.prev != NULL)
            {
                owner->node18A4.prev->next = owner->node18A4.next;
            }
            owner->node18A4.next = NULL;
            owner->node18A4.prev = NULL;
            operator delete(owner);
        }
        cur = next;
    }
}

PrStageEntry24::PrStageEntry24()
{
    f00 = 0; f04 = 0; f08 = 0; f0C = 0; f10 = 0; f14 = 0;
    node18.owner = NULL; node18.next = NULL; node18.prev = NULL;
    f04 = f00;
    f0C = f08;
    node18.owner = this;
    node18.next = NULL;
    node18.prev = NULL;
}

PrStageEntry24::~PrStageEntry24()
{
    if (node18.next != NULL)
    {
        node18.next->prev = node18.prev;
    }
    if (node18.prev != NULL)
    {
        node18.prev->next = node18.next;
    }
    node18.next = NULL;
    node18.prev = NULL;
}

unsigned int __stdcall PrChecksumBytes(const unsigned char *buf, int totalSize)
{
    int len = totalSize - 8;
    unsigned int sum = 0;
    for (int i = 0; i < len; ++i)
    {
        sum += buf[i + 8];
    }
    return sum;
}

void PrScoreFile::InitScoreFile()
{
    memset(this, 0, sizeof(PrScoreFile));
    magic00 = 0x14443U;
    size08 = sizeof(PrScoreFile);
    for (int i = 0; i < 250; ++i)
    {
        entries[i].id = i;
        entries[i].val = g_PrScoreTable[i];
    }
}
