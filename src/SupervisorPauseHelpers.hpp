#pragma once

#include <stddef.h>

struct SpVec3
{
    float x;
    float y;
    float z;
};

struct SpAnmVmDel
{
    unsigned char pad000[0x4AC];
    void *extraData;                            // +0x4AC

    SpAnmVmDel *DeleteSelf();
};

struct SpComObjVtbl
{
    void *pad00[2];
    unsigned long (__stdcall *release)(void *self); // +0x08
};

struct SpComObj
{
    SpComObjVtbl *vtbl;
};

struct SpSoundBufVtbl
{
    void *pad00[12];
    int (__stdcall *play)(void *self, unsigned int, unsigned int, unsigned int); // +0x30
};

struct SpSoundBuf
{
    SpSoundBufVtbl *vtbl;
};

struct SpSoundDesc
{
    int pad00[3];
    unsigned int playFlags0C;                   // +0x0C
};

struct SpSoundSlot
{
    SpSoundBuf *buf00;                          // +0x00
    int pad04;                                  // +0x04
    SpSoundDesc *desc08;                        // +0x08
    int pad0C;                                  // +0x0C
    int pad10;                                  // +0x10
    int active14;                               // +0x14

    void Resume();
};
typedef char SpSoundSlotSizeCheck[(sizeof(SpSoundSlot) == 0x18) ? 1 : -1];

struct SpCritSec
{
    unsigned char raw[0x18];
};
typedef char SpCritSecSizeCheck[(sizeof(SpCritSec) == 0x18) ? 1 : -1];

struct SpSupervisor
{
    unsigned char pad000[0x114];
    int field114;                               // +0x114
    unsigned char pad118[0x200 - 0x118];
    unsigned int flags200;                      // +0x200
    unsigned char pad204[0x590 - 0x204];
    unsigned int flags590;                      // +0x590
    unsigned char pad594[0x810 - 0x594];
    SpCritSec critSecs810[12];                  // +0x810..+0x92F
    unsigned char lockDepth930[12];             // +0x930..+0x93B
    unsigned char pad93C[0x9C4 - 0x93C];

    void InitCritSecs();
    void DeleteCritSecs();
    void UnlockCritSec(int idx);
    void LockCritSec(int idx);
    int GetFlag200Bit6();
    int GetFlag200Bit5();
    int GetFlag200Bit3();
    int GetFlag200Bit1();
    int GetField114();
    int GetFlag200Bit0();
    int GetFlag200Bit2();
};
typedef char SpSupervisorSizeCheck[(sizeof(SpSupervisor) == 0x9C4) ? 1 : -1];

struct SpComHolder
{
    unsigned char pad00[0x0C];
    SpComObj *obj0C;                            // +0x0C

    void Release0C();
};

struct SpAnmMgr
{
    unsigned char pad000000[0x4E9688];
    int field4E9688;                            // +0x4E9688
    unsigned char byte4E968C;                   // +0x4E968C
    unsigned char byte4E968D;                   // +0x4E968D
    unsigned char byte4E968E;                   // +0x4E968E
    unsigned char byte4E968F;                   // +0x4E968F
    unsigned char byte4E9690;                   // +0x4E9690
    unsigned char pad4E9691;
    unsigned char byte4E9692;                   // +0x4E9692
    unsigned char byte4E9693;                   // +0x4E9693
    int field4E9694;                            // +0x4E9694
    SpComObj *surf4E9698;                       // +0x4E9698

    static void ReleaseSurf();
    static void ResetByte8E();
    static void ResetByte93();
    static void ResetByte92();
    static void ResetByte90();
    static void ResetByte8F();
    static void ResetByte8C();
    static void ResetByte8D();
    static void ResetField88();
    static void ResetField94();
};

struct SpObj10
{
    unsigned char pad00[0x10];
    int field10;
    int GetField10();
};

