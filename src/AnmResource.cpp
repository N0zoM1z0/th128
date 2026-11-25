#include "AnmResource.hpp"

#include "AnmManager.hpp"

#include <string.h>
#include <windows.h>

// The original dispatches into the large interpreter/draw routine at
// 0x00459D90. Until its renderer is reconstructed, binding runs the recovered
// command-execution subset and leaves unsupported commands pending.
static float *AnmGameSpeed()
{
    return reinterpret_cast<float *>(0x004B6F58);
}

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

static void InitializeVmTimer(AnmVm *vm)
{
    if (!vm->timer070.initialized)
    {
        vm->timer070.previous = -999999;
        vm->timer070.current = 0;
        vm->timer070.currentF = 0.0f;
        vm->timer070.gameSpeed = AnmGameSpeed();
        vm->timer070.initialized = 1;
    }

    vm->timer070.currentF = 0.0f;
    vm->timer070.current = 0;
    vm->timer070.previous = -1;
    vm->flags &= ~1U;
}

void AnmResource::BindVmScript(AnmVm *vm, int scriptIndex)
{
    const unsigned int scriptData = scriptOffsets[scriptIndex];
    if (scriptData == 0 || isInvalid != 0)
    {
        memset(vm, 0, sizeof(AnmVm));
        return;
    }

    vm->ResetForReuse();
    vm->anmScriptIndex = (short)scriptIndex;
    vm->anmFileId = (short)fileId;
    vm->flags &= ~0xC00U;
    vm->resource = this;
    vm->stateFlags = (vm->stateFlags & ~0x40U) | 0x80U;
    vm->scriptDataOffset = scriptData;
    vm->scriptDataOffsetCopy = scriptData;

    InitializeVmTimer(vm);
    vm->Update();
    ++GetGlobalAnmManager()->activeVmCount;

    vm->scale2X = 1.0f;
    vm->scale2Y = 1.0f;
    if ((vm->stateFlags & 0xC0U) == 0x80U)
    {
        vm->stateFlags = (vm->stateFlags & ~0x80U) | 0x40U;
    }
}

void AnmResource::BindVmAtPosition(
    AnmVm *vm,
    int scriptIndex,
    const ZunVec3 *position)
{
    const unsigned int scriptData = scriptOffsets[scriptIndex];
    if (scriptData == 0 || isInvalid != 0)
    {
        memset(vm, 0, sizeof(AnmVm));
        return;
    }

    vm->ResetForReuse();
    vm->secondaryPosition = *position;
    vm->anmScriptIndex = (short)scriptIndex;
    vm->anmFileId = (short)fileId;
    vm->flags &= ~0xC00U;
    vm->resource = this;
    vm->stateFlags = (vm->stateFlags & ~0x40U) | 0x80U;
    vm->scriptDataOffset = scriptData;
    vm->scriptDataOffsetCopy = scriptData;

    InitializeVmTimer(vm);
    vm->Update();
    ++GetGlobalAnmManager()->activeVmCount;

    if ((vm->stateFlags & 0xC0U) == 0x80U)
    {
        vm->stateFlags = (vm->stateFlags & ~0x80U) | 0x40U;
    }
}

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
