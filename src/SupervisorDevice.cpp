#include "SupervisorDevice.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include "AnmResource.hpp"

#include <d3d9.h>

namespace
{
    inline void RecreateRenderTarget(AnmTextureEntry *entry)
    {
        entry->flags |= 1U;
        g_Supervisor.d3dDevice->CreateTexture(
            g_Supervisor.backBufferWidth, g_Supervisor.backBufferHeight,
            1, D3DUSAGE_RENDERTARGET,
            static_cast<D3DFORMAT>(g_Supervisor.backBufferFormat),
            D3DPOOL_DEFAULT, reinterpret_cast<IDirect3DTexture9 **>(&entry->texture), 0);
        entry->bytesPerPixel = g_Supervisor.backBufferFormat == D3DFMT_X8R8G8B8 ? 4 : 2;
    }
}

void __stdcall ReleaseAnmRenderTargets()
{
    AnmResource **slot = g_AnmManager->loadedAnms;
    for (int remaining = 38; remaining != 0; ++slot, --remaining)
    {
        if (*slot != 0)
        {
            for (int index = 0; index < (*slot)->textureCount; ++index)
            {
                if (((*slot)->textures[index].flags & 1U) != 0 && (*slot)->textures[index].texture != 0)
                {
                    reinterpret_cast<IDirect3DTexture9 *>((*slot)->textures[index].texture)->Release();
                    (*slot)->textures[index].texture = 0;
                }
            }
        }
    }
}

void __stdcall RecreateAnmRenderTargets()
{
    AnmResource **slot = g_AnmManager->loadedAnms;
    for (int remaining = 38; remaining != 0; ++slot, --remaining)
    {
        if (*slot != 0)
        {
            for (int index = 0; index < (*slot)->textureCount; ++index)
            {
                if (((*slot)->textures[index].flags & 1U) != 0)
                {
                    RecreateRenderTarget(&(*slot)->textures[index]);
                }
            }
        }
    }
}
