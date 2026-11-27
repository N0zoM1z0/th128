#pragma once

#include <stddef.h>

struct BulletVec3
{
    float x;
    float y;
    float z;

    BulletVec3 operator+(const BulletVec3 &rhs) const;
    BulletVec3 operator-(const BulletVec3 &rhs) const;
    void Set(float px, float py, float pz);
    void AddInPlace(const BulletVec3 &rhs);
    void ScaleInPlace(float s);
    float LengthSq2D() const;
    float GetX() const;
    float GetX2() const;
    float GetZ() const;
    float GetZ2() const;
};

struct BulletAngle
{
    float value;

    BulletAngle *Sub(float a);
    BulletAngle *Add(float a);
};

struct BulletTimerMini
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    void SetScaleFloat(float s);                // 0x0046AA20 (external)
    void SetScaleInt(int v);
    void SetScaleNegInt(int v);
    void SetScaleMinusOne();
    BulletTimerMini *ClearFlag0();

    inline void EnsureInit();
    inline void SetInt(int v);
    inline void ResetToMinus1();
};

struct BulletAnmManagerMini
{
    unsigned char pad00[0x4E90D8];
    struct BulletAnmResMini *bulletAnm;         // +0x4E90D8
    unsigned char pad4E90DC[0x8C3900 - 0x4E90DC];
    unsigned int colorMod;                      // +0x8C3900
    int colorModEnabled;                        // +0x8C3904

    static void ResetColorMod();
    static void SetColorMod(unsigned int c);
};

struct BulletAnmResMini
{
    void Release();                             // 0x00465F60 (external)
};

struct BulletTypeInfo
{
    int field00;                                // +0x000 (0x4B24D0)
    unsigned char pad04[0x104 - 0x004];
    float field104;                             // +0x104 (0x4B25D4)
    float field108;                             // +0x108 (0x4B25D8)
    unsigned char pad10C[0x120 - 0x10C];
};
typedef char BulletTypeInfoSizeCheck[(sizeof(BulletTypeInfo) == 0x120) ? 1 : -1];

struct BulletItemMini
{
    unsigned int flags;                         // +0x000
    unsigned char pad004[0x4B4 - 0x004];
    void *extraA;                               // +0x4B4
    unsigned char pad4B8[0x99C - 0x4B8];
    void *extraB;                               // +0x99C
    unsigned char pad9A0[0xA2A - 0x9A0];
    unsigned short stateA2A;                    // +0xA2A
    unsigned char padA2C[0x1184 - 0xA2C];
    BulletTimerMini timer1;                     // +0x1184..+0x1197
    BulletTimerMini timer2;                     // +0x1198..+0x11AB
    unsigned char pad11AC[0x11B8 - 0x11AC];

    void ResetTimersAndClearFlag0();
    ~BulletItemMini();
};
typedef char BulletItemMiniSizeCheck[(sizeof(BulletItemMini) == 0x11B8) ? 1 : -1];

struct BulletSub380
{
    unsigned char pad000[0x374];
    int field374;                               // +0x374
    unsigned char pad378[0x380 - 0x378];

    BulletSub380 *Reset();
};

struct BulletZero54
{
    unsigned char data[0x54];
    BulletZero54 *Zero();
};

struct BulletBoundsCheck
{
    float x;
    float y;

    int IsOutOfBoundsTall(float w, float h);
    int IsOutOfBounds(float w, float h);
};

struct BulletManagerMini
{
    unsigned char pad00[0x8A7F9C];
    BulletAnmResMini *anm;                      // +0x8A7F9C

    static BulletAnmResMini *GetAnm();
    static int UnloadAnm();
};

struct BombInfMini
{
    unsigned char pad00[0x3C];
    int active;                                 // +0x3C

    static bool IsActive();
};

struct FreezeInfMini
{
    unsigned char pad00[0x28];
    int timer28;                                // +0x28
    unsigned char pad2C[0x7C - 0x2C];
    unsigned int flags7C;                       // +0x7C
    int field80;                                // +0x80

    bool IsTimerGe60();
    static void CheckEnd();
};

struct PlayerMini
{
    unsigned char pad00[0xECC];
    BulletVec3 pos;                             // +0xECC
    unsigned char padED8[0x87B0 - 0xED8];
    float float87B0;                            // +0x87B0
    unsigned char pad87B4[0x438D8 - 0x87B4];
    BulletTimerMini invulnTimer;                // +0x438D8..+0x438EB

    BulletVec3 *GetPosPtr();
    void SetFloat87B0(float v);
    static void __stdcall SetInvulnTimer(int v);
};

struct EnemyMgrMini
{
    unsigned char pad00[0x65DDC];
    void *field65DDC;                           // +0x65DDC
    unsigned char pad65DE0[0x6B1FC - 0x65DE0];
    unsigned int flags6B1FC;                    // +0x6B1FC
    int field6B200;                             // +0x6B200
    unsigned char pad6B204[0x6B218 - 0x6B204];
    int field6B218;                             // +0x6B218

    void *GetField65DDC();
    static void SetFlag6B1FCBit2();
    static void Trigger6B200();
};

struct BulletField08  { unsigned char pad[0x08]; int v; int Get(); };
struct BulletField04  { unsigned char pad[0x04]; int v; int Get(); };
struct BulletFlag90   { unsigned char pad[0x90]; unsigned int f; unsigned int GetBit0(); };
struct BulletField54  { unsigned char pad[0x54]; int v; int Get(); };
struct BulletField5A34{ unsigned char pad[0x5A34]; int v; int Get(); };
struct BulletRetThis  { BulletRetThis *Self(); };

extern BulletAnmManagerMini *g_BulletAnmManager;    // 0x004D2E50
extern BulletManagerMini *g_BulletManagerMini;      // 0x004B8930
extern BombInfMini *g_BombInfMini;                  // 0x004B892C
extern FreezeInfMini *g_FreezeInfMini;              // 0x004B8934
extern EnemyMgrMini *g_EnemyMgrMini;                // 0x004B8938
extern PlayerMini *g_PlayerMini;                    // 0x004B8A80
extern float g_BulletDefaultTimeScale;              // 0x004B6F58
extern int g_GameModeA;                             // 0x004B4CF4
extern int g_GameModeB;                             // 0x004B4CF8
extern BulletTypeInfo g_BulletTypes[64];            // 0x004B24D0

float __stdcall BulletNormalizeAngle(float a);      // 0x0046A700 (external)

int BulletGetModeIndex();
int BulletGetTypeField00(int idx);
float BulletGetTypeField104(int idx);
float BulletGetTypeField108(int idx);

void BulletNoOp1();
void BulletNoOp2();
int BulletRet1A();
int BulletRet1B();
int BulletRet0A();
int BulletRet0B();
void BulletNoOp3();
void BulletNoOp4();
int BulletRet1C();
int BulletRet0C();
int BulletRet0D();
int BulletRet0E();
