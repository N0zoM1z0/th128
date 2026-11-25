#ifndef TH128_ENGINE_UTIL_HPP
#define TH128_ENGINE_UTIL_HPP

#include <windows.h>

struct EngineVec2
{
    float x;
    float y;
};

void __stdcall RotateVec2(EngineVec2 *out, const EngineVec2 *in, float angle);
int __stdcall IsWindowMinimized(HWND hwnd);
int __cdecl GetWindowsVersionCode();
const char *__stdcall ReadShiftJisLine(char *dst, const char *src, int maxLen);
unsigned char __stdcall ChecksumBytes(const unsigned char *data, unsigned int len);
unsigned int __stdcall ChecksumDwords(const unsigned int *data, int byteLen);

#endif
