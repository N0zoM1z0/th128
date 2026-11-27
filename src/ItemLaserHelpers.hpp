#pragma once

#include <stddef.h>
#include <stdlib.h>

struct IlVec3
{
    float x;
    float y;
    float z;

    void FromAngle(float angle, float radius);
};

struct IlGlobals
{
    unsigned char pad00[0x4C];
    int field4C;                                // +0x4C (0x4B4D0C)
    unsigned char pad50[0x78 - 0x50];
    int field78;                                // +0x78 (0x4B4D38)
    unsigned char pad7C[0xBC - 0x7C];
};
typedef char IlGlobalsSizeCheck[(sizeof(IlGlobals) == 0xBC) ? 1 : -1];

struct IlHelpMgr
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x24 - 0x04];
    unsigned int timerFlags24;                  // +0x24
    int field28;                                // +0x28
    int pad2C;                                  // +0x2C
    int field30;                                // +0x30
    unsigned char pad34[0xB4 - 0x34];
    int fieldB4;                                // +0xB4
    unsigned char padB8[0xF8 - 0xB8];
    int fieldF8;                                // +0xF8
    int fieldFC;                                // +0xFC
    unsigned char pad100[0x1C0 - 0x100];

    IlHelpMgr();
    static IlHelpMgr *Create();
    IlHelpMgr *DestroySelf();
};
typedef char IlHelpMgrSizeCheck[(sizeof(IlHelpMgr) == 0x1C0) ? 1 : -1];

struct IlTable20
{
    unsigned char pad00[0x120];
    int (*rows)[5];                             // +0x120

    int GetFirst(int idx);
    int *GetRow1();
};

struct IlAnmVm4E8
{
    unsigned char pad000[0x4AC];
    void *extraData;                            // +0x4AC
    unsigned char pad4B0[0x4E8 - 0x4B0];

    IlAnmVm4E8();
    ~IlAnmVm4E8()
    {
        if (extraData != NULL)
        {
            free(extraData);
        }
        extraData = NULL;
    }
};
typedef char IlAnmVm4E8SizeCheck[(sizeof(IlAnmVm4E8) == 0x4E8) ? 1 : -1];

struct IlItemEntry
{
    IlAnmVm4E8 vm0;                             // +0x000
    IlAnmVm4E8 vm1;                             // +0x4E8
    unsigned char pad9D0[0xA00 - 0x9D0];
    unsigned int timerFlA00;                    // +0xA00
    unsigned char padA04[0xA14 - 0xA04];
    unsigned int timerFlA14;                    // +0xA14
    unsigned char padA18[0xA40 - 0xA18];

    IlItemEntry();
    ~IlItemEntry();
};
typedef char IlItemEntrySizeCheck[(sizeof(IlItemEntry) == 0xA40) ? 1 : -1];

struct IlItemMgr
{
    IlItemMgr *DestroySelf();
    static void DestroyGlobal();
    int CalcHook();
    int DrawHook();
};

struct IlGameThread
{
    unsigned char pad00[0x60];
    unsigned int flags60;                       // +0x60
};

struct IlPlayerMgr
{
    unsigned char pad0000[0xF7C];
    int fieldF7C;                               // +0xF7C
    unsigned char pad0F80[0x438DC - 0xF80];
    int field438DC;                             // +0x438DC
    unsigned char pad438E0[0x438EC - 0x438E0];
    unsigned int flags438EC;                    // +0x438EC
    unsigned char pad438F0[0x43A90 - 0x438F0];
    int field43A90;                             // +0x43A90

    static int GetF7C();
    static int Get43A90();
    static void SetFlag3AndInc();
    static void ClearFlag3AndZero();
    static int IsActive438DCOrFlag1();
};

struct Il4B8934
{
    unsigned char pad00[0x7C];
    unsigned int flags7C;                       // +0x7C
    int pad80;
    int field84;                                // +0x84

    static void AddField84(int delta);
};

struct IlLaserTimer
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    IlLaserTimer() { flags &= ~1U; }
};

struct IlLaserSub48
{
    unsigned int flags;                         // +0x00
    unsigned char pad04[0x44];

    IlLaserSub48() { flags &= ~1U; }
};
typedef char IlLaserSub48SizeCheck[(sizeof(IlLaserSub48) == 0x48) ? 1 : -1];

struct IlFlagBit0
{
    unsigned int flags;
    IlFlagBit0() { flags &= ~1U; }
};

struct IlLaserNodeVtbl
{
    void *pad[5];
    void (__thiscall *draw)(void *self);        // +0x14
};

struct IlLaserNode
{
    IlLaserNodeVtbl *vtbl;                      // +0x00
    IlLaserNode *prev;                          // +0x04
    IlLaserNode *next;                          // +0x08
    int state0C;                                // +0x0C

    void LinkAfter(IlLaserNode *other);
};

struct IlLaserBase
{
    void *vtbl;                                 // +0x00
    unsigned char pad04[0x14 - 0x04];
    IlLaserTimer timer14;                       // +0x14..+0x27 (flags at +0x24)
    IlLaserTimer timer28;                       // +0x28..+0x3B (flags at +0x38)
    IlLaserTimer timer3C;                       // +0x3C..+0x4F (flags at +0x4C)
    IlVec3 pos50;                               // +0x50
    unsigned char pad5C[0x68 - 0x5C];
    float angle68;                              // +0x68
    unsigned char pad6C[0x50 - 0x6C + 0x44];    // 0x50..0x93 -> wait: 0x94 - 0x6C = 0x28
};

