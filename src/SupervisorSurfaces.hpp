#pragma once

#include <stddef.h>

struct IDirect3DDevice9;
struct IDirect3DSurface9;
struct AnmVm;
struct AnmResource;

// One observed viewport-state block is copied as 0x46 DWORDs. Its remaining
// members are not yet reconstructed; this stores runtime state, not code.
struct SupervisorViewportBlock
{
    unsigned char unknown[0x118];
};

// Partial graphics view rooted at Supervisor + 8 (0x004D2E68).
struct SupervisorSurfaceState
{
    unsigned char unknown000[8];
    IDirect3DDevice9 *device;                // +0x008
    unsigned char unknown00C[0x1A0];
    IDirect3DSurface9 *firstTextureSurface;  // +0x1AC
    IDirect3DSurface9 *secondTextureSurface; // +0x1B0
    IDirect3DSurface9 *backBuffer;           // +0x1B4
    unsigned char unknown1B8[4];
    AnmVm *firstVm;                         // +0x1BC
    AnmVm *secondVm;                        // +0x1C0
    AnmVm *thirdVm;                         // +0x1C4
    unsigned char unknown1C8[0x3C];
    SupervisorViewportBlock sourceViewport; // +0x204
    unsigned char unknown31C[0x118];
    SupervisorViewportBlock fallbackViewport; // +0x434
    unsigned char unknown54C[0x3C];
    AnmResource *resource;                  // +0x588
};

typedef char SupervisorSurfaceFirstOffset[
    (offsetof(SupervisorSurfaceState, firstTextureSurface) == 0x1AC) ? 1 : -1];
typedef char SupervisorSurfaceSourceOffset[
    (offsetof(SupervisorSurfaceState, sourceViewport) == 0x204) ? 1 : -1];
typedef char SupervisorSurfaceFallbackOffset[
    (offsetof(SupervisorSurfaceState, fallbackViewport) == 0x434) ? 1 : -1];
typedef char SupervisorSurfaceResourceOffset[
    (offsetof(SupervisorSurfaceState, resource) == 0x588) ? 1 : -1];

void __stdcall RestoreSupervisorSurfaces(SupervisorSurfaceState *state);
