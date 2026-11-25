#include "../src/AnmVm.hpp"
#include "../src/AnmVmView.hpp"

// Non-runnable LTCG context: keep each view-mode call visible to the linker
// so it can select the same private interprocedural calling convention.
extern "C" void __stdcall AnmVmViewProbe(
    const AnmVm *vm, float *topLeft, float *topRight,
    float *bottomLeft, float *bottomRight)
{
    BuildAnmVmViewCornersMode0(vm, topLeft, topRight, bottomLeft, bottomRight);
    BuildAnmVmViewCornersMode1(vm, topLeft, topRight, bottomLeft, bottomRight);
    BuildAnmVmViewCornersMode23(vm, topLeft, topRight, bottomLeft, bottomRight);
}
