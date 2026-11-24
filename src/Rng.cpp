#include "Rng.hpp"

#include <windows.h>

namespace
{
    const volatile float kRandomUnsignedRange = 4294967296.0f;
}

extern unsigned int g_GameFlags;        // bit 15: worker threads share the generator
extern CRITICAL_SECTION g_RngLock;      // 0x004D3768
extern unsigned char g_RngLockDepth;    // 0x004D37A2

unsigned short Rng::GetU16()
{
    if (g_GameFlags & 0x8000)
    {
        EnterCriticalSection(&g_RngLock);
        g_RngLockDepth++;
    }
    generationCount++;
    unsigned short step = (seed ^ 0x9630) - 0x6553;
    seed = ((step & 0xC000) >> 14) + (step << 2);
    if (g_GameFlags & 0x8000)
    {
        LeaveCriticalSection(&g_RngLock);
        g_RngLockDepth--;
    }
    return seed;
}

unsigned int Rng::GetU32()
{
    if (g_GameFlags & 0x8000)
    {
        EnterCriticalSection(&g_RngLock);
        g_RngLockDepth++;
    }
    unsigned short high = (seed ^ 0x9630) - 0x6553;
    generationCount += 2;
    unsigned short rotated = ((high & 0xC000) >> 14) + (high << 2);
    unsigned short low = (rotated ^ 0x9630) - 0x6553;
    seed = ((low & 0xC000) >> 14) + (low << 2);
    if (g_GameFlags & 0x8000)
    {
        LeaveCriticalSection(&g_RngLock);
        g_RngLockDepth--;
    }
    return (high << 16) | low;
}

float Rng::GetFloat()
{
    return (float)((float)GetU32() * (1.0 / 4294967296.0));
}

__declspec(noinline) float Rng::GetFloat(float scale)
{
    scale = (float)((float)GetU32() * (1.0 / 4294967296.0)) * scale;
    return scale;
}

float Rng::GetFloatSigned()
{
    return (float)((float)GetU32() * (2.0 / 4294967296.0) - 1.0);
}

__declspec(noinline) float Rng::GetFloatSigned(float scale)
{
    scale *= (float)((float)GetU32() * (2.0 / 4294967296.0) - 1.0);
    return scale;
}

float Rng::GetAngle()
{
    return (float)((float)GetU32() / 683565248.0 - static_cast<double>(3.14159265358979323846f));
}
