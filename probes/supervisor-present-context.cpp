// Non-runnable VC10 /GL context for the three real Present implementations.
#include "../src/SupervisorFrame.hpp"

extern "C" void __stdcall SupervisorPresentProbe(SupervisorFrame *first, SupervisorFrame *second)
{
    first->PresentAdaptive();
    first->PresentImmediate();
    second->PresentWithSleep();
}
