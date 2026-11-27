#pragma once

#include <stddef.h>

struct CardVec2
{
    float x;
    float y;

    float LengthSq() const;
    float Dot(const CardVec2 &rhs) const;
    float Cross(const CardVec2 &rhs) const;
    int IsOutOfBoundsPad(float px, float py) const;
    float DistSq(const CardVec2 &rhs) const;
    void Set(float px, float py);
    CardVec2 operator*(float s) const;
};

struct CardVec3
{
    float x;
    float y;
    float z;

    void SubInPlace(const CardVec3 &rhs);
};

struct CardTimerMini
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    inline void EnsureInit();
    inline void ResetToMinus1();
};

struct CardScalarInterp
{
    float start;                                // +0x00
    float end;                                  // +0x04
    float vel0;                                 // +0x08
    float vel1;                                 // +0x0C
    CardTimerMini timer;                        // +0x10..+0x23
    int duration;                               // +0x24
    int mode;                                   // +0x28

    void SetStart(const float &v);
    void SetEnd(const float &v);
    void SetVel0(const float &v);
    void SetVel1(const float &v);
    void SetDuration(int d);
    int GetDuration() const;
};

struct CardAnmVmMini
{
    unsigned char pad000[0x3F0];
    int field3F0;                               // +0x3F0
    unsigned char pad3F4[0x3F8 - 0x3F4];
    short interruptId;                          // +0x3F8
    unsigned char pad3FA[0x4B0 - 0x3FA];
    unsigned int flags4B0;                      // +0x4B0
    unsigned char pad4B4[0x4C8 - 0x4B4];
    void (__fastcall *onInterrupt)(CardAnmVmMini *self, int id); // +0x4C8

    void ClearBlendBits();
    void SetBlendMode1();
    void SetInterrupt2();
    void SetInterrupt(short id);
    unsigned int GetFlag0();
    int *GetField3F0Ptr();
};

struct CardMotionMini
{
    unsigned char pad00[0xA4];
    CardVec3 posA;                              // +0xA4
    CardVec3 posB;                              // +0xB0
    CardVec3 vecC;                              // +0xBC
    CardVec3 vecD;                              // +0xC8
    CardTimerMini timer;                        // +0xD4..+0xE7
    int duration;                               // +0xE8
    int flagEC;                                 // +0xEC

    void Init(const CardVec3 *a, const CardVec3 *b, int dur, unsigned char flag);
};

struct CardReplayTarget
{
    unsigned char pad00[0xA8];
    int encoded;                                // +0xA8
};

struct CardGuiMini
{
    unsigned char pad00[0x5C];
    int arr5C[9];                               // +0x5C
    int arr80[4];                               // +0x80..+0x8F

    static int GetArr80(int idx);
    static int GetArr5C(int idx);
    void IncCap80();
    void IncCap84();
    void IncCap88();
    void IncCap8C();
};

struct CardStageMini
{
    unsigned char pad00[0x37F8];
    unsigned int flags37F8;                     // +0x37F8

    static void ClearFlag0();
    static void SetFlag0();
};

struct CardReplayEntry
{
    unsigned char data[0x98];
};

struct CardReplayMgrMini
{
    unsigned char pad00[0x37A0];
    CardReplayEntry entries[64];                // +0x37A0

    static CardReplayEntry *GetEntry(int idx);
};

struct CardFreezeMini
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x28 - 0x04];
    int timer28;                                // +0x28
    unsigned char pad2C[0x34 - 0x2C];
    unsigned int timerFlags34;                  // +0x34
    unsigned char pad38[0x7C - 0x38];
    unsigned int flags7C;                       // +0x7C
    unsigned char pad80[0xC0 - 0x80];

    CardFreezeMini();
    static void CheckEndNoClear80();
};
typedef char CardFreezeMiniSizeCheck[(sizeof(CardFreezeMini) == 0xC0) ? 1 : -1];

struct CardBombMini
{
    unsigned char pad00[0x3C];
    int active;                                 // +0x3C
};

struct CardChainElemMini
{
    unsigned int pad00;
    unsigned int flags;                         // +0x04
};

struct CardChainPair
{
    unsigned char pad00[0x08];
    CardChainElemMini *calcElem;                // +0x08
    CardChainElemMini *drawElem;                // +0x0C

    void EnableBoth();
};

struct CardPauseMini
{
    unsigned char pad00[0x10];
    int state10;                                // +0x10
    unsigned char pad14[0x20 - 0x14];
    int arr20[16];                              // +0x20

    static bool IsState1();
    static int GetArr20(int idx);
};

struct CardAsciiMini
{
    unsigned char pad00[0x1C84F];
    unsigned char alphaByte;                    // +0x1C84F
    unsigned char pad1C850[0x1C894 - 0x1C850];
    void *anmRes;                               // +0x1C894

    void *GetAnmRes();
    static void SetAlphaByte(unsigned char a);
};

struct CardBulletIter
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x9D8 - 0x04];
    unsigned char sub9D8[0x20];                 // +0x9D8
    float f9F8;                                 // +0x9F8
    float pad9FC;
    float fA00;                                 // +0xA00

    void *GetSub9D8();
    float GetF9F8();
    float GetFA00();
    unsigned int GetFlag0();
};

struct CardBulletMgrMini
{
    unsigned char pad00[0x64];
    unsigned char pool[4];                      // +0x64

    static void *GetPool();
};

struct CardPlayerMini
{
    unsigned char pad00[0x10];
    int field10;                                // +0x10

    static int GetField10();
};

