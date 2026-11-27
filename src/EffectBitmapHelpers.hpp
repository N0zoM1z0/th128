#pragma once

#include <stddef.h>

struct EffVec2
{
    float x;
    float y;
};

struct EffVec3
{
    float x;
    float y;
    float z;
};

struct EffListNode
{
    void *owner;                                // +0x00
    EffListNode *next;                          // +0x04
    EffListNode *prev;                          // +0x08

    void InsertAfter(EffListNode *node);
    void Unlink();
};

struct EffBitmapMgr
{
    unsigned char pad000[0x65DE4];
    unsigned int bitmap[448 * 12];              // +0x65DE4..+0x6B1E3 (5376 dwords = 1792 * 3)

    void OrMask(const EffVec2 *pos, const unsigned int *mask, int h, int w);
    void ClearMask(const EffVec2 *pos, const unsigned int *mask, int h, int w);
};

int EffBitmapCountBits(EffBitmapMgr *bm);

struct EffPtrArray
{
    unsigned char pad00[0x34];
    void **items;                               // +0x34
    int count;                                  // +0x38

    void FreeAll();
};

struct EffItemMgrMini
{
    unsigned char pad00[0x14];
    unsigned char pool[0x1CAB00];               // +0x14
    int pad1CAB14;
    int field1CAB18;                            // +0x1CAB18
    int pad1CAB1C;
    int field1CAB20;                            // +0x1CAB20

    static void ResetPool();
};

struct EffAnmVmMini
{
    unsigned char pad000[0x42C];
    void *anmRes;                               // +0x42C
    unsigned char pad430[0x464 - 0x430];
    int pos464[3];                              // +0x464
    unsigned char pad470[0x4AC - 0x470];
    void *extraData;                            // +0x4AC
    unsigned int flags4B0;                      // +0x4B0
};

struct EffAnmNodeMini
{
    EffAnmVmMini *vm;                           // +0x00
    EffAnmNodeMini *next;                       // +0x04
};

struct EffAnmMgrMini
{
    unsigned char pad00[0x4E90E0];
    void *enemyAnms[8];                         // +0x4E90E0
    unsigned char pad4E9100[0x8B9704 - 0x4E9100];
    EffAnmNodeMini *listA;                      // +0x8B9704
    unsigned int pad8B9708;
    EffAnmNodeMini *listB;                      // +0x8B970C
};

struct EffEnemyVtbl
{
    void *pad[5];
    void (__thiscall *scalarDtor)(void *self, int flags); // +0x14
};

struct EffEnemyObj
{
    EffEnemyVtbl *vtbl;
};

struct EffEnemyNode
{
    EffEnemyObj *obj;                           // +0x00
    EffEnemyNode *next;                         // +0x04
};

struct EffChainElem
{
    unsigned int pad00;
    unsigned int flags;                         // +0x04
};

struct EffEnemyMgrMini
{
    unsigned char pad00[0x08];
    EffChainElem *calcElem;                     // +0x08
    EffChainElem *drawElem;                     // +0x0C
    unsigned char pad10[0x5C - 0x10];
    int field5C;                                // +0x5C
    unsigned char pad60[0xB8 - 0x60];
    EffEnemyNode *headB8;                       // +0xB8
    unsigned char padBC[0xC4 - 0xBC];
    int fieldC4;                                // +0xC4

    void CleanupAll();
    void DisableBoth();
};

struct EffInputStateMini
{
    unsigned char pad00[0x08];
    unsigned int repeatMask;                    // +0x08
    unsigned int triggerMask;                   // +0x0C
    unsigned int field10;                       // +0x10
    unsigned char pad14[0x94 - 0x14];
    unsigned int repeatCounters[32];            // +0x94..+0x113
    unsigned char pad114[0x194 - 0x114];
    unsigned int holdCounters[32];              // +0x194..+0x213
    unsigned int pad214;
    unsigned int curMask;                       // +0x218
    unsigned int prevMask;                      // +0x21C
    unsigned int outRepeat;                     // +0x220
    unsigned int outTrigger;                    // +0x224
    unsigned int outRelease;                    // +0x228
    unsigned int pad22C;
    unsigned int outHeld8;                      // +0x230

