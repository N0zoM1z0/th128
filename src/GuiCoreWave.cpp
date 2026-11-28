#include "GuiCoreWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

int GcwAsciiInf::Init() {
    asciiAnm1c894 = g_GcwAnmMgr->PreloadAnm(2, "ascii.anm");
    if (asciiAnm1c894 == NULL) {
        g_GcwErrorCtx.Log("ascii anm fail\n");
        return -1;
    }

    GcwChainElem *c = new GcwChainElem();
    c->flags4 = (c->flags4 & ~2u) | 1u;
    c->cb8 = (void *)&GcwAsciiCalc;
    c->onRegC = NULL;
    c->onCut10 = NULL;
    c->arg20 = this;
    g_GcwChain->RegisterCalc(c, 4);
    calcC = c;

    GcwChainElem *d0 = new GcwChainElem();
    d0->flags4 = (d0->flags4 & ~2u) | 1u;
    d0->cb8 = (void *)&GcwAsciiDraw0;
    d0->onRegC = NULL;
    d0->onCut10 = NULL;
    d0->arg20 = this;
    g_GcwChain->RegisterDraw(d0, 0x4a);
    draw10 = d0;

    GcwChainElem *d1 = new GcwChainElem();
    d1->flags4 = (d1->flags4 & ~2u) | 1u;
    d1->cb8 = (void *)&GcwAsciiDraw1;
    d1->onRegC = NULL;
    d1->onCut10 = NULL;
    d1->arg20 = this;
    g_GcwChain->RegisterDraw(d1, 0x2f);
    draw1c8a0 = d1;

    GcwChainElem *d2 = new GcwChainElem();
    d2->flags4 = (d2->flags4 & ~2u) | 1u;
    d2->cb8 = (void *)&GcwAsciiDraw2;
    d2->onRegC = NULL;
    d2->onCut10 = NULL;
    d2->arg20 = this;
    g_GcwChain->RegisterDraw(d2, 0x37);
    draw1c8a4 = d2;

    GcwAnmResource *res = asciiAnm1c894;
    vm14.ResetForReuse();
    vm14.res42c = res;
    vm14.SetSprite(res, 0);

    GcwAnmResource *res2 = asciiAnm1c894;
    vm4fc.ResetForReuse();
    vm4fc.res42c = res2;
    vm4fc.SetSprite(res2, 0x62);
    return 0;
}

void GcwEnemyFlagMgr::Cleanup() {
    if (flag3c != 0 && (g_GcwGrazeA + g_GcwGrazeB * 2) == 5) {
        GcwDeleteVmByIdPtr(&vm40);
        GcwAddGrazeOrPoints(0xa);
    }
    GcwDeleteVmByIdPtr(&vm40);
    GcwDeleteVmByIdPtr(&vm44);
    GcwDeleteVmByIdPtr(&vm48);

    g_GcwChain->Cut(calc8);
    g_GcwChain->Cut(drawC);
    g_GcwChain->Cut(extra554);

    g_GcwEnemyFlagMgr = NULL;
    if (bgMini538 != NULL) {
        delete bgMini538;
    }
    bgMini538 = NULL;
    if (buf534 != NULL) {
        free(buf534);
        buf534 = NULL;
    }
    if (buf4f8 != NULL) {
        free(buf4f8);
    }
    buf4f8 = NULL;
}

void GcwSpellCard::Finish() {
    GcwSpellCard *sc = g_GcwSpellCard;
    if ((sc->flags7c & 1u) == 0) {
        return;
    }
    g_GcwPlayer->flags37f8 |= 1u;
    g_GcwAnmMgr->StopVmById(sc->vm14, 1);
    g_GcwAnmMgr->StopVmById(sc->vm18, 1);
    g_GcwAnmMgr->StopVmById(sc->vm1c, 1);
    sc->flags7c &= ~1u;
    GcwDeleteVmByIdPtr(&sc->vm10);
    sc->flags7c &= ~0x40u;
    GcwDeleteVmByIdPtr(&sc->vm20);

    GcwScoreSpell *sp = &g_GcwScoreFile->spells37a0[sc->spellId78];
    if (sp->seenCount88 < 99999) {
        ++sp->seenCount88;
    }

    if ((sc->flags7c & 2u) != 0) {
        if (g_GcwReplayMgr->mode10 != 1) {
            GcwScoreSpell *sp2 = &g_GcwScoreFile->spells37a0[sc->spellId78];
            if (sp2->clearCount80 < 99999) {
                ++sp2->clearCount80;
            }
        }
        GcwPlaySfx(0x30);
        if ((sc->flags7c & 4u) != 0) {
            g_GcwPopupMgr->Show(7, sc->bonus80);
            GcwScoreSpell *sp3 = &g_GcwScoreFile->spells37a0[sc->spellId78];
            if (sp3->noMissCount84 < 99999) {
                ++sp3->noMissCount84;
            }
        } else {
            g_GcwPopupMgr->Show(8, sc->bonus80);
        }
        g_GcwScore += sc->bonus80 / 10;
        if (g_GcwScore >= 1000000000) {
            g_GcwScore = 999999999;
        }
    } else {
        g_GcwPopupMgr->Show(9, 0);
    }
}

