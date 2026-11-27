#include "LzssStreamHelpers.hpp"

#include <stdlib.h>
#include <string.h>

int LsObj08::Get08() { return f08; }

void LsLzssResetVars()
{
    g_Ls4D0AB0 = 1;
    g_Ls4B8AB4 = 0x2000;
    g_Ls4B8ABC = 0;
    g_Ls4B8AB8 = 0;
}

void LsLzssInitTree()
{
    memset(g_LsRingBuf, 0, sizeof(g_LsRingBuf));
    for (int i = 0; i < 0x2001; ++i)
    {
        g_LsTree[i].parent = 0;
        g_LsTree[i].left = 0;
        g_LsTree[i].right = 0;
    }
}

void LsLzssContractNode(int oldNode, int newNode)
{
    g_LsTree[newNode].parent = g_LsTree[oldNode].parent;
    if (g_LsTree[g_LsTree[oldNode].parent].right == oldNode)
    {
        g_LsTree[g_LsTree[oldNode].parent].right = newNode;
    }
    else
    {
        g_LsTree[g_LsTree[oldNode].parent].left = newNode;
    }
    g_LsTree[oldNode].parent = 0;
}

void LsLzssReplaceNode(int oldNode, int newNode)
{
    int p = g_LsTree[oldNode].parent;
    if (g_LsTree[p].left == oldNode)
    {
        g_LsTree[p].left = newNode;
    }
    else
    {
        g_LsTree[p].right = newNode;
    }
    g_LsTree[newNode].parent = g_LsTree[oldNode].parent;
    g_LsTree[newNode].left = g_LsTree[oldNode].left;
    g_LsTree[newNode].right = g_LsTree[oldNode].right;
    g_LsTree[g_LsTree[newNode].left].parent = newNode;
    g_LsTree[g_LsTree[newNode].right].parent = newNode;
    g_LsTree[oldNode].parent = 0;
}

int LsLzssFindNextNode(int node)
{
    int next = g_LsTree[node].left;
    while (g_LsTree[next].right != 0)
    {
        next = g_LsTree[next].right;
    }
    return next;
}

LsMemStream::LsMemStream()
{
    vtbl = &g_LsMemStreamVtbl;
    size04 = 0;
    cur08 = NULL;
    buf0C = NULL;
}

bool __stdcall LsMemStream::Seek(int offset, int origin)
{
    switch (origin)
    {
    case 0:
        if (offset < 0 || (unsigned int)offset >= size04)
        {
            return false;
        }
        cur08 = buf0C + offset;
        return true;
    case 1:
        if ((int)((buf0C + size04) - cur08) <= offset)
        {
            return false;
        }
        cur08 += offset;
        return true;
    case 2:
        if (offset > 0 || (unsigned int)(offset < 0 ? -offset : offset) >= size04)
        {
            return false;
        }
        cur08 = buf0C + size04 + offset;
        return true;
    default:
        return false;
    }
}

int LsAbs0::Abs() { return v < 0 ? -v : v; }

LsResStream::LsResStream()
{
    size04 = 0;
    cur08 = NULL;
    buf0C = NULL;
    vtbl = &g_LsResStreamVtbl;
}

LsMemStream *LsMemStream::ScalarDelete(unsigned int flags)
{
    vtbl = &g_LsMemStreamVtbl;
    if (buf0C != NULL)
    {
        free(buf0C);
        buf0C = NULL;
    }
    buf0C = NULL;
    size04 = 0;
    cur08 = NULL;
    vtbl = &g_LsBaseStreamVtbl;
    if ((flags & 1U) != 0)
    {
        operator delete(this);
    }
    return this;
}

void LsMemStream::CloseDtor()
{
    vtbl = &g_LsMemStreamVtbl;
    if (buf0C != NULL)
    {
        free(buf0C);
        buf0C = NULL;
    }
    buf0C = NULL;
    size04 = 0;
    cur08 = NULL;
    vtbl = &g_LsBaseStreamVtbl;
}

void *LsInit4B4D80()
{
    g_Ls4B4D80[1] = 0;
    g_Ls4B4D80[2] = 0;
    g_Ls4B4D80[3] = 0;
    g_Ls4B4D80[4] = 0;
    g_Ls4B4D80[0] = (int)&g_LsVtbl4A49A0;
    return g_Ls4B4D80;
}

void LsNoOp451510() {}

LsGdiFont::~LsGdiFont()
{
    if (hFont00 != NULL)
    {
        g_LsDeleteObject(hFont00);
        hFont00 = NULL;
    }
    if (cap20 >= 16)
    {
        operator delete(str0C.ptr);
    }
    cap20 = 15;
    len1C = 0;
    str0C.buf[0] = '\0';
}