    void UpdateRepeatMasks();
    unsigned int GetConfirmMask();
    unsigned int TestTrigger(unsigned int mask);
    int TestTriggerOrRepeat(unsigned int mask);
};

struct EffCursorMini
{
    int value;                                  // +0x00
    int previous;                               // +0x04
    int count;                                  // +0x08
    unsigned char pad0C[0x8C - 0x0C];
    int field8C;                                // +0x8C
    unsigned char pad90[0xD0 - 0x90];
    int fieldD0;                                // +0xD0
    int fieldD4;                                // +0xD4

    void SavePrevious();
    int ClampSet(int v);
    void SetFieldD0(int v);
    void SetCount(int c);
    int GetValue();
    void ResetMenu();
};

struct EffMotion34
{
    int pos[3];                                 // +0x00
    int vel[3];                                 // +0x0C
    float f18;                                  // +0x18
    float f1C;                                  // +0x1C
    float f20;                                  // +0x20
    float f24;                                  // +0x24
    float f28;                                  // +0x28
    float f2C;                                  // +0x2C
    int field30;                                // +0x30

    void CopyTo(EffMotion34 &dst) const;
};
typedef char EffMotion34SizeCheck[(sizeof(EffMotion34) == 0x34) ? 1 : -1];

struct EffZero20
{
    int d[8];
};

void EffZero20Init(EffZero20 *self);

struct EffZero24Global
{
    int d[9];
    EffZero24Global();
};

struct EffGuiMini
{
    unsigned char pad00[0xB4];
    struct EffGuiSub *subB4;                    // +0xB4
    unsigned char padB8[0xC0 - 0xB8];
    int fieldC0;                                // +0xC0

    static int GetFieldC0();
    static int GetSubB4Field08();
    static EffGuiSub *GetSubB4();
};

struct EffGuiSub
{
    unsigned int pad00;
    unsigned int pad04;
    int field08;                                // +0x08
};

struct EffMode558
{
    unsigned char pad00[0x558];
    int field558;                               // +0x558
    unsigned char pad55C[0x590 - 0x55C];
    unsigned int flags590;                      // +0x590

    void SetMode558(int v);
};

struct EffClear0A  { int v; void Clear(); };
struct EffClear0B  { int v; void Clear(); };
struct EffSet0     { int v; void Set(int x); };
struct EffCopyVec3 { EffVec3 v; void CopyFrom(const EffVec3 *src); };
struct EffZero34   { unsigned char data[0x34]; void Zero(); };
struct EffSetAC    { unsigned char pad[0xAC]; int v; void Set(int x); };
struct EffGet0A    { int v; int Get(); };
struct EffGet0B    { int v; int Get(); };
struct EffGet04    { unsigned int pad; int v; int Get(); };
struct EffGet08    { unsigned int pad[2]; int v; int Get(); };
struct EffTable8C  { unsigned char pad[0x8C]; int (*rows)[2]; int Get(int idx); };

extern EffItemMgrMini *g_EffItemMgr;            // 0x004B8A5C
extern EffAnmMgrMini *g_EffAnmMgr;              // 0x004D2E50
extern EffGuiMini *g_EffGuiMgr;                 // 0x004B8948
extern EffZero24Global *g_Eff4B8940;            // 0x004B8940
extern int g_Eff4B4D6C;                         // 0x004B4D6C
extern float g_Eff4B4D60;                       // 0x004B4D60

unsigned int EffPopCount32(unsigned int x);
int EffGet4B4D6C();
int __stdcall EffCirclesOverlap2D(float x1, float y1, float r1, float x2, float y2, float r2);
void __stdcall EffAdd4B4D60(float delta);
void EffNoOp410D70();
void EffNoOp411D60();
int EffRet1_4120C0();
int EffRet1_4120D0();
int EffRet1_4120E0();
int EffRet1_4120F0();
int EffRet0_412260();
int EffRet0_412390();
int __fastcall EffMotionAttach(EffAnmVmMini *vm, const EffMotion34 *src);
