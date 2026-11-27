#include "AsciiInfHelpers.hpp"

#include <stddef.h>
#include <string.h>

int GetGameManagerField4()
{
    return g_GameManagerMini->field04;
}

AsciiEntry *AsciiEntry::ClearFlag0()
{
    flags &= ~1U;
    return this;
}

AsciiInf *AsciiInf::Create()
{
    AsciiInf *p = new AsciiInf();
    if (p->Init() != 0)
    {
        if (p != NULL)
        {
            delete p;
        }
        return NULL;
    }
    return p;
}

AsciiInf *AsciiInf::Destroy(AsciiInf *p)
{
    delete p;
    return p;
}

void AsciiInf::DestroyGlobal()
{
    if (g_AsciiInf != NULL)
    {
        delete g_AsciiInf;
    }
}

int AsciiInf::OnTickInner()
{
    ResetStyle();
    UpdatePopups();
    ++frameCount;
    return 1;
}

int AsciiInf::OnTick()
{
    ResetStyle();
    UpdatePopups();
    ++frameCount;
    return 1;
}

int AsciiInf::OnDraw0()
{
    return DrawLayer0();
}

int AsciiInf::OnDraw2()
{
    DrawLayer(2);
    return 1;
}

AsciiEntry *AsciiInf::FindEntryByIdPtr(const int *idPtr)
{
    AsciiInf *inst = g_AsciiInf;
    int count = inst->entryCount;
    int target = *idPtr;
    for (int i = 0; i < count; ++i)
    {
        if (target == inst->entries[i].id)
        {
            return &inst->entries[i];
        }
    }
    return &inst->entries[320];
}

unsigned int AsciiInf::GetStructSize()
{
    return sizeof(AsciiInf);
}

bool AsciiIdEquals(const int *a, const int *b)
{
    return *a == *b;
}

AsciiEntry *AsciiInf::FindEntryById(int id)
{
    AsciiInf *inst = g_AsciiInf;
    int count = inst->entryCount;
    for (int i = 0; i < count; ++i)
    {
        if (id == inst->entries[i].id)
        {
            return &inst->entries[i];
        }
    }
    return &inst->entries[320];
}

void AsciiAnmVmMini::SetByte3F3(unsigned char v)
{
    b3F3 = v;
}

void AsciiAnmVmMini::SetInt3F0(int v)
{
    field3F0 = v;
}

AsciiSpriteMini *AsciiAnmVmMini::GetSprite()
{
    return sprite;
}

AsciiSpriteMini *AsciiAnmResMini::GetSprite(int index)
{
    return &sprites[index];
}

void AsciiAnmVmMini::BindSprite(AsciiSpriteMini *spr)
{
    sprite = spr;
    float v = spr->u0;
    uv2x = v;
    uv0x = v;
    v = spr->u1;
    uv3x = v;
    uv1x = v;
    v = spr->v0;
    uv1y = v;
    uv0y = v;
    v = spr->v1;
    uv3y = v;
    uv2y = v;
}

void AsciiSetIntCell(int val, int *cell)
{
    *cell = val;
}

void AsciiInf::NoOp1() {}

void AsciiInf::SetColor(unsigned int c)
{
    color = c;
}

void AsciiInf::ResetAlign()
{
    align = 0;
}

void AsciiInf::SetScale(float sx, float sy)
{
    scaleX = sx;
    scaleY = sy;
}

void AsciiInf::SetFontId(int id)
{
    fontId = id;
}

void AsciiInf::ResetShadow()
{
    shadow = 0;
}

void AsciiInf::SetLayer(int l)
{
    layer = l;
}

void AsciiInf::SetBlendMode(int mode)
{
    blendMode = mode;
}

void AsciiInf::ResetFlags()
{
    unk1C874 = 0;
    unk1C880 = 0;
}

void AsciiInf::ResetColors()
{
    color = 0xFFFFFFFF;
    color2 = 0xFFFFFFFF;
    unk1C854 = 0;
}

void AsciiInf::ResetStyle()
{
    align = 0;
    scaleX = 1.0f;
    fontId = 0;
    scaleY = 1.0f;
    shadow = 0;
    layer = 0;
    blendMode = 0;
    unk1C874 = 0;
    unk1C880 = 0;
    unk1C88C = 9;
    color = 0xFFFFFFFF;
    color2 = 0xFFFFFFFF;
    unk1C854 = 0;
    unk1C878 = 1;
    unk1C87C = 1;
}

void AsciiInf::NoOp2() {}
void AsciiInf::NoOp3() {}

void AsciiMatrix4x4::SetIdentity()
{
    m[3][2] = 0.0f;
    m[3][1] = 0.0f;
    m[3][0] = 0.0f;
    m[2][3] = 0.0f;
    m[2][1] = 0.0f;
    m[2][0] = 0.0f;
    m[1][3] = 0.0f;
    m[1][2] = 0.0f;
    m[1][0] = 0.0f;
    m[0][3] = 0.0f;
    m[0][2] = 0.0f;
    m[0][1] = 0.0f;
    m[3][3] = 1.0f;
    m[2][2] = 1.0f;
    m[1][1] = 1.0f;
    m[0][0] = 1.0f;
}

void AsciiZero44A::Zero() { v = 0; }
void AsciiZero34::Zero()  { v = 0; }
void AsciiZero44B::Zero() { v = 0; }
void AsciiZero24A::Zero() { v = 0; }
void AsciiZero24B::Zero() { v = 0; }

void AsciiListNode::Init(void *o)
{
    owner = o;
    next = NULL;
    prev = NULL;
}

void AsciiAnmVmMini::Reset()
{
    AsciiListNode s464 = node464;
    int s20 = saved20;
    memset(this, 0, sizeof(AsciiAnmVmMini));
    scaleX = 1.0f;
    scaleY = 1.0f;
    scaleZ = 1.0f;
    scaleW = 1.0f;
    node464.owner = s464.owner;
    uvScaleX = 1.0f;
    node464.next = s464.next;
    uvScaleY = 1.0f;
    saved20 = s20;
    node464.prev = s464.prev;
    field3F0 = -1;
    matrix.SetIdentity();
    flags4B0 = 7;
    timeCur = 0;
    timeSub = 0.0f;
    timePrev = -999999;
    interp0 = 0;
    interp1 = 0;
    interp2 = 0;
    interp3 = 0;
    interp4 = 0;
    interp5 = 0;
    interp6 = 0;
    interp7 = 0;
    interp8 = 0;
    interp9 = 0;
    flags4B4 = (flags4B4 & ~0x80U) | 0x40U;
    node04.Init(this);
    node10.Init(this);
}
