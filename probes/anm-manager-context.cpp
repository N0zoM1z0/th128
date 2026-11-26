// Non-runnable compiler context: most callers load the manager from its
// global pointer and pass ids held in object fields; others pass a manager
// pointer they already hold.
#include "../src/AnmManager.hpp"

AnmManager **g_ProbeAnmManagerAddress = &g_AnmManager;

struct ProbeVmOwner
{
    unsigned int vmIds[4];
};

extern "C" unsigned int __stdcall AnmManagerProbe(AnmManager *manager, ProbeVmOwner *owner, unsigned int id)
{
    unsigned int found = 0;
    for (int i = 0; i < 3; ++i)
    {
        AnmId query = { owner->vmIds[i] };
        found += g_AnmManager->GetVmById(query) != 0;
    }
    AnmId query = { id };
    found += g_AnmManager->GetVmById(query) != 0;
    query.id = owner->vmIds[3];
    found += manager->GetVmById(query) != 0;
    return found;
}
