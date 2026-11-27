#pragma once

#include <stddef.h>

struct LsLzssNode
{
    int parent;                                 // +0x00
    int left;                                   // +0x04
    int right;                                  // +0x08
};
typedef char LsLzssNodeSizeCheck[(sizeof(LsLzssNode) == 0x0C) ? 1 : -1];

struct LsMemStream
{
    void *vtbl;                                 // +0x00
    unsigned int size04;                        // +0x04
    unsigned char *cur08;                       // +0x08
    unsigned char *buf0C;                       // +0x0C

    LsMemStream();
    bool __stdcall Seek(int offset, int origin);
    void CloseDtor();
    LsMemStream *ScalarDelete(unsigned int flags);
};

struct LsResStream
{
    void *vtbl;
    unsigned int size04;
    unsigned char *cur08;
    unsigned char *buf0C;

    LsResStream();
};

struct LsObj08 { int f00, f04, f08; int Get08(); };
struct LsAbs0 { int v; int Abs(); };

struct LsGdiFont
{
    void *hFont00;                              // +0x00
    int width04;                                // +0x04
    int height08;                               // +0x08
    union {
        char buf[16];
        char *ptr;
    } str0C;                                    // +0x0C..+0x1B
    unsigned int len1C;                         // +0x1C
    unsigned int cap20;                         // +0x20
    int pad24;                                  // +0x24
    int weight28;                               // +0x28

    ~LsGdiFont();
    void CreateFontObj();
    void ReleaseFont();
};

struct LsSurfaceGlobal
{
    unsigned char pad000[0x100];
    int fmt100;                                 // +0x100 (0x4B4ED0)
    int f104;                                   // +0x104 (0x4B4ED4)
    int f108;                                   // +0x108 (0x4B4ED8)
    int byteSize10C;                            // +0x10C (0x4B4EDC)
    int pitch110;                               // +0x110 (0x4B4EE0)
    int f114;                                   // +0x114 (0x4B4EE4)
    int f118;                                   // +0x118 (0x4B4EE8)
    int f11C;                                   // +0x11C (0x4B4EEC)
    unsigned char *pixels120;                   // +0x120 (0x4B4EF0)

    int Get100();
    int IsValid11C();
    int Get114();
    int Get104();
    int Get00();
    int Get108();
    int Get110();
    unsigned char *Get120();
};

struct LsStdString
{
    union {
        char buf[16];
        char *ptr;
    } u;                                        // +0x00..+0x0F
    unsigned int len10;                         // +0x10
    unsigned int cap14;                         // +0x14

    bool Inside(const char *p);
    unsigned int GetLen();
    char *DataA();
    char *DataB();
    char *DataC();
    void TidyFree();
    void SetEos(unsigned int n);
    void Tidy(bool built, unsigned int newSize);
    void ResetEmpty();
};

struct LsPair08
{
    int f00;
    int f04;
    int f08;
    void Set8Zero4(int v);
    void SetBothAndZero8(int v);
};

struct LsZero0Twice
{
    volatile int v;
    void ZeroTwice();
};

struct LsComObjVtbl
{
    void *pad00[2];
    unsigned long (__stdcall *release)(void *self);
};
struct LsComObj { LsComObjVtbl *vtbl; };

struct LsComPair
{
    int f00;
    LsComObj *obj04;
    LsComObj *obj08;
    int Get04();
    LsComObj **GetAddr08();
    void Release04();
    void Release08();
};

struct LsObj0C
{
    int f00, f04, f08;
    int f0C;
    int Get0C();
    void Zero0C();
    int *GetAddr0C();
};

struct LsFade44
{
    unsigned int flags00;
    unsigned char pad04[0x40 - 0x04];
    unsigned int timerFl40;
    LsFade44();
};
typedef char LsFade44SizeCheck[(sizeof(LsFade44) == 0x44) ? 1 : -1];

struct LsD3dDevVtbl
{
    void *pad00[16];
    int (__stdcall *reset)(void *self, void *pp);                                               // +0x40
    int (__stdcall *present)(void *self, void *a, void *b, void *c, void *d);                   // +0x44
    void *pad48[25];
    int (__stdcall *clear)(void *self, unsigned int cnt, void *rects, unsigned int flags, unsigned int color, float z, unsigned int stencil); // +0xAC
};
struct LsD3dDev { LsD3dDevVtbl *vtbl; };

