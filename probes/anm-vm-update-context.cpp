#include "../src/AnmVm.hpp"
#include "../src/AnmManager.hpp"

typedef AnmVm *(__fastcall *GetVmByIdTargetAbiPointer)(
    void *, AnmManager *, volatile unsigned int);
GetVmByIdTargetAbiPointer volatile g_KeepGetVmByIdTargetAbi =
    &GetVmByIdTargetAbi;

extern "C" int __stdcall AnmVmUpdateProbe(AnmVm *vm)
{
    return vm->Update();
}
