#include "GuiMathHelpers.hpp"

#include <string.h>

void GmMoveObj::Set10(float v) { f10 = v; }
void GmMoveObj::Set0C(float v) { f0C = v; }
float GmMoveObj::Get20() { return f20; }
float GmMoveObj::Get24() { return f24; }
void GmMoveObj::SetFlag0() { flags30 |= 1U; }
float GmMoveObj::Get18() { return f18; }
void GmMoveObj::ClearFlag1() { flags30 &= ~2U; }
void GmMoveObj::ClearFlags01() { flags30 &= ~3U; }
float GmMoveObj::Get08() { return f08; }
float GmMoveObj::Get2C() { return f2C; }
void GmMoveObj::Set24(float v) { f24 = v; }
void GmMoveObj::Set20(float v) { f20 = v; }
void GmMoveObj::Set18(float v) { f18 = v; }
int GmMoveObj::GetFlag1() { return (flags30 >> 1) & 1U; }
int GmMoveObj::GetFlag0() { return flags30 & 1U; }
void GmMoveObj::Set04(float v) { f04 = v; }
float GmMoveObj::Get04() { return f04; }
void GmMoveObj::Set00(float v) { f00 = v; }
float GmMoveObj::Get00() { return f00; }

void GmMoveObj::SetVec3At0C(const GmVec3 *v)
{
    f0C = v->x;
    f10 = v->y;
    f14 = v->z;
}

void GmCopyVec3::CopyFrom(const GmVec3 *src) { v = *src; }
void GmSet3F4::Set(int x) { v = x; }
int *GmOff1C::Get() { return &f; }
int *GmOff458::Get() { return &f; }
int *GmOff0C::Get() { return &f; }
int GmGet08::Get() { return v; }
int GmGet558::Get() { return v; }

float GmScaleObj::Mul60x40() { return f60 * f40; }
float GmScaleObj::Mul64x44() { return f64 * f44; }

int Gm4B8954::GetFlag1() { return (g_Gm4B8954->flags60 >> 1) & 1U; }

void GmNoOp41F360() {}
void GmNoOp41F730() {}
void GmNoOp41FB90() {}

int __stdcall GmRectsOverlap2D(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
{
    double hw1 = w1 * 0.5;
    double hw2 = w2 * 0.5;
    if (x2 + hw2 >= x1 - hw1 && x2 - hw2 <= x1 + hw1)
    {
        double hh1 = h1 * 0.5;
        double hh2 = h2 * 0.5;
        if (y2 + hh2 >= y1 - hh1 && y2 - hh2 <= y1 + hh1)
        {
            return 1;
        }
    }
    return 0;
}

void GmPosToUv(const GmVec2 *pos, float *u, float *v)
{
    *u = (float)(pos->x / 640.0);
    *v = (float)(pos->y / 480.0);
    if (*u < 0.0f)
    {
        *u = 0.0f;
    }
    if (*v < 0.0f)
    {
        *v = 0.0f;
    }
}

void GmVec3NormalizeScale(GmVec3 *dst, const GmVec3 *src, float scale)
{
    g_GmVec3Normalize(dst, src);
    dst->x *= scale;
    dst->y *= scale;
    dst->z *= scale;
}

float GmVec2LenSq(const GmVec2 *v)
{
    return v->x * v->x + v->y * v->y;
}

void __stdcall GmSetDoubleWrapped1C(GmMoveObj *obj, float angle)
{
    obj->f1C = GmWrapAngle(GmWrapAngle(angle));
}

void *GmIdentity(void *p) { return p; }

GmFpsCounter::GmFpsCounter()
{
    memset(this, 0, sizeof(GmFpsCounter));
    flags00 |= 2U;
    g_GmFpsCounter = this;
}

GmFpsCounter *GmFpsCounter::DestroySelf()
{
    GmFpsCounterDtor(this);
    operator delete(this);
    return this;
}

void GmFpsCounter::DestroyGlobal()
{
    GmFpsCounter *c = g_GmFpsCounter;
    if (c != NULL)
    {
        GmFpsCounterDtor(c);
        operator delete(c);
    }
}

int GmFpsCounter::DrawHook()
{
    return GmFpsCounterDraw(this);
}

int GmGuiMini::GetField5904() { return g_GmGui->field5904; }

void GmGuiMini::ClearRow5914(int idx)
{
    *(int *)(rows5914 + idx * 0x54) = 0;
}

void GmGuiMini::SetFlag3AndClear5A3C()
{
    GmGuiMini *g = g_GmGui;
    g->flags5A04 |= 8U;
    g->field5A3C = 0;
}

int GmGuiMini::GetFlag3() { return (flags5A04 >> 3) & 1U; }
void GmGuiMini::SetFlag4() { g_GmGui->flags5A04 |= 0x10U; }
