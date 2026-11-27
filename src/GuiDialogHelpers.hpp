#pragma once

#include <stddef.h>

struct GdGuiFlags
{
    unsigned char pad0000[0x5A04];
    unsigned int flags5A04;                     // +0x5A04

    int GetFlag4();
};

struct GdEnemyFlags
{
    unsigned char pad0000[0x506C];
    int field506C;                              // +0x506C
    unsigned char pad5070[0x50AC - 0x5070];
    unsigned int flags50AC;                     // +0x50AC

    int IsNotFlag5();
    int IsNotFlag0();
    int IsFlag0Or4OrPos506C();
};

struct GdEnemyMgrMini
{
    unsigned char pad00[0x7C];
    unsigned int flags7C;                       // +0x7C

    static int GetFlag0();
};

struct Gd4B8934
{
    unsigned char pad00[0x94];
    int field94;                                // +0x94
    unsigned char pad98[0xA8 - 0x98];
    int encodedTime;                            // +0xA8

    static int GetField94();
    static void __stdcall DecodeTime(int *outSec, int *outCs);
};

struct GdTable66C
{
    unsigned char pad00[0x08];
    unsigned char entries[1][0x66C];            // +0x08

    void *GetEntry(int idx);
};

struct GdGuiObj
{
    int CalcHook();
    GdGuiObj *DestroySelf();
    static void DestroyGlobal();
};

struct GdStageInfo40
{
    unsigned char data[40];
};

struct GdStageOwner
{
    unsigned char pad00[0x54];
    int field54;                                // +0x54
    int field58;                                // +0x58
    unsigned char pad5C[0x6C - 0x5C];
    int field6C;                                // +0x6C
    unsigned char pad70[0x7C - 0x70];
    int field7C;                                // +0x7C
    unsigned char pad80[0x90 - 0x80];
    unsigned int flags90;                       // +0x90

    void Set7C(int v);
    void Set6C(int v);
    void SetStageIdx(int idx);
    int GetFlag4();
    int GetFlag3();
    int GetFlag5();
};

struct GdReplaySub
{
    unsigned char pad00[0x0A];
    unsigned char byte0A;                       // +0x0A
};

struct GdReplayMgr
{
    unsigned char pad00[0x1C];
    GdReplaySub *sub1C;                         // +0x1C

    static int GetSubByte0ABit0();
};

struct GdPair     { int a; int b; int Diff(); };
struct GdOff40    { unsigned char pad[0x40]; int f; int *Get(); };
struct GdOff464   { unsigned char pad[0x464]; int f; int *Get(); };
struct GdGet3F0   { unsigned char pad[0x3F0]; int v; int Get(); };
struct GdGetFltA0 { unsigned char pad[0xA0]; float v; float Get(); };
struct GdGetB0    { unsigned char pad[0xB0]; int v; int Get(); };
struct GdGetA4    { unsigned char pad[0xA4]; int v; int Get(); };
struct GdSet0     { int v; void Set(int x); };
struct GdGet04    { unsigned int pad; int v; int Get(); };

struct GdVmState
{
    unsigned char pad000[0x3F8];
    short state3F8;                             // +0x3F8
    short pad3FA;
    unsigned char pad3FC[0x4C8 - 0x3FC];
    void (__fastcall *onStateChange)(GdVmState *self, int state); // +0x4C8

    void SetState3();
    void SetStatePlus7(int s);
};

struct Gd4B8924
{
    unsigned char pad0000[0x37F8];
    unsigned int flags37F8;                     // +0x37F8

    static int GetFlag3();
};

extern GdEnemyMgrMini *g_GdEnemyMgr;            // 0x004B8948
extern Gd4B8934 *g_Gd4B8934;                    // 0x004B8934
extern GdGuiObj *g_GdGuiObj;                    // 0x004B8950
extern char g_GdDecodeBuf[256];                 // 0x004DB980
extern int g_Gd4B4D3C;                          // 0x004B4D3C
extern unsigned int g_Gd4B4D50;                 // 0x004B4D50
extern GdStageInfo40 g_GdStageTable[32];        // 0x004B1A30
extern GdStageInfo40 *g_GdCurStageInfo;         // 0x004B8A98
extern int g_GdPower;                           // 0x004B4CDC
extern GdReplayMgr *g_GdReplayMgr;              // 0x004B8A84
extern unsigned int g_GdInput210;               // 0x004DB210
extern int g_GdHold18C[32];                     // 0x004DB18C
extern unsigned int g_GdInput21C;               // 0x004DB21C
extern int g_Gd4B4D2C;                          // 0x004B4D2C
extern int g_Gd4D33CC;                          // 0x004D33CC
extern int g_Gd4B4D04;                          // 0x004B4D04
extern int g_Gd4B4D08;                          // 0x004B4D08
extern int g_Gd4B4CFC;                          // 0x004B4CFC
extern int g_Gd4B4D00;                          // 0x004B4D00
extern int g_Gd4B4CC0;                          // 0x004B4CC0
extern Gd4B8924 *g_Gd4B8924;                    // 0x004B8924

extern void __stdcall GdGuiDtor(GdGuiObj *g);   // 0x00420CE0
extern int __stdcall GdGuiCalc(GdGuiObj *g);    // 0x004210B0
extern int GdStrCmp473090(const char *a, const char *b); // 0x00473090

char *GdDecodeDialogString(const char *src);
int GdGet4B4D3C();
int GdGet4B4D50Bit2();
void GdSet4B4D50Bit2();
int GdGetPowerFloor10();
int GdCall473090(const char *a, const char *b);
unsigned int __stdcall GdTestInput210(unsigned int mask);
int GdGetHold18C(int bit);
unsigned int __stdcall GdTestInput21C(unsigned int mask);
int GdGet4B4D2C();
int GdGet4D33CC();
int GdGet4B4D04();
int GdGet4B4D08();
int GdGet4B4CFC();
int GdGet4B4D00();
void GdNoOp425BD0();
int GdGet4B4CC0();
