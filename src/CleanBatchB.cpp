#include "CleanBatchB.hpp"
#include <string.h>

CbbAnmMgr *g_CbbAnmMgr;
int g_CbbTblA[16];
int g_CbbTblB[16];
int g_CbbTblC[16];
int g_CbbTblD[16];
CbbEndingMgr *g_CbbEndingMgr;
int g_CbbMotiv;
int g_CbbMotivLocked;
CbbGui *g_CbbGui;
CbbScoreMgr *g_CbbScoreMgr;
unsigned char g_CbbDefaultCfg[18];
CbbReplayMenu *g_CbbReplayMenu;

// 1. 0x00410ea0 (222B)
void __stdcall CbbCleanupEnemies(CbbEnemyMgr *mgr) {
    CbbAnmMgr *am = g_CbbAnmMgr;
    for (int i = 0; i < 8; ++i) {
        void *f = am->enemyFiles[i];
        for (CbbListNode *n = am->worldHead; n != 0; ) {
            CbbListNode *next = n->next;
            CbbAnmVm *vm = (CbbAnmVm *)n->data;
            if (vm->anmFile == f) {
                vm->flags |= 0x20000000U;
            }
            n = next;
        }
        for (CbbListNode *n = am->uiHead; n != 0; ) {
            CbbListNode *next = n->next;
            CbbAnmVm *vm = (CbbAnmVm *)n->data;
            if (vm->anmFile == f) {
                vm->flags |= 0x20000000U;
            }
            n = next;
        }
    }
    for (CbbListNode *n = mgr->listHead; n != 0; ) {
        CbbListNode *next = n->next;
        CbbEnemyItem *e = (CbbEnemyItem *)n->data;
        if (e != 0) {
            e->Destroy(1);
        }
        n = next;
    }
    mgr->fieldc4 = 0;
    mgr->field5c = 0;
    if (mgr->holder8 != 0) {
        mgr->holder8->flags &= ~2U;
    }
    if (mgr->holderC != 0) {
        mgr->holderC->flags &= ~2U;
    }
}

// 2. 0x0046dc90 (193B)
HRESULT CbbWaveFile::Read(unsigned char *buf, DWORD sizeToRead, DWORD *sizeRead) {
    if (fromMemory != 0) {
        if (memCur == 0) {
            return (HRESULT)0x800401f0;
        }
        if (sizeRead != 0) {
            *sizeRead = 0;
        }
        if (memCur + sizeToRead > memBase + memSize) {
            sizeToRead = (DWORD)(memBase - memCur + memSize);
        }
        memcpy(buf, memCur, sizeToRead);
        memCur += sizeToRead;
        if (sizeRead != 0) {
            *sizeRead = sizeToRead;
        }
        return S_OK;
    } else {
        if (fileHandle == 0) {
            return (HRESULT)0x800401f0;
        }
        if (buf == 0 || sizeRead == 0) {
            return E_INVALIDARG;
        }
        DWORD n = sizeToRead;
        if (sizeToRead > remBytes) {
            n = remBytes;
        }
        remBytes -= n;
        DWORD got;
        ReadFile(fileHandle, buf, n, &got, 0);
        *sizeRead = got;
        return S_OK;
    }
}

// 3. 0x0043c280 (162B)
int CbbStageLoader::Load(const char *path) {
    hdr = (CbbStageTableHdr *)CbbOpenFile(path, 0, 0);
    if (hdr == 0) {
        return -1;
    }
    for (int i = 0; i < (int)hdr->count; ++i) {
        hdr->entries[i].offset += (unsigned int)hdr;
        CbbStageRec *rec = (CbbStageRec *)hdr->entries[i].offset;
        while (rec->type >= 0) {
            rec->f24 = g_CbbTblA[rec->f24];
            rec->f28 = g_CbbTblB[rec->f28];
            rec->f2c = g_CbbTblC[rec->f2c];
            rec->f30 = g_CbbTblD[rec->f30];
            ++rec;
        }
    }
    return 0;
}

// 4. 0x00410fa0 (155B)
CbbEndingMgr::CbbEndingMgr() {
    memset(this, 0, sizeof(CbbEndingMgr));
    flags |= 2;
    g_CbbEndingMgr = this;
}

// 5. 0x00427260 (152B)
void __stdcall CbbAddMotivation(int delta) {
    if (g_CbbMotivLocked != 0) {
        return;
    }
    int oldVal = g_CbbMotiv;
    int newVal = oldVal + delta;
    g_CbbMotiv = newVal;
    if (newVal / 10000 > oldVal / 10000) {
        CbbPlaySfx(0x12);
        g_CbbGui->ShowBanner(4, 4, 0);
    }
    if (g_CbbMotiv > 100000) {
        g_CbbMotiv = 100000;
    } else if (g_CbbMotiv < 0) {
        g_CbbMotivLocked = 1;
    }
}

// 6. 0x004433a0 (142B)
CbbScoreMgr::CbbScoreMgr() {
    memset(this, 0, sizeof(CbbScoreMgr));
    flags |= 2;
    g_CbbScoreMgr = this;
}

// 7. 0x00442630 (137B)
CbbListNode *CbbReplayMgr::AllocStageBlock(int stage) {
    CbbReplayBlock *blk = new CbbReplayBlock();
    CbbListNode *cur = (CbbListNode *)&lists[stage];
    CbbListNode *n = &blk->node;
    while (cur->next != 0) {
        cur = cur->next;
    }
    if (cur->next != 0) {
        n->next = cur->next;
        cur->next->prev = n;
    }
    cur->next = n;
    n->prev = cur;
    ++blockCount;
    return n;
}

// 8. 0x004277c0 (119B)
void CbbSubObj::Init() {
    memset(this, 0, sizeof(CbbSubObj));
    flags38 |= 0x100;
    b1e = 0;
    version = 0x128001;
    w16 = 600;
    w18 = 600;
    b1a = 0;
    b1b = 1;
    b1c = 1;
    b1d = 0;
    memcpy(cfg, g_CbbDefaultCfg, 18);
    w22 = 0x200;
    b1f = 2;
    b20 = 100;
    b21 = 80;
    f28 = 0x80000000U;
    f2c = 0x80000000U;
}

// 9. 0x00436400 (104B)
CbbReplayMenu::CbbReplayMenu() {
    memset(this, 0, sizeof(CbbReplayMenu));
    flags |= 2;
    g_CbbReplayMenu = this;
}

// 10. 0x00445010 (99B)
static inline CbbAnmVm *CbbFindChild(CbbAnmVm *vm, int scriptId) {
    for (CbbListNode *n = &vm->listNode; n != 0; n = n->next) {
        CbbAnmVm *c = (CbbAnmVm *)n->data;
        if (c->scriptId == scriptId) {
            return c;
        }
        if (scriptId == -1 && c != vm) {
            return c;
        }
    }
    return 0;
}

void CbbTitleScreen::ClearChildFlag(int slot, int scriptId) {
    CbbAnmVm *vm = g_CbbAnmMgr->FindVm(anmIds[slot]);
    if (vm == 0) {
        anmIds[slot] = 0;
    }
    CbbFindChild(vm, scriptId)->flags &= ~2U;
}

// 11. 0x00444fa0 (99B)
void CbbTitleScreen::SetChildFlag(int scriptId) {
    CbbAnmVm *vm = g_CbbAnmMgr->FindVm(anmId610);
    if (vm == 0) {
        anmId610 = 0;
    }
    CbbFindChild(vm, scriptId)->flags |= 2U;
}