void LsGdiFont::CreateFontObj()
{
    if (hFont00 != NULL)
    {
        g_LsDeleteObject(hFont00);
        hFont00 = NULL;
    }
    const char *face = cap20 >= 16 ? str0C.ptr : str0C.buf;
    hFont00 = g_LsCreateFontA(height08, width04, 0, 0, weight28, 0, 0, 0, 1, 0, 0, 0, 0x30, face);
}

void LsGdiFont::ReleaseFont()
{
    if (hFont00 != NULL)
    {
        g_LsDeleteObject(hFont00);
        hFont00 = NULL;
    }
}

LsSurfaceGlobal *LsInitSurface()
{
    g_LsSurface.f104 = 0;
    g_LsSurface.f108 = 0;
    g_LsSurface.f114 = 0;
    g_LsSurface.f11C = 0;
    g_LsSurface.f118 = 0;
    g_LsSurface.pixels120 = NULL;
    g_LsSurface.fmt100 = -1;
    return &g_LsSurface;
}

void LsFillSurfaceRgb444(unsigned int color)
{
    unsigned short c = (unsigned short)((((color >> 20) & 0xF) << 8) | (((color >> 12) & 0xF) << 4) | ((color >> 4) & 0xF));
    unsigned short *p = (unsigned short *)g_LsSurface.pixels120;
    for (int i = 0; i < g_LsSurface.byteSize10C; i += 2, ++p)
    {
        *p = c;
    }
}

void LsAccumRgbaPixel(int *rgbSum, const unsigned char *px, int *count)
{
    if (px[3] != 0)
    {
        rgbSum[0] += px[2];
        rgbSum[1] += px[1];
        rgbSum[2] += px[0];
        ++(*count);
    }
}

bool __stdcall LsInvertSurfaceAlpha(int height)
{
    int total = g_LsSurface.pitch110 * height;
    unsigned char *p = g_LsSurface.pixels120;
    switch (g_LsSurface.fmt100)
    {
    case 21:
        for (int i = 3; i < total; i += 4)
        {
            p[i] = (unsigned char)~p[i];
        }
        return true;
    case 25:
        for (int i = 0; i < total; i += 2, p += 2)
        {
            unsigned short v = *(unsigned short *)p;
            unsigned short nv = (unsigned short)((v & 0x7FFFU) | (~v & 0x8000U));
            *(unsigned short *)p = nv;
            if ((nv & 0x8000U) == 0)
            {
                *(unsigned short *)p = (unsigned short)(nv & 0x8000U);
            }
        }
        return true;
    case 26:
        for (int i = 1; i < total; i += 2)
        {
            p[i] ^= 0xF0U;
        }
        return true;
    default:
        return false;
    }
}

bool LsStdString::Inside(const char *p)
{
    if (p == NULL)
    {
        return false;
    }
    const char *base = cap14 >= 16 ? u.ptr : u.buf;
    if (p < base)
    {
        return false;
    }
    const char *base2 = cap14 >= 16 ? u.ptr : u.buf;
    return base2 + len10 > p;
}

unsigned int LsStdString::GetLen() { return len10; }
int LsStrLenInline(const char *s) { return (int)strlen(s); }
void LsZeroSurfacePixels() { memset(g_LsSurface.pixels120, 0, g_LsSurface.byteSize10C); }
int LsSurfaceGlobal::Get100() { return fmt100; }
int LsSurfaceGlobal::IsValid11C() { return f11C != 0; }
int LsSurfaceGlobal::Get114() { return f114; }
int LsSurfaceGlobal::Get104() { return f104; }
int LsSurfaceGlobal::Get00() { return *(int *)this; }
void LsPair08::Set8Zero4(int v) { f04 = 0; f08 = v; }
int LsSurfaceGlobal::Get108() { return f108; }
int LsSurfaceGlobal::Get110() { return pitch110; }
unsigned char *LsSurfaceGlobal::Get120() { return pixels120; }
char *LsStdString::DataA() { return cap14 >= 16 ? u.ptr : u.buf; }
char *LsStdString::DataB() { return cap14 >= 16 ? u.ptr : u.buf; }
char *LsStdString::DataC() { return cap14 >= 16 ? u.ptr : u.buf; }

void LsStdString::TidyFree()
{
    if (cap14 >= 16)
    {
        operator delete(u.ptr);
    }
    cap14 = 15;
    len10 = 0;
    u.buf[0] = '\0';
}

