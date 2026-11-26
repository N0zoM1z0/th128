#include "SupervisorSurfaces.hpp"
#include "AnmResource.hpp"

#include <d3d9.h>

void __stdcall RestoreSupervisorSurfaces(SupervisorSurfaceState *state)
{
    if (state->firstTextureSurface != 0)
        return;

    reinterpret_cast<IDirect3DTexture9 *>(state->resource->textures[2].texture)->GetSurfaceLevel(
        0, &state->firstTextureSurface);
    state->resource->BindVmScript(state->firstVm, 0x52);
    if (state->firstTextureSurface == 0)
        state->fallbackViewport = state->sourceViewport;

    reinterpret_cast<IDirect3DTexture9 *>(state->resource->textures[3].texture)->GetSurfaceLevel(
        0, &state->secondTextureSurface);
    state->resource->BindVmScript(state->secondVm, 0x55);
    state->resource->BindVmScript(state->thirdVm, 0x52);
    state->device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &state->backBuffer);
}
