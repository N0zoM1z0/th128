#include "CleanBatchL.hpp"
#include <string.h>

namespace th128 {

int CblReplay::StepInput() {
    if (g_CblReplayActive == 0) {
        return 1;
    }
    if (stages[curStage3ac].frame < 0) {
        g_CblInput0 = 0;
        g_CblInputC = 0;
        g_CblInput10 = 0;
        return 1;
    }
    if (curStage3ac < 0) {
        g_CblInput0 = 0;
        g_CblInputC = 0;
        g_CblInput10 = 0;
        ++totalFrames3a4;
        return 1;
    }
    g_CblInput4 = g_CblInput0;
    if (stages[curStage3ac].frame < stages[curStage3ac].header->frameCount) {
        unsigned short* p = stages[curStage3ac].inputPtr;
        if (p[0] == 0xFFFF && p[1] == 0xFFFF && p[2] == 0xFFFF) {
            g_CblInput0 = 0;
            g_CblInputC = 0;
            g_CblInput10 = 0;
            CblOnReplayEnd();
            return 1;
        }
        g_CblInput0 = p[0];
        g_CblInputC = stages[curStage3ac].inputPtr[1];
        g_CblInput10 = stages[curStage3ac].inputPtr[2];
        g_CblInputObj.Update();
        slowByte3a0 = *stages[curStage3ac].slowPtr;
        stages[curStage3ac].inputPtr += 3;
        if ((totalFrames3a4 % 30) == 0) {
            ++stages[curStage3ac].slowPtr;
        }
    } else {
        g_CblInput0 = 0;
        g_CblInputC = 0;
        g_CblInput10 = 0;
    }
    ++stages[curStage3ac].frame;
    ++totalFrames3a4;
    return 1;
}

void CblPauseMenu::SetupNameEntry() {
    if (g_CblMode == 0x10 && field1f4 != 0) {
        g_CblModePtr = &g_CblData1;
        g_CblMode = 0x17;
        g_CblMode2 = 0x17;
    }
    int rank = CblFindScoreRank(&g_CblScoreMgr->entries[g_CblDiff]);
    if (g_CblMode == 0x10 && field1f4 != 0) {
        g_CblModePtr = &g_CblData2;
        g_CblMode = 0x10;
        g_CblMode2 = 0x10;
    }
    if (rank < 0) {
        field1f8 = 1;
        return;
    }
    field40 = 0x19;
    field108 = 1;
    if (field40 != 0 && rank >= field40) {
        rank = field40 - 1;
    }
    field38 = rank;
    if (field118 == 0) {
        field110 = 0;
    } else if (field118 > 0) {
        field110 = 0;
    } else {
        field110 = field118 - 1;
    }
    field118 = 0x5b;
    field1e0 = 1;
    strcpy(name2cc, g_CblScoreMgr->lastName3374);
    nameLen1f0 = 0;
    if (strcmp(name2cc, "        ") != 0) {
        CblPlaySe(&field110, -1);
    }
    int len = 8;
    while (len > 0 && name2cc[len - 1] == ' ') {
        --len;
    }
    nameLen1f0 = len;
    field1f8 = 0;
}

void CblStageBgInit() {
    CblStageBg* bg = g_CblStageBg;
    bg->chain8->flags4 |= 2;
    bg->chainC->flags4 |= 2;
    bg->chain383c->flags4 |= 2;
    int vmIdx = 0;
    for (int i = 0; i < bg->header10->count; ++i) {
        bg->objs14[i]->flag3 = 1;
        CblBgQuad* q = (CblBgQuad*)((char*)bg->objs14[i] + 0x1c);
        while (q->type >= 0) {
            CblBgVm* vm = &bg->vms1c8[vmIdx];
            CblInitVm(bg->anm1c4, vm, q->scriptId4);
            vm->w4d0 = 0x1010;
            CblUpdateVm(vm);
            q->vmIdx6 = (short)vmIdx;
            q = (CblBgQuad*)((char*)q + q->size2);
            ++vmIdx;
        }
    }
    bg->field4c = bg->field1c;
}

void __stdcall CblEnemyMgrClearAll(CblEnemyMgr* self) {
    CblAnmMgr* mgr = g_CblAnmMgr;
    for (int i = 0; i < 8; ++i) {
        unsigned int id = mgr->slotIds[i];
        for (CblAnmNode* n = mgr->listA; n != 0; ) {
            CblAnmNode* next = n->next;
            CblAnmVm* vm = n->vm;
            if (vm->tag42c == id) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
        for (CblAnmNode* n = mgr->listB; n != 0; ) {
            CblAnmNode* next = n->next;
            CblAnmVm* vm = n->vm;
            if (vm->tag42c == id) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
    }
    for (CblEnemyNode* n = self->enemiesB8; n != 0; ) {
        CblEnemyNode* next = n->next;
        if (n->obj != 0) {
            n->obj->V5(1);
        }
        n = next;
    }
    self->fieldC4 = 0;
    self->field5c = 0;
    if (self->chain8 != 0) {
        self->chain8->flags4 &= ~2u;
    }
    if (self->chainC != 0) {
        self->chainC->flags4 &= ~2u;
    }
}

void CblSwapKey(int slot, int newKey, CblKeyConfig* cfg) {
    if (cfg->keys[slot] == newKey) {
        return;
    }
    for (int i = 0; i < 5; ++i) {
        if (i != slot && cfg->keys[i] == newKey) {
            cfg->keys[i] = cfg->keys[slot];
        }
    }
    cfg->keys[slot] = (short)newKey;
    CblSaveConfig(cfg);
    g_CblSeMgr.PlaySe(7);
}

void __stdcall CblDecodeFreezePercent(int* outWhole, int* outFrac) {
    int v = g_CblGame2->valA8;
    int r = v % 100;
    int a = ((v / 100) % 1000 + 934) % 1000;
    int b = (r + 67) % 100;
    if (v / 100000 - 22 != a + b) {
        *outWhole = 999;
        *outFrac = 99;
        return;
    }
    int v2 = g_CblGame2->valA8;
    *outWhole = ((v2 / 100) % 1000 + 934) % 1000;
    *outFrac = ((v2 % 100) + 67) % 100;
}

int CblScoreTable::GetStageValue(int mode, int idx) {
    switch (mode) {
    case 0:
        return arr16c[idx].value;
    case 1:
        return (idx == 0) ? arr16c[0].value : arr16c[idx + 2].value;
    case 2:
        return arr220[idx].value;
    case 3:
        return (idx == 0) ? arr220[0].value : arr220[idx + 2].value;
    case 4:
        return arr2d4[idx].value;
    case 5:
        return (idx == 0) ? arr2d4[0].value : arr2d4[idx + 2].value;
    case 6:
        return field388;
    default:
        return field148;
    }
}

} // namespace th128