void LsNoOp453420() {}
void __stdcall LsNoOpS1_453430(int) {}
void LsNoOp453440() {}
void LsNoOp453450() {}
void LsMemCpyWrap(void *dst, const void *src, size_t n) { memcpy(dst, src, n); }
void LsOpDeleteWrap(void *p) { operator delete(p); }
void LsCopyByte(char *dst, const char *src) { *dst = *src; }

void LsStdString::SetEos(unsigned int n)
{
    len10 = n;
    (cap14 >= 16 ? u.ptr : u.buf)[n] = '\0';
}

void LsStdString::Tidy(bool built, unsigned int newSize)
{
    if (built && cap14 >= 16)
    {
        char *ptr = u.ptr;
        if (newSize != 0)
        {
            memcpy(u.buf, ptr, newSize);
        }
        operator delete(ptr);
    }
    cap14 = 15;
    len10 = newSize;
    u.buf[newSize] = '\0';
}

void LsStdString::ResetEmpty()
{
    cap14 = 15;
    len10 = 0;
    u.buf[0] = '\0';
}

void LsNoOp453510() {}
void LsZero0Twice::ZeroTwice() { v = 0; v = 0; }
void LsNoOp4535B0() {}

void LsResetLogBuf()
{
    g_LsLogPtr = g_LsLogBuf;
    g_LsLogBuf[0] = '\0';
}

void LsPair08::SetBothAndZero8(int v)
{
    f00 = v;
    f04 = v;
    f08 = 0;
}

LsBuf20AC *LsResetBuf20AC()
{
    memset(&g_LsBuf20AC, 0, sizeof(g_LsBuf20AC));
    return &g_LsBuf20AC;
}

int *LsBuf20AC::GetCurSlot() { return &arr207C[idx2078 * 3]; }
void LsWriteDword(int *dst, int val) { *dst = val; }
void LsSet4D2E6C(void *p) { g_Ls4D2E6C = p; }
int LsComPair::Get04() { return (int)obj04; }
LsComObj **LsComPair::GetAddr08() { return &obj08; }

void LsComPair::Release04()
{
    if (obj04 != NULL)
    {
        obj04->vtbl->release(obj04);
        obj04 = NULL;
    }
}

void LsComPair::Release08()
{
    if (obj08 != NULL)
    {
        obj08->vtbl->release(obj08);
        obj08 = NULL;
    }
}

void *LsDelete464240(void *p)
{
    Ls464240(p);
    operator delete(p);
    return p;
}

void *LsDeleteRaw(void *p)
{
    operator delete(p);
    return p;
}

void LsRestoreAccessibilityKeys()
{
    g_LsSystemParametersInfoA(0x11, 0, (void *)g_Ls4D599C, 2);
    g_LsSystemParametersInfoA(0x55, 0, (void *)g_Ls4D59A0, 2);
    g_LsSystemParametersInfoA(0x56, 0, (void *)g_Ls4D59A4, 2);
    g_LsImmDisableIME(1);
}

void LsDisableImeDll()
{
    void *mod = g_LsLoadLibraryA("imm32.dll");
    if (mod != NULL)
    {
        void (__stdcall *fn)(int) = (void (__stdcall *)(int))g_LsGetProcAddress(mod, "ImmDisableIME");
        if (fn != NULL)
        {
            fn(0);
        }
    }
}

int LsShowCursorWrap(int show) { return g_LsShowCursor(show); }
int LsObj0C::Get0C() { return f0C; }
void LsObj0C::Zero0C() { f0C = 0; }
int *LsObj0C::GetAddr0C() { return &f0C; }

LsFade44::LsFade44()
{
    timerFl40 &= ~1U;
    memset(this, 0, sizeof(LsFade44));
    flags00 |= 2U;
}

void LsClearAndPresentTwice()
{
    g_LsD3dDev->vtbl->clear(g_LsD3dDev, 0, NULL, 3, 0xFF000000U, 1.0f, 0);
    if (g_LsD3dDev->vtbl->present(g_LsD3dDev, NULL, NULL, NULL, NULL) < 0)
    {
        g_LsD3dDev->vtbl->reset(g_LsD3dDev, &g_LsPresentParams);
    }
    g_LsD3dDev->vtbl->clear(g_LsD3dDev, 0, NULL, 3, 0xFF000000U, 1.0f, 0);
    if (g_LsD3dDev->vtbl->present(g_LsD3dDev, NULL, NULL, NULL, NULL) < 0)
    {
        g_LsD3dDev->vtbl->reset(g_LsD3dDev, &g_LsPresentParams);
    }
}