struct CardLaserNode
{
    unsigned char pad00[0x08];
    CardLaserNode *next;                        // +0x08
};

struct CardLaserMgrMini
{
    unsigned char pad00[0x18];
    CardLaserNode *head;                        // +0x18
    unsigned char pad1C[0x5FC - 0x1C];
    CardLaserNode *cur;                         // +0x5FC

    static CardLaserNode *BeginIter();
    static CardLaserNode *NextIter();
};

struct CardEffectSub68
{
    unsigned char pad00[0x30];
    unsigned int flags30;                       // +0x30
    unsigned char pad34[0x44 - 0x34];
    unsigned int flags44;                       // +0x44
    unsigned char pad48[0x68 - 0x48];

    CardEffectSub68();
};
typedef char CardEffectSub68SizeCheck[(sizeof(CardEffectSub68) == 0x68) ? 1 : -1];

struct CardEffectSub54
{
    unsigned char pad00[0x4C];
    unsigned int flags4C;                       // +0x4C
    unsigned char pad50[0x54 - 0x50];

    CardEffectSub54();
};
typedef char CardEffectSub54SizeCheck[(sizeof(CardEffectSub54) == 0x54) ? 1 : -1];

struct CardEffectMgrMini
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x08 - 0x04];
    CardChainElemMini *calcElem;                // +0x08
    CardChainElemMini *drawElem;                // +0x0C
    CardEffectSub54 sub0;                       // +0x10..+0x63 (+0x5C flags)
    CardEffectSub54 sub1;                       // +0x64..+0xB7 (+0xB0 flags)
    CardEffectSub54 sub2;                       // +0xB8..+0x10B (+0x104 flags)
    CardEffectSub68 items[4000];                // +0x10C..+0x65A0B (4000 * 0x68 = 0x65900)
    unsigned char pad65A0C[0x6B1F8 - 0x65A0C];
    unsigned int flags6B1F8;                    // +0x6B1F8
    unsigned int flags6B1FC;                    // +0x6B1FC
    int field6B200;                             // +0x6B200
    unsigned char pad6B204[0x6B21C - 0x6B204];

    CardEffectMgrMini();
    int GetField6B200();
    static void EnableChainElems();
};
typedef char CardEffectMgrMiniSizeCheck[(sizeof(CardEffectMgrMini) == 0x6B21C) ? 1 : -1];

struct CardEnemyMini
{
    unsigned char pad00[0x38];
    int field38;                                // +0x38
    unsigned char pad3C[0x48 - 0x3C];
    int field48;                                // +0x48
    unsigned char pad4C[0x54 - 0x4C];
    float f54;                                  // +0x54
    float f58;                                  // +0x58
    unsigned char pad5C[0x64 - 0x5C];
    int field64;                                // +0x64
    float f68;                                  // +0x68
    float f6C;                                  // +0x6C
    int pad70;
    int field74;                                // +0x74
    int field78;                                // +0x78

    void SetMode12();
    void SetTargetPos(float px, float py);
    float GetF68();
    float GetF6C();
};

struct CardPtr11FC { unsigned char pad[0x11FC]; int v; int *Ptr(); };
struct CardTable148 { unsigned char pad[0x148]; int rows[8][9]; int Get(int idx); };
struct CardField74 { unsigned char pad[0x74]; int v; int Get(); };
struct CardField04Sub { unsigned int pad00; int v; int Sub300(); };
struct CardField4C { unsigned char pad[0x4C]; int v; int Get(); };
struct CardPtr08 { unsigned char pad[0x08]; int v; int *Ptr(); };
struct CardField10 { unsigned char pad[0x10]; int v; int Get(); };
struct CardZeroCheck { int v; bool IsZero(); };
struct CardPtr10 { unsigned char pad[0x10]; int v; int *Ptr(); };
struct CardStatus60 { unsigned char pad[0x60]; unsigned int flags; };

extern CardStatus60 *g_CardStatus60;            // 0x004B8954
extern CardVec3 g_CardDefaultVec;               // 0x004D2E54
extern float g_CardDefaultTimeScale;            // 0x004B6F58
extern CardGuiMini *g_CardGuiMini;              // 0x004B8948
extern CardStageMini *g_CardStageMini;          // 0x004B8928
extern CardReplayMgrMini *g_CardReplayMgr;      // 0x004B8A88
extern signed char g_CardByteTable[250];        // 0x004B23D0
extern CardFreezeMini *g_CardFreezeMini;        // 0x004B8934
extern CardBombMini *g_CardBombMini;            // 0x004B892C
extern int g_CardGlobal4D33F0;                  // 0x004D33F0
extern CardPauseMini *g_CardPauseMini;          // 0x004B8A84
extern CardAsciiMini *g_CardAsciiMini;          // 0x004B8920
extern CardPlayerMini *g_CardPlayerMini;        // 0x004B8A80
extern CardBulletMgrMini *g_CardBulletMgr;      // 0x004B8930
extern CardLaserMgrMini *g_CardLaserMgr;        // 0x004B8A60
extern CardEffectMgrMini *g_CardEffectMgr;      // 0x004B8938

unsigned int CardGetStatusBit2();
unsigned int CardGetStatusBit0Or2();
void CardNoOp40D300();
void __stdcall CardEncodeReplaySeed(int a, int b, CardReplayTarget *out);
int CardCountByteMatches250(int val);
int CardRet0_40D660();
int CardRet0_40D670();
int CardGet4D33F0();
void CardNoOp40E5D0();
void CardNoOp40E5F0();
void CardNoOp40E810();
