#pragma once

#include <stddef.h>

struct LmVec3
{
    float x;
    float y;
    float z;

    float DistSq2D(const LmVec3 *other);
    void ScaleTo(LmVec3 *out, float s);
    int IsOutsidePlayfield(float halfW, float halfH);
    void FromAngle(float angle, float radius);
};

struct LmTimer
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    void SetFloat(float v);
};

struct LmLaserNodeVtbl
{
    void *pad00[7];
    void (__thiscall *cancel)(void *self, int a, int b);                                                     // +0x1C
    int (__thiscall *cancelBoxA)(void *self, const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b); // +0x20
    int (__thiscall *cancelCircle)(void *self, const LmVec3 *pos, float f, int a, int b);                    // +0x24
    int (__thiscall *cancelBoxB)(void *self, const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b, int c); // +0x28
    void *pad2C;
    int (__thiscall *check30)(void *self, int a, float f);                                                   // +0x30
};

struct LmLaserNode
{
    LmLaserNodeVtbl *vtbl;                      // +0x00
    LmLaserNode *prev;                          // +0x04
    LmLaserNode *next;                          // +0x08
    int state0C;                                // +0x0C
    unsigned char pad10[0x7C - 0x10];
    unsigned char byte7C;                       // +0x7C
    unsigned char byte7D;                       // +0x7D
    unsigned char pad7E[0x5A0 - 0x7E];
    LmTimer timer5A0;                           // +0x5A0..+0x5B3
    int field5B4;                               // +0x5B4

    int SetByte7CIfZero(int a, int b);
};

struct LmLaserMgr
{
    unsigned char pad00[0x18];
    LmLaserNode *head18;                        // +0x18
    unsigned char pad1C[0x5DC - 0x1C];
    LmVec3 boxMin5DC;                           // +0x5DC
    LmVec3 boxMax5E8;                           // +0x5E8
    unsigned int pad5F4;
    unsigned int flags5F8;                      // +0x5F8

    int CancelInBoxA(const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b);
    int CancelInBoxB(const LmVec3 *minP, const LmVec3 *maxP, float f, int a, int b, int c);
    static int ResetAllTimersAndCancel();
    static int __stdcall CancelInCircle(const LmVec3 *pos, float f, int a, int b);
    static int CancelAll(int a, int b);
    int CheckAll30(int a, float f);
};

struct LmLaserOwner
{
    unsigned char pad000[0x5B8];
    int type5B8;                                // +0x5B8
    int sub5BC;                                 // +0x5BC
};

struct LmLaserLine
{
    unsigned char pad000[0x4E4];
    LmLaserOwner *owner4E4;                     // +0x4E4

    int GetTableVal(int idx);
    int GetSubPlus234();
};

struct LmLaserCmd
{
    unsigned char pad00[0x10];
    int arg10;                                  // +0x10
    unsigned char pad14[0x20 - 0x14];
    int type20;                                 // +0x20
    int flag24;                                 // +0x24
    int pad28;                                  // +0x28
};
typedef char LmLaserCmdSizeCheck[(sizeof(LmLaserCmd) == 0x2C) ? 1 : -1];

struct LmLaserInf
{
    unsigned char pad000[0x0C];
    int state0C;                                // +0x0C
    unsigned char pad010[0x594 - 0x10];
    int cmdIdx594;                              // +0x594
    int field598;                               // +0x598
    unsigned char pad59C[0x5B4 - 0x59C];
    int field5B4;                               // +0x5B4
    unsigned char pad5B8[0x61C - 0x5B8];
    LmLaserCmd cmds61C[18];                     // +0x61C..+0x933
    unsigned char pad934[0xDE8 - 0x934];
    unsigned int flagsDE8;                      // +0xDE8

    void ProcessCmds();
};

struct LmCurveNode
{
    LmCurveNode *next;                          // +0x00
    LmCurveNode *prev;                          // +0x04
    float f08;                                  // +0x08
    float f0C;                                  // +0x0C
    int i10;                                    // +0x10
    int i14;                                    // +0x14
    int i18;                                    // +0x18
    int i1C;                                    // +0x1C
    int i20;                                    // +0x20
    int i24;                                    // +0x24
    int i28;                                    // +0x28
    float f2C;                                  // +0x2C
    float f30;                                  // +0x30
    float f34;                                  // +0x34
    float f38;                                  // +0x38

