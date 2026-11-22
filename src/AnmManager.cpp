#include "AnmManager.hpp"

#include <stddef.h>

AnmVm *AnmManager::GetVmById(unsigned int id)
{
    if (id == 0)
    {
        return NULL;
    }
    for (AnmVmListNode *node = worldVms; node != NULL; node = node->next)
    {
        if (node->vm->id == id)
        {
            return node->vm;
        }
    }
    for (AnmVmListNode *node = uiVms; node != NULL; node = node->next)
    {
        if (node->vm->id == id)
        {
            return node->vm;
        }
    }
    return NULL;
}
