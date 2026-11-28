#include "LifecycleWaveB.hpp"

#include <stdlib.h>

namespace th128 {

inline void LwbChainMgr::Cut(LwbChainElem *elem) {
    if (elem == NULL) {
        return;
    }
    if (g_LwbGameFlags & 0x8000) {
        EnterCriticalSection(&g_LwbChainCs);
        ++g_LwbChainLockDepth;
    }
    LwbChainCutInternal(this, elem);
    if (g_LwbGameFlags & 0x8000) {
        LeaveCriticalSection(&g_LwbChainCs);
        --g_LwbChainLockDepth;
    }
}

void LwbHelpInf::Shutdown() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    g_LwbAnmMgr->ReleaseSlotPtr(g_LwbAnmMgr->slots[31]);
    g_LwbHelpInf = NULL;
}

void LwbIceObj::Shutdown() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    g_LwbAnmMgr->ReleaseSlotPtr(g_LwbAnmMgr->slots[36]);
    g_LwbIceObj = NULL;
}

LwbBulletMgr::~LwbBulletMgr() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    LwbMarkResourceVmsForDelete(g_LwbAnmMgr, anmRes8a7f9c);
    g_LwbBulletMgr = NULL;
}

LwbPauseOrReplay::~LwbPauseOrReplay() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    for (int i = 0; i < 0x19; ++i) {
        if (entries[i] != NULL) {
            delete entries[i];
        }
    }
    g_LwbPauseOrReplay = NULL;
}

LwbScoreBoard::~LwbScoreBoard() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    g_LwbScoreBoard = NULL;
    if (buf4c4 != NULL) {
        free(buf4c4);
        buf4c4 = NULL;
    }
    buf4c4 = NULL;
}

LwbItemMgr::~LwbItemMgr() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    g_LwbItemMgr = NULL;
}

LwbEffectMgr::~LwbEffectMgr() {
    LwbMarkResourceVmsForDelete(g_LwbAnmMgr, anmRes10);
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
    g_LwbEffectMgr = NULL;
}

LwbSimpleChainOwner::~LwbSimpleChainOwner() {
    g_LwbChainMgr->Cut(chain8);
    g_LwbChainMgr->Cut(chainC);
}

int LwbTitleBootWait() {
    if (LwbInit445330() != 0) {
        g_LwbMode3c0 = (((~(g_LwbGameFlags >> 13)) & 1) | 2);
        return 0;
    }
    if (g_LwbSplash != NULL) {
        while (g_LwbSplash->timer528 < 0xB4 && (g_LwbGameFlags & 0x180) == 0) {
            Sleep(16);
        }
        g_LwbAnmMgr->ReleaseSlotPtr(g_LwbAnmMgr->slots[1]);
    }
    g_LwbTitleSub->chainC->flags |= 2u;
    g_LwbFlag59e8 = 1;
    return 0;
}

} // namespace th128
