#include "AnmHelpers.hpp"

#include <string.h>

extern int __stdcall DrawAnmVmWithManager(AnmVm *vm, AnmManager *manager);
extern int __stdcall BindAnmFileKeepState(AnmVm *vm, AnmResource *resource, int scriptIndex);

namespace AnmHelpers
{
static inline AnmVm *ResolveVmIdPtrInline(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm == 0)
    {
        id->id = 0;
    }
    return vm;
}

static inline AnmVm *FindChildVmInline(AnmVm *vm, int scriptIndex)
{
    for (AnmVmListNode *node = &vm->childNode; node != 0; node = node->next)
    {
        if (node->vm->anmScriptIndex == scriptIndex || (scriptIndex == -1 && node->vm != vm))
        {
            return node->vm;
        }
    }
    return 0;
}
void __stdcall SetVmGamePosition(AnmVm *vm, const ZunVec3 *pos)
{
    vm->position.x = (float)(pos->x + 128.0 + 192.0);
    vm->position.y = (float)(pos->y + 16.0);
    vm->position.z = pos->z;
}

void __stdcall SetVmGamePosition2(AnmVm *vm, const ZunVec3 *pos)
{
    vm->position.x = (float)(pos->x + 128.0 + 192.0);
    vm->position.y = (float)(pos->y + 16.0);
    vm->position.z = pos->z;
}

void __stdcall DrawSingleVm(AnmVm *vm)
{
    DrawAnmVmWithManager(vm, g_AnmManager);
}

void __stdcall SetVmRotX(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown024 = v;
}

void __stdcall SetVmRotY(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown028 = v;
}

void __stdcall SetVmRotZ(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown02C = v;
}

void __stdcall SetVmRotY2(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown028 = v;
}

void __stdcall SetVmRotX2(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown024 = v;
}

void __stdcall SetVmRotZ2(AnmVm *vm, float v)
{
    vm->flags |= 4u;
    vm->unknown02C = v;
}

unsigned int *__stdcall FindChildIdFromVm(unsigned int *outId, AnmVm *vm, int scriptIndex)
{
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

AnmVm *__stdcall FindChildVmFromVm(AnmVm *vm, int scriptIndex)
{
    for (AnmVmListNode *node = &vm->childNode; node != 0; node = node->next)
    {
        if (node->vm->anmScriptIndex == scriptIndex || (scriptIndex == -1 && node->vm != vm))
        {
            return node->vm;
        }
    }
    return 0;
}

void __stdcall ClearD3DTexture(AnmD3DTextureSurfaceView **tex)
{
    AnmD3DSurfaceView *surface;
    (*tex)->GetSurfaceLevel(0, &surface);
    AnmD3DSurfaceDesc desc;
    surface->GetDesc(&desc);
    AnmD3DLockedRect locked;
    surface->LockRect(&locked, 0, 0);
    memset(locked.pBits, 0, desc.Height * locked.Pitch);
    surface->UnlockRect();
    if (surface != 0)
    {
        surface->Release();
    }
}

void __stdcall ClearD3DTextureBlack(AnmD3DTextureSurfaceView **tex)
{
    AnmD3DSurfaceView *surface;
    (*tex)->GetSurfaceLevel(0, &surface);
    AnmD3DSurfaceDesc desc;
    surface->GetDesc(&desc);
    AnmD3DLockedRect locked;
    surface->LockRect(&locked, 0, 0);
    switch (desc.Format)
    {
    case 20:
    case 23:
        memset(locked.pBits, 0, desc.Height * locked.Pitch);
        break;
    case 21:
    case 22:
    {
        unsigned char *p = (unsigned char *)locked.pBits;
        for (unsigned int i = 0; i < desc.Width * desc.Height; ++i)
        {
            p[3] = 0xFF;
            p[2] = 0;
            p[1] = 0;
            p[0] = 0;
            p += 4;
        }
        break;
    }
    case 26:
    {
        unsigned short *p = (unsigned short *)locked.pBits;
        for (unsigned int i = 0; i < desc.Width * desc.Height; ++i)
        {
            *p++ = 0xF000;
        }
        break;
    }
    default:
        break;
    }
    surface->UnlockRect();
    if (surface != 0)
    {
        surface->Release();
    }
}

void __stdcall MarkVmTreeForDeleteById(AnmId id)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm != 0)
    {
        vm->flags |= 0x20000000u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags |= 0x20000000u;
            }
        }
    }
}

void __stdcall SetVmPosById(AnmId id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm != 0)
    {
        vm->position = *pos;
    }
}

void __stdcall SetVmPosById2(AnmId id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm != 0)
    {
        vm->position = *pos;
    }
}

void __stdcall SetVmRotZById(AnmId id, float z)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm != 0)
    {
        vm->flags |= 4u;
        vm->unknown02C = z;
    }
}

void __stdcall SetVmGamePosById(AnmId id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(id);
    if (vm != 0)
    {
        vm->position.x = (float)(pos->x + 128.0 + 192.0);
        vm->position.y = (float)(pos->y + 16.0);
        vm->position.z = pos->z;
    }
}

ZunVec3 *__stdcall GetVmPosById(AnmManager *mgr, AnmId id)
{
    AnmVm *vm = mgr->GetVmById(id);
    if (vm != 0)
    {
        return &vm->position;
    }
    return 0;
}