    void CopyFrom(const LmCurveNode *src);
};
typedef char LmCurveNodeSizeCheck[(sizeof(LmCurveNode) == 0x3C) ? 1 : -1];

struct LmLaserCurve
{
    unsigned char pad0000[0x12E4];
    void *buf12E4;                              // +0x12E4
    void *buf12E8;                              // +0x12E8
    LmCurveNode *head12EC;                      // +0x12EC

    int Cleanup();
};

struct LmFloatPair { float x; float y; };
struct LmArray88
{
    unsigned char pad00[0x88];
    LmFloatPair arr88[16];
};

struct LmIsNZ4
{
    int pad00;
    int field04;
    int IsNonZero4();
};

struct LmAnmVm
{
    unsigned char pad000[0x4E8];
    LmAnmVm();
};
typedef char LmAnmVmSizeCheck[(sizeof(LmAnmVm) == 0x4E8) ? 1 : -1];

struct LmChainNode
{
    int pad00;
    unsigned int flags04;
};

struct LmSupervisorChains
{
    unsigned char pad00000[0x0C];
    LmChainNode *node0C;                        // +0x0C
    LmChainNode *node10;                        // +0x10
    unsigned char pad00014[0x1C8A0 - 0x14];
    LmChainNode *node1C8A0;                     // +0x1C8A0
    LmChainNode *node1C8A4;                     // +0x1C8A4
};

struct LmMotherSub10
{
    void *vtbl;                                 // +0x00
    int f04;                                    // +0x04
    int f08;                                    // +0x08
    int f0C;                                    // +0x0C
    int f10;                                    // +0x10
    unsigned char pad14[0x20 - 0x14];

    LmMotherSub10();
};
typedef char LmMotherSub10SizeCheck[(sizeof(LmMotherSub10) == 0x20) ? 1 : -1];

struct LmMother
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x10 - 0x04];
    LmMotherSub10 sub10;                        // +0x10..+0x2F
    LmAnmVm vm30;                               // +0x30..+0x517
    unsigned char pad518[0x52C - 0x518];

    LmMother();
    static LmMother *Create();
    LmMother *DestroySelf();
    static void DestroyGlobal();
    int OnCalcInner();
    int OnCalcHook();
};
typedef char LmMotherSizeCheck[(sizeof(LmMother) == 0x52C) ? 1 : -1];

struct LmObj558
{
    unsigned char pad000[0x200];
    unsigned int flags200;                      // +0x200
    unsigned char pad204[0x558 - 0x204];
    int field558;                               // +0x558

    void Set558(int v);
    int GetFlag200Bit4();
};

extern LmLaserMgr *g_LmLaserMgr;                // 0x004B8A60
extern float g_LmDefaultTimeScale;              // 0x004B6F58
extern int g_LmLaserTable[16][18][4];           // 0x004B24D4
extern void *g_LmSub10Vtbl;                     // 0x004A6050
extern LmMother *g_LmMother;                    // 0x004B8A64
extern LmSupervisorChains *g_LmSupervisor;      // 0x004B8920
extern int g_Lm4D2E68;                          // 0x004D2E68
extern unsigned int g_Lm4D59E8;                 // 0x004D59E8
extern unsigned int g_Lm4D33F8;                 // 0x004D33F8
extern int g_Lm4D33C0;                          // 0x004D33C0

extern int __stdcall LmMotherInit(LmMother *m); // 0x00433110
extern void __stdcall LmMotherDtor(LmMother *m);// 0x00433210
extern void __stdcall Lm435D20(void *p);        // 0x00435D20

int LmRet0_42C170();
int LmRet0_42DB90();
int LmRet0_432A70();
int __stdcall LmRet0_S5(int a, int b, int c, int d, int e);
int __stdcall LmRet0_S6(int a, int b, int c, int d, int e, int f);
int __stdcall LmRet0_S4(int a, int b, int c, int d);
int __stdcall LmRet0_S2(int a, int b);
void LmNoOp432AE0();
void LmNoOp432B30();
float LmGetFloat88(int idx, const LmArray88 *a);
void LmCopyCurveNode(const LmCurveNode *src, LmCurveNode *dst);
void LmNoOp432DE0();
void LmPauseAllChains();
void LmNoOp433620();
