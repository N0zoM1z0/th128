// Non-runnable VC10 /GL context for SupervisorTime::GetElapsedSeconds.
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
LARGE_INTEGER g_QpcFrequency;
LARGE_INTEGER g_QpcStart;
double g_TimeOrigin = 0.0;

unsigned int *g_ProbeSupFlags = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeSupCs = g_Supervisor.criticalSections;
unsigned char *g_ProbeSupDepth = g_Supervisor.lockDepths;

double __cdecl GetElapsedSeconds();
typedef double (__cdecl *GetElapsedPtr)();
GetElapsedPtr g_ProbeGetElapsed = GetElapsedSeconds;

extern "C" double __stdcall SupervisorTimeProbe()
{
    return GetElapsedSeconds() + GetElapsedSeconds();
}
