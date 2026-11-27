#pragma once

#include <stddef.h>

struct TaTimer
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    TaTimer() { flags &= ~1U; }
};

struct TaAnmVm
{
    unsigned char pad000[0x60];
    float field60;                              // +0x60
    unsigned char pad064[0x464 - 0x64];
    float field464;                             // +0x464
    float field468;                             // +0x468
    unsigned char pad46C[0x4B0 - 0x46C];
    unsigned int flags4B0;                      // +0x4B0

    void SetFloat60Flag3(float v);
    void SetFloat468(float v);
    void SetFloat464(float v);
};

struct TaNode4Sub { int pad00; int field04; };
struct TaNode4 { int pad00; TaNode4Sub *sub04; int IsNextNonZero(); };

struct TaStringList
{
    unsigned char pad00[0x34];
    void **arr34;                               // +0x34
    int count38;                                // +0x38

    void FreeAll();
};

struct TaSub10
{
    void *vtbl;
    int f04, f08, f0C, f10;
    unsigned char pad14[0x20 - 0x14];
    TaSub10() : vtbl(&g_TaSub10Vtbl), f04(0), f08(0), f0C(0), f10(0) {}
    static void *g_TaSub10Vtbl;
};

struct TaMenuBlock
{
    int f00;                                    // +0x00 (cursor)
    int pad04;                                  // +0x04
    int f08;                                    // +0x08 (999)
    unsigned char pad0C[0x8C - 0x0C];
    int f8C;                                    // +0x8C (0)
    unsigned char pad90[0xD0 - 0x90];
    int fD0;                                    // +0xD0 (1)
    int fD4;                                    // +0xD4 (0)

    TaMenuBlock()
    {
        f8C = 0;
        f00 = 0;
        fD4 = 0;
        fD0 = 1;
        f08 = 999;
    }
};
typedef char TaMenuBlockSizeCheck[(sizeof(TaMenuBlock) == 0xD8) ? 1 : -1];

struct TaStaffMgr
{
    unsigned int flags00;                       // +0x00
    unsigned char pad04[0x10 - 0x04];
    TaSub10 sub10;                              // +0x10..+0x2F
    unsigned char pad30[0x3C - 0x30];
    TaMenuBlock menu3C;                         // +0x3C..+0x113
    TaMenuBlock menu114;                        // +0x114..+0x1EB
    TaMenuBlock menu1EC;                        // +0x1EC..+0x2C3
    unsigned char pad2C4[0x2E4 - 0x2C4];

    TaStaffMgr();
    static TaStaffMgr *Create();
    TaStaffMgr *DestroySelf();
    static void DestroyPtr(TaStaffMgr *m);
};
typedef char TaStaffMgrSizeCheck[(sizeof(TaStaffMgr) == 0x2E4) ? 1 : -1];

struct TaBuf8C
{
    void *vtbl;                                 // +0x00
    unsigned char pad04[0x8C - 0x04];
    void *buf8C;                                // +0x8C

    TaBuf8C *DeleteSelf();
};

struct TaTitleMgr
{
    void *vtbl;                                 // +0x00
    unsigned int flags04;                       // +0x04
    unsigned char pad08[0x1C - 0x08];
    int state1C;                                // +0x1C
    int prevState20;                            // +0x20
    int subState24;                             // +0x24
    TaMenuBlock menu28;                         // +0x28..+0xFF
    TaMenuBlock menu100;                        // +0x100..+0x1D7
    TaMenuBlock menu1D8;                        // +0x1D8..+0x2AF
    unsigned char pad2B0[0x2B4 - 0x2B0];
    TaTimer timer2B4;                           // +0x2B4..+0x2C7 (flags at +0x2C4)
    unsigned char pad2C8[0x5944 - 0x2C8];
    TaMenuBlock menu5944;                       // +0x5944..+0x5A1B
    unsigned char pad5A1C[0x5BD0 - 0x5A1C];
    TaSub10 sub5BD0;                            // +0x5BD0..+0x5BEF? wait: size is 0x5BEC -> 0x1C!
};

struct TaSub1C
{
    void *vtbl;
    int f04, f08, f0C, f10;
    unsigned char pad14[0x1C - 0x14];
    TaSub1C() : vtbl(&TaSub10::g_TaSub10Vtbl), f04(0), f08(0), f0C(0), f10(0) {}
};
typedef char TaSub1CSizeCheck[(sizeof(TaSub1C) == 0x1C) ? 1 : -1];

struct TaTitleMgrExact
{
    unsigned int flags04;                       // +0x04
    unsigned char pad08[0x1C - 0x08];
    int state1C;                                // +0x1C
    int prevState20;                            // +0x20
    int subState24;                             // +0x24
    TaMenuBlock menu28;                         // +0x28..+0xFF
    TaMenuBlock menu100;                        // +0x100..+0x1D7
    TaMenuBlock menu1D8;                        // +0x1D8..+0x2AF
    unsigned char pad2B0[0x2B4 - 0x2B0];
    TaTimer timer2B4;                           // +0x2B4..+0x2C7
    unsigned char pad2C8[0x5944 - 0x2C8];
    TaMenuBlock menu5944;                       // +0x5944..+0x5A1B
    unsigned char pad5A1C[0x5BD0 - 0x5A1C];
    TaSub1C sub5BD0;                            // +0x5BD0..+0x5BEB

