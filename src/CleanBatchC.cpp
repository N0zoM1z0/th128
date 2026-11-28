#include "CleanBatchC.hpp"
#include <string.h>

CbcAnmMgr *g_CbcAnmMgr;
CbcStageAnimMgr *g_CbcStageAnimMgr;
unsigned int g_CbcInputFlags;
unsigned int g_CbcSupFlags;
int g_CbcFrameSkip;
CbcGamePtr *g_CbcGamePtr;
int g_CbcFreezeGauge;
CbcMainMgr *g_CbcMainMgr;
int g_CbcFlag4d59e8;
int g_CbcGlobal4d2e68;
void *g_CbcSubD0Vtbl;
CbcChain g_CbcChain;
CbcSoundMgr g_CbcSoundMgr;

// 1. 0x00466d60 (124B)
void CbcAnmVm::RegisterWorldTail(int *outId) {
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    listNode.data = this;
    listNode.next = 0;
    listNode.prev = 0;
    if (mgr->worldHead == 0) {
        mgr->worldHead = &listNode;
    } else {
        CbcListNode *tail = mgr->worldTail;
        if (tail->next != 0) {
            listNode.next = tail->next;
            tail->next->prev = &listNode;
        }
        tail->next = &listNode;
        listNode.prev = tail;
    }
    mgr->worldTail = &listNode;
    ++mgr->nextId;
    if (mgr->nextId == 0) {
        ++mgr->nextId;
    }
    id = mgr->nextId;
    *outId = mgr->nextId;
}

// 2. 0x00466de0 (119B)
void CbcAnmVm::RegisterWorldHead(int *outId) {
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    listNode.data = this;
    listNode.next = 0;
    listNode.prev = 0;
    if (mgr->worldHead == 0) {
        mgr->worldTail = &listNode;
    } else {
        CbcListNode *head = mgr->worldHead;
        if (listNode.next != 0) {
            head->next = listNode.next;
            listNode.next->prev = head;
        }
        listNode.next = head;
        head->prev = &listNode;
    }
    mgr->worldHead = &listNode;
    ++mgr->nextId;
    if (mgr->nextId == 0) {
        ++mgr->nextId;
    }
    id = mgr->nextId;
    *outId = mgr->nextId;
}

// 3. 0x00466e60 (124B)
void CbcAnmVm::RegisterUiTail(int *outId) {
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    listNode.data = this;
    listNode.next = 0;
    listNode.prev = 0;
    if (mgr->uiHead == 0) {
        mgr->uiHead = &listNode;
    } else {
        CbcListNode *tail = mgr->uiTail;
        if (tail->next != 0) {
            listNode.next = tail->next;
            tail->next->prev = &listNode;
        }
        tail->next = &listNode;
        listNode.prev = tail;
    }
    mgr->uiTail = &listNode;
    ++mgr->nextId;
    if (mgr->nextId == 0) {
        ++mgr->nextId;
    }
    id = mgr->nextId;
    *outId = mgr->nextId;
}

// 4. 0x00466ee0 (119B)
void CbcAnmVm::RegisterUiHead(int *outId) {
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    listNode.data = this;
    listNode.next = 0;
    listNode.prev = 0;
    if (mgr->uiHead == 0) {
        mgr->uiTail = &listNode;
    } else {
        CbcListNode *head = mgr->uiHead;
        if (listNode.next != 0) {
            head->next = listNode.next;
            listNode.next->prev = head;
        }
        listNode.next = head;
        head->prev = &listNode;
    }
    mgr->uiHead = &listNode;
    ++mgr->nextId;
    if (mgr->nextId == 0) {
        ++mgr->nextId;
    }
    id = mgr->nextId;
    *outId = mgr->nextId;
}

// 5. 0x00406350 (241B)
void CbcStartStageAnim() {
    CbcStageAnimMgr *mgr = g_CbcStageAnimMgr;
    mgr->ce8->flags |= 2;
    mgr->ceC->flags |= 2;
    mgr->ce383c->flags |= 2;
    int vmIdx = 0;
    for (int i = 0; i < *mgr->hdrCount; ++i) {
        mgr->entries[i]->flag3 = 1;
        CbcAnimSubEntry *sub = &mgr->entries[i]->firstSub;
        while (sub->valid >= 0) {
            CbcAnmVm *vm = &mgr->vms[vmIdx];
            CbcResetVm(vm);
            vm->field4d0 = 0x1010;
            CbcSetVmScript(vm, mgr->anmFile, sub->script);
            sub->vmIdx = (short)vmIdx;
            ++vmIdx;
            sub = (CbcAnimSubEntry *)((unsigned char *)sub + sub->stride);
        }
    }
    mgr->val4c = mgr->val1c;
}

// 6. 0x00412fc0 (178B)
static inline CbcAnmVm *CbcFindVmInline(CbcAnmMgr *mgr, int id) {
    if (id == 0) {
        return 0;
    }
    for (CbcListNode *n = mgr->worldHead; n != 0; n = n->next) {
        CbcAnmVm *vm = (CbcAnmVm *)n->data;
        if (vm->id == id) {
            return vm;
        }
    }
    for (CbcListNode *n = mgr->uiHead; n != 0; n = n->next) {
        CbcAnmVm *vm = (CbcAnmVm *)n->data;
        if (vm->id == id) {
            return vm;
        }
    }
    return 0;
}

