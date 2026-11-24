#include "../src/AnmVm.hpp"

extern "C" int __stdcall AnmVmUpdateProbe(AnmVm *vm)
{
    return vm->Update();
}
