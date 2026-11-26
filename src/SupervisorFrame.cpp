#include "SupervisorFrame.hpp"
#include "Supervisor.hpp"
#include "AnmManager.hpp"
#include "AnmDraw.hpp"
#include "AnmViewport.hpp"
#include "Chain.hpp"

extern unsigned int g_WindowFlags; // 0x004D59A8
double __cdecl GetElapsedSeconds();

namespace
{
    // Frame-only fields not yet named in the wider Supervisor layout.
    struct SupervisorDrawState
    {
        unsigned char unknown000[0x324];
        AnmSpriteViewport defaultViewport; // +0x324 (0x004D3184)
        unsigned char unknown414[0x144];
        int drawingWorld;                  // +0x558 (0x004D33B8)
        unsigned char unknown55C[0x43C];
        int fogEnabled;                    // +0x998 (0x004D37F8)
        unsigned char unknown99C[0x24];
        double frameWorkTime;              // +0x9C0 (0x004D3820)
    };

    inline SupervisorDrawState &DrawState()
    {
        return *reinterpret_cast<SupervisorDrawState *>(&g_Supervisor);
    }
}

int __stdcall RunAdaptiveFrame(SupervisorFrame *frame)
{
    if ((g_WindowFlags & 0x10) != 0)
    {
        while (frame->currentTime > frame->nextTickTime)
            frame->nextTickTime += 1.0 / 60.0;
    }
    g_AnmManager->FlushSpriteBatch();
    g_AnmSpriteViewport = &DrawState().defaultViewport;
    g_AnmSpriteViewport->Set2DView();
    g_Supervisor.d3dDevice->SetViewport(&g_AnmSpriteViewport->viewport);
    DrawState().drawingWorld = 1;
    const int result = g_Chain->RunCalcChain();
    if (result == 0)
    {
        g_Supervisor.thread.Stop();
        return 1;
    }
    if (result == -1)
    {
        g_Supervisor.thread.Stop();
        return 2;
    }
    ++frame->framesSinceDraw;
    if (g_Supervisor.configFlag304E + 1 <= frame->framesSinceDraw)
    {
        g_Supervisor.d3dDevice->BeginScene();
        AnmDrawLeaf::ResetBuffers();
        DrawState().fogEnabled = 0xFF;
        reinterpret_cast<AnmFogState *>(reinterpret_cast<unsigned char *>(&g_Supervisor) + 8)->Disable();
        g_Chain->RunDrawChain();
        g_AnmManager->FlushSpriteBatch();
        g_Supervisor.d3dDevice->SetTexture(0, 0);
        g_Supervisor.d3dDevice->EndScene();
        frame->framesSinceDraw = 0;
        frame->PresentAdaptive();
    }
    DrawState().frameWorkTime = GetElapsedSeconds() - frame->currentTime;
    return 0;
}

int __stdcall RunTimedFrame(SupervisorFrame *frame)
{
    frame->currentTime = GetElapsedSeconds();
    if (frame->currentTime < frame->previousTime)
        frame->nextTickTime = frame->currentTime;
    frame->previousTime = frame->currentTime;
    if ((frame->nextTickTime - frame->currentTime) * 1000.0 >= 1.5)
        Sleep(1);
    if (!(frame->currentTime > frame->nextTickTime))
        return 0;
    while (frame->currentTime > frame->nextTickTime)
        frame->nextTickTime += 1.0 / 60.0;

    g_AnmManager->FlushSpriteBatch();
    g_AnmSpriteViewport = &DrawState().defaultViewport;
    g_AnmSpriteViewport->Set2DView();
    g_Supervisor.d3dDevice->SetViewport(&g_AnmSpriteViewport->viewport);
    DrawState().drawingWorld = 1;
    const int result = g_Chain->RunCalcChain();
    if (result == 0)
    {
        g_Supervisor.thread.Stop();
        return 1;
    }
    if (result == -1)
    {
        g_Supervisor.thread.Stop();
        return 2;
    }
    ++frame->framesSinceDraw;
    if (g_Supervisor.configFlag304E + 1 <= frame->framesSinceDraw)
    {
        g_Supervisor.d3dDevice->BeginScene();
        AnmDrawLeaf::ResetBuffers();
        DrawState().fogEnabled = 0xFF;
        reinterpret_cast<AnmFogState *>(reinterpret_cast<unsigned char *>(&g_Supervisor) + 8)->Disable();
        g_Chain->RunDrawChain();
        g_AnmManager->FlushSpriteBatch();
        g_Supervisor.d3dDevice->SetTexture(0, 0);
        g_Supervisor.d3dDevice->EndScene();
        frame->framesSinceDraw = 0;
        frame->PresentImmediate();
    }
    DrawState().frameWorkTime = GetElapsedSeconds() - frame->currentTime;
    return 0;
}

int __stdcall RunVsyncFrame(SupervisorFrame *frame)
{
    g_AnmManager->FlushSpriteBatch();
    g_AnmSpriteViewport = &DrawState().defaultViewport;
    g_AnmSpriteViewport->Set2DView();
    g_Supervisor.d3dDevice->SetViewport(&g_AnmSpriteViewport->viewport);
    DrawState().drawingWorld = 1;
    const int result = g_Chain->RunCalcChain();
    if (result == 0)
    {
        g_Supervisor.thread.Stop();
        return 1;
    }
    if (result == -1)
    {
        g_Supervisor.thread.Stop();
        return 2;
    }
    ++frame->framesSinceDraw;
    if (g_Supervisor.configFlag304E + 1 <= frame->framesSinceDraw)
    {
        g_Supervisor.d3dDevice->BeginScene();
        AnmDrawLeaf::ResetBuffers();
        DrawState().fogEnabled = 0xFF;
        reinterpret_cast<AnmFogState *>(reinterpret_cast<unsigned char *>(&g_Supervisor) + 8)->Disable();
        g_Chain->RunDrawChain();
        g_AnmManager->FlushSpriteBatch();
        g_Supervisor.d3dDevice->SetTexture(0, 0);
        g_Supervisor.d3dDevice->EndScene();
        frame->framesSinceDraw = 0;
        frame->PresentWithSleep();
    }
    DrawState().frameWorkTime = GetElapsedSeconds() - frame->currentTime;
    return 0;
}
