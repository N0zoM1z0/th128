#include "EhBatchB.hpp"

EbbChain *g_EbbChain;
unsigned int g_EbbSupFlags;
CRITICAL_SECTION g_EbbCs;
unsigned char g_EbbCsCount;
EbbAnmMgr *g_EbbAnmMgr;
unsigned int g_EbbFlags4b4d50;
EbbStageObj *g_EbbStageA;
EbbStageObj *g_EbbStageB;
EbbPlayerObj *g_EbbPlayer;
void *g_EbbSaved4d2e28;
EbbTitleSub *g_EbbTitleSub;
EbbGameMgr *g_EbbGameMgr;
EbbLoadingMgr *g_EbbLoadingMgr;
EbbExtGui *g_EbbGui;
EbbExtPlayer *g_EbbExtPlayer;
EbbExtBullet *g_EbbBullet;
EbbExtEnemy *g_EbbEnemy;
EbbExtLaser *g_EbbLaser;
EbbExt4b8a90 *g_Ebb4b8a90;
EbbExtMain *g_EbbMain;
EbbScoreMgr *g_EbbScoreMgr;

inline void EbbChain::Cut(EbbChainElem *elem) {
    if (elem != 0) {
        if (g_EbbSupFlags & 0x8000) {
            EnterCriticalSection(&g_EbbCs);
            ++g_EbbCsCount;
        }
        EbbChainCutInternal(this, elem);
        if (g_EbbSupFlags & 0x8000) {
            LeaveCriticalSection(&g_EbbCs);
            --g_EbbCsCount;
        }
    }
}

static inline void EbbDeleteAnmSlot(EbbAnmMgr *mgr, int idx) {
    if (mgr->slots[idx] != 0) {
        mgr->slots[idx]->~EbbAnmSlot();
        ::operator delete(mgr->slots[idx]);
        mgr->slots[idx] = 0;
    }
}

// 1. 0x00403710 (601B)
EbbStageObj::~EbbStageObj() {
    g_EbbChain->Cut(calcElem);
    g_EbbChain->Cut(drawElem);
    g_EbbChain->Cut(elem383c);
    if (buf10 != 0) {
        free(buf10);
        buf10 = 0;
    }
    if (buf3840 != 0) {
        free(buf3840);
        buf3840 = 0;
    }
    buf3840 = 0;
    if (buf1c8 != 0) {
        free(buf1c8);
        buf1c8 = 0;
    }
    if (sub3818 != 0) {
        delete sub3818;
        sub3818 = 0;
    }
    if ((g_EbbFlags4b4d50 & 1) == 0) {
        int idx = (flags3810 & 1) + 3;
        if (idx >= 0 && (unsigned int)idx < 0x26) {
            EbbDeleteAnmSlot(g_EbbAnmMgr, idx);
        }
    }
    if (g_EbbStageA == this) {
        g_EbbStageA = 0;
    }
    if (g_EbbStageB == this) {
        g_EbbStageB = 0;
    }
}

// 2. 0x0043acd0 (463B)
EbbPlayerObj::~EbbPlayerObj() {
    g_EbbChain->Cut(calcElem);
    g_EbbChain->Cut(drawElem);
    g_EbbPlayer = 0;
    if (g_EbbFlags4b4d50 & 1) {
        g_EbbAnmMgr->KeepSlot(anm10);
        g_EbbSaved4d2e28 = buf_f7c;
    } else {
        EbbDeleteAnmSlot(g_EbbAnmMgr, 7);
        if (buf_f7c != 0) {
            free(buf_f7c);
            buf_f7c = 0;
        }
        g_EbbSaved4d2e28 = 0;
    }
}

// 3. 0x00444300 (470B)
EbbTitleSub::~EbbTitleSub() {
    EbbPreDtor444150(this);
    g_EbbChain->Cut(calcElem);
    g_EbbChain->Cut(drawElem);
    if (buf2e0 != 0) {
        free(buf2e0);
        buf2e0 = 0;
    }
    if (obj2d8 != 0) {
        delete obj2d8;
        obj2d8 = 0;
    }
    if (vobj2dc != 0) {
        delete vobj2dc;
        vobj2dc = 0;
    }
    EbbDeleteAnmSlot(g_EbbAnmMgr, 6);
    EbbDeleteAnmSlot(g_EbbAnmMgr, 5);
    g_EbbTitleSub = 0;
}

// 4. 0x00411100 (437B)
EbbGameMgr::~EbbGameMgr() {
    EbbPreDtor410dc0(this);
    g_EbbChain->Cut(calcElem);
    g_EbbChain->Cut(drawElem);
    delete g_EbbGui;
    delete g_EbbExtPlayer;
    delete g_EbbBullet;
    delete g_EbbEnemy;
    delete g_EbbLaser;
    delete g_Ebb4b8a90;
    g_EbbGameMgr = 0;
}

// 5. 0x00433210 (478B)
EbbLoadingMgr::~EbbLoadingMgr() {
    thread10.Reset();
    g_EbbChain->Cut(calcElem);
    g_EbbChain->Cut(drawElem);
    EbbHelper432ee0();
    EbbDeleteAnmSlot(g_EbbAnmMgr, 1);
    g_EbbLoadingMgr = 0;
    delete g_EbbMain;
    EbbDeleteAnmSlot(g_EbbAnmMgr, 0);
    EbbHelper443040();
    if (g_EbbScoreMgr != 0) {
        delete g_EbbScoreMgr;
        g_EbbScoreMgr = 0;
    }
}