    virtual void DummyVtbl();
    TaTitleMgrExact();
    void SetStateAndResetTimer(int newState);
    void SetSubStateAndResetTimer(int newSub);
    static void DestroyGlobal();
    TaTitleMgrExact *DestroySelf();
};
typedef char TaTitleMgrExactSizeCheck[(sizeof(TaTitleMgrExact) == 0x5BEC) ? 1 : -1];

struct TaMother
{
    unsigned char pad000[0x528];
    int field528;
    static int Get528();
};

struct TaScoreMgr
{
    unsigned char pad0000[0x660];
    int table660[8][0x19B];                     // +0x660
    unsigned char pad3370[0x337E - 0x660 - 8 * 0x19B * 4 + 0x337E];
};

struct TaScoreMgrExact
{
    unsigned char pad0000[0x660];
    int table660[1];                            // +0x660 (stride 0x19B ints)
    unsigned char pad0664[0x337E - 0x664];
    signed char clearFlags337E[16];             // +0x337E

    int IsClearFlag(int idx);
    static int AreAll6Cleared();
    static int __stdcall GetTable660(int row, int col);
    static void SetAllClearFlags();
};

struct TaHelpMgr
{
    unsigned char pad000[0x12C];
    float field12C;
    static void __stdcall SetFloat12C(float v);
};

struct TaObj04
{
    int f00;
    int f04;
    int GetField04();
    int IsPos0();
    int GetPlus7();
};

struct TaObjD4
{
    unsigned char pad00[0xD4];
    int fieldD4;
    void ZeroD4();
};

struct TaAnmMgr
{
    unsigned char pad000000[0x4E90C0];
    int slots4E90C0[16];
    int GetSlot4E90C0(int idx);
};

struct TaArchive
{
    int f00, f04, f08, f0C;
    TaArchive();
};

struct TaEntryItem
{
    const char *name00;
    int f04;
    int size08;
    int f0C;
};
typedef char TaEntryItemSizeCheck[(sizeof(TaEntryItem) == 0x10) ? 1 : -1];

struct TaEntryTable
{
    TaEntryItem *items00;
    int count04;

    int FindSize(const char *name);
    TaEntryItem *FindEntry(const char *name);
};

struct TaStreamBase
{
    void *vtbl;
    int f04;
    int f08;
    TaStreamBase();
};

struct TaVtbl850A { void *vtbl; void SetVtblA(); };
struct TaVtbl850B { void *vtbl; void SetVtblB(); };

extern TaStaffMgr *g_TaStaffMgr;                // 0x004B8A94
extern void *g_TaBuf8CVtbl;                     // 0x004A26DC
extern void *g_TaTitleVtbl;                     // 0x004A46CC
extern TaTitleMgrExact *g_TaTitleMgr;           // 0x004B8A9C
extern float g_TaDefaultTimeScale;              // 0x004B6F58
extern TaMother *g_TaMother;                    // 0x004B8A64
extern TaScoreMgrExact *g_TaScoreMgr;           // 0x004B8A88
extern TaHelpMgr *g_TaHelpMgr;                  // 0x004B8A58
extern unsigned int g_Ta4B4D50;                 // 0x004B4D50
extern void *g_TaStreamBaseVtbl;                // 0x004A4828
extern void *g_TaVtbl850;                       // 0x004A4850

extern int __stdcall TaStaffInit(TaStaffMgr *m);        // 0x00444240
extern void __stdcall TaStaffDtor(TaStaffMgr *m);       // 0x00444300
extern void __stdcall TaTitleDtor(TaTitleMgrExact *m);  // 0x00445450
extern unsigned long (__stdcall *g_TaGetModuleFileNameA)(void *, char *, unsigned long); // 0x0049A0B4

void TaNoOp444130();
void TaNoOp444CB0();
int TaRouteStageToIndex(int route, int stage);
int TaGetTitleSize();
int TaRet1_44B3B0();
int TaRet1_44B3C0();
int TaRet1_44B3D0();
const char *TaSkipLine(const char *p, int *rem);
char *__stdcall TaReadLine(char *p, int *rem, char *out);
int TaRouteStageToIndexSwitch(int route, int stage);
void TaClearFlag4B4D50Bit4();
void TaNoOp44F590();
char *TaStrDupMalloc(const char *s);
int TaStreamAdvance4(int **pp);
char *TaStreamSkipAlignedStr(char **pp);
void TaNoOp44FD40();
void TaNoOp44FD50();
void TaResolvePath(char *dst, const char *src);
const char *TaStripDirSlash(const char *s);
