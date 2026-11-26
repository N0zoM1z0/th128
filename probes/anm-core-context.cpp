// Non-runnable VC10 /GL context for Phase 2 core AnmManager + AnmResource.
#include "../src/AnmManager.hpp"
#include "../src/AnmResource.hpp"
#include "../src/Chain.hpp"
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
AnmManager *g_AnmManager = 0;

struct AnmPauseStateView;
AnmPauseStateView *g_AnmPauseState = 0;

struct AnmD3DDeviceView;
AnmD3DDeviceView *g_D3DDevice = 0;

int __stdcall DrawAnmVmWithManager(AnmVm *vm, AnmManager *manager)
{
    AnmManager *volatile *p = &manager;
    unsigned int x = vm->flags;
    return (int)(x ^ (*p)->nextVmId);
}

AnmResource *__stdcall LoadAnmSlot(AnmManager *manager, int slot, const char *filename)
{
    volatile int x = slot + (int)filename[0];
    return manager->loadedAnms[x & 3];
}

void __cdecl ChainDebugLog(const char *fmt, ...)
{
    volatile char c = fmt[0];
    (void)c;
}

unsigned int *g_ProbeSupFlags = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeSupCs = g_Supervisor.criticalSections;
unsigned char *g_ProbeSupDepth = g_Supervisor.lockDepths;
AnmManager **g_ProbeAnmManagerAddr = &g_AnmManager;

extern "C" void __stdcall AnmCoreProbe(
    AnmManager *m1, AnmManager *m2,
    AnmResource *r1, AnmResource *r2,
    AnmVm *v1, AnmVm *v2,
    const ZunVec3 *pos, unsigned int *id1, unsigned int *id2,
    int slot, short intr, const char *name)
{
    AllocateAnmVm();
    AnmId a1 = { *id1 };
    AnmId a2 = { *id2 };
    // GetVmPosById at 0x004676F0 also forwards a dynamic manager receiver.
    AnmVm *found1 = m1->GetVmById(a1);
    AnmVm *found2 = m2->GetVmById(a2);
    if (found1 != 0) *id1 = found1->id;
    if (found2 != 0) *id2 = found2->id;
    m1->StopVmById(a1, intr);
    m2->StopVmById(a2, (short)(intr + 1));
    m1->SetInterruptAndUpdate(a1, intr);
    m2->SetInterruptAndUpdate(a2, (short)(intr + 1));
    m1->MarkResourceVmsForDelete(r1);
    m2->MarkResourceVmsForDelete(r2);
    m1->DeleteVmById(id1);
    m2->DeleteVmById(id2);
    m1->FindChildVmByScript(id1, id2, slot);
    m2->FindChildVmByScript(id2, id1, slot + 1);
    m1->CleanupVm(v1);
    m2->CleanupVm(v2);
    m1->UpdateWorldVms();
    m2->UpdateWorldVms();
    m1->UpdateUiVms();
    m2->UpdateUiVms();
    m1->DrawVmGroup((unsigned int)slot);
    m2->DrawVmGroup((unsigned int)(slot + 1));
    static int (AnmManager::*const kLayers[])() = {
        &AnmManager::OnTickWorldCallback, &AnmManager::OnTickUiCallback,
        &AnmManager::DrawLayer0,
        &AnmManager::DrawLayer1, &AnmManager::DrawLayer2, &AnmManager::DrawLayer3,
        &AnmManager::DrawLayer5, &AnmManager::DrawLayer6, &AnmManager::DrawLayer7,
        &AnmManager::DrawLayer8, &AnmManager::DrawLayer9, &AnmManager::DrawLayer10,
        &AnmManager::DrawLayer11, &AnmManager::DrawLayer12, &AnmManager::DrawLayer13,
        &AnmManager::DrawLayer14, &AnmManager::DrawLayer15, &AnmManager::DrawLayer16,
        &AnmManager::DrawLayer17, &AnmManager::DrawLayer18, &AnmManager::DrawLayer19,
        &AnmManager::DrawLayer22, &AnmManager::DrawLayer23, &AnmManager::DrawLayer25,
        &AnmManager::DrawLayer26,
    };
    (m1->*kLayers[slot & 15])();
    if (m1->AreAllAnmsLoaded() || m2->AreAllAnmsLoaded()) *id2 = 1;
    m1->ReleaseAnm((unsigned int)slot);
    m2->ReleaseAnm((unsigned int)(slot + 1));
    if (r1->textures != 0) r1->textures->Release();
    if (r2->textures != 0) r2->textures->Release();
    if (m1->IsPoolVm(v1) || m2->IsPoolVm(v2)) *id1 = 1;
    AnmManager::PreloadAnm(slot, name);
    AnmManager::PreloadAnm(slot + 1, name);
    m1->FlushSpriteBatch();
    m2->FlushSpriteBatch();
    AnmResource tmpRes;
    tmpRes.Release();
    r1->Release();
    r2->Release();
    *id1 = r1->CreateVm(pos, slot, 0, 1).id;
    *id2 = r2->CreateVm(0, slot + 1, -1, 4).id;
}
