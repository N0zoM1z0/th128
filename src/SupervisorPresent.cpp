#include "SupervisorFrame.hpp"
#include "Supervisor.hpp"
#include "SupervisorDevice.hpp"
#include "SupervisorSurfaces.hpp"

#include <d3d9.h>

// Reviewed boundaries: screenshot handling uses ECX; state setup has no
// live input parameter. The two post-Present hooks read their own globals.
void __stdcall InitializeSupervisorRenderState();             // 0x00455A70
int __cdecl UpdatePostPresentTiming();                        // 0x0041F990
void __cdecl UpdatePostPresentReplay();                       // 0x0040DA50
double __cdecl GetElapsedSeconds();
extern void *g_PostPresentTimingState;                        // 0x004B894C
extern void *g_PostPresentReplayState;                        // 0x004B8934

namespace
{
    struct SupervisorPresentState
    {
        unsigned char unknown000[0x57C];
        int deviceState;                                     // 0x004D33DC
    };

    inline SupervisorSurfaceState *Surfaces()
    {
        return reinterpret_cast<SupervisorSurfaceState *>(
            reinterpret_cast<unsigned char *>(&g_Supervisor) + 8);
    }
}

void SupervisorFrame::PresentImmediate()
{
    HandleScreenshot();
    if (FAILED(g_Supervisor.d3dDevice->Present(0, 0, 0, 0)))
    {
        if (Surfaces()->firstTextureSurface != 0)
        {
            Surfaces()->firstTextureSurface->Release();
            Surfaces()->firstTextureSurface = 0;
        }
        if (Surfaces()->secondTextureSurface != 0)
        {
            Surfaces()->secondTextureSurface->Release();
            Surfaces()->secondTextureSurface = 0;
        }
        if (Surfaces()->backBuffer != 0)
        {
            Surfaces()->backBuffer->Release();
            Surfaces()->backBuffer = 0;
        }
        Surfaces()->firstTextureSurface = 0;
        ReleaseAnmRenderTargets();
        g_Supervisor.d3dDevice->Reset(
            reinterpret_cast<D3DPRESENT_PARAMETERS *>(&g_Supervisor.backBufferWidth));
        RecreateAnmRenderTargets();
        RestoreSupervisorSurfaces(Surfaces());
        InitializeSupervisorRenderState();
        reinterpret_cast<SupervisorPresentState *>(&g_Supervisor)->deviceState = 2;
    }
    if (g_PostPresentTimingState != 0)
        UpdatePostPresentTiming();
    if (g_PostPresentReplayState == 0)
        return;
    UpdatePostPresentReplay();
}

void SupervisorFrame::PresentWithSleep()
{
    currentTime = GetElapsedSeconds();
    if (g_Supervisor.configFrameSkip == 1)
    {
        const double elapsed = currentTime - lastPresentTime;
        if (elapsed < 1.0 / 60.0 && elapsed > 0.0)
        {
            const int delay = (int)((lastPresentTime + 1.0 / 60.0 - currentTime) * 1000.0 - 3.5);
            if (delay > 0)
                Sleep(delay);
        }
    }
    lastRasterTime = GetElapsedSeconds();
    HandleScreenshot();
    if (FAILED(g_Supervisor.d3dDevice->Present(0, 0, 0, 0)))
    {
        if (Surfaces()->firstTextureSurface != 0)
        {
            Surfaces()->firstTextureSurface->Release();
            Surfaces()->firstTextureSurface = 0;
        }
        if (Surfaces()->secondTextureSurface != 0)
        {
            Surfaces()->secondTextureSurface->Release();
            Surfaces()->secondTextureSurface = 0;
        }
        if (Surfaces()->backBuffer != 0)
        {
            Surfaces()->backBuffer->Release();
            Surfaces()->backBuffer = 0;
        }
        Surfaces()->firstTextureSurface = 0;
        ReleaseAnmRenderTargets();
        g_Supervisor.d3dDevice->Reset(
            reinterpret_cast<D3DPRESENT_PARAMETERS *>(&g_Supervisor.backBufferWidth));
        RecreateAnmRenderTargets();
        RestoreSupervisorSurfaces(Surfaces());
        InitializeSupervisorRenderState();
        reinterpret_cast<SupervisorPresentState *>(&g_Supervisor)->deviceState = 2;
    }
    lastPresentTime = GetElapsedSeconds();
    if (g_PostPresentTimingState != 0)
        UpdatePostPresentTiming();
    if (g_PostPresentReplayState == 0)
        return;
    UpdatePostPresentReplay();
}
