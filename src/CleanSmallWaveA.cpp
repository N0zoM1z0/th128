#include "CleanSmallWaveA.hpp"

#include <math.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

static __forceinline void CsaInsertAfter(CsaVmNode *target, CsaVmNode *node) {
    if (target->next != NULL) {
        node->next = target->next;
        target->next->prev = node;
    }
    target->next = node;
    node->prev = target;
}

static __forceinline void CsaAllocVmId(CsaAnmManager *mgr, CsaAnmVm *vm, int *outId) {
    ++mgr->nextId;
    if (mgr->nextId == 0) {
        ++mgr->nextId;
    }
    vm->id = mgr->nextId;
    *outId = mgr->nextId;
}

// 1. 0x00466D60 (124B)
void CsaRegisterWorldVmAtTail(CsaAnmVm *vm, int *outId) {
    CsaAnmManager *mgr = g_CsaAnmMgr;
    vm->node.vm = vm;
    vm->node.next = NULL;
    vm->node.prev = NULL;
    if (mgr->worldHead == NULL) {
        mgr->worldHead = &vm->node;
    } else {
        CsaInsertAfter(mgr->worldTail, &vm->node);
    }
    mgr->worldTail = &vm->node;
    CsaAllocVmId(mgr, vm, outId);
}

// 2. 0x00466E60 (124B)
void CsaRegisterUiVmAtTail(CsaAnmVm *vm, int *outId) {
    CsaAnmManager *mgr = g_CsaAnmMgr;
    vm->node.vm = vm;
    vm->node.next = NULL;
    vm->node.prev = NULL;
    if (mgr->uiHead == NULL) {
        mgr->uiHead = &vm->node;
    } else {
        CsaInsertAfter(mgr->uiTail, &vm->node);
    }
    mgr->uiTail = &vm->node;
    CsaAllocVmId(mgr, vm, outId);
}

// 3. 0x00466DE0 (119B)
void CsaRegisterWorldVmAtHead(CsaAnmVm *vm, int *outId) {
    CsaAnmManager *mgr = g_CsaAnmMgr;
    vm->node.vm = vm;
    vm->node.next = NULL;
    vm->node.prev = NULL;
    if (mgr->worldHead == NULL) {
        mgr->worldTail = &vm->node;
    } else {
        CsaInsertAfter(&vm->node, mgr->worldHead);
    }
    mgr->worldHead = &vm->node;
    CsaAllocVmId(mgr, vm, outId);
}

// 4. 0x00466EE0 (119B)
void CsaRegisterUiVmAtHead(CsaAnmVm *vm, int *outId) {
    CsaAnmManager *mgr = g_CsaAnmMgr;
    vm->node.vm = vm;
    vm->node.next = NULL;
    vm->node.prev = NULL;
    if (mgr->uiHead == NULL) {
        mgr->uiTail = &vm->node;
    } else {
        CsaInsertAfter(&vm->node, mgr->uiHead);
    }
    mgr->uiHead = &vm->node;
    CsaAllocVmId(mgr, vm, outId);
}

// 5. 0x00434CB0 (126B)
int CsaStartLoaderThread(unsigned (__stdcall *proc)(void *)) {
    if (g_CsaFlags & 0x8000) {
        EnterCriticalSection(&g_CsaThreadCs);
        ++g_CsaThreadCsDepth;
    }
    g_CsaLoaderThread.Stop();
    g_CsaLoaderThread.proc = (void *)proc;
    g_CsaLoaderThread.f10 = 1;
    g_CsaLoaderThread.f0c = 0;
    g_CsaLoaderThread.handle = (HANDLE)_beginthreadex(NULL, 0, proc, NULL, 0, &g_CsaLoaderThread.threadId);
    if (g_CsaFlags & 0x8000) {
        LeaveCriticalSection(&g_CsaThreadCs);
        --g_CsaThreadCsDepth;
    }
    return 0;
}

// 6. 0x00434970 (123B)
int CsaPlayStageBgm(const char *path, int bgmIdx) {
    char buf[256];
    strcpy(buf, path);
    char *dot = strrchr(buf, '.');
    dot[1] = 'w';
    dot[2] = 'a';
    dot[3] = 'v';
    g_CsaScoreData->bgmUnlocked[bgmIdx] = 1;
    CsaEnqueueBgmCommand(&g_CsaSoundMgr, 2, -1, buf);
    return 0;
}

// 7. 0x00434880 (114B)
int CsaPlayBgmWithLoop(int arg, const char *path) {
    char buf[256];
    strcpy(buf, path);
    char *dot = strrchr(buf, '.');
    dot[1] = 'w';
    dot[2] = 'a';
    dot[3] = 'v';
    CsaEnqueueBgmCommand(&g_CsaSoundMgr, 1, arg, buf);
    return 1;
}

// 8. 0x00450FE0 (107B)
bool CsaMemFile::Seek(int offset, int origin) {
    switch (origin) {
    case 0:
        if (offset < 0 || (unsigned int)offset >= this->size) {
            return false;
        }
        this->cur = this->base + offset;
        return true;
    case 1:
        if ((int)(this->base + this->size - this->cur) <= offset) {
            return false;
        }
        this->cur = this->base + this->size + offset; // wait: let's check target +047!
        return true;
    case 2:
        if (offset > 0 || (unsigned int)abs(offset) >= this->size) {
            return false;
        }
        this->cur = this->base + this->size + offset;
        return true;
    default:
        return false;
    }
}

// 9. 0x0045D2C0 (109B)
void CsaSetInterruptAll(CsaAnmVm2 *vms, int count, short interrupt) {
    for (int i = 0; i < count; ++i) {
        CsaAnmVm2 *vm = &vms[i];
        int *resId = vm->resIdPtr;
        bool loaded = false;
        if (resId != NULL && *resId >= 0) {
            CsaAnmRes *res = g_CsaAnmMgr->slots[*resId];
            if (res != NULL && res->texArray != NULL) {
                loaded = true;
            }
        }
        if (loaded) {
            if (vm->interruptCb != NULL) {
                vm->interruptCb(vm, interrupt);
            }
            vm->pendingInterrupt = interrupt;
        }
    }
}

static __forceinline CsaAnmVm *CsaFindChildVm(CsaAnmVm *root, int scriptId) {
    CsaVmNode *node = &root->childNode;
    while (node != NULL) {
        CsaAnmVm *vm = node->vm;
        if (vm->scriptId == scriptId || (scriptId == -1 && vm != root)) {
            return vm;
        }
        node = node->next;
    }
    return NULL;
}

// 10. 0x00444FA0 (99B)
void CsaCardObj::ShowVm(int scriptId) {
    CsaAnmVm *root = g_CsaAnmMgr->GetVmById(this->mainVmId);
    if (root == NULL) {
        this->mainVmId = 0;
    }
    CsaAnmVm *vm = CsaFindChildVm(root, scriptId);
    vm->flags |= 2;
}

// 11. 0x00445010 (99B)
void CsaCardObj::HideVmBySlot(int slot, int scriptId) {
    CsaAnmVm *root = g_CsaAnmMgr->GetVmById(this->vmIds[slot]);
    if (root == NULL) {
        this->vmIds[slot] = 0;
    }
    CsaAnmVm *vm = CsaFindChildVm(root, scriptId);
    vm->flags &= ~2u;
}

} // namespace th128
