// Non-runnable VC10 /GL context for the two real device-loss ANM passes.
#include "../src/SupervisorDevice.hpp"
#include "../src/Supervisor.hpp"
#include "../src/AnmManager.hpp"

Supervisor g_Supervisor;
AnmManager *g_AnmManager;

extern "C" void __stdcall SupervisorDeviceProbe()
{
    ReleaseAnmRenderTargets();
    RecreateAnmRenderTargets();
}
