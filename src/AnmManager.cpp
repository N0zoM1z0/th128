#include "AnmManager.hpp"

#include <new>

static unsigned int AssignVmId(AnmManager *manager, AnmVm *vm)
{
    ++manager->nextVmId;
    if (manager->nextVmId == 0)
    {
        ++manager->nextVmId;
    }
    vm->id = manager->nextVmId;
    return vm->id;
}

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

unsigned int AnmManager::RegisterWorldVmAtTail(AnmVm *vm)
{
    AnmVmListNode *node = &vm->managerNode;
    node->vm = vm;
    node->next = 0;
    node->previous = worldVmsTail;

    if (worldVmsTail != 0)
    {
        worldVmsTail->next = node;
    }
    else
    {
        worldVms = node;
    }
    worldVmsTail = node;

    return AssignVmId(this, vm);
}

unsigned int AnmManager::RegisterWorldVmAtHead(AnmVm *vm)
{
    AnmVmListNode *node = &vm->managerNode;
    node->vm = vm;
    node->previous = 0;
    node->next = worldVms;

    if (worldVms != 0)
    {
        worldVms->previous = node;
    }
    else
    {
        worldVmsTail = node;
    }
    worldVms = node;

    return AssignVmId(this, vm);
}

unsigned int AnmManager::RegisterUiVmAtTail(AnmVm *vm)
{
    AnmVmListNode *node = &vm->managerNode;
    node->vm = vm;
    node->next = 0;
    node->previous = uiVmsTail;

    if (uiVmsTail != 0)
    {
        uiVmsTail->next = node;
    }
    else
    {
        uiVms = node;
    }
    uiVmsTail = node;

    return AssignVmId(this, vm);
}

unsigned int AnmManager::RegisterUiVmAtHead(AnmVm *vm)
{
    AnmVmListNode *node = &vm->managerNode;
    node->vm = vm;
    node->previous = 0;
    node->next = uiVms;

    if (uiVms != 0)
    {
        uiVms->previous = node;
    }
    else
    {
        uiVmsTail = node;
    }
    uiVms = node;

    return AssignVmId(this, vm);
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
