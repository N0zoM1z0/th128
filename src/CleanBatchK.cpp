#include "CleanBatchK.hpp"
#include <string.h>

namespace th128 {

void CbkUpdateScoreDisplay() {
    CbkGui* gui = g_CbkGui;
    int cur = gui->displayScore;
    if (g_CbkScore != cur) {
        int step = (g_CbkScore - cur) / 32;
        if (step >= 0x8d55e) {
            step = 0x8d55e;
        } else if (step == 0) {
            step = 1;
        }
        if (gui->scoreStep < step) {
            gui->scoreStep = step;
        }
        int diff = g_CbkScore - cur;
        if (gui->scoreStep > diff) {
            gui->scoreStep = diff;
        }
        gui->displayScore += gui->scoreStep;
        if (gui->displayScore >= g_CbkScore) {
            gui->scoreStep = 0;
        }
    }
    if (g_CbkHighScore < gui->displayScore) {
        g_CbkHighScore = gui->displayScore;
        g_CbkHighScoreStage = g_CbkStage;
        g_CbkStageFlags |= 4;
        if ((g_CbkStageFlags & 4) == 0) {
            CbkShowPopup(gui, 3, 3, 0);
        }
    }
    if ((gui->flags5a04 & 0x10) == 0) {
        int threshold = (g_CbkDifficulty != 4)
            ? g_CbkNormalExtends[g_CbkExtendIdx]
            : g_CbkExtraExtends[g_CbkExtendIdx];
        if (gui->displayScore >= threshold) {
            CbkOnExtend(&g_CbkHighScore);
            ++g_CbkExtendIdx;
        }
    }
}

CbkEndingSub::~CbkEndingSub() {
    thread.Reset();
    CbkAnmMgr* mgr = g_CbkAnmMgr;
    for (int i = 0; i < 5; ++i) {
        CbkAnmVm* vm = mgr->FindVmInline(anmIds[i]);
        if (vm != 0) {
            vm->flags4b0 |= 0x20000000u;
            if (vm->parent18 == 0) {
                for (CbkAnmListNode* c = vm->children14; c != 0; c = c->next) {
                    c->vm->flags4b0 |= 0x20000000u;
                }
            }
        }
        anmIds[i] = 0;
    }
}

int CbkGui::Draw() {
    for (int i = 0; i < 8; ++i) {
        g_CbkAnmMgr->DrawVm(vmsA[i]);
    }
    for (int i = 0; i < 8; ++i) {
        g_CbkAnmMgr->DrawVm(vmsB[i]);
    }
    if (g_CbkEnemyMgr != 0 && g_CbkEnemyMgr->boss != 0) {
        unsigned int f = g_CbkEnemyMgr->boss->flags50ac;
        if (((~(f >> 5)) & 1) && ((~f) & 1)) {
            g_CbkAnmMgr->DrawVm(vm5378);
        }
    }
    return 1;
}

void CbkStageClear::ClearVms() {
    CbkAnmMgr* mgr = g_CbkAnmMgr;
    field2910 = 0;
    CbkAnmVm* vm = mgr->FindVm(vmId3834);
    if (vm != 0) {
        vm->flags4b0 |= 0x20000000u;
        if (vm->parent18 == 0) {
            for (CbkAnmListNode* c = vm->children14; c != 0; c = c->next) {
                c->vm->flags4b0 |= 0x20000000u;
            }
        }
    }
    vm = mgr->FindVm(vmId3838);
    if (vm != 0) {
        vm->flags4b0 |= 0x20000000u;
        if (vm->parent18 == 0) {
            for (CbkAnmListNode* c = vm->children14; c != 0; c = c->next) {
                c->vm->flags4b0 |= 0x20000000u;
            }
        }
    }
}

int __stdcall CbkPlayMusicTrack(int idx, const char* path) {
    char buf[0x100];
    strcpy(buf, path);
    char* dot = strchr(buf, '.');
    dot[1] = 'w';
    dot[2] = 'a';
    dot[3] = 'v';
    g_CbkScoreMgr->musicUnlocked[idx] = 1;
    g_CbkSoundMgr.PlayBgm(buf, 2, -1);
    return 0;
}

void CbkInitConfig(CbkConfig* cfg) {
    memset(cfg, 0, sizeof(CbkConfig));
    cfg->flags38 |= 0x100;
    cfg->b1e = 0;
    cfg->version = 0x128001;
    cfg->w16 = 600;
    cfg->w18 = 600;
    cfg->b1a = 0;
    cfg->b1b = 1;
    cfg->b1c = 1;
    cfg->b1d = 0;
    memcpy(cfg->keyMap, g_CbkDefaultKeys, 18);
    cfg->w22 = 0x200;
    cfg->b1f = 2;
    cfg->b20 = 100;
    cfg->b21 = 80;
    cfg->u28 = 0x80000000u;
    cfg->u2c = 0x80000000u;
}

int CbkEndingObj::OnCalc() {
    CbkEndingSubObj* sub = sub18;
    if (CbkUpdateEndingSub(sub) != 0) {
        g_CbkNextMode = (g_CbkSupFlags & 0x2000) ? 2 : 16;
        return 1;
    }
    CbkTickTimer(&sub->timer4);
    ++frame24;
    unsigned int sf = sub18->flags74;
    if ((sf & 4) == 0 && (flags20 & 2) == 0 && (sf & 2) != 0 && (g_CbkInput & 0x200) != 0) {
        if ((frame24 % 12) != 0) {
            return 6;
        }
    }
    return 1;
}

} // namespace th128