void CbcEffectSub::Cleanup() {
    subD0.Cleanup();
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    for (int i = 0; i < 5; ++i) {
        CbcAnmVm *vm = CbcFindVmInline(mgr, anmIds[i]);
        if (vm != 0) {
            vm->flags |= 0x20000000U;
            if (vm->parentFlag == 0) {
                for (CbcListNode *c = vm->childHead; c != 0; c = c->next) {
                    ((CbcAnmVm *)c->data)->flags |= 0x20000000U;
                }
            }
        }
        anmIds[i] = 0;
    }
    subD0.vtable = &g_CbcSubD0Vtbl;
    subD0.Cleanup();
}

// 7. 0x004062c0 (134B)
void CbcTwoVmOwner::ClearTwoVms() {
    CbcAnmMgr *mgr = g_CbcAnmMgr;
    field2910 = 0;
    CbcAnmVm *vm = mgr->FindVm(anm3834);
    if (vm != 0) {
        vm->flags |= 0x20000000U;
        if (vm->parentFlag == 0) {
            for (CbcListNode *c = vm->childHead; c != 0; c = c->next) {
                ((CbcAnmVm *)c->data)->flags |= 0x20000000U;
            }
        }
    }
    vm = mgr->FindVm(anm3838);
    if (vm != 0) {
        vm->flags |= 0x20000000U;
        if (vm->parentFlag == 0) {
            for (CbcListNode *c = vm->childHead; c != 0; c = c->next) {
                ((CbcAnmVm *)c->data)->flags |= 0x20000000U;
            }
        }
    }
}

// 8. 0x004686d0 (125B)
void __fastcall CbcChainCut(CbcChain *chain, CbcChainElem *elem) {
    if (elem == 0) {
        return;
    }
    CbcListNode *n = &chain->calcHead;
    while (n != 0) {
        if (n->data == elem) {
            goto found;
        }
        n = n->next;
    }
    n = &chain->drawHead;
    while (n != 0) {
        if (n->data == elem) {
            goto found;
        }
        n = n->next;
    }
    return;

found:
    if (chain->curNode == n) {
        chain->curNode = n->next;
    }
    if (n->prev == 0) {
        return;
    }
    if (n->next != 0) {
        n->next->prev = n->prev;
    }
    if (n->prev != 0) {
        n->prev->next = n->next;
    }
    n->next = 0;
    n->prev = 0;
    elem->cb = 0;
    if (elem->flags & 1) {
        elem->cb = 0;
        elem->onReg = 0;
        elem->onCut = 0;
        delete elem;
    }
}

// 9. 0x00413650 (116B)
int CbcEndingObj::OnCalc() {
    CbcEndingInner *in = inner;
    if (CbcEndingCheck(in) == 0) {
        CbcEndingStep(&in->sub04);
        ++frame24;
        if ((inner->flags74 & 4) == 0 && (flags20 & 2) == 0 &&
            (inner->flags74 & 2) != 0 && (g_CbcInputFlags & 0x200) != 0) {
            if (frame24 % 12 != 0) {
                return 6;
            }
        }
        return 1;
    }
    g_CbcFrameSkip = (g_CbcSupFlags & 0x2000) ? 2 : 16;
    return 1;
}

// 10. 0x0043df20 (108B)
void CbcPauseSync::Sync() {
    int p = g_CbcGamePtr->paused;
    if (p == 0) {
        if (stateCC != 0) {
            CbcSetVmScriptById(anmB0, 6);
            c6c = b5c;
            c70 = b60;
        }
        stateCC = p;
    } else {
        if (stateCC == 0) {
            CbcSetVmScriptById(anmB0, 3);
            a54 = c6c;
            a58 = c70;
        }
        stateCC = p;
    }
}

// 11. 0x0046ab00 (98B)
char *CbcReadShiftJisLine(const char *src, char *dst, int maxLen) {
    memset(dst, 0, maxLen);
    const char *s = src;
    char *d = dst;
    while (*s != '\n' && *s != '\r' && maxLen != 0) {
        *d++ = *s;
        unsigned char c = (unsigned char)*s;
        if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
            ++s;
            --maxLen;
            *d++ = *s;
        }
        ++s;
        --maxLen;
    }
    while (*s == '\n' || *s == '\r') {
        ++s;
    }
    return (char *)s;
}

// 12. 0x0041f410 (96B)
void __stdcall CbcAddFreezeGauge(int delta) {
    int oldVal = g_CbcFreezeGauge;
    g_CbcFreezeGauge += delta;
    if (g_CbcFreezeGauge > 10000) {
        g_CbcFreezeGauge = 10000;
    }
    if (oldVal < 10000 && g_CbcFreezeGauge >= 10000) {
        g_CbcSoundMgr.PlaySfx(0x4a);
    }
    if (oldVal < 3000 && g_CbcFreezeGauge >= 3000) {
        g_CbcSoundMgr.PlaySfx(0x49);
    }
}

// 13. 0x004334b0 (95B)
int CbcPauseObj::Close() {
    if (flags & 2) {
        CbcCloseHelper(&g_CbcGlobal4d2e68);
        g_CbcFlag4d59e8 = 1;
        CbcMainMgr *m = g_CbcMainMgr;
        m->ceC->flags |= 2;
        m->ce10->flags |= 2;
        m->ce1c8a0->flags |= 2;
        m->ce1c8a4->flags |= 2;
        g_CbcSupFlags &= ~0x2000U;
        g_CbcFrameSkip = 4;
        flags &= ~2U;
    }
    return 1;
}

// 14. 0x0041f800 (86B)
int CbcDrawOwner::RegisterDraw43() {
    CbcChainElem *elem = new CbcChainElem();
    elem->flags |= 3;
    elem->cb = (void *)&CbcDraw43Cb;
    elem->onReg = 0;
    elem->onCut = 0;
    elem->arg = this;
    g_CbcChain.RegisterDraw(elem, 0x43);
    drawElem = elem;
    return 0;
}
