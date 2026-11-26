#pragma once

#include "AnmVm.hpp"

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
