#include "CoreInputCollision.hpp"

namespace th128 {

unsigned int g_CiFlags = 0;
CiCriticalSection g_CiFileCs;
unsigned char g_CiFileLockCount = 0;
CiArchive g_CiArchive = { 0, 0 };
int g_CiWindowActive = 1;
CiDInputDevice *g_CiKeyboardDev = 0;
CiDInputDevice *g_CiJoystickDev = 0;
unsigned int g_CiJoyMinX = 0;
unsigned int g_CiJoyMaxX = 65535;
unsigned int g_CiJoyMinY = 0;
unsigned int g_CiJoyMaxY = 65535;
short g_CiJoyDeadzoneX = 600;
short g_CiJoyDeadzoneY = 600;
CiJoyConfig g_CiJoyConfig = { 0, 1, 2, 3, 4 };
unsigned char g_CiJoyRawButtons[0x80];

volatile int g_CiAnchorSink = 0;

__declspec(noinline) void __cdecl CiLogPrintf(const char *fmt, ...) {
    g_CiAnchorSink ^= (int)fmt;
}

__declspec(noinline) void __stdcall CiArchiveExtract(CiArchive *arc, const char *name, void *dst) {
    g_CiAnchorSink ^= (int)arc ^ (int)name ^ (int)dst;
}

} // namespace th128

using namespace th128;

extern "C" int __stdcall CoreInputCollisionProbe(
    const char *path,
    unsigned int *outSize,
    int fromDisk,
    float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7
) {
    volatile void *ptrs[] = {
        (void *)&CiLogPrintf,
        (void *)&CiArchiveExtract,
        (void *)&CiLoadFileFromArchiveOrDisk,
        (void *)&CiPollJoystickRaw,
        (void *)&CiPollInput,
        (void *)&CiHitTestSegments,
    };
    g_CiAnchorSink ^= (int)ptrs[0];

    int acc = 0;
    acc ^= (int)((void *(__stdcall *)(const char *, unsigned int *, int))ptrs[2])(path, outSize, fromDisk);
    acc ^= (int)CiPollJoystickRaw();
    acc ^= (int)CiPollInput();
    acc ^= ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[5])(f0, f1, f2, f3, f4, f5, f6, f7);
    return acc ^ g_CiAnchorSink;
}
