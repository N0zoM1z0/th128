#include "MediumCleanWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

static __forceinline void McwCutChainElem(McwChainElem *elem) {
    McwChain *chain = g_McwChain;
    if (elem != NULL) {
        if (g_McwFlags & 0x8000) {
            EnterCriticalSection(&g_McwChainCs);
            ++g_McwChainCsDepth;
        }
        McwChainCut(chain, elem);
        if (g_McwFlags & 0x8000) {
            LeaveCriticalSection(&g_McwChainCs);
            --g_McwChainCsDepth;
        }
    }
}

// 1. 0x00461A00 (242B)
int McwAnmManager::DrawAnmVmVertices(McwAnmVm *vm, const void *verts, int count) {
    if (this->batchCount != 0) {
        this->FlushSpriteBatch();
    }
    if (this->fvfState != 3) {
        g_McwDevice->SetFVF(0x144);
        this->fvfState = 3;
    }
    this->ApplyState(vm);
    void **texSlot = vm->sprite->texSlot;
    if (this->curTexSlot != texSlot) {
        this->curTexSlot = texSlot;
        g_McwDevice->SetTexture(0, *texSlot);
    }
    g_McwAnmMgr->FlushSpriteBatch();
    g_McwDevice->SetRenderState(14, 0);
    if (this->fvfState != 1) {
        g_McwDevice->SetTextureStageState(0, 6, 0);
        g_McwDevice->SetTextureStageState(0, 3, 0);
        this->fvfState = 1;
    }
    g_McwDevice->DrawPrimitiveUP(6, count - 2, verts, 0x1c);
    return 0;
}

// 2. 0x00452230 (224B)
bool McwTextImage::UploadToSurface(McwD3dSurface *surf) {
    if (this->hBitmap == NULL) {
        return false;
    }
    McwSurfaceDesc desc;
    surf->GetDesc(&desc);
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = this->width;
    rc.bottom = this->height;
    McwLockedRect locked;
    if (surf->LockRect(&locked, &rc, 0) != 0) {
        return false;
    }
    int dstPitch = locked.Pitch;
    int srcPitch = this->stride;
    unsigned char *src = this->bits;
    unsigned char *dst = (unsigned char *)locked.pBits;
    if (desc.Format == this->fmt) {
        for (int y = 0; y < this->height; ++y) {
            memcpy(dst, src, srcPitch);
            src += srcPitch;
            dst += dstPitch;
        }
    }
    surf->UnlockRect();
    return true;
}

// 3. 0x00452150 (224B)
bool McwTextImage::DownloadFromSurface(McwD3dSurface *surf) {
    if (this->hBitmap == NULL) {
        return false;
    }
    McwSurfaceDesc desc;
    surf->GetDesc(&desc);
    RECT rc;
    rc.left = 0;
    rc.top = 0;
    rc.right = this->width;
    rc.bottom = this->height;
    McwLockedRect locked;
    surf->LockRect(&locked, &rc, 0x10);
    int dstPitch = this->stride;
    int srcPitch = locked.Pitch;
    unsigned char *dst = this->bits;
    unsigned char *src = (unsigned char *)locked.pBits;
    if (desc.Format == this->fmt) {
        for (int y = 0; y < this->height; ++y) {
            memcpy(dst, src, dstPitch);
            src += srcPitch;
            dst += dstPitch;
        }
    }
    surf->UnlockRect();
    return true;
}

// 4. 0x00457C40 (223B)
int McwSoundManager::FindBgmIndex(const char *path) {
    char nameBuf[128];
    int idx = 0;
    const char *p = strrchr(path, '/');
    if (p != NULL || (p = strrchr(path, '\\')) != NULL) {
        strcpy(nameBuf, p + 1);
    } else {
        strcpy(nameBuf, path);
    }
    McwBgmEntry *tbl = this->bgmTable;
    while (tbl[idx].name[0] != '\0') {
        if (strcmp(tbl[idx].name, nameBuf) == 0) {
            break;
        }
        ++idx;
    }
    if (tbl[idx].name[0] == '\0') {
        return 0;
    }
    return idx;
}

