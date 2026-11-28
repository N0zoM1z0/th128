#include "StageGameWave.hpp"

#include <string.h>

namespace th128 {

inline void SgwChainMgr::Cut(SgwChainElem *elem) {
    if (elem != NULL) {
        if ((g_SgwSupFlags & 0x8000u) != 0) {
            EnterCriticalSection(&g_SgwSupCs0);
            ++g_SgwSupCsDepth0;
        }
        SgwChainCutImpl(this, elem);
        if ((g_SgwSupFlags & 0x8000u) != 0) {
            LeaveCriticalSection(&g_SgwSupCs0);
            --g_SgwSupCsDepth0;
        }
    }
}

int SgwStage::UpdateBgMeshes() {
    for (int i = 0; i < meshHdr10->count0; ++i) {
        SgwMeshEntry *m = meshes14[i];
        if ((m->flags3 & 1) != 0) {
            SgwMeshQuad *q = &m->firstQuad1c;
            int activeCount = 0;
            while (q->type0 >= 0) {
                SgwAnmVm *vm = &bgVms1c8[q->vmIdx6];
                vm->Update();
                if (vm->active424 != 0) {
                    ++activeCount;
                }
                q = (SgwMeshQuad *)((unsigned char *)q + q->stride2);
            }
            if (activeCount == 0) {
                m->flags3 &= ~1u;
            }
        }
    }
    return 0;
}

void SgwStage::ClearBgVms() {
    field2910 = 0;
    SgwAnmVm *vm = g_SgwAnmMgr->GetVmById(vm3834);
    if (vm != NULL) {
        vm->flags4b0 |= 0x20000000u;
        if (vm->childNode10.prev == NULL) {
            for (SgwAnmNode *n = vm->childNode10.next; n != NULL; n = n->next) {
                n->vm->flags4b0 |= 0x20000000u;
            }
        }
    }
    vm = g_SgwAnmMgr->GetVmById(vm3838);
    if (vm != NULL) {
        vm->flags4b0 |= 0x20000000u;
        if (vm->childNode10.prev == NULL) {
            for (SgwAnmNode *n = vm->childNode10.next; n != NULL; n = n->next) {
                n->vm->flags4b0 |= 0x20000000u;
            }
        }
    }
}

void SgwStage::EnableAndResetMeshes() {
    SgwStage *st = g_SgwStage;
    st->chain8->flags4 |= 2u;
    st->chainC->flags4 |= 2u;
    st->chain383c->flags4 |= 2u;

    int vmIdx = 0;
    for (int i = 0; i < st->meshHdr10->count0; ++i) {
        st->meshes14[i]->flags3 = 1;
        SgwMeshQuad *q = &st->meshes14[i]->firstQuad1c;
        while (q->type0 >= 0) {
            SgwAnmVm *vm = &st->bgVms1c8[vmIdx];
            vm->ResetForReuse();
            vm->word4d0 = 0x1010;
            st->bgAnm1c4->BindVmScript(vm, q->script4);
            q->vmIdx6 = (short)vmIdx;
            ++vmIdx;
            q = (SgwMeshQuad *)((unsigned char *)q + q->stride2);
        }
    }
    st->scriptCur4c = st->scriptStart1c;
}

SgwEnemyFlagMgr::SgwEnemyFlagMgr() {
    memset(this, 0, sizeof(SgwEnemyFlagMgr));
    flags0 |= 2u;
    g_SgwEnemyFlagMgr = this;
}

SgwEnemyFlagMgr *SgwEnemyFlagMgr::Create() {
    SgwEnemyFlagMgr *obj = new SgwEnemyFlagMgr();
    if (obj->Register() != 0) {
        if (obj != NULL) {
            obj->Cleanup();
            ::operator delete(obj);
        }
        return NULL;
    }
    return obj;
}

SgwBulletMgr::~SgwBulletMgr() {
    g_SgwChainMgr->Cut(chain8);
    g_SgwChainMgr->Cut(chainC);
    g_SgwAnmMgr->MarkResourceVmsForDelete(bulletAnm8a7f9c);
    g_SgwBulletMgr = NULL;
}

int SgwEffect::Update() {
    motion4.UpdateVelocity();
    motion4.UpdatePos();
    if (timer3c.current >= (g_SgwScoreState->val6b200 - 2000) / 80) {
        g_SgwAnmMgr->StopVmById(vm50, 1);
        if (CheckExpire() != 0) {
            SgwPlaySfx(0x47);
            active0 = 0;
            return 1;
        }
        if (g_SgwAnmMgr->GetVmById(vm50) == NULL) {
            vm50 = 0;
            active0 = 0;
            return 1;
        }
        SgwPlaySfx(0x47);
        active0 = 0;
        return 1;
    }
    timer3c.Increment();
    return 0;
}

void __stdcall SgwEnemyMgr::ResetList() {
    for (int i = 0; i < 8; ++i) {
        void *res = g_SgwAnmMgr->slots4e90e0[i];
        for (SgwAnmNode *n = g_SgwAnmMgr->world8b9704; n != NULL; ) {
            SgwAnmNode *next = n->next;
            SgwAnmVm *vm = n->vm;
            if (vm->res42c == res) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
        for (SgwAnmNode *n = g_SgwAnmMgr->ui8b970c; n != NULL; ) {
            SgwAnmNode *next = n->next;
            SgwAnmVm *vm = n->vm;
            if (vm->res42c == res) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
    }
    for (SgwEnemyNode *n = listB8; n != NULL; ) {
        SgwEnemyNode *next = n->next;
        if (n->enemy != NULL) {
            n->enemy->ScalarDtor(1);
        }
        n = next;
    }
    countC4 = 0;
    boss5c = NULL;
    if (chain8 != NULL) {
        chain8->flags4 &= ~2u;
    }
    if (chainC != NULL) {
        chainC->flags4 &= ~2u;
    }
}

SgwGameMgr::SgwGameMgr() {
    memset(this, 0, sizeof(SgwGameMgr));
    flags0 |= 2u;
    g_SgwGameMgr = this;
}

int SgwGameMgr::InitSubsystems() {
    SgwGameMgr *gm = g_SgwGameMgr;
    SgwCreatePlayer();
    SgwCreateBulletMgr();
    SgwCreateLaserMgr();
    SgwCreateSub443b40();
    SgwCreateEnemyMgr(gm->eclNames34[gm->cursor114.cur]);
    SgwCreateGui();
    int *ecl = (int *)g_SgwEnemyMgr->eclB4;
    gm->cursor1ec.count = ecl[2];
    gm->cursor1ec.Move(1);
    gm->cursor1ec.Move(-1);
    gm->ready30 = 1;
    return 0;
}

SgwItemMgr::~SgwItemMgr() {
    g_SgwAnmMgr->MarkResourceVmsForDelete(itemAnm10);
    g_SgwChainMgr->Cut(chain8);
    g_SgwChainMgr->Cut(chainC);
    g_SgwItemMgr = NULL;
}

void SgwMeshStrip::CopyVertices() {
    if (rows0 == 0) {
        return;
    }
    SgwVertex28 *src = verts10;
    for (int r = 0; r < rows0 - 1; ++r) {
        SgwVertex28 *dst = (SgwVertex28 *)vmsC[r]->vbuf4ac;
        for (int c = 0; c < cols4; ++c) {
            dst[0] = src[0];
            dst[1] = src[cols4];
            dst += 2;
            ++src;
        }
    }
}

int SgwEnding::Step() {
    SgwEndingInner *in = inner18;
    if (in->CheckDone() == 0) {
        in->timer4.Increment();
        ++frames24;
        if ((inner18->flags74 & 4u) == 0 && (flags20 & 2u) == 0 && (inner18->flags74 & 2u) != 0 &&
            (g_SgwInputHeld & 0x200u) != 0) {
            return (frames24 % 12 != 0) ? 6 : 1;
        }
        return 1;
    }
    g_SgwSceneNext = (g_SgwSupFlags & 0x2000u) ? 2 : 0x10;
    return 1;
}

} // namespace th128
