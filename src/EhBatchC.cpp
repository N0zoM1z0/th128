#include "EhBatchC.hpp"

EbcChain *g_EbcChain;
unsigned int g_EbcSupFlags;
CRITICAL_SECTION g_EbcCs;
unsigned char g_EbcCsCount;
EbcAnmMgr *g_EbcAnmMgr;
EbcMainMgr *g_EbcMainMgr;
EbcScoreMgr *g_EbcScoreMgr;
EbcTitleObj *g_EbcTitleObj;

inline void EbcChain::Cut(EbcChainElem *elem) {
    if (elem != 0) {
        if (g_EbcSupFlags & 0x8000) {
            EnterCriticalSection(&g_EbcCs);
            ++g_EbcCsCount;
        }
        EbcChainCutInternal(this, elem);
        if (g_EbcSupFlags & 0x8000) {
            LeaveCriticalSection(&g_EbcCs);
            --g_EbcCsCount;
        }
    }
}

static inline void EbcDeleteAnmSlot(EbcAnmMgr *mgr, int idx) {
    if (mgr->slots[idx] != 0) {
        mgr->slots[idx]->~EbcAnmSlot();
        ::operator delete(mgr->slots[idx]);
        mgr->slots[idx] = 0;
    }
}

static inline EbcAnmVmNode *EbcFindVm(EbcAnmMgr *mgr, unsigned int id) {
    for (EbcListNode *n = mgr->list1; n != 0; n = n->next) {
        EbcAnmVmNode *vm = (EbcAnmVmNode *)n->data;
        if (vm->id == id) {
            return vm;
        }
    }
    for (EbcListNode *n = mgr->list2; n != 0; n = n->next) {
        EbcAnmVmNode *vm = (EbcAnmVmNode *)n->data;
        if (vm->id == id) {
            return vm;
        }
    }
    return 0;
}

// 1. 0x004012a0 (583B)
EbcMainMgr::~EbcMainMgr() {
    g_EbcChain->Cut(elem0c);
    g_EbcChain->Cut(elem10);
    g_EbcChain->Cut(elem1c8a0);
    g_EbcChain->Cut(elem1c8a4);
    EbcDeleteAnmSlot(g_EbcAnmMgr, 2);
    EbcDeleteAnmSlot(g_EbcAnmMgr, 0);
    g_EbcMainMgr = 0;
}

// 2. 0x004434f0 (259B)
EbcScoreMgr::~EbcScoreMgr() {
    g_EbcChain->Cut(calcElem);
    g_EbcChain->Cut(drawElem);
    g_EbcScoreMgr = 0;
    EbcStopBgm(-1);
}

// 3. 0x00445450 (609B)
EbcTitleObj::~EbcTitleObj() {
    thread5bd0.Reset();
    g_EbcChain->Cut(calcElem);
    g_EbcChain->Cut(drawElem);
    EbcDeleteAnmSlot(g_EbcAnmMgr, 0x18);
    EbcDeleteAnmSlot(g_EbcAnmMgr, 0x19);
    for (int i = 0; i < 100; ++i) {
        if (replays[i] != 0) {
            delete replays[i];
        }
    }
    if (vmId620 != 0) {
        EbcAnmVmNode *vm = EbcFindVm(g_EbcAnmMgr, vmId620);
        if (vm != 0) {
            if (vm->cb != 0) {
                vm->cb(vm, 1);
            }
            vm->state3f8 = 1;
            if (vm->flag18 == 0) {
                for (EbcListNode *c = vm->children; c != 0; c = c->next) {
                    EbcAnmVmNode *cv = (EbcAnmVmNode *)c->data;
                    if (cv->cb != 0) {
                        cv->cb(cv, 1);
                    }
                    cv->state3f8 = 1;
                }
            }
        }
    }
    if (buf5bc4 != 0) {
        free(buf5bc4);
        buf5bc4 = 0;
    }
    g_EbcTitleObj = 0;
}
