// Non-runnable VC10 /GL context for the two real Present implementations.
#include "../src/SupervisorFrame.hpp"

extern "C" void __stdcall SupervisorPresentProbe(SupervisorFrame *first, SupervisorFrame *second)
{
    first->PresentImmediate();
    second->PresentWithSleep();
}
