#pragma once

#include <stddef.h>

struct EmVec2
{
    float x;
    float y;
};

struct EmVec3
{
    float x;
    float y;
    float z;
};

struct EmListNode
{
    void *owner;                                // +0x00
    EmListNode *next;                           // +0x04
    EmListNode *prev;                           // +0x08
};

struct EmItemNode
{
    unsigned char pad00[0x08];
    EmItemNode *next;                           // +0x08
    unsigned char pad0C[0x80 - 0x0C];
    int id;                                     // +0x80
};

struct EmItemMgrMini
{
    unsigned char pad00[0x18];
    EmItemNode *head18;                         // +0x18

    static EmItemNode *FindById(int id);
};

struct EmChainElem
{
    unsigned int pad00;
    unsigned int flags;                         // +0x04
};

struct EmHpSub
{
    int hp;                                     // +0x00 (0x5004)
    int field04;                                // +0x04 (0x5008)
    int field08;                                // +0x08 (0x500C)
    int rawHp;                                  // +0x0C (0x5010)
    int minHp;                                  // +0x10 (0x5014)
    unsigned int flags14;                       // +0x14 (0x5018)
};

struct EmEnemyObj
{
    unsigned char pad0000[0x11FC];
    EmVec2 pos;                                 // +0x11FC
    unsigned char pad1204[0x145C - 0x1204];
    EmListNode node;                            // +0x145C
    unsigned char pad1468[0x5004 - 0x1468];
    EmHpSub hp;                                 // +0x5004..+0x501B
    unsigned char pad501C[0x50AC - 0x501C];
    unsigned int flags50AC;                     // +0x50AC

    void UnlinkFromMgr();
};

struct EmEnemyNode
{
    EmEnemyObj *obj;                            // +0x00
    EmEnemyNode *next;                          // +0x04
};

struct EmEnemyMgr
{
    unsigned int flags00;                       // +0x00
    unsigned int pad04;                         // +0x04
    EmChainElem *calcElem;                      // +0x08
    EmChainElem *drawElem;                      // +0x0C
    unsigned char pad10[0xB0 - 0x10];
    unsigned int timerFlagsB0;                  // +0xB0
    unsigned int padB4;                         // +0xB4
    EmListNode *headB8;                         // +0xB8
    EmListNode *tailBC;                         // +0xBC
    int countC0;                                // +0xC0
    unsigned char padC4[0xCC - 0xC4];

    EmEnemyMgr();
    static void EnableBoth();
    static EmEnemyMgr *__stdcall Create(const char *arg);
    EmEnemyMgr *DestroySelf();
    static void DestroyGlobal();
    void DamageInCircle(const EmVec2 *pos, float radius);
    void DamageInColumn(const EmVec2 *pos, float halfW);
    void DamageInCircle2(const EmVec2 *pos, float radius);
    static EmEnemyObj *__stdcall FindNearest(const EmVec2 *pos, float maxDist);
};
typedef char EmEnemyMgrSizeCheck[(sizeof(EmEnemyMgr) == 0xCC) ? 1 : -1];

struct EmSub1098B
{
    void *vtbl;                                 // +0x00
    unsigned char pad04[0x8C - 0x04];
    void *buf8C;                                // +0x8C
    unsigned char pad90[0x1090 - 0x90];
    int field1090;                              // +0x1090
    int field1094;                              // +0x1094

    EmSub1098B();
    ~EmSub1098B();
    EmSub1098B *DestroySelf();
};
typedef char EmSub1098BSizeCheck[(sizeof(EmSub1098B) == 0x1098) ? 1 : -1];

struct EmSlot10
{
    int f00;                                    // +0x00 (+0x50C4)
    int f04;                                    // +0x04 (+0x50C8)
    int f08;                                    // +0x08 (+0x50CC)
    int f0C;                                    // +0x0C (+0x50D0)
};

struct EmSlotsObj
{
    unsigned char pad0000[0x50C4];
    EmSlot10 slots[16];                         // +0x50C4

    void SetSlotF0C(int idx, int v);
};

struct EmTimer
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10
};

struct EmInterp2D
{
    EmVec2 start;                               // +0x00
    EmVec2 end;                                 // +0x08
    EmVec2 vel0;                                // +0x10
    EmVec2 vel1;                                // +0x18
    EmTimer timer;                              // +0x20..+0x33
    int duration;                               // +0x34
    int mode;                                   // +0x38

    void SetMode(int m);
    void SetStart(const EmVec2 *v);
    void SetEnd(const EmVec2 *v);
    void SetVel0Global();
    void SetVel1Global();
    void SetDuration(int d);
    void ResetTimer();
    int GetDuration();
    void ClearDuration();
};

struct EmShort41E { unsigned char pad[0x41E]; short v; int Get(); };

struct EmField5C
{
    unsigned char pad00[0x5C];
    int field5C;                                // +0x5C
    int pad60;
    int field64;                                // +0x64

    void Set5C(int v);
};

struct EmFloat28
{
    unsigned char pad00[0x28];
    float f28;                                  // +0x28
    float f2C;                                  // +0x2C
    unsigned int flags30;                       // +0x30

    float Get28();
    float Get2C();
    void SetFlag1();
    void Set2C(float v);
};

extern EmItemMgrMini *g_EmItemMgr;              // 0x004B8A60
extern EmEnemyMgr *g_EmEnemyMgr;                // 0x004B8948
extern void *g_EmVtbl4A2720;                    // 0x004A2720
extern void *g_EmVtbl4A26DC;                    // 0x004A26DC
extern float g_EmDefaultTimeScale;              // 0x004B6F58
extern EmVec2 g_EmGlobalVel;                    // 0x004D2E60
extern int g_Em4D33CC;                          // 0x004D33CC
extern int g_Em4B4D30;                          // 0x004B4D30

extern int __stdcall EmEnemyMgrInit(EmEnemyMgr *mgr, const char *arg); // 0x00415310
extern void EmEnemyMgrDtor(EmEnemyMgr *mgr);    // 0x00415660
extern float __stdcall EmWrapAngle(float a);    // 0x0046A700

int EmRet0_415610();
int EmRet0_415620();
int EmRet1_415A10();
int EmRet1_415A60();
void EmNoOp41E450();
void EmVec2Sub(EmVec2 *out, const EmVec2 *a, const EmVec2 *b);
void EmVec2Add(EmVec2 *out, const EmVec2 *a, const EmVec2 *b);
float __stdcall EmAngleDiff(float a, float b);
float EmVec2DistSq(const EmVec2 *a, const EmVec2 *b);
int EmGet4D33CC();
int EmGet4B4D30();
void __stdcall EmSetWrapped28(EmFloat28 *obj, float angle);
