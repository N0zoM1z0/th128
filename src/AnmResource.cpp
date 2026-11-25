#include "AnmResource.hpp"

#include "AnmManager.hpp"
#include "Supervisor.hpp"

#include <stdlib.h>
#include <string.h>

struct AnmD3DTextureView
{
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
};

void AnmTextureEntry::Release()
{
    if (texture != 0)
    {
        reinterpret_cast<AnmD3DTextureView *>(texture)->Release();
        texture = 0;
    }
    if (rawData != 0)
    {
        free(rawData);
        rawData = 0;
    }
}

AnmResource::AnmResource()
{
    memset(this, 0, sizeof(AnmResource));
}

void AnmResource::Release()
{
    if (anmFileHeader == 0)
    {
        return;
    }

    g_AnmManager->MarkResourceVmsForDelete(this);

    for (int i = 0; i < textureCount; ++i)
    {
        AnmTextureEntry *entry = &textures[i];
        if (entry->texture != 0)
        {
            reinterpret_cast<AnmD3DTextureView *>(entry->texture)->Release();
            entry->texture = 0;
        }
        if (entry->rawData != 0)
        {
            free(entry->rawData);
            entry->rawData = 0;
        }
    }

    if (textures != 0)
    {
        free(textures);
        textures = 0;
    }
    if (anmFiles != 0)
    {
        free(anmFiles);
        anmFiles = 0;
    }
    if (scriptOffsets != 0)
    {
        free((void *)scriptOffsets);
        scriptOffsets = 0;
    }
    if (extraData134 != 0)
    {
        free(extraData134);
        extraData134 = 0;
    }
    if (anmFileHeader != 0)
    {
        free(anmFileHeader);
        anmFileHeader = 0;
    }
}

unsigned int *AnmResource::CreateVm(
    const ZunVec3 *position,
    unsigned int *outId,
    int scriptIndex,
    int initialState,
    unsigned int flags)
{
    g_Supervisor.EnterCriticalSection(9);
    ++activeVmCount;

    AnmVm *vm = AllocateAnmVm();
    if (initialState >= 0)
    {
        vm->scriptState = initialState;
    }
    vm->stateFlags |= 2U;

    if (position == 0 && (flags & 8U) == 0)
    {
        ZunVec3 zero;
        zero.x = 0.0f;
        zero.y = 0.0f;
        zero.z = 0.0f;
        vm->position = zero;
    }
    else if ((flags & 1U) != 0)
    {
        vm->position.x = (float)(position->x + 128.0 + 192.0);
        vm->position.y = (float)(position->y + 16.0);
        vm->position.z = position->z;
    }
    else
    {
        vm->position = *position;
    }

    if ((flags & 8U) == 0)
    {
        BindVmScript(vm, scriptIndex);
    }
    else
    {
        BindVmAtPosition(vm, scriptIndex, position);
    }

    unsigned int id;
    if ((flags & 4U) != 0 && (flags & 2U) != 0)
    {
        unsigned int tmp;
        id = *vm->RegisterUiVmAtHead(&tmp);
        vm->stateFlags &= ~0xC0U;
    }
    else if ((flags & 4U) != 0)
    {
        unsigned int tmp;
        id = *vm->RegisterUiVmAtTail(&tmp);
        vm->stateFlags &= ~0xC0U;
    }
    else if ((flags & 2U) != 0)
    {
        unsigned int tmp;
        id = *vm->RegisterWorldVmAtHead(&tmp);
    }
    else
    {
        unsigned int tmp;
        id = *vm->RegisterWorldVmAtTail(&tmp);
    }

    *outId = id;
    g_Supervisor.LeaveCriticalSection(9);
    return outId;
}