struct LsBuf20AC
{
    unsigned char pad0000[0x2078];
    int idx2078;                                // +0x2078
    int arr207C[12];                            // +0x207C (stride 12 bytes = 3 ints)

    int *GetCurSlot();
};
typedef char LsBuf20ACSizeCheck[(sizeof(LsBuf20AC) == 0x20AC) ? 1 : -1];

extern int g_Ls4D0AB0;                          // 0x004D0AB0
extern int g_Ls4B8AB4;                          // 0x004B8AB4
extern int g_Ls4B8ABC;                          // 0x004B8ABC
extern int g_Ls4B8AB8;                          // 0x004B8AB8
extern unsigned char g_LsRingBuf[0x2000];       // 0x004D0AB8
extern LsLzssNode g_LsTree[0x2001];             // 0x004B8AA8
extern void *g_LsMemStreamVtbl;                 // 0x004A491C
extern void *g_LsResStreamVtbl;                 // 0x004A4964
extern void *g_LsBaseStreamVtbl;                // 0x004A4850
extern void *g_LsVtbl4A49A0;                    // 0x004A49A0
extern int g_Ls4B4D80[5];                       // 0x004B4D80
extern LsSurfaceGlobal g_LsSurface;             // 0x004B4DD0
extern char g_LsLogBuf[0x2000];                 // 0x004B4F50
extern char *g_LsLogPtr;                        // 0x004B6F50
extern LsBuf20AC g_LsBuf20AC;                   // 0x004D3970
extern void *g_Ls4D2E6C;                        // 0x004D2E6C
extern LsD3dDev *g_LsD3dDev;                    // 0x004D2E70
extern int g_LsPresentParams;                   // 0x004D2F5C
extern int g_Ls4D599C, g_Ls4D59A0, g_Ls4D59A4;  // 0x004D599C..0x004D59A4

extern int (__stdcall *g_LsDeleteObject)(void *);                               // 0x0049A030
extern void *(__stdcall *g_LsCreateFontA)(int, int, int, int, int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, const char *); // 0x0049A034
extern int (__stdcall *g_LsSystemParametersInfoA)(unsigned int, unsigned int, void *, unsigned int); // 0x0049A268
extern int (__stdcall *g_LsImmDisableIME)(unsigned int);                        // 0x0049A278
extern void *(__stdcall *g_LsLoadLibraryA)(const char *);                       // 0x0049A0F0
extern void *(__stdcall *g_LsGetProcAddress)(void *, const char *);             // 0x0049A0EC
extern int (__stdcall *g_LsShowCursor)(int);                                    // 0x0049A228
extern void __stdcall Ls464240(void *p);                                        // 0x00464240

void LsLzssResetVars();
void LsLzssInitTree();
void LsLzssContractNode(int oldNode, int newNode);
void LsLzssReplaceNode(int oldNode, int newNode);
int LsLzssFindNextNode(int node);
void *LsInit4B4D80();
void LsNoOp451510();
LsSurfaceGlobal *LsInitSurface();
void LsFillSurfaceRgb444(unsigned int color);
void LsAccumRgbaPixel(int *rgbSum, const unsigned char *px, int *count);
bool __stdcall LsInvertSurfaceAlpha(int height);
int LsStrLenInline(const char *s);
void LsZeroSurfacePixels();
void LsNoOp453420();
void __stdcall LsNoOpS1_453430(int);
void LsNoOp453440();
void LsNoOp453450();
void LsMemCpyWrap(void *dst, const void *src, size_t n);
void LsOpDeleteWrap(void *p);
void LsCopyByte(char *dst, const char *src);
void LsNoOp453510();
void LsNoOp4535B0();
void LsResetLogBuf();
LsBuf20AC *LsResetBuf20AC();
void LsWriteDword(int *dst, int val);
void LsSet4D2E6C(void *p);
void *LsDelete464240(void *p);
void *LsDeleteRaw(void *p);
void LsRestoreAccessibilityKeys();
void LsDisableImeDll();
int LsShowCursorWrap(int show);
void LsClearAndPresentTwice();
