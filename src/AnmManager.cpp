#include "AnmManager.hpp"

#include <new>
#include <stdlib.h>

#include "AnmResource.hpp"
#include "Chain.hpp"
#include "Supervisor.hpp"

#include <d3d9.h>

extern int __stdcall DrawAnmVmWithManager(AnmVm *vm, AnmManager *manager);

struct AnmPauseStateView
{
    unsigned char unknown000[0x60];
    unsigned int flags;
};

extern AnmPauseStateView *g_AnmPauseState;

AnmVm *AnmManager::GetVmById(AnmId vmId)
{
    if (vmId.id == 0)
    {
        return 0;
    }

    for (AnmVmListNode *node = worldVms; node != 0; node = node->next)
    {
        if (node->vm->id == vmId.id)
        {
            return node->vm;
        }
    }

    for (AnmVmListNode *node = uiVms; node != 0; node = node->next)
    {
        if (node->vm->id == vmId.id)
        {
            return node->vm;
        }
    }

    return 0;
}

AnmVm *AllocateAnmVm()
{
    AnmManager *manager = g_AnmManager;
    int index = manager->nextVmPoolIndex;
    AnmVm *vm = &manager->vmPool[index];

    if (manager->vmPoolUsed[index] == 0)
    {
        manager->vmPoolUsed[index] = 1;
    }
    else
    {
        index = (index + 1) % 0x1000;
        manager->nextVmPoolIndex = index;
        vm = &manager->vmPool[index];

        if (manager->vmPoolUsed[index] == 0)
        {
            vm->ResetForReuse();
            manager->vmPoolUsed[manager->nextVmPoolIndex] = 1;
        }
        else
        {
            vm = new AnmVm;
            vm->ResetForReuse();
        }
    }

    manager->nextVmPoolIndex = (manager->nextVmPoolIndex + 1) % 0x1000;
    return vm;
}

void AnmManager::StopVmById(AnmId id, short interrupt)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm == 0)
    {
        return;
    }

    if (vm->stopCallback != 0)
    {
        vm->stopCallback(vm, interrupt);
    }
    vm->currentFrame = interrupt;

    if (vm->childNode.previous == 0)
    {
        for (AnmVmListNode *node = vm->childNode.next;
             node != 0;
             node = node->next)
        {
            AnmVm *child = node->vm;
            if (child->stopCallback != 0)
            {
                child->stopCallback(child, interrupt);
            }
            child->currentFrame = interrupt;
        }
    }
}

void AnmManager::SetInterruptAndUpdate(AnmId id, short interrupt)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm == 0)
    {
        return;
    }

    if (vm->stopCallback != 0)
    {
        vm->stopCallback(vm, interrupt);
    }
    vm->currentFrame = interrupt;
    vm->Update();

    if (vm->childNode.previous == 0)
    {
        for (AnmVmListNode *node = vm->childNode.next;
             node != 0;
             node = node->next)
        {
            AnmVm *child = node->vm;
            if (child->stopCallback != 0)
            {
                child->stopCallback(child, interrupt);
            }
            child->currentFrame = interrupt;
            node->vm->Update();
        }
    }
}

void AnmManager::MarkResourceVmsForDelete(AnmResource *resource)
{
    for (AnmVmListNode *node = worldVms; node != 0;)
    {
        AnmVmListNode *next = node->next;
        AnmVm *vm = node->vm;
        if (vm->resource == resource)
        {
            vm->flags |= 0x20000000U;
        }
        node = next;
    }

    for (AnmVmListNode *node = uiVms; node != 0;)
    {
        AnmVmListNode *next = node->next;
        AnmVm *vm = node->vm;
        if (vm->resource == resource)
        {
            vm->flags |= 0x20000000U;
        }
        node = next;
    }
}

void AnmManager::DeleteVmById(unsigned int *id)
{
    AnmId vmId = { *id };
    AnmVm *vm = g_AnmManager->GetVmById(vmId);
    if (vm != 0)
    {
        vm->flags |= 0x20000000U;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next;
                 node != 0;
                 node = node->next)
            {
                node->vm->flags |= 0x20000000U;
            }
        }
    }
    *id = 0;
}

unsigned int *AnmManager::FindChildVmByScript(
    unsigned int *outId, unsigned int *rootId, int scriptIndex)
{
    AnmId vmId = { *rootId };
    AnmVm *vm = g_AnmManager->GetVmById(vmId);
    if (vm == 0)
    {
        *rootId = 0;
    }
    for (AnmVmListNode *node = &vm->childNode; node != 0; node = node->next)
    {
        if (node->vm->anmScriptIndex == scriptIndex || scriptIndex == -1)
        {
            *outId = node->vm->id;
            return outId;
        }
    }
    *outId = 0;
    return outId;
}

