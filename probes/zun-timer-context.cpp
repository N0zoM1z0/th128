// Non-runnable compiler context. Independent calls keep the inputs unknown
// while VC10 chooses its private interprocedural calling conventions.
#include "../src/ZunTimer.hpp"

static int CompareAll(const ZunTimer *a, int x, const ZunTimer *b, int y)
{
    return (*a < x) + (*b < y) + (*a <= x) + (*b <= y) + (*a >= x) + (*b >= y)
        + (*a == x) + (*b == y) + (*a > x) + (*b > y);
}

extern "C" int __stdcall ZunTimerProbe(ZunTimer *a, int x, ZunTimer *b, int y)
{
    a->SetCurrent(x);
    b->SetCurrent(y);
    a->SetCurrentF((float)y);
    a->Add((float)x);
    b->Add(0.5f);
    b->SetCurrentF((float)x);
    return ZunTimerModulo(a, x) + ZunTimerModulo(b, y) + CompareAll(a, x, b, y);
}
