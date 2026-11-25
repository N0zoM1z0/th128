#include "AnmManager.hpp"

#include <new>
#include <stdlib.h>

int DrawAnmVm(AnmVm *vm);

AnmVm *AnmManager::GetVmById(unsigned int id)
{
    if (id == 0)
    {
        return 0;
    }

    for (AnmVmListNode *node = worldVms; node != 0; node = node->next)
    {
        if (node->vm->id == id)
        {
            return node->vm;
        }
    }

    for (AnmVmListNode *node = uiVms; node != 0; node = node->next)
    {
        if (node->vm->id == id)
        {
            return node->vm;
        }
    }

    return 0;
}

__declspec(noinline) AnmVm *__fastcall GetVmByIdTargetAbi(
    void *unused, AnmManager *manager, volatile unsigned int id)
{
    (void)unused;
    const unsigned int lookupId = id;
    if (lookupId == 0)
    {
        return 0;
    }

    for (AnmVmListNode *node = manager->worldVms;
         node != 0; node = node->next)
    {
        if (node->vm->id == lookupId)
        {
            return node->vm;
        }
    }

    for (AnmVmListNode *node = manager->uiVms;
         node != 0; node = node->next)
    {
        if (node->vm->id == lookupId)
        {
            return node->vm;
        }
    }

    return 0;
}

static int AdvanceVmPoolIndex(int index)
{
    return (index + 1) % 0x1000;
}

AnmVm *AllocateAnmVm()
{
    AnmManager *manager = GetGlobalAnmManager();
    int index = manager->nextVmPoolIndex;
    AnmVm *vm = &manager->vmPool[index];

    if (manager->vmPoolUsed[index] == 0)
    {
        manager->vmPoolUsed[index] = 1;
    }
    else
    {
        index = AdvanceVmPoolIndex(index);
        manager->nextVmPoolIndex = index;
        vm = &manager->vmPool[index];

        if (manager->vmPoolUsed[index] == 0)
        {
            vm->ResetForReuse();
            manager->vmPoolUsed[index] = 1;
        }
        else
        {
            vm = new AnmVm;
            if (vm != 0)
            {
                vm->ResetForReuse();
            }
        }
    }

    manager->nextVmPoolIndex = AdvanceVmPoolIndex(manager->nextVmPoolIndex);
    return vm;
}

void AnmManager::StopVmById(unsigned int id)
{
    AnmVm *vm = GetVmById(id);
    if (vm == 0)
    {
        return;
    }

    if (vm->stopCallback != 0)
    {
        vm->stopCallback(vm, 0);
        vm->stopCallback = 0;
    }
    vm->currentFrame = 0;

    if (vm->childNode.previous == 0)
    {
        for (AnmVmListNode *node = vm->childNode.next;
             node != 0;
             node = node->next)
        {
            AnmVm *child = node->vm;
            if (child->stopCallback != 0)
            {
                child->stopCallback(child, 0);
                child->stopCallback = 0;
            }
            child->currentFrame = 0;
        }
    }
}


namespace
{
    typedef void (__stdcall *AnmCriticalSectionRoutine)(void *);

    class AnmVmUpdateLock
    {
    public:
        AnmVmUpdateLock()
            : locked((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
        {
            if (locked)
            {
                AnmCriticalSectionRoutine enter =
                    *reinterpret_cast<AnmCriticalSectionRoutine *>(0x0049A088);
                enter(reinterpret_cast<void *>(0x004D3750));
                ++*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
            }
        }

        ~AnmVmUpdateLock()
        {
            if (locked)
            {
                AnmCriticalSectionRoutine leave =
                    *reinterpret_cast<AnmCriticalSectionRoutine *>(0x0049A08C);
                leave(reinterpret_cast<void *>(0x004D3750));
                --*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
            }
        }

    private:
        bool locked;
    };

    void AppendVmToRenderList(AnmVm **tails, unsigned int group, AnmVm *vm)
    {
        AnmVm *tail = tails[group];
        tail->drawListNext = vm;
        tails[group] = vm;
        vm->drawListNext = 0;
    }
}

void AnmManager::CleanupVm(AnmVm *vm)
{
    AnmVmListNode *managerNode = &vm->managerNode;

    if (managerNode == worldVmsTail)
    {
        worldVmsTail = managerNode->previous;
    }
    if (managerNode == worldVms)
    {
        worldVms = managerNode->next;
    }
    if (managerNode == uiVmsTail)
    {
        uiVmsTail = managerNode->previous;
    }
    if (managerNode == uiVms)
    {
        uiVms = managerNode->next;
    }

    if (vm->onDestroyCallback != 0)
    {
        vm->onDestroyCallback(vm);
    }

    if (managerNode->previous != 0)
    {
        managerNode->previous->next = managerNode->next;
    }
    if (managerNode->next != 0)
    {
        managerNode->next->previous = managerNode->previous;
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

    if (vm->childNode.previous != 0)
    {
        vm->childNode.previous->next = vm->childNode.next;
    }
    if (vm->childNode.next != 0)
    {
        vm->childNode.next->previous = vm->childNode.previous;
    }
    vm->childNode.next = 0;
    vm->childNode.previous = 0;

    const unsigned int vmAddress = (unsigned int)vm;
    const unsigned int poolStart = (unsigned int)&vmPool[0];
    const unsigned int poolEnd = poolStart + sizeof(vmPool);
    if (vmAddress >= poolStart && vmAddress < poolEnd)
    {
        const unsigned int poolIndex =
            (vmAddress - poolStart) / sizeof(AnmVm);
        vmPoolUsed[poolIndex] = 0;
        free(vm->extraData);
        vm->extraData = 0;
        return;
    }

    free(vm->extraData);
    vm->extraData = 0;
    delete vm;
}

int AnmManager::UpdateWorldVms()
{
    AnmVmUpdateLock updateLock;

    AnmVm *tails[31];
    for (unsigned int i = 0; i != 31; ++i)
    {
        renderListHeads[i].drawListNext = 0;
        tails[i] = &renderListHeads[i];
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
            AppendVmToRenderList(tails, (unsigned int)vm->scriptState, vm);
        }

        ++updateCount;
        node = next;
    }

    return 1;
}

int AnmManager::UpdateUiVms()
{
    AnmVmUpdateLock updateLock;

    renderListHeads[31].drawListNext = 0;
    renderListHeads[32].drawListNext = 0;
    AnmVm *tails[2] = {
        &renderListHeads[31],
        &renderListHeads[32]
    };
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
                AppendVmToRenderList(
                    tails,
                    (unsigned int)(vm->scriptState - 31),
                    vm);
            }
        }

        ++updateCount;
        node = next;
    }

    return 1;
}


int AnmManager::DrawVmGroup(unsigned int group)
{
    if (group >= 33U)
    {
        return 1;
    }

    AnmVmUpdateLock drawLock;
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
            DrawAnmVm(vm);
        }
    }

    return 1;
}
