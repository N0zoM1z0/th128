#include "GuiLaserWave.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

inline void GlwChainMgr::Cut(GlwChainElem *elem) {
    if (elem != NULL) {
        if ((g_GlwSupFlags & 0x8000u) != 0) {
            EnterCriticalSection(&g_GlwSupCs0);
            ++g_GlwSupCsDepth0;
        }
        GlwChainCutImpl(this, elem);
        if ((g_GlwSupFlags & 0x8000u) != 0) {
            LeaveCriticalSection(&g_GlwSupCs0);
            --g_GlwSupCsDepth0;
        }
    }
}

void __stdcall GlwDecodeSpellBonus(int *outHi, int *outLo) {
    int enc = g_GlwSpellCard->encodedA8;
    int hi = ((enc / 100) % 1000 + 0x3a6) % 1000;
    int lo = (enc % 100 + 0x43) % 100;
    if ((enc / 100000) - 0x16 != hi + lo) {
        *outHi = 999;
        *outLo = 99;
        return;
    }
    int enc2 = g_GlwSpellCard->encodedA8;
    *outHi = ((enc2 / 100) % 1000 + 0x3a6) % 1000;
    *outLo = (enc2 % 100 + 0x43) % 100;
}

void GlwGui::ReleaseMsgAndSlot27() {
    GlwGui *gui = g_GlwGui;
    if (gui->msgObj5a1c != NULL) {
        GlwDestroyMsgObj(gui->msgObj5a1c);
        gui->msgObj5a1c = NULL;
    }
    if ((g_GlwGameFlags & 9u) != 0) {
        return;
    }
    if (g_GlwAnmMgr->slot27_4e912c != NULL) {
        GlwReleaseAnm(g_GlwAnmMgr->slot27_4e912c);
        ::operator delete(g_GlwAnmMgr->slot27_4e912c);
        g_GlwAnmMgr->slot27_4e912c = NULL;
    }
    gui->field5904 = 0;
    if (gui->msgBuf5a20 != NULL) {
        free(gui->msgBuf5a20);
        gui->msgBuf5a20 = NULL;
    }
    gui->msgBuf5a20 = NULL;
}

void __stdcall GlwAddFreezeGauge(int delta) {
    int prev = g_GlwFreezeGauge;
    if (g_GlwFreezeEmpty != 0) {
        return;
    }
    int next = prev + delta;
    g_GlwFreezeGauge = next;
    if (next / 10000 > prev / 10000) {
        GlwPlaySfx(0x12);
        g_GlwPopupMgr->Show(4, 0);
    }
    if (g_GlwFreezeGauge > 100000) {
        g_GlwFreezeGauge = 100000;
    } else if (g_GlwFreezeGauge < 0) {
        g_GlwFreezeEmpty = 1;
    }
}

void __stdcall GlwAddMotivGauge(int delta) {
    int prev = g_GlwMotivGauge;
    int next = prev + delta;
    g_GlwMotivGauge = next;
    if (next > 30000) {
        next = 30000;
        g_GlwMotivGauge = 30000;
    }
    if (next / 10000 > prev / 10000) {
        GlwPlaySfx(0x48);
        g_GlwPopupMgr->Show(2, 0);
        GlwGuiUpdateMeterAnms(g_GlwPopupMgr);
        return;
    }
    if (next < 0) {
        g_GlwMotivGauge = 0;
    }
}

void GlwConfig::ResetDefaults() {
    memset(this, 0, sizeof(GlwConfig));
    flags38 |= 0x100u;
    f1e = 0;
    version0 = 0x128001u;
    w16 = 0x258;
    h18 = 0x258;
    f1a = 0;
    f1b = 1;
    f1c = 1;
    f1d = 0;
    memcpy(keys4, g_GlwInputKeys, 18);
    f22 = 0;
    f23 = 2;
    f1f = 2;
    f20 = 100;
    f21 = 80;
    posX28 = (int)0x80000000u;
    posY2c = (int)0x80000000u;
}

GlwLaserItemMgr::~GlwLaserItemMgr() {
    g_GlwChainMgr->Cut(chain8);
    g_GlwChainMgr->Cut(chainC);
    g_GlwLaserItemMgr = NULL;
}

int GlwLaserMgr::Spawn(int type, void *initArg) {
    GlwLaserMgr *mgr = g_GlwLaserMgr;
    if (mgr->count5d4 >= 0x100) {
        return 0;
    }
    ++mgr->nextId5d8;
    if (mgr->nextId5d8 < 0x10000) {
        mgr->nextId5d8 = 0x10000;
    }
    GlwLaserBase *laser;
    switch (type) {
    case 0:
        laser = new GlwLaserType0();
        break;
    case 1:
        laser = new GlwLaserType1();
        break;
    case 2:
        laser = new GlwLaserType2();
        break;
    case 3:
        laser = new GlwLaserType3();
        break;
    default:
        return mgr->nextId5d8;
    }
    laser->id80 = mgr->nextId5d8;
    laser->prev4 = mgr->tail5d0;
    mgr->tail5d0->next8 = laser;
    ++mgr->count5d4;
    mgr->tail5d0 = laser;
    laser->Init(initArg);
    return mgr->nextId5d8;
}

void GlwLaserInfinite::Slot01() {
    while (stepIdx594 < 0x12) {
        GlwLaserStep *st = &steps61c[stepIdx594];
        if (st->cmd20 == 0) {
            return;
        }
        if (st->flag24 == 0 && wait598 != 0) {
            return;
        }
        switch (st->cmd20) {
        case 0x80:
            param5b4 = st->param10;
            break;
        case 0x400:
            stateC = 3;
            break;
        case 0x100000:
            if (st->param10 != 0) {
                flagsDe8 = (flagsDe8 & ~0xc0u) | 0x20u;
                ++stepIdx594;
                continue;
            } else {
                flagsDe8 &= ~0xe0u;
                ++stepIdx594;
                continue;
            }
        }
        ++stepIdx594;
    }
}

} // namespace th128
