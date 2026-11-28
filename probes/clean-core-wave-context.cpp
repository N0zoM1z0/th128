#include "../src/CleanCoreWave.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

unsigned int g_CcwSupFlags = 0;
CcwJoyKeyCfg g_CcwJoyKeyCfg;
CcwJoyCaps g_CcwJoyCaps;
CcwIDirectInputDevice8 *g_CcwDiJoy = NULL;
short g_CcwPadDeadX = 0;
short g_CcwPadDeadY = 0;

int g_CcwSoundEnabled = 0;
CcwIDirectSound8 *g_CcwDSound = NULL;
CcwSoundBuffer g_CcwSfxEntries[64];
void *g_CcwWaveBufs[128];
int g_CcwLoadThreadState = 0;
CcwErrorCtx g_CcwErrorCtx;

int CcwWaveFile::g_CcwBgmBaseOffset = 0;

unsigned char g_CcwStageFlags = 0;
char g_CcwAppDataDir[260];
char g_CcwOrigDir[260];

static volatile int g_Sink = 0;

__declspec(noinline) void __cdecl CcwDebugLog(const char *fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    g_Sink += buf[0];
}

__declspec(noinline) void __cdecl CcwSoundDebugLog(const char *fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    g_Sink += buf[0] + 1;
}

static __declspec(noinline) void CcwLockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx + (int)t;
}

static __declspec(noinline) void CcwUnlockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx * 3 + (int)t;
}

__declspec(noinline) const char *__cdecl CcwErrorCtx::Log(const char *fmt, ...) {
    char local[0x2000];
    va_list args;
    CcwLockCs(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    CcwUnlockCs(3);
    return fmt;
}

__declspec(noinline) HRESULT CcwWaveFile::Read(unsigned char *dst, DWORD size, DWORD *outRead) {
    if (isMemory7c != 0) {
        DWORD n = size;
        if ((int)n > memRemaining88) n = (DWORD)memRemaining88;
        memcpy(dst, memCur84, n);
        memCur84 += n;
        memRemaining88 -= (int)n;
        *outRead = n;
        return 0;
    }
    DWORD got = 0;
    ReadFile(fileHandle8c, dst, size, &got, NULL);
    remaining8 -= (int)got;
    *outRead = got;
    return 0;
}

typedef int (__stdcall *FileExistsFn)(const char *);
volatile FileExistsFn g_FileExistsPtr = &CcwFileExists;

__declspec(noinline) int __stdcall CcwFileExists(const char *path) {
    DWORD a = GetFileAttributesA(path);
    return a != INVALID_FILE_ATTRIBUTES;
}

__declspec(noinline) int CcwOpenReadStream(const char *path) {
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    g_Sink += (int)h + path[0];
    return h == INVALID_HANDLE_VALUE ? -1 : 0;
}

static HANDLE g_CcwReadHandle = INVALID_HANDLE_VALUE;
__declspec(noinline) void *CcwReadStream(unsigned int size) {
    HANDLE h = g_CcwReadHandle;
    if (h == INVALID_HANDLE_VALUE) return NULL;
    void *buf = malloc(size);
    DWORD got = 0;
    ReadFile(h, buf, size, &got, NULL);
    if (got != size) {
        free(buf);
        CloseHandle(h);
        g_CcwReadHandle = INVALID_HANDLE_VALUE;
        return NULL;
    }
    return buf;
}

__declspec(noinline) void CcwCloseStream() {
    CloseHandle((HANDLE)g_Sink);
}

__declspec(noinline) void *__stdcall CcwLoadPackedFile(const char *path, unsigned int *outSize, int isExternal) {
    DWORD t = GetTickCount();
    *outSize = t;
    g_Sink += (int)path + isExternal + (int)t;
    return (void *)t;
}

__declspec(noinline) unsigned char *__stdcall CcwDecryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int block, int limit) {
    unsigned char k = key;
    if (k == 0) k = 1;
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    int tail = size % block < block / 4 ? size % block : 0;
    int length = size;
    if (limit <= size) length = limit;
    unsigned char *temp = (unsigned char *)malloc(length);
    if (temp != NULL) {
        size -= (size & 1) + tail;
        memcpy(temp, data, length);
        for (int i = 0; i < length; ++i) {
            data[i] = temp[i] ^ k ^ (unsigned char)(t1 ^ t2 ^ t3);
            k += step;
        }
        free(temp);
    }
    return data;
}

__declspec(noinline) void __stdcall CcwDecompressLzss(const unsigned char *src, unsigned int srcSize, void *dst, unsigned int dstSize) {
    DWORD t = GetTickCount();
    g_Sink += (int)src + (int)srcSize + (int)dst + (int)dstSize + (int)t;
}

typedef unsigned int (__fastcall *MergeJoyFn)(unsigned int);
volatile MergeJoyFn g_MergeJoyPtr = &CcwMergeJoyInput;

} // namespace th128

extern "C" int __stdcall CleanCoreWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return (int)g_MergeJoyPtr((unsigned int)arg);
    if (selector == 2) return ((CcwSoundBuffer *)arg)->LoadWave((const char *)(arg + 4));
    if (selector == 3) return (int)((CcwDSound *)arg)->FillBufferWithSound((CcwIDirectSoundBuffer *)(arg + 4), arg & 1);
    if (selector == 4) return ((CcwReplayMgr *)arg)->LoadReplay((const char *)(arg + 4));
    if (selector == 5) {
        unsigned int sz = 0;
        CcwLoadPackedFile((const char *)arg, &sz, selector);
        return (int)sz + g_Sink;
    }
    return 0;
}
