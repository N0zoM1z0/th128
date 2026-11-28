#include "CleanBatchJ.hpp"

namespace th128 {

static inline unsigned int CbjPopCnt(unsigned int x) {
    x -= (x >> 1) & 0x55555555u;
    x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
    x = (x + (x >> 4)) & 0x0f0f0f0fu;
    x += x >> 8;
    x += x >> 16;
    return x & 0x3fu;
}

unsigned int CbjCountIceBits(CbjIceGrid* grid) {
    unsigned int* p = grid->bits;
    unsigned int sum = 0;
    for (int i = 0; i < 0x1500; ++i) {
        sum += CbjPopCnt(*p++);
    }
    return sum;
}

void CbjStageClearObj::Clear() {
    CbjAnmMgr* mgr = g_CbjAnmMgr;
    for (int i = 0; i < 8; ++i) {
        void* slot = mgr->slots[i];
        for (CbjAnmListNode* n = mgr->listA; n != 0; ) {
            CbjAnmListNode* next = n->next;
            CbjAnmVm* vm = n->vm;
            if (vm->anm42c == slot) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
        for (CbjAnmListNode* n = mgr->listB; n != 0; ) {
            CbjAnmListNode* next = n->next;
            CbjAnmVm* vm = n->vm;
            if (vm->anm42c == slot) {
                vm->flags4b0 |= 0x20000000u;
            }
            n = next;
        }
    }
    for (CbjVObjNode* n = listB8; n != 0; ) {
        CbjVObjNode* next = n->next;
        if (n->obj != 0) {
            n->obj->Slot5(1);
        }
        n = next;
    }
    fieldC4 = 0;
    field5c = 0;
    if (chain8 != 0) {
        chain8->flags &= ~2u;
    }
    if (chainC != 0) {
        chainC->flags &= ~2u;
    }
}

void __stdcall CbjDecodeScoreVal(int* outHigh, int* outLow) {
    CbjScoreState* st = g_CbjScoreState;
    int v = st->valA8;
    int hi = (v / 100 % 1000 + 934) % 1000;
    int lo = (v % 100 + 67) % 100;
    if (v / 100000 - 22 != hi + lo) {
        *outHigh = 999;
        *outLow = 99;
        return;
    }
    int v2 = st->valA8;
    *outHigh = (v2 / 100 % 1000 + 934) % 1000;
    *outLow = (v2 % 100 + 67) % 100;
}

void __stdcall CbjAddBombPiece(int delta) {
    int oldVal = g_CbjBombVal;
    if (g_CbjBombLocked != 0) {
        return;
    }
    int newVal = oldVal + delta;
    g_CbjBombVal = newVal;
    if (newVal / 10000 > oldVal / 10000) {
        CbjPlaySfx(0x12);
        CbjShowPopup(g_CbjGui, 4, 4, 0);
    }
    if (g_CbjBombVal > 100000) {
        g_CbjBombVal = 100000;
    } else if (g_CbjBombVal < 0) {
        g_CbjBombLocked = 1;
    }
}

void __stdcall CbjAddLifePiece(int delta) {
    int oldVal = g_CbjLifeVal;
    int newVal = oldVal + delta;
    g_CbjLifeVal = newVal;
    if (newVal > 30000) {
        newVal = 30000;
        g_CbjLifeVal = 30000;
    }
    if (newVal / 10000 > oldVal / 10000) {
        CbjPlaySfx(0x48);
        CbjShowPopup(g_CbjGui, 2, 2, 0);
        CbjOnLifeUp(g_CbjGui);
    } else if (newVal < 0) {
        g_CbjLifeVal = 0;
    }
}

BOOL __stdcall CbjEnumJoyAxisCb(const CbjJoyObjInst* obj, void* ctx) {
    (void)ctx;
    if (obj->dwType & 3) {
        CbjJoyPropRange range;
        range.diph.dwSize = sizeof(CbjJoyPropRange);
        range.diph.dwHeaderSize = sizeof(CbjJoyPropHeader);
        range.diph.dwHow = 2;
        range.diph.dwObj = obj->dwType;
        range.lMin = -1000;
        range.lMax = 1000;
        if (FAILED(g_CbjJoyDevices[g_CbjJoyIdx]->SetProperty((const GUID*)4, &range.diph))) {
            return FALSE;
        }
    }
    return TRUE;
}

void CbjMesh::CopyVerts() {
    if (rows == 0) {
        return;
    }
    CbjVertex* s = src;
    for (int r = 0; r < rows - 1; ++r) {
        CbjVertex* d = anms[r]->dst;
        for (int c = 0; c < cols; ++c) {
            d[0] = s[0];
            d[1] = s[cols];
            d += 2;
            s += 1;
        }
    }
}

unsigned int CbjChecksumDwords(const unsigned int* data, int size) {
    size -= (size & 3);
    unsigned int sum = 0;
    while (size > 0) {
        sum += *data;
        ++data;
        size -= 4;
    }
    return sum;
}

} // namespace th128
