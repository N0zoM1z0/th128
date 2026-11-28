#include "ChainWave.hpp"

namespace th128 {

static inline void LockChain() {
    if (g_CwGameFlags & 0x8000) {
        EnterCriticalSection(&g_CwChainCs);
        ++g_CwChainLockDepth;
    }
}

static inline void UnlockChain() {
    if (g_CwGameFlags & 0x8000) {
        LeaveCriticalSection(&g_CwChainCs);
        --g_CwChainLockDepth;
    }
}

CwChain::CwChain() {
    pauseFlag = 0;
}

int CwChain::RunCalcChain() {
    LockChain();
    int count;
restart:
    CwChainNode *node = calcHead.node.next;
    count = 0;
    while (node != NULL) {
        nextNode = node->next;
        CwChainElem *elem = node->owner;
        if (elem->cb != NULL) {
            if ((elem->flags & 2) != 0) {
                if (pauseFlag != 0) {
                    goto do_unreg;
                }
            repeat_cb:
                UnlockChain();
                int res = elem->cb(elem->arg);
                LockChain();
                switch (res) {
                case 0:
                    CwChainCutInternal(this, elem);
                    break;
                case 2:
                    if ((elem->flags & 2) != 0) {
                        goto repeat_cb;
                    }
                    break;
                case 4:
                case 8:
                    count = 0;
                    goto done;
                case 3:
                    count = 1;
                    goto done;
                case 5:
                    count = -1;
                    goto done;
                case 6:
                    goto restart;
                case 7:
                do_unreg:
                    if (elem->onUnreg != NULL) {
                        elem->onUnreg(elem->arg);
                    }
                    break;
                default:
                    break;
                }
            }
            ++count;
        }
        node = nextNode;
    }
done:
    UnlockChain();
    return count;
}

int CwChain::RunDrawChain() {
    CwChain *chain = g_CwChain;
    LockChain();
    CwChainNode *node = chain->drawHead.node.next;
    int count = 0;
    while (node != NULL) {
        chain->nextNode = node->next;
        CwChainElem *elem = node->owner;
        if (elem->cb != NULL) {
            if ((elem->flags & 2) != 0) {
            repeat_cb:
                UnlockChain();
                int res = elem->cb(elem->arg);
                LockChain();
                switch (res) {
                case 0:
                    CwChainCutInternal(chain, elem);
                    break;
                case 2:
                    if ((elem->flags & 2) != 0) {
                        goto repeat_cb;
                    }
                    break;
                case 4:
                    count = 0;
                    goto done;
                case 3:
                    count = 1;
                    goto done;
                case 5:
                    count = -1;
                    goto done;
                default:
                    break;
                }
            }
            ++count;
        }
        node = chain->nextNode;
    }
done:
    UnlockChain();
    return count;
}

void CwChain::CutList(CwChainElem *head) {
    CwChainNode *node = head->node.next;
    while (node != NULL) {
        CwChainElem *elem = node->owner;
        CwChainNode *next = node->next;
        if (elem != NULL) {
            LockChain();
            CwChainCutInternal(this, elem);
            UnlockChain();
        }
        node = next;
    }
}

void CwChain::Cut(CwChainElem *elem) {
    if (elem != NULL) {
        LockChain();
        CwChainCutInternal(this, elem);
        UnlockChain();
    }
}

} // namespace th128
