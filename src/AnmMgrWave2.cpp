#include "AnmMgrWave2.hpp"

namespace th128 {

static inline void LockAnm() {
    if ((g_Amw2SupervisorFlags & 0x8000u) != 0) {
        EnterCriticalSection(&g_Amw2CritSec);
        ++g_Amw2CritSecDepth;
    }
}

static inline void UnlockAnm() {
    if ((g_Amw2SupervisorFlags & 0x8000u) != 0) {
        LeaveCriticalSection(&g_Amw2CritSec);
        --g_Amw2CritSecDepth;
    }
}

int Amw2AnmManager::DrawLayer4() {
    g_Amw2CurrentViewport = &g_Amw2Viewport2;
    g_Amw2Viewport2.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 2;
    if (g_Amw2FogEnabled != 0) {
        g_Amw2AnmManager->FlushAnmSpriteBatch();
        g_Amw2D3dDevice->SetRenderState(0x1c, g_Amw2FogEnabled = 0);
    }
    return this->DrawVmGroup(4);
}

int Amw2AnmManager::DrawLayer21() {
    g_Amw2CurrentViewport = &g_Amw2Viewport1;
    g_Amw2Viewport1.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 1;
    g_Amw2AnmManager->FlushAnmSpriteBatch();
    g_Amw2D3dDevice->SetRenderState(0x0e, 0);
    g_Amw2AnmManager->FlushAnmSpriteBatch();
    g_Amw2D3dDevice->SetRenderState(0x17, 8);
    return this->DrawVmGroup(0x15);
}

int Amw2AnmManager::DrawLayer24() {
    g_Amw2CurrentViewport = &g_Amw2Viewport0;
    g_Amw2Viewport0.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 0;
    int result = this->DrawVmGroup(0x18);
    g_Amw2CurrentViewport = &g_Amw2Viewport1;
    g_Amw2Viewport1.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 1;
    return result;
}

int Amw2AnmManager::DrawLayer32() {
    g_Amw2CurrentViewport = &g_Amw2Viewport0;
    g_Amw2Viewport0.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 0;
    int result = this->DrawVmGroup(0x20);
    g_Amw2CurrentViewport = &g_Amw2Viewport1;
    g_Amw2Viewport1.Set2DView();
    g_Amw2D3dDevice->SetViewport(&g_Amw2CurrentViewport->viewport);
    g_Amw2CurrentViewportId = 1;
    return result;
}

int Amw2AnmManager::UpdateWorldVms() {
    LockAnm();
    Amw2AnmVm *tails[31];
    for (int i = 0; i < 31; ++i) {
        tails[i] = &this->layerHeads[i];
        this->layerHeads[i].nextDraw = 0;
    }

    Amw2ListNode *node = this->worldListHead;
    while (node != 0) {
        Amw2AnmVm *vm = node->entry;
        Amw2ListNode *next = node->next;
        if ((vm->flags & 0x20000000u) != 0) {
            this->CleanupVm(vm);
        } else if (vm->onTick != 0 && vm->onTick(vm) != 0) {
            this->CleanupVm(vm);
        } else if (vm->Update() != 0) {
            this->CleanupVm(vm);
        } else {
            tails[vm->layer]->nextDraw = vm;
            tails[vm->layer] = vm;
            vm->nextDraw = 0;
            ++this->activeVmCount;
        }
        node = next;
    }

    UnlockAnm();
    return 1;
}

int Amw2AnmManager::DrawVmGroup(int layer) {
    LockAnm();
    Amw2AnmVm *vm = this->layerHeads[layer].nextDraw;
    while (vm != 0) {
        if ((vm->flags & 0x20000000u) == 0) {
            if (vm->onDraw != 0) {
                vm->onDraw(vm);
            }
            vm->Draw(this);
        }
        vm = vm->nextDraw;
    }
    UnlockAnm();
    return 1;
}

Amw2VmId Amw2AnmVm::RegisterWorldVmAtTail() {
    Amw2AnmManager *mgr = g_Amw2AnmManager;
    Amw2ListNode *node = &this->worldNode;
    node->entry = this;
    node->next = 0;
    node->prev = 0;
    if (mgr->worldListHead == 0) {
        mgr->worldListHead = node;
    } else {
        Amw2ListNode *tail = mgr->worldListTail;
        if (tail->next != 0) {
            node->next = tail->next;
            tail->next->prev = node;
        }
        tail->next = node;
        node->prev = tail;
    }
    mgr->worldListTail = node;
    mgr->nextVmId += 1;
    if (mgr->nextVmId == 0) {
        mgr->nextVmId += 1;
    }
    this->id.value = mgr->nextVmId;
    return Amw2VmId(mgr->nextVmId);
}

Amw2VmId Amw2AnmVm::RegisterUiVmAtTail() {
    Amw2AnmManager *mgr = g_Amw2AnmManager;
    Amw2ListNode *node = &this->worldNode;
    node->entry = this;
    node->next = 0;
    node->prev = 0;
    if (mgr->uiListHead == 0) {
        mgr->uiListHead = node;
    } else {
        Amw2ListNode *tail = mgr->uiListTail;
        if (tail->next != 0) {
            node->next = tail->next;
            tail->next->prev = node;
        }
        tail->next = node;
        node->prev = tail;
    }
    mgr->uiListTail = node;
    mgr->nextVmId += 1;
    if (mgr->nextVmId == 0) {
        mgr->nextVmId += 1;
    }
    this->id.value = mgr->nextVmId;
    return Amw2VmId(mgr->nextVmId);
}

Amw2VmId Amw2AnmResource::CreateVm(const Amw2Vec3 *pos, int scriptId, int layer, unsigned int flags) {
    LockAnm();
    ++this->refCount;
    Amw2AnmVm *vm = AllocateAnmVm();
    if (layer >= 0) {
        vm->layer = layer;
    }
    vm->flags2 |= 2u;
    if (pos == 0 && (flags & 8u) == 0) {
        vm->pos = Amw2Vec3(0.0f, 0.0f, 0.0f);
    } else if ((flags & 1u) != 0) {
        vm->pos.x = (float)((double)pos->x + 128.0 + 192.0);
        vm->pos.y = (float)((double)pos->y + 16.0);
        vm->pos.z = pos->z;
    } else {
        vm->pos = *pos;
    }

    if ((flags & 8u) == 0) {
        this->BindVmScript(vm, scriptId);
    } else {
        this->BindVmAtPosition(vm, scriptId);
    }

    Amw2VmId id;
    if ((flags & 4u) != 0 && (flags & 2u) != 0) {
        id = vm->RegisterUiVmAtHead();
        vm->flags2 &= ~0xc0u;
    } else if ((flags & 4u) != 0) {
        id = vm->RegisterUiVmAtTail();
        vm->flags2 &= ~0xc0u;
    } else if ((flags & 2u) != 0) {
        id = vm->RegisterWorldVmAtHead();
    } else {
        id = vm->RegisterWorldVmAtTail();
    }

    UnlockAnm();
    return id;
}

Amw2VmId Amw2AnmResource::SpawnChildVm(int scriptId, Amw2AnmVm *parent, unsigned int flags) {
    LockAnm();
    ++this->refCount;
    Amw2AnmVm *vm = AllocateAnmVm();
    vm->pos = Amw2Vec3(0.0f, 0.0f, 0.0f);
    vm->layer = parent->layer;
    vm->flags2 |= 2u;
    this->BindVmScript(vm, scriptId);
    vm->parent = parent;

    Amw2VmId id;
    if ((flags & 4u) != 0 && (flags & 2u) != 0) {
        id = vm->RegisterUiVmAtHead();
    } else if ((flags & 4u) != 0) {
        id = vm->RegisterUiVmAtTail();
    } else if ((flags & 2u) != 0) {
        id = vm->RegisterWorldVmAtHead();
    } else {
        id = vm->RegisterWorldVmAtTail();
    }

    Amw2ListNode *parentNode = &parent->childNode;
    Amw2ListNode *childNode = &vm->childNode;
    if (parentNode->next != 0) {
        childNode->next = parentNode->next;
        parentNode->next->prev = childNode;
    }
    parentNode->next = childNode;
    childNode->prev = parentNode;

    UnlockAnm();
    return id;
}

void SetInterruptAndUpdate(Amw2VmId id, short interrupt) {
    Amw2AnmVm *vm = g_Amw2AnmManager->GetVmById(id);
    if (vm == 0) {
        return;
    }
    if (vm->onInterrupt != 0) {
        vm->onInterrupt(vm, interrupt);
    }
    vm->pendingInterrupt = interrupt;
    vm->Update();
    if (vm->childNode.prev == 0) {
        for (Amw2ListNode *n = vm->childNode.next; n != 0; n = n->next) {
            Amw2AnmVm *child = n->entry;
            if (child->onInterrupt != 0) {
                child->onInterrupt(child, interrupt);
            }
            child->pendingInterrupt = interrupt;
            n->entry->Update();
        }
    }
}

} // namespace th128
