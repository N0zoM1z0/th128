#pragma once

#include "AnmVm.hpp"
#include "AnmVmGeometry.hpp"

struct AnmManager;
struct IDirect3DDevice9;

// Partial graphics-state view rooted at Supervisor + 8 (0x004D2E68).
struct AnmFogState
{
    unsigned char unknown000[8];
    IDirect3DDevice9 *device;          // +0x008
    unsigned char unknown00C[0x984];
    int enabled;                     // +0x990

    long Enable();
    long Disable();
};
typedef char AnmFogStateDeviceOffset[(offsetof(AnmFogState, device) == 8) ? 1 : -1];
typedef char AnmFogStateEnabledOffset[(offsetof(AnmFogState, enabled) == 0x990) ? 1 : -1];

ZunVec3 __stdcall GetAnmVmDrawPosition(const AnmVm *vm);

// Shared transformed sprite quad at 0x004DAF28, in TL, TR, BL, BR order.
extern AnmVmEffect10Vertex g_AnmSpriteVertices[4];

namespace AnmDrawLeaf
{
    void __stdcall ApplyState(AnmVm *vm, AnmManager *manager);
    int __stdcall AppendSprite(AnmManager *manager, const AnmVmEffect10Vertex *vertices);
    int __stdcall PixelAlignedSprite(AnmVm *vm, AnmManager *manager);
    int __stdcall UnrotatedSprite(AnmManager *manager, AnmVm *vm);
    int __stdcall RotatedSprite(AnmVm *vm, AnmManager *manager);
    int __stdcall AlternateRotatedSprite(AnmVm *vm, AnmManager *manager);

    // 0x0045E920: private register assignment remains an LTCG hypothesis.
    int __stdcall SubmitSprite(AnmManager *manager, AnmVm *vm, unsigned int flags);
}
