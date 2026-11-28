#include "EhBatchF.hpp"

namespace th128 {

inline void EbfChainMgr::Cut(EbfChainElem* elem) {
    if (elem == 0) {
        return;
    }
    if (g_EbfLockFlags & 0x8000) {
        EnterCriticalSection(&g_EbfChainCs);
        ++g_EbfChainLockCount;
    }
    EbfChainCutInternal(this, elem);
    if (g_EbfLockFlags & 0x8000) {
        LeaveCriticalSection(&g_EbfChainCs);
        --g_EbfChainLockCount;
    }
}

inline void EbfAnmMgr::ReleaseSlot() {
    if (anmSlot != 0) {
        EbfReleaseAnm(anmSlot);
        ::operator delete(anmSlot);
        anmSlot = 0;
    }
}

EbfPlayer::~EbfPlayer() {
    g_EbfChainMgr->Cut(chain8);
    g_EbfChainMgr->Cut(chainC);
    g_EbfPlayer = 0;
    if (g_EbfStageFlags & 1) {
        g_EbfAnmMgr->KeepPlayerAnm(field10);
        g_EbfSavedBuf = bufF7c;
    } else {
        g_EbfAnmMgr->ReleaseSlot();
        if (bufF7c != 0) {
            free(bufF7c);
            bufF7c = 0;
        }
        g_EbfSavedBuf = 0;
    }
}

EbfAnmMgr::~EbfAnmMgr() {
    for (EbfAnmListNode* n = listA; n != 0; ) {
        EbfAnmListNode* next = n->next;
        EbfDestroyVm(n->vm);
        n = next;
    }
    for (EbfAnmListNode* n = listB; n != 0; ) {
        EbfAnmListNode* next = n->next;
        EbfDestroyVm(n->vm);
        n = next;
    }
}

} // namespace th128
