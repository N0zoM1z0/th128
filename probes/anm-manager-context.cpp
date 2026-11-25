// Non-runnable compiler context: most callers load the manager from its
// global pointer and pass ids held in object fields; others pass a manager
// pointer they already hold.
#include "../src/AnmManager.hpp"

AnmManager **g_ProbeAnmManagerAddress = &g_AnmManager;

typedef AnmVm *(__fastcall *GetVmByIdTargetAbiPointer)(
    void *, AnmManager *, volatile unsigned int);
GetVmByIdTargetAbiPointer volatile g_KeepGetVmByIdTargetAbi =
    &GetVmByIdTargetAbi;

struct ProbeVmOwner
{
    unsigned int vmIds[4];
};

extern "C" unsigned int __stdcall AnmManagerProbe(AnmManager *manager, ProbeVmOwner *owner, unsigned int id)
{
    unsigned int found = 0;
    found += g_AnmManager->GetVmById(owner->vmIds[0]) != 0;
    found += g_AnmManager->GetVmById(owner->vmIds[1]) != 0;
    found += g_AnmManager->GetVmById(owner->vmIds[2]) != 0;
    found += g_AnmManager->GetVmById(id) != 0;
    found += manager->GetVmById(owner->vmIds[3]) != 0;
    return found;
}
