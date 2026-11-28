#include "ZeroCallBatchA.hpp"
#include <string.h>

ZcaGaugeObj *g_ZcaGaugeObj;
ZcaLzssNode g_ZcaLzssTree[4097];
ZcaStageMgr *g_ZcaStageMgr;
int g_ZcaStage;
int g_ZcaRoute;
ZcaStageInfo g_ZcaStageTable[32];
ZcaStageInfo *g_ZcaStagePtr;
ZcaEnemyMgr *g_ZcaEnemyMgr;
int g_ZcaTextBytes;
unsigned short *g_ZcaTextPixels;
int g_ZcaBulletTable[32][18][4];
ZcaAnmMgr *g_ZcaAnmMgr;

// 1. 0x00420020 (165B)
void __stdcall ZcaDecodeGauge(int *outA, int *outB) {
    ZcaGaugeObj *obj = g_ZcaGaugeObj;
    int v = obj->valA8;
    int a = ((v / 100) % 1000 + 934) % 1000;
    int b = (v % 100 + 67) % 100;
    if (v / 100000 - 22 != a + b) {
        *outA = 999;
        *outB = 99;
        return;
    }
    int v2 = obj->valA8;
    *outA = ((v2 / 100) % 1000 + 934) % 1000;
    *outB = (v2 % 100 + 67) % 100;
}

// 2. 0x00450d70 (134B)
void ZcaLzssReplaceNode(int oldNode, int newNode) {
    int parent = g_ZcaLzssTree[oldNode].parent;
    if (g_ZcaLzssTree[parent].left == oldNode) {
        g_ZcaLzssTree[parent].left = newNode;
    } else {
        g_ZcaLzssTree[parent].right = newNode;
    }
    g_ZcaLzssTree[newNode].parent = g_ZcaLzssTree[oldNode].parent;
    g_ZcaLzssTree[newNode].left = g_ZcaLzssTree[oldNode].left;
    g_ZcaLzssTree[newNode].right = g_ZcaLzssTree[oldNode].right;
    g_ZcaLzssTree[g_ZcaLzssTree[newNode].left].parent = newNode;
    g_ZcaLzssTree[g_ZcaLzssTree[newNode].right].parent = newNode;
    g_ZcaLzssTree[oldNode].parent = 0;
}

// 3. 0x00412700 (131B)
void ZcaCopyMeshStrip(ZcaMesh *m) {
    if (m->rows == 0) {
        return;
    }
    ZcaVertex *src = m->src;
    for (int y = 0; y < m->rows - 1; ++y) {
        ZcaVertex *dst = m->strips[y]->verts;
        for (int x = 0; x < m->cols; ++x) {
            dst[0] = src[0];
            dst[1] = src[m->cols];
            dst += 2;
            ++src;
        }
    }
}

// 4. 0x004273a0 (120B)
void __stdcall ZcaAdvanceStage(int branch) {
    if (g_ZcaStageMgr->flag74 != 0) {
        if ((g_ZcaStage == 1 || g_ZcaStage == 6 || g_ZcaStage == 11) && (g_ZcaRoute % 2 == 1)) {
            g_ZcaStage += 3;
        } else {
            g_ZcaStage += 1;
        }
    } else {
        if ((g_ZcaStage == 1 || g_ZcaStage == 6 || g_ZcaStage == 11) && branch != 0) {
            ++g_ZcaRoute;
            g_ZcaStage += 3;
        } else {
            g_ZcaStage += 1;
        }
    }
    g_ZcaStagePtr = &g_ZcaStageTable[g_ZcaStage];
}

// 5. 0x004142e0 (116B)
void ZcaResetEnemySub(ZcaEnemySub *s) {
    s->f1008 = 0;
    s->f100c = 0;
    for (int i = 0; i < 8; ++i) {
        s->slots[i].flags &= ~1U;
    }
    for (int i = 0; i < 8; ++i) {
        s->slots[i].val = 0;
    }
}

// 6. 0x0046ac00 (115B)
int ZcaChecksumDwords(const int *data, int byteLen) {
    int rem = byteLen & 3;
    int n = byteLen - rem;
    int sum = 0;
    for (int i = 0; i < n; i += 4) {
        sum += *data++;
    }
    return sum;
}