struct SpGuiMgr
{
    unsigned char pad0000[0x58FC];
    int score58FC;                              // +0x58FC
    unsigned char pad5900[0x5A1C - 0x5900];
    int field5A1C;                              // +0x5A1C

    static int Get5A1C();
    static void SyncScore();
};

#pragma pack(push, 4)
struct SpFpsCounter
{
    unsigned char pad00[0x24];
    double d24;                                 // +0x24
    double d2C;                                 // +0x2C

    static float GetSlowdownPct();
};
#pragma pack(pop)

struct SpHelpMgr
{
    unsigned char pad000[0x128];
    int field128;                               // +0x128

    static int Get128();
};

struct SpChainNode
{
    int pad00;
    unsigned int flags04;
};

struct SpPauseMgr
{
    unsigned int flags00;                       // +0x00
    unsigned int pad04;
    SpChainNode *node08;                        // +0x08
    SpChainNode *node0C;                        // +0x0C
    unsigned char pad10[0x20 - 0x10];
    unsigned int timerFl20;                     // +0x20
    unsigned char pad24[0x34 - 0x24];
    unsigned int timerFl34;                     // +0x34
    int field38;                                // +0x38
    int pad3C;
    int field40;                                // +0x40
    unsigned char pad44[0xC4 - 0x44];
    int fieldC4;                                // +0xC4
    unsigned char padC8[0x108 - 0xC8];
    int field108;                               // +0x108
    int field10C;                               // +0x10C
    int field110;                               // +0x110
    int pad114;
    int field118;                               // +0x118
    unsigned char pad11C[0x19C - 0x11C];
    int field19C;                               // +0x19C
    unsigned char pad1A0[0x1E0 - 0x1A0];
    int field1E0;                               // +0x1E0
    int field1E4;                               // +0x1E4
    unsigned char pad1E8[0x2E0 - 0x1E8];

    SpPauseMgr();
    static void PauseBothChains();
};
typedef char SpPauseMgrSizeCheck[(sizeof(SpPauseMgr) == 0x2E0) ? 1 : -1];

struct SpCmp0
{
    int v;
    int NotEqual(int x);
    int LessThan(int x);
    int Equal(int x);
};

struct SpArrayD4
{
    unsigned char pad00[0x90];
    int items90[17];                            // +0x90
    int countD4;                                // +0xD4

    void Push90(int val);
};

struct SpCaptureSlot
{
    int f00;
    int f04;
    int f08;
    int f0C;
    int f10;
    int f14;
    int f18;
    int f1C;
    int f20;
    int f24;
};
typedef char SpCaptureSlotSizeCheck[(sizeof(SpCaptureSlot) == 0x28) ? 1 : -1];

struct SpCaptureMgr
{
    SpCaptureSlot slots[4];
    int AllocSlot(int a, int b, int c, int d, int e, int f);
};

struct SpVm428Sub
{
    int pad00;
    int field04;
};

struct SpVm428
{
    unsigned char pad000[0x428];
    SpVm428Sub *sub428;
    int GetSub428Field04();
};

struct SpGameThread
{
    unsigned char pad00[0x08];
    SpChainNode *node08;                        // +0x08
    unsigned char pad0C[0x14 - 0x0C];
    int field14;                                // +0x14

    static int Get14();
    static int IsNode08Flag1();
};

struct SpPlayerOpt
{
    unsigned char pad00[0xE4];
};
typedef char SpPlayerOptSizeCheck[(sizeof(SpPlayerOpt) == 0xE4) ? 1 : -1];

struct SpPlayer
{
    unsigned char pad0000[0xECC];
    SpVec3 posECC;                              // +0xECC
    SpVec3 posED8;                              // +0xED8
    unsigned char padEE4[0xEF0 - 0xEE4];
    SpVec3 posEF0;                              // +0xEF0
    SpVec3 posEFC;                              // +0xEFC
    unsigned char padF08[0xF64 - 0xF08];
    int fieldF64;                               // +0xF64
    int fieldF68;                               // +0xF68
    unsigned char padF6C[0x8888 - 0xF6C];
    int optFlag8888;                            // +0x8888 (stride 0xE4 x 8)
    unsigned char pad888C[0x438EC - 0x888C];
    unsigned int flags438EC;                    // +0x438EC

