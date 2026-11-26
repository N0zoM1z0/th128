// Non-runnable VC10 /GL context for the real graphics-surface restore owner.
#include "../src/SupervisorSurfaces.hpp"

extern "C" void __stdcall SupervisorSurfacesProbe(SupervisorSurfaceState *state)
{
    RestoreSupervisorSurfaces(state);
}
