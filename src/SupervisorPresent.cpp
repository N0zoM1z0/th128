#include "SupervisorFrame.hpp"
#include "Supervisor.hpp"
#include "SupervisorDevice.hpp"
#include "SupervisorSurfaces.hpp"
#include "SupervisorRenderState.hpp"
#include "PostPresentUpdates.hpp"

#include <d3d9.h>

double __cdecl GetElapsedSeconds();

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

void SupervisorFrame::PresentAdaptive()
{
    currentTime = GetElapsedSeconds();
    estimatedSleepMilliseconds = (int)((lastPresentTime + 1.0 / 60.0 - currentTime) * 1000.0 - 2.5);
    if (sleepProfiles[sleepProfileIndex].sleepMilliseconds >= estimatedSleepMilliseconds)
    {
        sleepProfiles[sleepProfileIndex].sleepMilliseconds =
            estimatedSleepMilliseconds < 0 ? 0 : estimatedSleepMilliseconds;
        sleepProfiles[sleepProfileIndex].stableFrameCount = 0;
    }
    else
    {
        ++sleepProfiles[sleepProfileIndex].stableFrameCount;
        if (sleepProfiles[sleepProfileIndex].stableFrameCount >= 15)
        {
            if (sleepProfiles[sleepProfileIndex].sleepMilliseconds <
                    sleepProfiles[sleepProfileIndex].maximumMilliseconds)
                ++sleepProfiles[sleepProfileIndex].sleepMilliseconds;
            sleepProfiles[sleepProfileIndex].stableFrameCount = 0;
        }
    }
    if (estimatedSleepMilliseconds < 0)
        estimatedSleepMilliseconds = 0;

    D3DRASTER_STATUS raster;
    raster.ScanLine = 0;
    const double now = GetElapsedSeconds();
    if (now < lastRasterTime)
        lastRasterTime = now;
    if (now - lastRasterTime < 1.0 / 59.0)
    {
        while (GetElapsedSeconds() - lastRasterTime < 0.013)
            Sleep(1);

        unsigned int previousScanLine = raster.ScanLine;
        raster.InVBlank = FALSE;
        while (g_Supervisor.d3dDevice->GetRasterStatus(0, &raster) == D3D_OK)
        {
            if (previousScanLine > raster.ScanLine || raster.ScanLine == 0 || raster.InVBlank)
                break;
            previousScanLine = raster.ScanLine;
        }
    }
    else if (sleepProfiles[sleepProfileIndex].sleepMilliseconds > 0)
    {
        --sleepProfiles[sleepProfileIndex].sleepMilliseconds;
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
    if (g_PostPresentTimingState != 0)
        UpdatePostPresentTiming();
    if (g_PostPresentReplayState != 0)
        UpdatePostPresentReplay();
    GetElapsedSeconds();
    if (sleepProfiles[sleepProfileIndex].sleepMilliseconds > 0)
        Sleep(sleepProfiles[sleepProfileIndex].sleepMilliseconds);
    lastPresentTime = GetElapsedSeconds();
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
