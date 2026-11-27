#pragma once

#include <stddef.h>

namespace th128 {

struct LdStartupInfoA {
    unsigned long cb;
    char *lpReserved;
    char *lpDesktop;
    char *lpTitle;
    unsigned long dwX;
    unsigned long dwY;
    unsigned long dwXSize;
    unsigned long dwYSize;
    unsigned long dwXCountChars;
    unsigned long dwYCountChars;
    unsigned long dwFillAttribute;
    unsigned long dwFlags;
    unsigned short wShowWindow;
    unsigned short cbReserved2;
    unsigned char *lpReserved2;
    void *hStdInput;
    void *hStdOutput;
    void *hStdError;
};

struct LdProcessInfo {
    void *hProcess;
    void *hThread;
    unsigned long dwProcessId;
    unsigned long dwThreadId;
};

struct LdSurface {
    unsigned int format;
    unsigned int width;
    unsigned int height;
    unsigned int pad0c;
    int pitch;
    unsigned int pad14[3];
    unsigned char *pixels;
};

extern unsigned char g_LdLzssWindow[0x2000];
extern LdSurface g_LdSurf;

unsigned char *__stdcall LdLzssDecompress(const unsigned char *src, int srcLen, unsigned char *dst, unsigned int dstSize);
int __stdcall LdCreateProcessAndWait(const char *appName, char *cmdLine, int wait);
bool __stdcall LdBleedTransparentPixels(unsigned int height);

} // namespace th128