// 7. 0x00415270 (97B)
void ZcaUnlinkEnemy(ZcaEnemy *e) {
    ZcaEnemyMgr *mgr = g_ZcaEnemyMgr;
    if (mgr->head == &e->node) {
        mgr->head = e->node.next;
    }
    if (mgr->tail == &e->node) {
        mgr->tail = e->node.prev;
    }
    if (e->node.next != 0) {
        e->node.next->prev = e->node.prev;
    }
    if (e->node.prev != 0) {
        e->node.prev->next = e->node.next;
    }
    e->node.next = 0;
    e->node.prev = 0;
    --mgr->count;
}

// 8. 0x0044f390 (94B)
char *__stdcall ZcaReadLine(char *src, int *rem, char *dst) {
    char *p = src;
    while (*p != '\n' && *p != '\r') {
        if (*rem == 0) {
            return p;
        }
        ++p;
        --(*rem);
    }
    if (*rem == 0) {
        return p;
    }
    *p = 0;
    strcpy(dst, src);
    ++p;
    --(*rem);
    while (*p == '\n' || *p == '\r') {
        if (*rem == 0) {
            break;
        }
        ++p;
        --(*rem);
    }
    return p;
}

// 9. 0x00442820 (82B)
int ZcaReplayBuf::ChecksumBytes(int len) {
    int n = len - 8;
    int sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += bytes[i + 8];
    }
    return sum;
}

// 10. 0x004518e0 (66B)
void ZcaFillText4444(unsigned int color) {
    unsigned int val = (((((color >> 20) & 0xf) << 4) | ((color >> 12) & 0xf)) << 4) | ((color >> 4) & 0xf);
    unsigned short *dst = g_ZcaTextPixels;
    for (int i = 0; i < g_ZcaTextBytes; i += 2, ++dst) {
        *dst = (unsigned short)val;
    }
}

// 11. 0x00468190 (57B)
void ZcaResetTwoLists(ZcaTwoLists *p) {
    p->a.flags &= ~1U;
    p->a.f08 = 0;
    p->a.f0c = 0;
    p->a.f10 = 0;
    p->a.f00 = 0;
    p->a.self = &p->a;
    p->a.f18 = 0;
    p->a.f1c = 0;

    p->b.flags &= ~1U;
    ZcaSubList *b = &p->b;
    b->f08 = 0;
    b->f0c = 0;
    b->f10 = 0;
    b->f00 = 0;
    b->self = b;
    b->f18 = 0;
    b->f1c = 0;
    p->f4c = 0;
}

// 12. 0x0041de50 (56B)
void ZcaDropOwner::SetDrop(int idx, int type, int a, int b, int c) {
    drops[idx].type = type;
    if (type >= 0) {
        drops[idx].a = a;
        drops[idx].b = b;
        drops[idx].c = c;
    }
}

// 13. 0x0040ccb0 (56B)
int __fastcall ZcaGetBulletParam16(ZcaBullet *b, int idx) {
    ZcaBulletInner *in = b->inner;
    if (g_ZcaBulletTable[in->s11b0][0][0] < 0) {
        return idx;
    }
    return g_ZcaBulletTable[in->s11b0][in->s11b2][idx];
}

// 14. 0x0045dfd0 (54B)
int ZcaIsAnmLoaded(ZcaAnmVm *vm) {
    int *p = vm->ptr428;
    ZcaAnmMgr *mgr = g_ZcaAnmMgr;
    if (p == 0) {
        return 0;
    }
    int idx = *p;
    if (idx < 0 || mgr->slots[idx] == 0) {
        return 0;
    }
    return mgr->slots[idx]->field120 != 0;
}

// 15. 0x0042abd0 (54B)
int __fastcall ZcaGetBulletParam32(ZcaLaser *l, int idx) {
    ZcaLaserInner *in = l->inner;
    if (g_ZcaBulletTable[in->i5b8][0][0] < 0) {
        return idx;
    }
    return g_ZcaBulletTable[in->i5b8][in->i5bc][idx];
}

// 16. 0x004427b0 (51B)
ZcaReplaySlot::ZcaReplaySlot() {
    memset(this, 0, sizeof(ZcaReplaySlot));
    f04 = f00;
    f0c = f08;
    self = this;
    f1c = 0;
    f20 = 0;
}
