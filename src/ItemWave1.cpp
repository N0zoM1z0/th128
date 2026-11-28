#include "ItemWave1.hpp"

namespace th128 {

static inline void CutChainLocked(void *elem) {
    Iw1ChainManager *mgr = g_Iw1ChainManager;
    if (elem != 0) {
        if ((g_Iw1SupervisorFlags & 0x8000u) != 0) {
            EnterCriticalSection(&g_Iw1ChainCritSec);
            ++g_Iw1ChainCritSecDepth;
        }
        mgr->Cut(elem);
        if ((g_Iw1SupervisorFlags & 0x8000u) != 0) {
            LeaveCriticalSection(&g_Iw1ChainCritSec);
            --g_Iw1ChainCritSecDepth;
        }
    }
}

int GetFreezeLevel(const Iw1Globals *g) {
    int tableIdx = (g_Iw1Globals.modeFlag != 0) ? 1 : 0;
    for (int i = 0; i < 15; ++i) {
        if (g->freezeRate < (float)g_Iw1FreezeThresholds[tableIdx][i]) {
            return i;
        }
    }
    return 15;
}

void __stdcall AddMotivation(int delta) {
    int prev = g_Iw1Globals.motivation;
    if (g_Iw1Globals.gameOverFlag != 0) {
        return;
    }
    int next = prev + delta;
    g_Iw1Globals.motivation = next;
    if (next / 10000 > prev / 10000) {
        PlaySfx(0x12);
        g_Iw1Gui->ShowPopup(4, 4, 0);
    }
    if (g_Iw1Globals.motivation > 100000) {
        g_Iw1Globals.motivation = 100000;
    } else if (g_Iw1Globals.motivation < 0) {
        g_Iw1Globals.gameOverFlag = 1;
    }
}

void __stdcall AddPerfectFreeze(int delta) {
    int prev = g_Iw1Globals.perfectFreeze;
    int next = prev + delta;
    g_Iw1Globals.perfectFreeze = next;
    if (next > 30000) {
        next = 30000;
        g_Iw1Globals.perfectFreeze = next;
    }
    if (next / 10000 > prev / 10000) {
        PlaySfx(0x48);
        g_Iw1Gui->ShowPopup(2, 2, 0);
        g_Iw1Gui->UpdateMeterAnms();
    } else if (next < 0) {
        g_Iw1Globals.perfectFreeze = 0;
    }
}

int Iw1HelpInf::Initialize(Iw1HelpInf *self) {
    Iw1ChainElem *c = new Iw1ChainElem();
    c->flags = (c->flags & ~2u) | 1u;
    c->callback = (void *)&HelpCalcCb;
    c->onRegister = 0;
    c->onCut = 0;
    c->arg = self;
    RegisterCalcChain(c, 0x0a);
    self->calcChain = c;

    Iw1ChainElem *d = new Iw1ChainElem();
    d->flags = (d->flags & ~2u) | 1u;
    d->callback = (void *)&HelpDrawCb;
    d->onRegister = 0;
    d->onCut = 0;
    d->arg = self;
    RegisterDrawChain(d, 0x40);
    self->drawChain = d;

    StartLoaderThread(&HelpLoaderEntry);
    self->timer.Reset();
    self->state = 0;
    return 0;
}

Iw1ItemManager::~Iw1ItemManager() {
    CutChainLocked(this->calcChain);
    CutChainLocked(this->drawChain);
    g_Iw1ItemManager = 0;
}

void Iw1Item::CollectPoint() {
    int level = GetFreezeLevel(&g_Iw1Globals);
    if (level >= g_Iw1Globals.maxItemCount) {
        ++g_Iw1Globals.score;
        if (g_Iw1Globals.score >= 1000000000) {
            g_Iw1Globals.score = 999999999;
        }
        AddScorePopup(&this->pos, 100, -1);
        return;
    }
    if (g_Iw1Globals.itemCount < g_Iw1Globals.maxItemCount) {
        ++g_Iw1Globals.itemCount;
        if (g_Iw1Globals.itemCount > g_Iw1Globals.maxItemCount) {
            g_Iw1Globals.itemCount = g_Iw1Globals.maxItemCount;
        }
        int div = g_Iw1Globals.itemDivisor;
        if (((g_Iw1Globals.itemCount - 1) / div) != (g_Iw1Globals.itemCount / div)) {
            TriggerItemBonus(g_Iw1Player);
            AddScorePopup(&this->pos, -1, (int)0xffffff40u);
            PlaySfxWithPan(0x0d, this->pos.x);
            g_Iw1Globals.powerMeter += 12;
            if (g_Iw1Globals.powerMeter > 0x400) {
                g_Iw1Globals.powerMeter = 0x400;
            } else if (g_Iw1Globals.powerMeter < -0x400) {
                g_Iw1Globals.powerMeter = -0x400;
            }
        }
    }
}

} // namespace th128