// 5. 0x00410EA0 (222B)
void McwStageObj::PauseAllStageAnmsAndEnemies() {
    McwAnmManager *anm = g_McwAnmMgr;
    for (int i = 0; i < 8; ++i) {
        void *res = anm->stageAnms[i];
        McwVmNode *node = anm->worldListHead;
        while (node != NULL) {
            McwVmNode *next = node->next;
            McwAnmVm *vm = node->vm;
            if (vm->anmRes == res) {
                vm->flags |= 0x20000000;
            }
            node = next;
        }
        node = anm->uiListHead;
        while (node != NULL) {
            McwVmNode *next = node->next;
            McwAnmVm *vm = node->vm;
            if (vm->anmRes == res) {
                vm->flags |= 0x20000000;
            }
            node = next;
        }
    }
    McwEnemyNode *en = this->enemyListHead;
    while (en != NULL) {
        McwEnemyNode *next = en->next;
        if (en->enemy != NULL) {
            en->enemy->Pause(1);
        }
        en = next;
    }
    this->fc4 = 0;
    this->f5c = 0;
    if (this->calcElem != NULL) {
        this->calcElem->flags &= ~2u;
    }
    if (this->drawElem != NULL) {
        this->drawElem->flags &= ~2u;
    }
}

// 6. 0x00408370 (220B)
McwBulletManager::~McwBulletManager() {
    McwCutChainElem(this->calcElem);
    McwCutChainElem(this->drawElem);
    g_McwAnmMgr->MarkResourceVmsForDelete(this->anmRes);
    g_McwBulletMgr = NULL;
}

// 7. 0x00443A60 (209B)
McwSoundTest::~McwSoundTest() {
    McwCutChainElem(this->calcElem);
    McwCutChainElem(this->drawElem);
    g_McwSoundTest = NULL;
    if (this->buf4c4 != NULL) {
        free(this->buf4c4);
        this->buf4c4 = NULL;
    } else {
        this->buf4c4 = NULL;
    }
}

// 8. 0x00428430 (203B)
McwItemManager::~McwItemManager() {
    McwCutChainElem(this->calcElem);
    McwCutChainElem(this->drawElem);
    g_McwItemMgr = NULL;
}

// 9. 0x00442BE0 (200B)
void McwScoreHeader::InitHeader() {
    memset(this, 0, sizeof(McwScoreHeader));
    this->magic = 0x25453;
    this->size = sizeof(McwScoreHeader);
    strcpy(this->name, "        ");
    unsigned short *p = this->rndData;
    for (int i = 0; i < 0x1ee; ++i, ++p) {
        if (g_McwFlags & 0x8000) {
            EnterCriticalSection(&g_McwRngCs);
            ++g_McwRngCsDepth;
        }
        unsigned short v = (g_McwRngSeed ^ 0x9630) - 0x6553;
        ++g_McwRngCount;
        g_McwRngSeed = (unsigned short)((v >> 14) + (v << 2));
        if (g_McwFlags & 0x8000) {
            LeaveCriticalSection(&g_McwRngCs);
            --g_McwRngCsDepth;
        }
        *p = g_McwRngSeed;
    }
}

// 10. 0x004511D0 (173B)
bool McwMemStream::LoadFromResource(const char *resName, int unusedFlag) {
    (void)unusedFlag;
    this->Close();
    HRSRC hRes = FindResourceA(NULL, resName, RT_RCDATA);
    if (hRes != NULL) {
        HGLOBAL hMem = LoadResource(NULL, hRes);
        if (hMem != NULL) {
            void *pData = LockResource(hMem);
            if (pData == NULL) {
                FreeResource(NULL);
                this->Close();
                return true;
            }
            DWORD sz = SizeofResource(NULL, hRes);
            this->size = sz;
            void *buf = malloc(sz);
            this->base = buf;
            if (buf == NULL) {
                this->Close();
                return true;
            }
            memcpy(buf, pData, sz);
            this->cur = this->base;
            FreeResource(hMem);
            return true;
        }
    }
    this->Close();
    return true;
}

} // namespace th128