    static SpVec3 *GetPosED8();
    static SpVec3 *GetPosEF0();
    static SpVec3 *GetPosEFC();
    void ResetAllOptFlags();
    void SetFlag438ECBit1(int bit);
    SpPlayer *DestroySelf();
    static void DestroyGlobal();
    void SetFlag1AndZeroF64();
};

struct SpFlag10 { unsigned char pad[0x10]; unsigned int fl; SpFlag10(); };
struct SpFlagC8 { unsigned char pad[0xC8]; unsigned int fl; SpFlagC8(); };
struct SpFlag5C { unsigned char pad[0x5C]; unsigned int fl; SpFlag5C(); };

struct SpStageInfo
{
    unsigned char raw[0x28];
};
typedef char SpStageInfoSizeCheck[(sizeof(SpStageInfo) == 0x28) ? 1 : -1];

extern unsigned char g_Sp4D304B;                // 0x004D304B
extern SpComObj *g_SpSurf3014;                  // 0x004D3014
extern SpComObj *g_SpSurf3018;                  // 0x004D3018
extern SpComObj *g_SpSurf301C;                  // 0x004D301C
extern int g_Sp4B4D18;                          // 0x004B4D18
extern int g_Sp4B4D14;                          // 0x004B4D14
extern SpStageInfo g_SpStageInfos[16];          // 0x004B1A30
extern SpStageInfo *g_SpStageInfoPtr;           // 0x004B8A98
extern int g_Sp4D33BC;                          // 0x004D33BC
extern int g_Sp4D33C0;                          // 0x004D33C0
extern SpAnmMgr *g_SpAnmMgr;                    // 0x004D2E50
extern SpSupervisor g_SpSupervisor;             // 0x004D2E68
extern unsigned int g_Sp4D33F8;                 // 0x004D33F8
extern SpGuiMgr *g_SpGuiMgr;                    // 0x004B8950
extern int g_Sp4B4CC4;                          // 0x004B4CC4
extern int g_Sp4B4CC0;                          // 0x004B4CC0
extern SpFpsCounter *g_SpFpsCounter;            // 0x004B894C
extern SpHelpMgr *g_SpHelpMgr;                  // 0x004B8A58
extern SpPauseMgr *g_SpPauseMgr;                // 0x004B8A7C
extern SpSoundSlot g_SpSoundSlots[76];          // 0x004D73F0
extern SpGameThread *g_SpGameThread;            // 0x004B8954
extern SpPlayer *g_SpPlayer;                    // 0x004B8A80

extern void (__stdcall *g_SpInitCritSec)(SpCritSec *);   // 0x0049A090
extern void (__stdcall *g_SpDelCritSec)(SpCritSec *);    // 0x0049A094
extern void (__stdcall *g_SpEnterCritSec)(SpCritSec *);  // 0x0049A088
extern void (__stdcall *g_SpLeaveCritSec)(SpCritSec *);  // 0x0049A08C
extern void __stdcall SpPlayerDtor(SpPlayer *p);         // 0x0043ACD0

int SpCheck4D304B();
void SpReleaseAllSurfaces();
void SpSyncStageInfoPtr();
void SpVec3Cross(SpVec3 *out, const SpVec3 *a, const SpVec3 *b);
const char *SpStrChrWrap(const char *s, int ch);
void SpReset4D33BC();
void SpWriteWord(unsigned short val, unsigned short *dst);
SpSupervisor *SpResetSupervisor();
int SpRet0_436580();
int SpRet0_436590();
void SpResumeAllSoundBuffers();
int SpRet0_43ACB0();
int SpRet0_43ACC0();
