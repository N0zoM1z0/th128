#include "../src/LzssStreamHelpers.hpp"
#include <new>

int g_Ls4D0AB0;
int g_Ls4B8AB4;
int g_Ls4B8ABC;
int g_Ls4B8AB8;
unsigned char g_LsRingBuf[0x2000];
LsLzssNode g_LsTree[0x2001];
void *g_LsMemStreamVtbl;
void *g_LsResStreamVtbl;
void *g_LsBaseStreamVtbl;
void *g_LsVtbl4A49A0;
int g_Ls4B4D80[5];
LsSurfaceGlobal g_LsSurface;
char g_LsLogBuf[0x2000];
char *g_LsLogPtr;
LsBuf20AC g_LsBuf20AC;
void *g_Ls4D2E6C;
LsD3dDev *g_LsD3dDev;
int g_LsPresentParams;
int g_Ls4D599C, g_Ls4D59A0, g_Ls4D59A4;

int (__stdcall *g_LsDeleteObject)(void *);
void *(__stdcall *g_LsCreateFontA)(int, int, int, int, int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, const char *);
int (__stdcall *g_LsSystemParametersInfoA)(unsigned int, unsigned int, void *, unsigned int);
int (__stdcall *g_LsImmDisableIME)(unsigned int);
void *(__stdcall *g_LsLoadLibraryA)(const char *);
void *(__stdcall *g_LsGetProcAddress)(void *, const char *);
int (__stdcall *g_LsShowCursor)(int);

typedef bool (__stdcall LsMemStream::*SeekFn)(int, int);
typedef LsMemStream *(LsMemStream::*ScalarDelFn)(unsigned int);
typedef void (LsMemStream::*CloseDtorFn)();
typedef int (LsSurfaceGlobal::*IsValidFn)();
typedef void (__stdcall *NoOpS1Fn)(int);

extern "C" int __stdcall LzssStreamHelpersProbe(
    LsObj08 *o08, LsMemStream *ms, LsResStream *rs, LsAbs0 *a0, LsGdiFont *gf,
    LsSurfaceGlobal *sg, LsStdString *ss, LsPair08 *p08, LsZero0Twice *z0,
    LsComPair *cp, LsObj0C *o0c, LsFade44 *f44, LsBuf20AC *b20,
    int *ip, const unsigned char *px, char *cbuf, const char *cstr, void *vp,
    int a, int b)
{
    int acc = 0;
    acc += o08->Get08();
    LsLzssResetVars();
    LsLzssInitTree();
    LsLzssContractNode(a, b);
    LsLzssReplaceNode(a, b);
    acc += LsLzssFindNextNode(a);
    new (ms) LsMemStream();
    SeekFn pSeek = &LsMemStream::Seek;
    ScalarDelFn pDel = &LsMemStream::ScalarDelete;
    CloseDtorFn pClose = &LsMemStream::CloseDtor;
    acc += (ms->*pSeek)(a, b);
    acc += (int)(ms->*pDel)((unsigned int)a);
    (ms->*pClose)();
    acc += a0->Abs();
    new (rs) LsResStream();
    acc += (int)LsInit4B4D80();
    LsNoOp451510();
    gf->~LsGdiFont();
    gf->CreateFontObj();
    gf->ReleaseFont();
    acc += (int)LsInitSurface();
    LsFillSurfaceRgb444((unsigned int)a);
    LsAccumRgbaPixel(ip, px, ip);
    acc += LsInvertSurfaceAlpha(a);
    acc += ss->Inside(cstr) + (int)ss->GetLen() + LsStrLenInline(cstr);
    LsZeroSurfacePixels();
    IsValidFn pVal = &LsSurfaceGlobal::IsValid11C;
    acc += sg->Get100() + (sg->*pVal)() + sg->Get114() + sg->Get104() + sg->Get00()
        + sg->Get108() + sg->Get110() + (int)sg->Get120();
    p08->Set8Zero4(a);
    p08->SetBothAndZero8(b);
    acc += (int)ss->DataA() + (int)ss->DataB() + (int)ss->DataC();
    ss->TidyFree();
    LsNoOp453420();
    NoOpS1Fn pS1 = &LsNoOpS1_453430;
    pS1(a);
    LsNoOp453440(); LsNoOp453450();
    LsMemCpyWrap(cbuf, cstr, (size_t)a);
    LsOpDeleteWrap(vp);
    LsCopyByte(cbuf, cstr);
    ss->SetEos((unsigned int)a);
    ss->Tidy(a != 0, (unsigned int)b);
    ss->ResetEmpty();
    LsNoOp453510();
    z0->ZeroTwice();
    LsNoOp4535B0();
    LsResetLogBuf();
    acc += (int)LsResetBuf20AC() + (int)b20->GetCurSlot();
    LsWriteDword(ip, a);
    LsSet4D2E6C(vp);
    acc += cp->Get04() + (int)cp->GetAddr08();
    cp->Release04(); cp->Release08();
    acc += (int)LsDelete464240(vp) + (int)LsDeleteRaw(vp);
    LsRestoreAccessibilityKeys();
    LsDisableImeDll();
    acc += LsShowCursorWrap(a);
    acc += o0c->Get0C() + (int)o0c->GetAddr0C();
    o0c->Zero0C();
    new (f44) LsFade44();
    LsClearAndPresentTwice();
    return acc;
}
