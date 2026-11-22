// Non-runnable compiler context. Independent calls keep the inputs unknown
// while VC10 chooses its private interprocedural calling convention.
#include "../src/ZunTimer.hpp"

extern "C" int __stdcall ZunTimerProbe(ZunTimer *a, int x, ZunTimer *b, int y)
{
    return ZunTimerModulo(a, x) + ZunTimerModulo(b, y);
}
