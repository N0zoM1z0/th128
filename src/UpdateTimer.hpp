#pragma once

#include "ZunTimer.hpp"

// Shared update-path setter, distinct from ZunTimer::SetCurrent at 0x00403240.
// ANM script updates and HelpInf both call the copy at 0x00403280.
__declspec(noinline) void __stdcall SetUpdateTimerCurrent(ZunTimer *timer, int value);
