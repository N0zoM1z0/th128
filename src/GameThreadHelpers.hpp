#pragma once

#include <stddef.h>

struct GtVec3
{
    float x;
    float y;
    float z;
};

struct GtChainElem
{
    unsigned int pad00;
    unsigned int flags;                         // +0x04
};

struct GtGuiMini
{
    unsigned char pad0000[0x5A04];
    unsigned int flags5A04;                     // +0x5A04
    unsigned int pad5A08;
    int field5A0C;                              // +0x5A0C

    static int IsBannerActive();
};

struct GtPairChainA
{
    unsigned int pad00[2];
    GtChainElem *a;                             // +0x08
    GtChainElem *b;                             // +0x0C

    static void EnableBoth();
};

struct GtPairChainB
{
    unsigned int pad00[2];
    GtChainElem *a;                             // +0x08
    GtChainElem *b;                             // +0x0C

    static void EnableBoth();
};

struct GtFpsDouble
{
    unsigned char pad00[0x24];
    double d24;                                 // +0x24
    double d2C;                                 // +0x2C

    static double __stdcall Set24(double v);
    static double __stdcall Set2C(double v);
};

struct GtLaserVtbl
{
    void *pad[6];
    void (__thiscall *onDelete)(void *self);    // +0x18
};

struct GtLaserNode
{
    GtLaserVtbl *vtbl;                          // +0x00
    GtLaserNode *prev;                          // +0x04
    GtLaserNode *next;                          // +0x08

    void Unlink();
};

struct GtLaserMgr
{
    unsigned char pad00[0x18];
    GtLaserNode *head18;                        // +0x18

    static void DeleteAll();
};

struct GtEnemyTimerMgr
{
    unsigned char pad00[0xA0];
    int timerPrev;                              // +0xA0
    int timerCur;                               // +0xA4
    float timerCurF;                            // +0xA8
    float *timerSpeed;                          // +0xAC
    unsigned int timerFlags;                    // +0xB0
    unsigned int padB4;
    void *headB8;                               // +0xB8
    void *tailBC;                               // +0xBC

    static int ResetTimerAndLists();
};

struct GtGameThread
{
    unsigned char pad00[0x60];
    unsigned int flags60;                       // +0x60
    unsigned char pad64[0x74 - 0x64];
    int isReplay;                               // +0x74

    static void DestroyGlobal();
    GtGameThread *DestroySelf();
    int DrawHook();
    int ClearAnmIfNotFlag2();
};

struct GtAnmClear
{
    unsigned char pad00[0xA0];
    int fA0;                                    // +0xA0
    int fA4;                                    // +0xA4
    int fA8;                                    // +0xA8
    int fAC;                                    // +0xAC

    static int ClearRet1();
    static void Clear();
};

struct GtStageInfo40
{
    unsigned char data[40];
};

struct GtTimestampObj
{
    unsigned char pad00[0x88];
    double ts88;                                // +0x88

    static void __stdcall Save(GtTimestampObj *obj);
};

struct GtSoundComVtbl
{
    void *pad0[9];
    int (__stdcall *getStatus)(void *self, unsigned int *out); // +0x24
    void *pad1[8];
    int (__stdcall *stop)(void *self);          // +0x48
};

struct GtSoundCom
{
    GtSoundComVtbl *vtbl;
};

struct GtSoundSlot
{
    GtSoundCom *buf;                            // +0x00
    unsigned int pad04[4];
    unsigned int wasPlaying;                    // +0x14

    void StopAndSaveStatus();
};
typedef char GtSoundSlotSizeCheck[(sizeof(GtSoundSlot) == 0x18) ? 1 : -1];

struct GtReplayMgr
{
    unsigned char pad00[0x0C];
    GtChainElem *elem0C;                        // +0x0C
    unsigned char pad10[0x3A8 - 0x10];
    GtChainElem *elem3A8;                       // +0x3A8

    static void DisableBoth();
};

