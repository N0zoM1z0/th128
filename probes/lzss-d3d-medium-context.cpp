#include "LzssD3dMedium.hpp"

namespace th128 {

unsigned char g_LdLzssWindow[0x2000];
LdSurface g_LdSurf;
volatile int g_LdAnchorSink = 0;

} // namespace th128

using namespace th128;

extern "C" int __stdcall LzssD3dMediumProbe(
    const unsigned char *src,
    int srcLen,
    unsigned char *dst,
    unsigned int dstSize,
    const char *appName,
    char *cmdLine,
    int wait,
    unsigned int height
) {
    volatile void *ptrs[] = {
        (void *)&LdLzssDecompress,
        (void *)&LdCreateProcessAndWait,
        (void *)&LdBleedTransparentPixels,
    };
    g_LdAnchorSink ^= (int)ptrs[0];

    int acc = 0;
    acc ^= (int)((unsigned char *(__stdcall *)(const unsigned char *, int, unsigned char *, unsigned int))ptrs[0])(src, srcLen, dst, dstSize);
    acc ^= ((int (__stdcall *)(const char *, char *, int))ptrs[1])(appName, cmdLine, wait);
    acc ^= ((bool (__stdcall *)(unsigned int))ptrs[2])(height) ? 1 : 0;
    return acc ^ g_LdAnchorSink;
}
