#include "../src/AnmVm.hpp"
#include "../src/AnmManager.hpp"

extern "C" int __stdcall AnmVmUpdateProbe(AnmVm *vm)
{
    return vm->Update();
}
