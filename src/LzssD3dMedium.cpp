#include "LzssD3dMedium.hpp"

#include <stdlib.h>

extern "C" {
__declspec(dllimport) int __stdcall CreateProcessA(
    const char *lpApplicationName,
    char *lpCommandLine,
    void *lpProcessAttributes,
    void *lpThreadAttributes,
    int bInheritHandles,
    unsigned long dwCreationFlags,
    void *lpEnvironment,
    const char *lpCurrentDirectory,
    th128::LdStartupInfoA *lpStartupInfo,
    th128::LdProcessInfo *lpProcessInformation);
__declspec(dllimport) int __stdcall GetExitCodeProcess(void *hProcess, unsigned long *lpExitCode);
}

namespace th128 {

static inline unsigned int LdReadBits(const unsigned char *&src, const unsigned char *srcBase, int srcLen, unsigned int &bitBuf, unsigned char &bitMask, int numBits) {
    unsigned int val = 0;
    for (unsigned int mask = 1u << (numBits - 1); mask != 0; mask >>= 1) {
        if (bitMask == 0x80) {
            bitBuf = *src;
            if ((int)(src - srcBase) < srcLen) {
                src++;
            } else {
                bitBuf = 0;
            }
        }
        if (bitBuf & bitMask) {
            val |= mask;
        }
        bitMask >>= 1;
        if (bitMask == 0) {
            bitMask = 0x80;
        }
    }
    return val;
}

unsigned char *__stdcall LdLzssDecompress(const unsigned char *src, int srcLen, unsigned char *dst, unsigned int dstSize) {
    unsigned char bitMask = 0x80;
    unsigned int bitBuf = 0;
    if (dst == 0) {
        dst = (unsigned char *)malloc(dstSize);
        if (dst == 0) {
            return 0;
        }
    }
    unsigned char *out = dst;
    const unsigned char *in = src;
    int winPos = 1;
    for (;;) {
        if (bitMask == 0x80) {
            bitBuf = *in;
            if ((int)(in - src) < srcLen) {
                in++;
            } else {
                bitBuf = 0;
            }
        }
        unsigned int flag = bitBuf & bitMask;
        bitMask >>= 1;
        if (bitMask == 0) {
            bitMask = 0x80;
        }
        if (flag != 0) {
            unsigned int ch = LdReadBits(in, src, srcLen, bitBuf, bitMask, 8);
            *out++ = (unsigned char)ch;
            g_LdLzssWindow[winPos] = (unsigned char)ch;
            winPos = (winPos + 1) & 0x1fff;
        } else {
            int offset = (int)LdReadBits(in, src, srcLen, bitBuf, bitMask, 13);
            if (offset == 0) {
                break;
            }
            int len = (int)LdReadBits(in, src, srcLen, bitBuf, bitMask, 4) + 2;
            for (int i = 0; i <= len; ++i) {
                unsigned char c = g_LdLzssWindow[(offset + i) & 0x1fff];
                *out++ = c;
                g_LdLzssWindow[winPos] = c;
                winPos = (winPos + 1) & 0x1fff;
            }
        }
    }
    while (bitMask != 0x80) {
        bitMask >>= 1;
        if (bitMask == 0) {
            bitMask = 0x80;
        }
    }
    return dst;
}

int __stdcall LdCreateProcessAndWait(const char *appName, char *cmdLine, int wait) {
    LdStartupInfoA si;
    LdProcessInfo pi;
    si.cb = sizeof(si);
    si.lpReserved = 0;
    si.lpDesktop = 0;
    si.lpTitle = 0;
    si.dwX = 0x80000000u;
    si.dwY = 0x80000000u;
    si.dwXSize = 0x80000000u;
    si.dwYSize = 0x80000000u;
    si.dwXCountChars = 80;
    si.dwYCountChars = 25;
    si.dwFillAttribute = 0;
    si.dwFlags = 0;
    si.wShowWindow = 10;
    si.cbReserved2 = 0;
    si.lpReserved2 = 0;
    si.hStdInput = 0;
    si.hStdOutput = 0;
    si.hStdError = 0;

    if (!CreateProcessA(appName, cmdLine, 0, 0, 0, 0x20, 0, 0, &si, &pi)) {
        return -1;
    }
    if (wait != 0) {
        unsigned long code = 0x103;
        while (code == 0x103) {
            GetExitCodeProcess(pi.hProcess, &code);
        }
        return (int)code;
    }
    return 0;
}

bool __stdcall LdBleedTransparentPixels(unsigned int height) {
    switch (g_LdSurf.format) {
    case 0x1a: {
        unsigned short *p = (unsigned short *)g_LdSurf.pixels;
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < g_LdSurf.width; ++x) {
                if ((*p & 0xf000) == 0) {
                    unsigned int cnt = 0;
                    unsigned int r = 0;
                    unsigned int g = 0;
                    unsigned int b = 0;
                    if (x > 0 && (p[-1] & 0xf000) != 0) {
                        r += (p[-1] >> 8) & 0xf;
                        g += (p[-1] >> 4) & 0xf;
                        b += p[-1] & 0xf;
                        cnt++;
                    }
                    if (x < g_LdSurf.width - 1 && (p[1] & 0xf000) != 0) {
                        r += (p[1] >> 8) & 0xf;
                        g += (p[1] >> 4) & 0xf;
                        b += p[1] & 0xf;
                        cnt++;
                    }
                    if (y > 0) {
                        unsigned short v = p[-(g_LdSurf.pitch / 2)];
                        if ((v & 0xf000) != 0) {
                            r += (v >> 8) & 0xf;
                            g += (v >> 4) & 0xf;
                            b += v & 0xf;
                            cnt++;
                        }
                    }
                    if (y < g_LdSurf.height - 1) {
                        unsigned short v = p[g_LdSurf.pitch / 2];
                        if ((v & 0xf000) != 0) {
                            r += (v >> 8) & 0xf;
                            g += (v >> 4) & 0xf;
                            b += v & 0xf;
                            cnt++;
                        }
                    }
                    if (cnt > 1) {
                        r /= cnt;
                        g /= cnt;
                        b /= cnt;
                    }
                    *p = (unsigned short)((*p & 0xf00f) | (((r >> 1) & 0xf) << 8) | (((g >> 1) & 0xf) << 4));
                    *p = (unsigned short)((*p & 0xfff0) | ((b >> 1) & 0xf));
                }
                p++;
            }
        }
        break;
    }
    case 0x15: {
        unsigned char *p = g_LdSurf.pixels;
        for (unsigned int y = 0; y < height; ++y) {
            for (unsigned int x = 0; x < g_LdSurf.width; ++x) {
                if (p[3] == 0) {
                    unsigned int cnt = 0;
                    unsigned int r = 0;
                    unsigned int g = 0;
                    unsigned int b = 0;
                    if (x > 0 && p[-1] != 0) {
                        r += p[-2];
                        g += p[-3];
                        b += p[-4];
                        cnt++;
                    }
                    if (x < g_LdSurf.width - 1 && p[7] != 0) {
                        r += p[6];
                        g += p[5];
                        b += p[4];
                        cnt++;
                    }
                    if (y > 0) {
                        unsigned char *up = p - (g_LdSurf.pitch / 4) * 4;
                        if (up[3] != 0) {
                            r += up[2];
                            g += up[1];
                            b += up[0];
                            cnt++;
                        }
                    }
                    if (y < g_LdSurf.height - 1) {
                        unsigned char *dn = p + (g_LdSurf.pitch / 4) * 4;
                        if (dn[3] != 0) {
                            r += dn[2];
                            g += dn[1];
                            b += dn[0];
                            cnt++;
                        }
                    }
                    if (cnt > 1) {
                        r /= cnt;
                        g /= cnt;
                        b /= cnt;
                    }
                    p[2] = (unsigned char)r;
                    p[1] = (unsigned char)g;
                    p[0] = (unsigned char)b;
                }
                p += 4;
            }
        }
        break;
    }
    }
    return true;
}

} // namespace th128
