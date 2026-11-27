#include "ZeroCallMedium13.hpp"

#include <string.h>
#include <windows.h>

Zc13GuiCounter *g_Zc13Gui;
int g_Zc13Diff;
int g_Zc13RankThresholds[2][15];
unsigned int g_Zc13CritFlags;
unsigned char g_Zc13CritSec[0x3a];
unsigned char g_Zc13CritDepth;
Zc13LzssNode g_Zc13LzssTree[4096];
float g_Zc13TimeScale;

void __stdcall Zc13DecodeScoreCounter(int *outA, int *outB) {
    Zc13GuiCounter *gui = g_Zc13Gui;
    int enc = gui->encoded;
    int a = (enc / 100 % 1000 + 934) % 1000;
    int b = (enc % 100 + 67) % 100;
    if (enc / 100000 - 22 != a + b) {
        *outA = 999;
        *outB = 99;
        return;
    }
    int enc2 = gui->encoded;
    *outA = (enc2 / 100 % 1000 + 934) % 1000;
    *outB = (enc2 % 100 + 67) % 100;
}

int Zc13GetRankIndex(const Zc13RankObj *obj) {
    int tbl = (g_Zc13Diff != 0) ? 1 : 0;
    for (int i = 0; i < 15; ++i) {
        if (obj->val < (float)g_Zc13RankThresholds[tbl][i]) {
            return i;
        }
    }
    return 15;
}

int __stdcall Zc13AabbOverlap(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh) {
    if (ax - aw * 0.5 > bx + bw * 0.5) {
        return 0;
    }
    if (ax + aw * 0.5 < bx - bw * 0.5) {
        return 0;
    }
    if (ay - ah * 0.5 > by + bh * 0.5) {
        return 0;
    }
    if (ay + ah * 0.5 < by - bh * 0.5) {
        return 0;
    }
    return 1;
}

unsigned int Zc13RngNextU32(Zc13Rng *rng) {
    if ((g_Zc13CritFlags & 0x8000u) != 0) {
        EnterCriticalSection((CRITICAL_SECTION *)g_Zc13CritSec);
        ++g_Zc13CritDepth;
    }
    unsigned short s1 = (unsigned short)((rng->state ^ 0x9630u) - 0x6553u);
    unsigned short hi = s1;
    unsigned short next1 = (unsigned short)((hi >> 14) + (hi << 2));
    ++rng->count;
    unsigned short s2 = (unsigned short)((next1 ^ 0x9630u) - 0x6553u);
    unsigned short lo = s2;
    rng->state = (unsigned short)((lo >> 14) + (lo << 2));
    ++rng->count;
    if ((g_Zc13CritFlags & 0x8000u) != 0) {
        LeaveCriticalSection((CRITICAL_SECTION *)g_Zc13CritSec);
        --g_Zc13CritDepth;
    }
    return ((unsigned int)hi << 16) | (unsigned int)lo;
}

Zc13IceNode *__stdcall Zc13FindCollidingIce(unsigned int cx, unsigned int cy, Zc13IceGrid *grid, const Zc13Float2 *pos, float radius) {
    if (cx > 7 || cy > 9) {
        return 0;
    }
    for (Zc13IceNode *n = grid->cells[cx][cy].head; n != 0; n = n->next) {
        if (n->f38 <= 0 && n->f24 > 5) {
            float nx = n->x;
            float ny = n->y;
            float nr = n->radius;
            float px = pos->x;
            float py = pos->y;
            float dx = px - nx;
            float dy = py - ny;
            float sumR = nr + radius;
            if (dx * dx + dy * dy <= sumR * sumR) {
                return n;
            }
        }
    }
    return 0;
}

Zc13BossSlot::Zc13BossSlot() {
    memset(this, 0, sizeof(Zc13BossSlot));
    f374 = -1;
}

Zc13BossStruct::Zc13BossStruct() {
}

void Zc13LzssReplaceNode(int oldIdx, int newIdx) {
    int parent = g_Zc13LzssTree[oldIdx].parent;
    if (g_Zc13LzssTree[parent].left == oldIdx) {
        g_Zc13LzssTree[parent].left = newIdx;
    } else {
        g_Zc13LzssTree[parent].right = newIdx;
    }
    g_Zc13LzssTree[newIdx].parent = g_Zc13LzssTree[oldIdx].parent;
    g_Zc13LzssTree[newIdx].left = g_Zc13LzssTree[oldIdx].left;
    g_Zc13LzssTree[newIdx].right = g_Zc13LzssTree[oldIdx].right;
    g_Zc13LzssTree[g_Zc13LzssTree[newIdx].left].parent = newIdx;
    g_Zc13LzssTree[g_Zc13LzssTree[newIdx].right].parent = newIdx;
    g_Zc13LzssTree[oldIdx].parent = 0;
}

void __stdcall Zc13InitInterpTimer(Zc13InterpObj *obj, int f160, unsigned char f164, unsigned char f13c, unsigned char f140) {
    obj->f160 = f160;
    obj->f164 = f164;
    obj->f13c = f13c;
    obj->f144 = 0;
    obj->f148 = 0;
    obj->f140 = f140;
    if ((obj->timerFlags & 1) == 0) {
        obj->timerCurF = 0.0f;
        obj->timerCur = 0;
        obj->timerPrev = -999999;
        obj->timerScale = &g_Zc13TimeScale;
        obj->timerFlags |= 1;
    }
    obj->timerCurF = 0.0f;
    obj->timerCur = 0;
    obj->timerPrev = -1;
}
