#pragma once

#include <stddef.h>

struct GmVec2
{
    float x;
    float y;
};

struct GmVec3
{
    float x;
    float y;
    float z;
};

struct GmMoveObj
{
    float f00;                                  // +0x00
    float f04;                                  // +0x04
    float f08;                                  // +0x08
    float f0C;                                  // +0x0C
    float f10;                                  // +0x10
    float f14;                                  // +0x14
    float f18;                                  // +0x18
    float f1C;                                  // +0x1C
    float f20;                                  // +0x20
    float f24;                                  // +0x24
    float f28;                                  // +0x28
    float f2C;                                  // +0x2C
    unsigned int flags30;                       // +0x30

    void Set10(float v);
    void Set0C(float v);
    float Get20();
    float Get24();
    void SetFlag0();
    float Get18();
    void ClearFlag1();
    void ClearFlags01();
    float Get08();
    float Get2C();
    void Set24(float v);
    void Set20(float v);
    void Set18(float v);
    int GetFlag1();
    int GetFlag0();
    void Set04(float v);
    float Get04();
    void Set00(float v);
    float Get00();
    void SetVec3At0C(const GmVec3 *v);
};

struct GmCopyVec3 { GmVec3 v; void CopyFrom(const GmVec3 *src); };
struct GmSet3F4   { unsigned char pad[0x3F4]; int v; void Set(int x); };
struct GmOff1C    { unsigned char pad[0x1C]; int f; int *Get(); };
struct GmOff458   { unsigned char pad[0x458]; int f; int *Get(); };
struct GmOff0C    { unsigned char pad[0x0C]; int f; int *Get(); };
struct GmGet08    { unsigned int pad[2]; int v; int Get(); };
struct GmGet558   { unsigned char pad[0x558]; int v; int Get(); };

struct GmScaleObj
{
    unsigned char pad00[0x40];
    float f40;                                  // +0x40
    float f44;                                  // +0x44
    unsigned char pad48[0x60 - 0x48];
    float f60;                                  // +0x60
    float f64;                                  // +0x64

    float Mul60x40();
    float Mul64x44();
};

struct Gm4B8954
{
    unsigned char pad00[0x60];
    unsigned int flags60;                       // +0x60

    static int GetFlag1();
};

struct GmFpsCounter
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x8C - 0x04];

    GmFpsCounter();
    GmFpsCounter *DestroySelf();
    static void DestroyGlobal();
    int DrawHook();
};
typedef char GmFpsCounterSizeCheck[(sizeof(GmFpsCounter) == 0x8C) ? 1 : -1];

struct GmGuiMini
{
    unsigned char pad0000[0x5904];
    int field5904;                              // +0x5904
    unsigned char pad5908[0x5914 - 0x5908];
    unsigned char rows5914[0x5A04 - 0x5914];
    unsigned int flags5A04;                     // +0x5A04
    unsigned char pad5A08[0x5A3C - 0x5A08];
    int field5A3C;                              // +0x5A3C

    static int GetField5904();
    void ClearRow5914(int idx);
    static void SetFlag3AndClear5A3C();
    int GetFlag3();
    static void SetFlag4();
};

extern Gm4B8954 *g_Gm4B8954;                    // 0x004B8954
extern GmFpsCounter *g_GmFpsCounter;            // 0x004B894C
extern GmGuiMini *g_GmGui;                      // 0x004B8950
extern GmVec3 *(__stdcall *g_GmVec3Normalize)(GmVec3 *, const GmVec3 *); // 0x0049A2A0

extern float __stdcall GmWrapAngle(float a);    // 0x0046A700
extern void GmFpsCounterDtor(GmFpsCounter *c);  // 0x0041F860
extern int __stdcall GmFpsCounterDraw(GmFpsCounter *c); // 0x0041FA90

void GmNoOp41F360();
void GmNoOp41F730();
void GmNoOp41FB90();
int __stdcall GmRectsOverlap2D(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);
void GmPosToUv(const GmVec2 *pos, float *u, float *v);
void GmVec3NormalizeScale(GmVec3 *dst, const GmVec3 *src, float scale);
float GmVec2LenSq(const GmVec2 *v);
void __stdcall GmSetDoubleWrapped1C(GmMoveObj *obj, float angle);
void *GmIdentity(void *p);
