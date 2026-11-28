#include "CleanSmallWaveB.hpp"

#include <process.h>
#include <string.h>

namespace th128 {

// 1. 0x004456C0 (106B)
CsbTitleMgr *CsbCreateTitleMgr() {
    CsbTitleMgr *mgr = new CsbTitleMgr();
    g_CsbTitleFlag = 0;
    mgr->thread.Stop();
    mgr->thread.proc = (void *)CsbTitleWorker;
    mgr->thread.f10 = 1;
    mgr->thread.f0c = 0;
    mgr->thread.handle = (HANDLE)_beginthreadex(NULL, 0, CsbTitleWorker, mgr, 0, &mgr->thread.threadId);
    return mgr;
}

// 2. 0x0042A0A0 (106B)
CsbLaserMgr::CsbLaserMgr() {
    memset(this, 0, sizeof(CsbLaserMgr));
    this->f5d8 = 0x10000;
    g_CsbLaserMgr = this;
}

CsbLaserMgr *CsbCreateLaserMgr() {
    CsbLaserMgr *mgr = new CsbLaserMgr();
    if (CsbInitLaserMgr(mgr) != 0) {
        if (mgr != NULL) {
            delete mgr;
        }
        return NULL;
    }
    return mgr;
}

// 3. 0x00432EE0 (99B)
int CsbReleaseEndingAnms() {
    CsbAnmMgr *anm = g_CsbAnmMgr;
    if (anm->slot5 != NULL) {
        anm->slot5->Release();
        delete anm->slot5;
        anm->slot5 = NULL;
    }
    anm = g_CsbAnmMgr;
    if (anm->slot6 != NULL) {
        anm->slot6->Release();
        delete anm->slot6;
        anm->slot6 = NULL;
    }
    return 0;
}

// 4. 0x0046AB00 (98B)
const char *CsbReadShiftJisLine(const char *src, char *dst, int maxLen) {
    memset(dst, 0, maxLen);
    char *out = dst;
    while (*src != '\n' && *src != '\r' && maxLen != 0) {
        *out++ = *src;
        unsigned char c = (unsigned char)*src;
        if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
            ++src;
            --maxLen;
            *out++ = *src;
        }
        ++src;
        --maxLen;
    }
    while (*src == '\n' || *src == '\r') {
        ++src;
    }
    return src;
}

// 5. 0x00415270 (97B)
void CsbUnlinkEnemy(CsbEnemy *enemy) {
    CsbEnemyMgr *mgr = g_CsbEnemyMgr;
    CsbEnemyNode *node = &enemy->node;
    if (mgr->head == node) {
        mgr->head = node->next;
    }
    if (mgr->tail == node) {
        mgr->tail = node->prev;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    node->next = NULL;
    node->prev = NULL;
    --mgr->count;
}

// 6. 0x0041F410 (96B)
void __stdcall CsbAddFreezeGauge(int delta) {
    int prev = g_CsbFreezeGauge;
    g_CsbFreezeGauge += delta;
    if (g_CsbFreezeGauge > 10000) {
        g_CsbFreezeGauge = 10000;
    }
    if (prev < 10000 && g_CsbFreezeGauge >= 10000) {
        CsbPlaySfx(0x4a);
    }
    if (prev < 3000 && g_CsbFreezeGauge >= 3000) {
        CsbPlaySfx(0x49);
    }
}

// 7. 0x0041F860 (95B)
CsbFogMgr::~CsbFogMgr() {
    CsbChainElem *elem = this->drawElem;
    CsbChain *chain = g_CsbChain;
    if (elem != NULL) {
        if (g_CsbFlags & 0x8000) {
            EnterCriticalSection(&g_CsbChainCs);
            ++g_CsbChainCsDepth;
        }
        CsbChainCut(chain, elem);
        if (g_CsbFlags & 0x8000) {
            LeaveCriticalSection(&g_CsbChainCs);
            --g_CsbChainCsDepth;
        }
    }
    g_CsbFogMgr = NULL;
}

// 8. 0x00412020 (95B)
inline CsbItemMgr2::CsbItemMgr2() {
    memset(this, 0, sizeof(CsbItemMgr2));
    this->flags |= 2;
    g_CsbItemMgr2 = this;
}

CsbItemMgr2 *CsbCreateItemMgr2() {
    CsbItemMgr2 *mgr = new CsbItemMgr2();
    if (CsbInitItemMgr2(mgr) != 0) {
        if (mgr != NULL) {
            delete mgr;
        }
        return NULL;
    }
    return mgr;
}

// 9. 0x0044F390 (94B)
char *__stdcall CsbReadTextLine(char *src, char *dst, int *remaining) {
    char *p = src;
    while (*p != '\n' && *p != '\r') {
        if (*remaining == 0) {
            return p;
        }
        ++p;
        --(*remaining);
    }
    if (*remaining == 0) {
        return p;
    }
    *p = '\0';
    strcpy(dst, src);
    ++p;
    --(*remaining);
    while ((*p == '\n' || *p == '\r') && *remaining != 0) {
        ++p;
        --(*remaining);
    }
    return p;
}

// 10. 0x00420BF0 (94B)
CsbGuiSubObj *CsbGuiSubObj::Destroy() {
    CsbDeleteVmByIdPtr(&this->vmIds[0]);
    CsbDeleteVmByIdPtr(&this->vmIds[1]);
    CsbDeleteVmByIdPtr(&this->vmIds[2]);
    CsbDeleteVmByIdPtr(&this->vmIds[3]);
    CsbDeleteVmByIdPtr(&this->vmIds[4]);
    CsbDeleteVmByIdPtr(&this->vmIds[5]);
    CsbDeleteVmByIdPtr(&this->vmIds[6]);
    CsbDeleteVmByIdPtr(&this->vmIds[7]);
    CsbDeleteVmByIdPtr(&this->vmIds[8]);
    CsbDeleteVmByIdPtr(&this->vmIds[9]);
    operator delete(this);
    return this;
}

} // namespace th128
