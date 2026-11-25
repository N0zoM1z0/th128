#include "EngineUtil.hpp"

#include <math.h>
#include <string.h>

void __stdcall RotateVec2(EngineVec2 *out, const EngineVec2 *in, float angle)
{
    float s = (float)sin((double)angle);
    float c = (float)cos((double)angle);
    float rx = in->x * c - in->y * s;
    out->y = in->x * s + in->y * c;
    out->x = rx;
}

int __stdcall IsWindowMinimized(HWND hwnd)
{
    return ShowWindow(hwnd, SW_HIDE) != 0;
}

int __cdecl GetWindowsVersionCode()
{
    OSVERSIONINFOA info;
    memset(&info, 0, sizeof(info));
    info.dwOSVersionInfoSize = sizeof(info);
    GetVersionExA(&info);
    switch (info.dwPlatformId)
    {
    case VER_PLATFORM_WIN32s:
        return 31;
    case VER_PLATFORM_WIN32_WINDOWS:
        if (info.dwMajorVersion == 4)
        {
            return info.dwMinorVersion != 0 ? 98 : 95;
        }
        return (int)(info.dwMajorVersion * 100 + info.dwMinorVersion);
    case VER_PLATFORM_WIN32_NT:
        if (info.dwMajorVersion == 5)
        {
            return 2000;
        }
        return (int)((info.dwMajorVersion * 100 + info.dwMinorVersion) * 10);
    default:
        return -1;
    }
}

const char *__stdcall ReadShiftJisLine(char *dst, const char *src, int maxLen)
{
    memset(dst, 0, maxLen);
    char *out = dst;
    while (*src != '\n' && *src != '\r' && maxLen != 0)
    {
        *out++ = *src;
        if (((unsigned char)*src >= 0x81 && (unsigned char)*src <= 0x9F) ||
            ((unsigned char)*src >= 0xE0 && (unsigned char)*src <= 0xFC))
        {
            ++src;
            --maxLen;
            *out++ = *src;
        }
        ++src;
        --maxLen;
    }
    while (*src == '\n' || *src == '\r')
    {
        ++src;
    }
    return src;
}

unsigned char __stdcall ChecksumBytes(const unsigned char *data, unsigned int len)
{
    unsigned char sum = 0;
    while (len != 0)
    {
        sum += *data++;
        --len;
    }
    return sum;
}

unsigned int __stdcall ChecksumDwords(const unsigned int *data, int byteLen)
{
    byteLen -= (byteLen & 3);
    unsigned int sum = 0;
    while (byteLen > 0)
    {
        sum += *data++;
        byteLen -= 4;
    }
    return sum;
}
