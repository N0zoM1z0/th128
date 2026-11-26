// Non-runnable VC10 /GL context for the real D3D9 state initializer.
#include "../src/SupervisorRenderState.hpp"
#include "../src/Supervisor.hpp"
#include "../src/AnmManager.hpp"

Supervisor g_Supervisor;
AnmManager *g_AnmManager;

extern "C" void __stdcall SupervisorRenderStateProbe()
{
    InitializeSupervisorRenderState();
}
