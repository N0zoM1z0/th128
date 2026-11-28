#include "CleanBatchF.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

volatile int g_CbfSink = 0;

__declspec(noinline) const char *__cdecl CbfLog::Print(const char *msg, ...) {
    char local[0x2000];
    va_list args;
    va_start(args, msg);
    vsprintf(local, msg, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    return msg;
}

__declspec(noinline) void __cdecl CbfDebugLog(...) {
}

__declspec(noinline) CbfPreloadAnmObj *__stdcall CbfPreloadAnm(CbfAnmMgr *mgr, int slot, const char *name) {
    CbfAnmMgr *volatile *pm = &mgr;
    int volatile *ps = &slot;
    g_CbfSink += (1 << ((int)name & 7)) + (int)*pm + *ps;
    DWORD t = GetTickCount();
    return (CbfPreloadAnmObj *)(g_CbfSink + (int)t);
}

__declspec(noinline) void *__stdcall CbfOpenFile(const char *path, int *outSize, int mode) {
    const char *volatile *pp = &path;
    int *volatile *po = &outSize;
    int volatile *pm = &mode;
    DWORD t = GetTickCount();
    g_CbfSink += (int)*pp + (int)*po + *pm + (int)t;
    return (void *)g_CbfSink;
}

__declspec(noinline) void *CbfLoadAnm(int slot, const char *name) {
    g_CbfSink += (1 << slot);
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_CbfSink += (int)t1 * 3 + (int)t2 * 5 + (int)t3 + (int)name[0] + (int)name[1];
    g_CbfSink += (int)t1 * 7 + (int)t2 * 11 + (int)name[2];
    return (void *)((int)name + (int)t1 + (int)t2 + (int)t3);
}

__declspec(noinline) void __stdcall CbfSetText(void *mgr, const char *str, int arg, void *out) {
    void *volatile *po = &out;
    g_CbfSink += arg * 3 + (1 << ((int)str & 7)) + ((int)mgr * 5) + (int)*po;
}

__declspec(noinline) void CbfStageLoader::LoadEcl(const char *name) {
    g_CbfSink += (int)this + (int)name[0];
}

typedef void (__stdcall *CbfFn1)(CbfCleanupObj *);
typedef void (__stdcall *CbfFn2)(int *, int *);
typedef int (CbfStageLoader::*CbfFn3)(CbfChunkHeader *);

volatile CbfFn1 g_CbfP1 = &CbfCleanup410ea0;
volatile CbfFn2 g_CbfP2 = &CbfDecodeLivesBombs;
volatile CbfFn3 g_CbfP3 = &CbfStageLoader::LoadResources;

extern "C" int __stdcall CleanBatchFProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) g_CbfP1((CbfCleanupObj *)args[0]);
    if (mode & 2) g_CbfP2((int *)args[1], (int *)args[2]);
    if (mode & 4) {
        CbfStageLoader *ldr = (CbfStageLoader *)args[3];
        r += (ldr->*g_CbfP3)((CbfChunkHeader *)args[4]);
    }
    if (mode & 8) r += CbfLoadStd((CbfStdObj *)args[5], (const char *)args[6]);
    if (mode & 16) r += (int)CbfFormatCommaNumber(args[7], mode, mode + 1);
    if (mode & 32) CbfOpenFile((const char *)args[6], (int *)args[1], mode);
    if (mode & 64) ((CbfLog *)args[8])->Print((const char *)args[6]);
    if (mode & 128) CbfPreloadAnm((CbfAnmMgr *)args[0], mode, (const char *)args[6]);
    if (mode & 256) CbfSetText(args[0], (const char *)args[6], mode, args[7]);
    return r + g_CbfSink;
}
