#include "SupervisorSurfaceCopies.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include "AnmResource.hpp"
#include "AnmD3DX.hpp"

namespace
{
    struct AnmSurfaceCopyRequest
    {
        int anmSlot;
        int textureIndex;
        int sourceX, sourceY, sourceWidth, sourceHeight;
        int destinationX, destinationY, destinationWidth, destinationHeight;
    };
}

void __stdcall FlushAnmSurfaceCopies()
{
    AnmManager *manager = g_AnmManager;
    AnmSurfaceCopyRequest *request = reinterpret_cast<AnmSurfaceCopyRequest *>(manager);
    for (int remaining = 4; remaining != 0; ++request, --remaining)
    {
        if (request->anmSlot >= 0)
        {
            {
                if (manager->loadedAnms[request->anmSlot]->textures[request->textureIndex].texture == 0)
                    goto consumed;
                manager->FlushSpriteBatch();
                IDirect3DSurface9 *backBuffer;
                if (g_Supervisor.d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer) != D3D_OK)
                    goto consumed;
                IDirect3DSurface9 *destination;
                if (reinterpret_cast<IDirect3DTexture9 *>(
                        manager->loadedAnms[request->anmSlot]->textures[request->textureIndex].texture)->GetSurfaceLevel(
                        0, &destination) != D3D_OK)
                {
                    backBuffer->Release();
                    goto consumed;
                }
                RECT sourceRect = {
                    request->sourceX, request->sourceY,
                    request->sourceX + request->sourceWidth,
                    request->sourceY + request->sourceHeight
                };
                RECT destinationRect = {
                    request->destinationX, request->destinationY,
                    request->destinationX + request->destinationWidth,
                    request->destinationY + request->destinationHeight
                };
                if (D3DXLoadSurfaceFromSurface(destination, 0, &destinationRect,
                        backBuffer, 0, &sourceRect, 2, 0) == D3D_OK)
                {
                    reinterpret_cast<IDirect3DTexture9 *>(
                        manager->loadedAnms[request->anmSlot]->textures[request->textureIndex].texture)->AddDirtyRect(0);
                }
                destination->Release();
                backBuffer->Release();
            }
        consumed:
            request->anmSlot = -1;
        }
    }
}
