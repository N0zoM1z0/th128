// Non-runnable VC10 /GL entry for the real FPS and replay-pause callbacks.
#include "../src/PostPresentUpdates.hpp"

extern "C" void __stdcall PostPresentUpdatesProbe()
{
    UpdatePostPresentTiming();
    UpdatePostPresentReplay();
}