int AnmManager::CleanupVm(AnmVm *vm)
{
    AnmVmListNode *managerNode = &vm->managerNode;

    if (managerNode == worldVmsTail)
    {
        worldVmsTail = vm->managerNode.previous;
    }
    if (managerNode == worldVms)
    {
        worldVms = vm->managerNode.next;
    }
    if (managerNode == uiVmsTail)
    {
        uiVmsTail = vm->managerNode.previous;
    }
    if (managerNode == uiVms)
    {
        uiVms = vm->managerNode.next;
    }

    if (vm->onDestroyCallback != 0)
    {
        vm->onDestroyCallback(vm);
    }

    if (managerNode->next != 0)
    {
        managerNode->next->previous = managerNode->previous;
    }
    if (managerNode->previous != 0)
    {
        managerNode->previous->next = managerNode->next;
    }
    managerNode->next = 0;
    managerNode->previous = 0;

    if (vm->childNode.previous == 0)
    {
        for (AnmVmListNode *childNode = vm->childNode.next;
             childNode != 0;
             childNode = childNode->next)
        {
            childNode->vm->parentVm = 0;
            childNode->vm->flags |= 0x20000000U;
        }
    }

    if (vm->childNode.next != 0)
    {
        vm->childNode.next->previous = vm->childNode.previous;
    }
    if (vm->childNode.previous != 0)
    {
        vm->childNode.previous->next = vm->childNode.next;
    }
    vm->childNode.next = 0;
    vm->childNode.previous = 0;

    if (vm >= &vmPool[0] && vm < &vmPool[0x1000])
    {
        vmPoolUsed[vm - &vmPool[0]] = 0;
        if (vm->extraData != 0)
        {
            free(vm->extraData);
        }
        vm->extraData = 0;
        return 0;
    }

    if (vm->extraData != 0)
    {
        free(vm->extraData);
    }
    vm->extraData = 0;
    delete vm;
    return 0;
}

int AnmManager::UpdateWorldVms()
{
    g_Supervisor.EnterCriticalSection(9);

    AnmVm *tails[31];
    for (int i = 0; i < 31; ++i)
    {
        tails[i] = &renderListHeads[i];
        tails[i]->drawListNext = 0;
    }

    for (AnmVmListNode *node = worldVms; node != 0;)
    {
        AnmVm *vm = node->vm;
        AnmVmListNode *next = node->next;

        if ((vm->flags & 0x20000000U) != 0)
        {
            CleanupVm(vm);
        }
        else if (vm->extraDataUpdateCallback != 0 &&
                 vm->extraDataUpdateCallback(vm) != 0)
        {
            CleanupVm(vm);
        }
        else if (vm->Update() != 0)
        {
            CleanupVm(vm);
        }
        else
        {
            tails[vm->scriptState]->drawListNext = vm;
            tails[vm->scriptState] = vm;
            vm->drawListNext = 0;
        }

        ++updateCount;
        node = next;
    }

    g_Supervisor.LeaveCriticalSection(9);
    return 1;
}

int AnmManager::UpdateUiVms()
{
    g_Supervisor.EnterCriticalSection(9);

    AnmVm *tails[2];
    for (int i = 0; i < 2; ++i)
    {
        tails[i] = &renderListHeads[31 + i];
        renderListHeads[31 + i].drawListNext = 0;
    }
    updateCount = 0;

    for (AnmVmListNode *node = uiVms; node != 0;)
    {
        AnmVm *vm = node->vm;
        AnmVmListNode *next = node->next;

        if ((vm->flags & 0x20000000U) != 0)
        {
            CleanupVm(vm);
        }
        else
        {
            if (vm->extraDataUpdateCallback != 0)
            {
                vm->extraDataUpdateCallback(vm);
            }

            if (vm->Update() != 0)
            {
                CleanupVm(vm);
            }
            else
            {
                if (vm->scriptState != 31 && vm->scriptState != 32)
                {
                    vm->scriptState = 31;
                }
                tails[vm->scriptState - 31]->drawListNext = vm;
                tails[vm->scriptState - 31] = vm;
                vm->drawListNext = 0;
            }
        }

        ++updateCount;
        node = next;
    }

    g_Supervisor.LeaveCriticalSection(9);
    return 1;
}