struct IlLaserBaseFull
{
    void *vtbl;                                 // +0x00
    unsigned char pad04[0x14 - 0x04];
    IlLaserTimer timer14;                       // +0x14..+0x27
    IlLaserTimer timer28;                       // +0x28..+0x3B
    IlLaserTimer timer3C;                       // +0x3C..+0x4F
    IlVec3 pos50;                               // +0x50
    unsigned char pad5C[0x68 - 0x5C];
    float angle68;                              // +0x68
    unsigned char pad6C[0x50];                  // +0x6C..+0xBB -> wait: 0x50 + 0x44 = 0x94
};

struct IlLaserBaseExact
{
    unsigned char pad04[0x14 - 0x04];
    IlLaserTimer timer14;                       // +0x14..+0x27 (flags at +0x24)
    IlLaserTimer timer28;                       // +0x28..+0x3B (flags at +0x38)
    IlLaserTimer timer3C;                       // +0x3C..+0x4F (flags at +0x4C)
    IlVec3 pos50;                               // +0x50
    unsigned char pad5C[0x68 - 0x5C];
    float angle68;                              // +0x68
    unsigned char pad6C[0x94 - 0x6C];
    IlLaserSub48 subs94[18];                    // +0x94..+0x5A3
    unsigned char pad5A4[0x5B0 - 0x5A4];
    IlFlagBit0 flags5B0;                        // +0x5B0
    unsigned char pad5B4[0x5C0 - 0x5B4];

    virtual void DummyVtbl();
    IlLaserBaseExact();
    void OffsetPosA(float radius, IlVec3 *out);
    void OffsetPosB(float radius, IlVec3 *out);
    void OffsetPosC(float radius, IlVec3 *out);
    void OffsetPosD(float radius, IlVec3 *out);
};
typedef char IlLaserBaseExactSizeCheck[(sizeof(IlLaserBaseExact) == 0x5C0) ? 1 : -1];

struct IlCopy354 { unsigned int d[0xD5]; };

struct IlCurveNode
{
    IlCurveNode *next;                          // +0x00
    IlCurveNode *prev;                          // +0x04
    float val08;                                // +0x08
    float val0C;                                // +0x0C
    unsigned char pad10[0x3C - 0x10];
};
typedef char IlCurveNodeSizeCheck[(sizeof(IlCurveNode) == 0x3C) ? 1 : -1];

struct IlCurveObj
{
    unsigned char pad0000[0x12EC];
    IlCurveNode *head12EC;                      // +0x12EC

    IlCurveNode *AppendNode(float f);
};

struct IlLaserMgr
{
    unsigned char pad00[0x10];
    IlLaserBaseExact base10;                    // +0x10..+0x5CF
    IlLaserNode *tail5D0;                       // +0x5D0
    int count5D4;                               // +0x5D4
    int nextId5D8;                              // +0x5D8
    unsigned char pad5DC[0x5F8 - 0x5DC];
    unsigned int flags5F8;                      // +0x5F8
    unsigned int pad5FC;

    IlLaserMgr();
    void SetFlag5F8Bit0(int bit);
    void PushTail(IlLaserNode *n);
    void UnlinkAndDec(IlLaserNode *n);
    IlLaserMgr *DestroySelf();
    static void DestroyGlobal();
    int Draw();
    int DrawHook();
};
typedef char IlLaserMgrSizeCheck[(sizeof(IlLaserMgr) == 0x600) ? 1 : -1];

struct IlIsNZ0 { int v; int IsNonZero(); };

extern IlGlobals g_IlGlobals;                   // 0x004B4CC0
extern IlHelpMgr *g_IlHelpMgr;                  // 0x004B8A58
extern IlPlayerMgr *g_IlPlayerMgr;              // 0x004B8A80
extern IlItemMgr *g_IlItemMgr;                  // 0x004B8A5C
extern IlGameThread *g_IlGameThread;            // 0x004B8954
extern int g_Il4B4D38;                          // 0x004B4D38
extern int g_Il4B4CDC;                          // 0x004B4CDC
extern int g_Il4B4D78;                          // 0x004B4D78
extern int g_Il4B4D34;                          // 0x004B4D34
extern int g_Il4B4D30;                          // 0x004B4D30
extern void *g_IlLaserBaseVtbl;                 // 0x004A2BB4
extern float g_IlDefaultTimeScale;              // 0x004B6F58
extern Il4B8934 *g_Il4B8934;                    // 0x004B8934
extern IlLaserMgr *g_IlLaserMgr;                // 0x004B8A60

extern int __stdcall IlHelpMgrInit(IlHelpMgr *h); // 0x00427A10
extern void IlHelpMgrDtor(IlHelpMgr *h);        // 0x00427B20
extern void IlItemMgrDtor(IlItemMgr *m);        // 0x00428430
extern int __stdcall IlItemMgrCalc(IlItemMgr *m); // 0x00428610
extern int IlItemMgrDraw(IlItemMgr *m);         // 0x00429210
extern void IlLaserMgrDtor(IlLaserMgr *m);      // 0x00429FB0

IlGlobals *IlResetGlobals();
void IlNoOp4281C0();
int IlRet0_4293C0();
int IlGet4B4D38();
void __stdcall IlAddPowerClamped(float v);
int IlGet4B4D34();
void IlClamp4B4D30Min();
void IlNoOp4299B0();
void IlNoOp429CD0();
void IlCopy354A(const IlCopy354 *src, IlCopy354 *dst);
void IlCopy354B(const IlCopy354 *src, IlCopy354 *dst);
