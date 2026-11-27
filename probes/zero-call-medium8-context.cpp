#include "../src/ZeroCallMedium8.hpp"

namespace th128 {
unsigned char g_Zc8LzssRing[0x2000];
Zc8AnmMgrLine *g_Zc8AnmMgr = 0;
IDirect3DDevice9 *g_Zc8D3DDevice = 0;
} // namespace th128

using namespace th128;

extern "C" int __stdcall ZeroCallMedium8Probe(
    Zc8AnmVm *vm,
    Zc8AnmLoaded *loaded,
    int spriteId,
    const Zc8Vec2 *pts,
    const unsigned int *colors,
    int count,
    Zc8AnmMgrLine *mgr,
    IDirect3DDevice9 *dev
) {
    g_Zc8AnmMgr = mgr;
    g_Zc8D3DDevice = dev;
    int acc = Zc8AnmSetSprite(vm, loaded, spriteId);
    acc += Zc8AnmDrawLineStrip(count, pts, colors);
    return acc;
}
