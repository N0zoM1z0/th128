#pragma once

#include <stddef.h>

struct EclField60Global
{
    unsigned char pad00[0x60];
    float field60;                              // +0x60
};

struct EclSlot2C
{
    unsigned int flags;                         // +0x00
    int val;                                    // +0x04
    unsigned char pad08[0x24];
};
typedef char EclSlot2CSizeCheck[(sizeof(EclSlot2C) == 0x2C) ? 1 : -1];

struct EclSub1000
{
    unsigned char pad000[0x1000];
    int field1000;                              // +0x1000
    int field1004;                              // +0x1004
    int field1008;                              // +0x1008
    int field100C;                              // +0x100C
    unsigned char pad1010[0x1040 - 0x1010];
    EclSlot2C slots[8];                         // +0x1040..+0x119F

    void ResetSlots();
};

struct EclClear1000A { unsigned char pad[0x1000]; int a; int b; void Clear(); };
struct EclClear1000B { unsigned char pad[0x1000]; int a; int b; void Clear(); };

struct EclListNode
{
    void *data;                                 // +0x00
    EclListNode *next;                          // +0x04
};

struct EclVmVtbl
{
    void *pad0;
    void *pad1;
    int *(__thiscall *getIntVar)(void *self, int id);   // +0x08
    void *pad3;
    float *(__thiscall *getFloatVar)(void *self, int id); // +0x10
};

struct EclVmObj
{
    EclVmVtbl *vtbl;
};

struct EclInstrHdr
{
    unsigned char pad00[0x08];
    unsigned short flags;                       // +0x08
    unsigned short pad0A;
    unsigned int pad0C;
    int intArgs[4];                             // +0x10
};

struct EclInstrHdrF
{
    unsigned char pad00[0x08];
    unsigned short flags;                       // +0x08
    unsigned short pad0A;
    unsigned int pad0C;
    float floatArgs[4];                         // +0x10
};

struct EclCtxInner
{
    float time;                                 // +0x00
    EclInstrHdr *hdr;                           // +0x04
    unsigned char stackBuf[0x1004];             // +0x08..+0x100B
    int stackTop;                               // +0x100C
    unsigned int pad1010;
    EclVmObj *vm;                               // +0x1014
};

struct EclCtxHolder
{
    unsigned int pad00;
    EclCtxInner *ctx;                           // +0x04

    int *GetIntArg0();
    void Tail46FF70();
    float *GetFloatArg(int idx);
    void SubTime(float dt);
};

struct EclHost
{
    void *vtbl;                                 // +0x00
    void *field04;                              // +0x04
    float field08;                              // +0x08 (also start of EclSub1000 at +0x08!)
    int field0C;                                // +0x0C
    unsigned char pad10[0x1010 - 0x10];
    int field1010;                              // +0x1010
    int field1014;                              // +0x1014
    int field1018;                              // +0x1018
    EclHost *selfPtr;                           // +0x101C
    int field1020;                              // +0x1020
    unsigned char pad1024[0x104C - 0x1024];
    EclSlot2C slots2[8];                        // +0x104C..+0x11AB (note: +0x104C is slots[0].val + 0x08)
    unsigned char pad11AC[0x11A8 - 0x11AC + 4];
};

struct EclHostClean
{
    void *vtbl;                                 // +0x00
    void *field04;                              // +0x04
    EclSub1000 sub08;                           // +0x08..+0x11A7
    unsigned int flags11A8;                     // +0x11A8
    unsigned int pad11AC;                       // +0x11AC
    void *field11B0;                            // +0x11B0
    EclListNode *listHead;                      // +0x11B4
    int field11B8;                              // +0x11B8

    EclHostClean();
    void DestroyHost();
    void FreeList();
    void InitFields();
};

struct EclSub1098
{
    void *vtbl;                                 // +0x00
    unsigned char pad04[0x8C - 0x04];
    void *buf8C;                                // +0x8C
    unsigned char pad90[0x1090 - 0x90];
    int field1090;                              // +0x1090
    int field1094;                              // +0x1094

    EclSub1098();
    ~EclSub1098();
};
typedef char EclSub1098SizeCheck[(sizeof(EclSub1098) == 0x1098) ? 1 : -1];

struct EclHp
{
    int hp;                                     // +0x00
    int field04;                                // +0x04
    int field08;                                // +0x08
    int rawHp;                                  // +0x0C
    int minHp;                                  // +0x10
    unsigned int flags14;                       // +0x14

    void SetHpA(int v);
    int GetHp();
    void SetHpB(int v);
    int GetFlag1();
    void SetRawHpMul7(int v);
    void SetFlag0(int bit);
    int GetField04();
    void SetField04(int v);
    int GetField08();
    void SetField08(int v);
    void SetMinHp(int v);
    void SetHpC(int v);
    int ApplyDamage(int dmg);
};

struct EclGet84  { unsigned char pad[0x84]; int v; int Get(); };
struct EclIsZ0   { int v; int IsZero(); };
struct EclGet68  { unsigned char pad[0x68]; int v; int Get(); };
struct EclArr0C  { unsigned char pad[0x0C]; int a[8]; int Get(int idx); };

extern unsigned int g_EclInputMask;             // 0x004DAFF8
extern int g_EclKeyHoldTable[32];               // 0x004DB10C
extern EclField60Global *g_Ecl4B8930;           // 0x004B8930
extern void *g_EclHostVtbl;                     // 0x004A26E8
extern void *g_EclSub1098Vtbl;                  // 0x004A26DC

extern void EclTailTarget46FF70(EclCtxInner *ctx); // 0x0046FF70

int EclRet1_4136D0();
int EclRet1_413720();
int EclGetKeyHoldFrames(int bit);
unsigned int __stdcall EclTestInputMask(unsigned int mask);
void EclNoOp414260();
void __stdcall EclSet4B8930Field60(float v);
