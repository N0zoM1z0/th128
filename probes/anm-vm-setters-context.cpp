// Non-runnable compiler context. Independent calls keep the inputs unknown
// while VC10 chooses its private interprocedural calling conventions.
#include "../src/AnmVm.hpp"

extern "C" void __stdcall AnmVmSettersProbe(AnmVm *a, AnmVm *b, float x, float y)
{
    AnmVmSetScale(a, x, y);
    AnmVmSetScale(b, y, x);
    AnmVmSetScaleX(a, x);
    AnmVmSetScaleX(b, y);
    AnmVmSetScaleY(a, x);
    AnmVmSetScaleY(b, y);
    AnmVmSetScale2(a, x, y);
    AnmVmSetScale2(b, y, x);
    AnmVmSetUvScroll(a, x, y);
    AnmVmSetUvScroll(b, y, x);
}