int __stdcall GcwEclFile::LoadDeps(const unsigned int *header) {
    if (header[0] != 0x4d494e41u) {
        return 0;
    }
    const char *p = (const char *)(header + 2);
    for (unsigned int i = 0; i < header[1]; ++i) {
        GcwAnmResource *res;
        if (g_GcwAnmMgr->slots4e90e0[i] != NULL) {
            GcwDebugLog("::preloadAnim already : %s\n", p);
            res = g_GcwAnmMgr->slots4e90e0[i];
        } else {
            res = g_GcwAnmMgr->LoadAnm((int)(i + 8), p);
            if (res != NULL) {
                res->loading124 = 1;
                while ((g_GcwSupFlags & 0x180u) == 0) {
                    Sleep(1);
                    if (res->loading124 == 0) {
                        break;
                    }
                }
                GcwDebugLog("::preloadAnimEnd : %s\n", p);
            }
        }
        g_GcwEnemyMgr->anms84[i] = res;
        if (res == NULL) {
            g_GcwErrorCtx.Log("anim fail\n");
            return -1;
        }
        p += strlen(p) + 1;
    }
    int diff = (int)(p - (const char *)header) % 4;
    if (diff != 0) {
        p += 4 - diff;
    }
    const unsigned int *ecli = (const unsigned int *)p;
    if (ecli[0] == 0x494c4345u) {
        p += 8;
        for (unsigned int i = 0; i < ecli[1]; ++i) {
            LoadInclude(p);
            p += strlen(p) + 1;
        }
    }
    return 0;
}

void GcwEnemyMgr::Cleanup() {
    GcwEnemyMgrResetList(this);
    g_GcwChain->Cut(calc8);
    g_GcwChain->Cut(drawC);

    for (int i = 0; i < 0x20; ++i) {
        if (eclB4->bufsC[i] != NULL) {
            free(eclB4->bufsC[i]);
        }
    }
    if (eclB4 != NULL) {
        delete eclB4;
    }
    eclB4 = NULL;

    for (int i = 8; i < 16; ++i) {
        if (i >= 0 && (unsigned int)i < 0x26u) {
            GcwAnmResource *&slot = g_GcwAnmMgr->slots4e90e0[i - 8];
            if (slot != NULL) {
                slot->Release();
                delete slot;
                slot = NULL;
            }
        }
    }
    g_GcwEnemyMgr = NULL;
}

static GcwAnmVm *GcwFindVmInline(unsigned int id) {
    if (id == 0) {
        return NULL;
    }
    for (GcwAnmNode *n = g_GcwAnmMgr->worldVms8b9704; n != NULL; n = n->next) {
        if (n->vm->id0.id == id) {
            return n->vm;
        }
    }
    for (GcwAnmNode *n = g_GcwAnmMgr->uiVms8b970c; n != NULL; n = n->next) {
        if (n->vm->id0.id == id) {
            return n->vm;
        }
    }
    return NULL;
}

void GcwGui::Reset() {
    if (msgObj5a1c != NULL) {
        GcwDestroyMsgObj(msgObj5a1c);
        msgObj5a1c = NULL;
    }
    if ((g_GcwGameFlags & 9u) == 0) {
        if (msgBuf5a20 != NULL) {
            free(msgBuf5a20);
            msgBuf5a20 = NULL;
        }
        msgBuf5a20 = NULL;
        g_GcwMsgBufGlobal = NULL;
    } else {
        g_GcwMsgBufGlobal = msgBuf5a20;
    }
    if (calc8 != NULL) {
        calc8->flags4 &= ~2u;
    }
    GcwDeleteVmByIdPtr(&vm5888);
    GcwDeleteVmByIdPtr(&vm588c);
    GcwDeleteVmByIdPtr(&vm5898);

    for (int i = 0; i < 10; ++i) {
        vals589c[i] = 0;
    }
    for (int i = 0; i < 10; ++i) {
        GcwAnmVm *vm = GcwFindVmInline(digitVms5860[i].id);
        if (vm != NULL) {
            vm->flags4b0 |= 0x20000000u;
            if (vm->childNode10.prev == NULL) {
                for (GcwAnmNode *n = vm->childNode10.next; n != NULL; n = n->next) {
                    n->vm->flags4b0 |= 0x20000000u;
                }
            }
        }
        digitVms5860[i].id = 0;
    }
    field5914 = 0;
    field5968 = 0;
    field59bc = 0;
    flags5a04 |= 0xe0u;
    field5954 = 0;
    field59a8 = 0;
    field59fc = 0;
}

} // namespace th128
