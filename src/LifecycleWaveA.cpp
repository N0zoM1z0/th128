#include "LifecycleWaveA.hpp"

#include <process.h>
#include <stdlib.h>

namespace th128 {

inline void LwaChainMgr::Cut(LwaChainElem *elem) {
    if (elem == NULL) {
        return;
    }
    if (g_LwaGameFlags & 0x8000) {
        EnterCriticalSection(&g_LwaChainCs);
        ++g_LwaChainLockDepth;
    }
    LwaChainCutInternal(this, elem);
    if (g_LwaGameFlags & 0x8000) {
        LeaveCriticalSection(&g_LwaChainCs);
        --g_LwaChainLockDepth;
    }
}

inline void LwaAnmMgr::ReleaseSlotPtr(void *&slot) {
    if (slot != NULL) {
        LwaReleaseAnm(slot);
        ::operator delete(slot);
        slot = NULL;
    }
}

inline void LwaAnmMgr::ReleaseSlotAt(int idx) {
    if (idx >= 0 && idx < 38) {
        ReleaseSlotPtr(g_LwaAnmMgr->slots[idx]);
    }
}

int LwaRegisterSupervisorChains() {
    g_LwaSupervisor.field554 = -2;
    g_LwaSupervisor.field558 = 0;
    g_LwaSupervisor.field560 = 0;

    LwaChainElem *e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb0;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    e->onReg = (void *)&LwaSupCb1;
    if (LwaRegisterCalc(e, 1) != 0) {
        return 0;
    }

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb2;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 1);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb3;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x0C);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb4;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x0E);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb5;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x27);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb6;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x29);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb7;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x33);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb8;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x34);

    e = new LwaChainElem();
    e->flags |= 3u;
    e->cb = (void *)&LwaSupCb9;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = &g_LwaSupervisor;
    LwaRegisterDraw(e, 0x4C);

    return 0;
}

LwaStageBg::~LwaStageBg() {
    if (flag3c != 0 && (g_LwaStageA + g_LwaStageB * 2) == 5) {
        LwaDeleteVmByIdPtr(&anm40);
        LwaAddGraze(10);
    }
    LwaDeleteVmByIdPtr(&anm40);
    LwaDeleteVmByIdPtr(&anm44);
    LwaDeleteVmByIdPtr(&anm48);
    g_LwaChainMgr->Cut(chain8);
    g_LwaChainMgr->Cut(chainC);
    g_LwaChainMgr->Cut(chain554);
    g_LwaStageBg = NULL;
    if (mini538 != NULL) {
        mini538->Cleanup();
        ::operator delete(mini538);
    }
    mini538 = NULL;
    if (buf534 != NULL) {
        free(buf534);
        buf534 = NULL;
    }
    if (buf4f8 != NULL) {
        free(buf4f8);
        buf4f8 = NULL;
    }
}

LwaEnemyMgr::~LwaEnemyMgr() {
    LwaEnemyPreCleanup(this);
    g_LwaChainMgr->Cut(chain8);
    g_LwaChainMgr->Cut(chainC);
    for (int i = 0; i < 32; ++i) {
        if (resB4->files[i] != NULL) {
            free(resB4->files[i]);
        }
    }
    if (resB4 != NULL) {
        delete resB4;
    }
    resB4 = NULL;
    for (int i = 8; i < 16; ++i) {
        g_LwaAnmMgr->ReleaseSlotAt(i);
    }
    g_LwaEnemyMgr = NULL;
}

LwaReplayMgr::~LwaReplayMgr() {
    ::operator delete(buf18);
    for (int i = 0; i < 17; ++i) {
        FreeStageChunks(i);
    }
    ::operator delete(buf1c);
    buf1c = NULL;
    for (int i = 0; i < 17; ++i) {
        ::operator delete(stagePtrs[i]);
        stagePtrs[i] = NULL;
    }
    g_LwaChainMgr->Cut(chain8);
    g_LwaChainMgr->Cut(chain3a8);
    g_LwaChainMgr->Cut(chainC);
    if (g_LwaReplayMgr == this) {
        g_LwaReplayMgr = NULL;
    }
}

void LwaCardInf::Shutdown() {
    g_LwaAnmMgr->ReleaseSlotPtr(g_LwaAnmMgr->slots[37]);
    LwaDeleteVmByIdPtr(&anm14);
    LwaDeleteVmByIdPtr(&anm18);
    LwaDeleteVmByIdPtr(&anm1c);
    g_LwaChainMgr->Cut(chain8);
    g_LwaChainMgr->Cut(chainC);
    g_LwaCardInf = NULL;
}

int LwaBulletMgr::Init() {
    anm8a7f9c = LwaPreloadAnm(6, "bullet.anm");
    if (anm8a7f9c == NULL) {
        g_LwaErrorCtx.Log("error : bullet.anm\n");
        return -1;
    }
    ptr10 = pool64;
    count8a780e = 6;
    LwaChainElem *e = new LwaChainElem();
    e->flags = (e->flags & ~2u) | 1u;
    e->cb = (void *)&LwaBulletCalc;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = this;
    LwaRegisterCalc(e, 0x18);
    chain8 = e;

    e = new LwaChainElem();
    e->flags = (e->flags & ~2u) | 1u;
    e->cb = (void *)&LwaBulletDraw;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = this;
    LwaRegisterDraw(e, 0x20);
    chainC = e;
    return 0;
}

int __stdcall LwaLoading::Init(LwaLoading *self) {
    LwaChainElem *e = new LwaChainElem();
    e->flags = (e->flags & ~2u) | 1u;
    e->cb = (void *)&LwaLoadCalc;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = self;
    LwaRegisterCalc(e, 3);
    self->chain8 = e;

    e = new LwaChainElem();
    e->flags = (e->flags & ~2u) | 1u;
    e->cb = (void *)&LwaLoadDraw;
    e->onReg = NULL;
    e->onUnreg = NULL;
    e->arg = self;
    LwaRegisterDraw(e, 0x3C);
    self->chainC = e;

    self->thread10.Stop();
    self->thread10.proc18 = LwaLoadThreadProc;
    self->thread10.running10 = 1;
    self->thread10.fieldC = 0;
    self->thread10.handle4 = (void *)_beginthreadex(NULL, 0, LwaLoadThreadProc, self, 0, &self->thread10.threadId8);
    return 0;
}

LwaLaserMgr::~LwaLaserMgr() {
    g_LwaChainMgr->Cut(chain8);
    g_LwaChainMgr->Cut(chainC);
    for (LwaLaserNode *n = head18; n != NULL; ) {
        LwaLaserNode *next = n->next;
        n->Unlink();
        n->prev->next = n->next;
        if (n->next != NULL) {
            n->next->prev = n->prev;
        }
        ::operator delete(n);
        n = next;
    }
    g_LwaLaserMgr = NULL;
}

} // namespace th128
