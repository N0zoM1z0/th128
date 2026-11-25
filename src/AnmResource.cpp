#include "AnmResource.hpp"

#include "AnmManager.hpp"

#include <string.h>
#include <windows.h>

class AnmVmCreationLock
{
public:
    AnmVmCreationLock()
    {
        flags = reinterpret_cast<volatile unsigned int *>(0x004D33F8);
        depth = reinterpret_cast<volatile unsigned char *>(0x004D37A1);
        criticalSection = reinterpret_cast<CRITICAL_SECTION *>(0x004D3750);
        locked = ((*flags & 0x8000) != 0);
        if (locked)
        {
            EnterCriticalSection(criticalSection);
            ++(*depth);
        }
    }

    ~AnmVmCreationLock()
    {
        if (locked)
        {
            LeaveCriticalSection(criticalSection);
            --(*depth);
        }
    }

private:
    volatile unsigned int *flags;
    volatile unsigned char *depth;
    CRITICAL_SECTION *criticalSection;
    bool locked;
};

unsigned int *AnmResource::CreateVm(
    const ZunVec3 *position,
    unsigned int *outId,
    int scriptIndex,
    int initialState,
    unsigned int flags)
{
    AnmVmCreationLock lock;
    ++activeVmCount;

    AnmVm *vm = AllocateAnmVm();
    if (vm == 0)
    {
        *outId = 0;
        return outId;
    }

    if (initialState >= 0)
    {
        vm->scriptState = initialState;
    }
    vm->stateFlags |= 2U;

    if (position == 0 && (flags & 8U) == 0)
    {
        vm->position.x = 0.0f;
        vm->position.y = 0.0f;
        vm->position.z = 0.0f;
    }
    else if ((flags & 1U) == 0)
    {
        vm->position = *position;
    }
    else
    {
        vm->position.x = position->x + 320.0f;
        vm->position.y = position->y + 16.0f;
        vm->position.z = position->z;
    }

    if ((flags & 8U) == 0)
    {
        BindVmScript(vm, scriptIndex);
    }
    else
    {
        BindVmAtPosition(vm, scriptIndex, position);
    }

    if ((flags & 4U) == 0)
    {
        if ((flags & 2U) == 0)
            vm->RegisterWorldVmAtTail(outId);
        else
            vm->RegisterWorldVmAtHead(outId);
    }
    else
    {
        if ((flags & 2U) == 0)
            vm->RegisterUiVmAtTail(outId);
        else
            vm->RegisterUiVmAtHead(outId);
        vm->stateFlags &= ~0xC0U;
    }

    return outId;
}
