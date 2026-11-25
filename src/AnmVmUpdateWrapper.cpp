#include "AnmVm.hpp"

// Keep the original stack-argument boundary around the per-VM update call.
// This is a normal COFF object so LTCG cannot fold it into a tail jump.
void __stdcall UpdateVmForBinding(AnmVm *vm)
{
    vm->Update();
}
