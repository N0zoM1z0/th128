#include "../src/ZeroCallMedium5.hpp"

namespace th128 {
Zc5Player *g_Zc5Player = 0;
} // namespace th128

using namespace th128;

static __declspec(noinline) int CallIsOffscreenAligned(
    Zc5BackgroundQuad *quad,
    const ZunVec3 *offset,
    float maxDistSq,
    const Zc5Camera *camera
) {
    volatile double alignStack = 0.5;
    int r = quad->IsOffscreen(offset, maxDistSq, camera);
    return r + (int)alignStack;
}

extern "C" int __stdcall ZeroCallMedium5Probe(
    Zc5BackgroundQuad *quad,
    const ZunVec3 *offset,
    float maxDistSq,
    const Zc5Camera *camera,
    Zc5ItemPool *pool,
    float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7
) {
    volatile void *ptrs[] = {
        (void *)&g_Zc5Player,
        (void *)&CallIsOffscreenAligned,
        (void *)&Zc5RotatedRectCircleHit,
        (void *)&Zc5SegmentSegmentHit,
    };
    int acc = (int)ptrs[(int)f0 & 3];
    acc += CallIsOffscreenAligned(quad, offset, maxDistSq, camera);
    Zc5ClearActiveItems(pool);
    acc += ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[2])(f0, f1, f2, f3, f4, f5, f6, f7);
    acc += ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[3])(f0, f1, f2, f3, f4, f5, f6, f7);
    return acc;
}