AnmVm *__stdcall ResolveVmIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm == 0)
    {
        id->id = 0;
    }
    return vm;
}

void __stdcall StopVmByIdPtr(AnmId *id, short intr)
{
    g_AnmManager->StopVmById(*id, intr);
}

void __stdcall SetInterruptAndUpdateByIdPtr(AnmId *id, short intr)
{
    g_AnmManager->SetInterruptAndUpdate(*id, intr);
}

void __stdcall HideVmTreeByIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags = (vm->flags & ~2u) | 0x80000u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags &= ~2u;
                node->vm->flags |= 0x80000u;
            }
        }
    }
}

void __stdcall ShowVmTreeByIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags = (vm->flags & ~0x80000u) | 2u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags |= 2u;
                node->vm->flags &= ~0x80000u;
            }
        }
    }
}

void __stdcall SetVisibleBitByIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags |= 2u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags |= 2u;
            }
        }
    }
}

void __stdcall ClearVisibleBitByIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags &= ~2u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags &= ~2u;
            }
        }
    }
}

void __stdcall DeleteVmByIdPtr(AnmId *id)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags |= 0x20000000u;
        if (vm->childNode.previous == 0)
        {
            for (AnmVmListNode *node = vm->childNode.next; node != 0; node = node->next)
            {
                node->vm->flags |= 0x20000000u;
            }
        }
    }
    id->id = 0;
}

void __stdcall SetVmPosByIdPtr(AnmId *id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->position = *pos;
    }
}

void __stdcall SetVmPosByIdPtr2(AnmId *id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->position = *pos;
    }
}

void __stdcall SetVmRotZByIdPtr(AnmId *id, float z)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->flags |= 4u;
        vm->unknown02C = z;
    }
}

void __stdcall SetVmGamePosByIdPtr(AnmId *id, const ZunVec3 *pos)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->position.x = (float)(pos->x + 128.0 + 192.0);
        vm->position.y = (float)(pos->y + 16.0);
        vm->position.z = pos->z;
    }
}

void __stdcall BindVmScriptByIdPtr(AnmId *id, int scriptIndex)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->BindAnmFile(vm->resource, scriptIndex);
    }
}

void __stdcall BindVmOwnResource(AnmVm *vm, int scriptIndex)
{
    if (vm->resource != 0)
    {
        vm->BindAnmFile(vm->resource, scriptIndex);
    }
}

void __stdcall BindVmResource(AnmVm *vm, AnmResource *res, int scriptIndex)
{
    vm->BindAnmFile(res, scriptIndex);
}

void __stdcall BindVmResource2(AnmVm *vm, AnmResource *res, int scriptIndex)
{
    vm->BindAnmFile(res, scriptIndex);
}

void __stdcall BindVmOwnResourceKeepState(AnmVm *vm, int scriptIndex)
{
    if (vm->resource != 0)
    {
        BindAnmFileKeepState(vm, vm->resource, scriptIndex);
    }
}

void __stdcall BindVmResourceKeepState(AnmVm *vm, AnmResource *res, int scriptIndex)
{
    BindAnmFileKeepState(vm, res, scriptIndex);
}

void __stdcall BindVmResourceByIdPtr(AnmId *id, AnmResource *res, int scriptIndex)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        vm->BindAnmFile(res, scriptIndex);
    }
}

void __stdcall BindVmKeepStateByIdPtr(AnmId *id, int scriptIndex)
{
    AnmVm *vm = g_AnmManager->GetVmById(*id);
    if (vm != 0)
    {
        BindAnmFileKeepState(vm, vm->resource, scriptIndex);
    }
}

unsigned int *__stdcall FindChildIdByIdPtr(unsigned int *outId, AnmId *rootId, int scriptIndex)
{
    AnmVm *vm = ResolveVmIdPtrInline(rootId);
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

AnmVm *__stdcall FindChildVmByIdPtr(AnmId *rootId, int scriptIndex)
{
    AnmVm *vm = ResolveVmIdPtrInline(rootId);
    return FindChildVmInline(vm, scriptIndex);
}

void __stdcall SetChildVisibleByScript(AnmId *rootId, int scriptIndex)
{
    AnmVm *vm = ResolveVmIdPtrInline(rootId);
    AnmVm *child = FindChildVmInline(vm, scriptIndex);
    child->flags |= 2u;
}

void __stdcall ClearChildVisibleByScript(AnmId *rootId, int scriptIndex)
{
    AnmVm *vm = ResolveVmIdPtrInline(rootId);
    AnmVm *child = FindChildVmInline(vm, scriptIndex);
    child->flags &= ~2u;
}

void __stdcall SetVmColorByIdPtr(AnmId *id, unsigned int color)
{
    AnmVm *vm = ResolveVmIdPtrInline(id);
    vm->color = color;
}

bool __stdcall AnmVmIdEquals(const AnmId *a, const AnmId *b)
{
    return a->id == b->id;
}

unsigned int __stdcall NextVmId(AnmId *counter)
{
    unsigned int prev = counter->id;
    counter->id = prev + 1u;
    return prev;
}
}