int AnmManager::DrawVmGroup(unsigned int group)
{
    g_Supervisor.EnterCriticalSection(9);

    for (AnmVm *vm = renderListHeads[group].drawListNext;
         vm != 0;
         vm = vm->drawListNext)
    {
        if ((vm->flags & 0x20000000U) == 0)
        {
            if (vm->extraDataDrawCallback != 0)
            {
                vm->extraDataDrawCallback(vm);
            }
            DrawAnmVmWithManager(vm, this);
        }
    }

    g_Supervisor.LeaveCriticalSection(9);
    return 1;
}

int AnmManager::OnTickWorldCallback()
{
    AnmPauseStateView *pause = g_AnmPauseState;
    if (pause != 0 &&
        ((pause->flags | (pause->flags >> 2)) & 1U) != 0 &&
        (pause->flags & 2U) != 0)
    {
        return 1;
    }
    return UpdateWorldVms();
}

int AnmManager::OnTickUiCallback() { return UpdateUiVms(); }
int AnmManager::DrawLayer0() { return DrawVmGroup(0); }
int AnmManager::DrawLayer1() { return DrawVmGroup(1); }
int AnmManager::DrawLayer2() { return DrawVmGroup(2); }
int AnmManager::DrawLayer3() { return DrawVmGroup(3); }
int AnmManager::DrawLayer5() { return DrawVmGroup(5); }
int AnmManager::DrawLayer6() { return DrawVmGroup(6); }
int AnmManager::DrawLayer7() { return DrawVmGroup(7); }
int AnmManager::DrawLayer8() { return DrawVmGroup(8); }
int AnmManager::DrawLayer9() { return DrawVmGroup(9); }
int AnmManager::DrawLayer10() { return DrawVmGroup(10); }
int AnmManager::DrawLayer11() { return DrawVmGroup(11); }
int AnmManager::DrawLayer12() { return DrawVmGroup(12); }
int AnmManager::DrawLayer13() { return DrawVmGroup(13); }
int AnmManager::DrawLayer14() { return DrawVmGroup(14); }
int AnmManager::DrawLayer15() { return DrawVmGroup(15); }
int AnmManager::DrawLayer16() { return DrawVmGroup(16); }
int AnmManager::DrawLayer17() { return DrawVmGroup(17); }
int AnmManager::DrawLayer18() { return DrawVmGroup(18); }
int AnmManager::DrawLayer19() { return DrawVmGroup(19); }
int AnmManager::DrawLayer22() { return DrawVmGroup(22); }
int AnmManager::DrawLayer23() { return DrawVmGroup(23); }
int AnmManager::DrawLayer25() { return DrawVmGroup(25); }
int AnmManager::DrawLayer26() { return DrawVmGroup(26); }

int AnmManager::AreAllAnmsLoaded()
{
    for (unsigned int i = 0; i < 38; ++i)
    {
        AnmResource *res = loadedAnms[i];
        if (res != 0)
        {
            if (res->releaseRequested != 0 || res->finalizeCursor != 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void AnmManager::ReleaseAnm(unsigned int slot)
{
    if (slot < 38 && loadedAnms[slot] != 0)
    {
        loadedAnms[slot]->Release();
        delete loadedAnms[slot];
        loadedAnms[slot] = 0;
    }
}

bool AnmManager::IsPoolVm(AnmVm *vm)
{
    return vm >= &vmPool[0] && vm < &vmPool[0x1000];
}

AnmResource *AnmManager::PreloadAnm(int slot, const char *filename)
{
    AnmManager *manager = g_AnmManager;
    if (manager->loadedAnms[slot] != 0)
    {
        ChainDebugLog("::preloadAnim already : %s\n", filename);
        return manager->loadedAnms[slot];
    }

    AnmResource *res = manager->LoadAnm(slot, filename);
    if (res == 0)
    {
        return 0;
    }

    res->finalizeCursor = 1;
    do
    {
        if ((g_Supervisor.flags & 0x180U) != 0)
        {
            break;
        }
        Sleep(1);
    } while (res->finalizeCursor != 0);

    ChainDebugLog("::preloadAnimEnd : %s\n", filename);
    return res;
}

void AnmManager::FlushSpriteBatch()
{
    if (spriteBatchCount == 0)
    {
        return;
    }

    if (g_AnmManager->spriteStageMode != 1)
    {
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        g_AnmManager->spriteStageMode = 1;
    }

    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    g_Supervisor.d3dDevice->SetFVF(0x144);
    g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, spriteBatchCount * 2, spriteVertexBufferStart, 0x1C);

    ++drawCallCount;
    spriteVertexBufferStart = spriteVertexBufferBase;
    spriteBatchCount = 0;
}