struct GtGlobalsObj
{
    int f00;                                    // +0x00
    int f04;                                    // +0x04
    int f08;                                    // +0x08
    int f0C;                                    // +0x0C
    int f10;                                    // +0x10
    int f14;                                    // +0x14
    int f18;                                    // +0x18
    int f1C;                                    // +0x1C
    unsigned char pad20[0x3C - 0x20];
    int f3C;                                    // +0x3C
    int f40;                                    // +0x40
    int f44;                                    // +0x44
    int f48;                                    // +0x48
    int f4C;                                    // +0x4C
    unsigned char pad50[0x58 - 0x50];
    int f58;                                    // +0x58
    int f5C;                                    // +0x5C
    int f60;                                    // +0x60
    int f64;                                    // +0x64
    int f68;                                    // +0x68
    int f6C;                                    // +0x6C
    int f70;                                    // +0x70
    int f74;                                    // +0x74
    int f78;                                    // +0x78
    int f7C;                                    // +0x7C
    int f80;                                    // +0x80
    unsigned char pad84[0x90 - 0x84];
    unsigned int flags90;                       // +0x90
    unsigned char pad94[0xA0 - 0x94];
    float fA0;                                  // +0xA0
    int fA4;                                    // +0xA4
    int fA8;                                    // +0xA8
    int fAC;                                    // +0xAC
    int fB0;                                    // +0xB0

    void Set44AndUpdateGui();
    void Inc60And64();
    void Inc68Clamp9();
    int Get58();
    void Set0C1AndClear10();
    void SetFlag3(int bit);
    void SetFlag1(int bit);
    void SetFlag0(int bit);
    int GetFlag1();
    int Get1C();
    void Set70(int v);
    void ClearFlag2();
    void Max08With78();
    void Set3C(int v);
    void Clear40();
    void SetB0(int v);
    void SetA4Clamp100000(int v);
    void SetFA0(float v);
    void Clear48();
    void Set04(int v);
    void Set80(int v);
    void Set68(int v);
    void Set4C(int v);
    void Clear60And64();
};

extern GtGuiMini *g_GtGui;                      // 0x004B8950
extern GtPairChainA *g_Gt4B8940;                // 0x004B8940
extern GtPairChainB *g_Gt4B8A90;                // 0x004B8A90
extern GtFpsDouble *g_Gt4B894C;                 // 0x004B894C
extern GtLaserMgr *g_Gt4B8A60;                  // 0x004B8A60
extern GtEnemyTimerMgr *g_GtEnemyMgr;           // 0x004B8948
extern float g_GtDefaultTimeScale;              // 0x004B6F58
extern GtGameThread *g_GtGameThread;            // 0x004B8954
extern int g_Gt4D59E8;                          // 0x004D59E8
extern GtAnmClear *g_GtAnmMgr;                  // 0x004D2E50
extern char g_GtPathBuf[260];                   // 0x004DB878
extern int g_Gt4B4CC8;                          // 0x004B4CC8
extern int g_Gt4B4D34;                          // 0x004B4D34
extern int g_Gt4B4D38;                          // 0x004B4D38
extern int g_Gt4B4D04;                          // 0x004B4D04
extern int g_Gt4B4D08;                          // 0x004B4D08
extern int g_GtStageNum;                        // 0x004B4D14
extern int g_GtRouteNum;                        // 0x004B4D44
extern GtStageInfo40 g_GtStageTable[32];        // 0x004B1A30
extern GtStageInfo40 *g_GtCurStageInfo;         // 0x004B8A98
extern int g_Gt4D5A88;                          // 0x004D5A88
extern GtSoundSlot g_GtSoundSlots[76];          // 0x004D73F0..0x004D7B0F
extern GtReplayMgr *g_GtReplayMgr;              // 0x004B8A84
extern int g_Gt4D33CC;                          // 0x004D33CC

extern void __stdcall GtGameThreadDtor(GtGameThread *gt); // 0x00426430
extern int __stdcall GtGameThreadDraw(GtGameThread *gt);  // 0x004269D0
extern void __stdcall GtGuiUpdateBombs(GtGuiMini *gui, int bombs, int pieces); // 0x0041FD50
extern double GtGetTimeSeconds();               // 0x00455160

void GtPlayfieldToScreen(GtVec3 *out, const GtVec3 *in);
char *GtCopyToPathBuf(const char *s);
int GtAddGrazeOrPoints(int delta);
void GtDecrementBomb();
void __stdcall GtAdvanceStage(int branch);
void GtStopAllSoundBuffers();
void __stdcall GtSet1CMul1000(GtGlobalsObj *obj, float sec);
int GtGet4D33CC();
int GtIs4D33CCZero();
void GtNoOp4277B0();
