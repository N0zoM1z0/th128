#include "../src/ZunTimer.hpp"

extern "C" void __stdcall ZunTimerCopyProbe(ZunTimer *destination, ZunTimer *source)
{
    destination->SetCurrent(*source);
}
