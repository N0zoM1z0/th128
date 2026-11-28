#include "CleanBatchM.hpp"
#include <string.h>

namespace th128 {

int CbmStageBg::UpdateQuads() {
    for (int i = 0; i < header10->count; ++i) {
        CbmBgObj* obj = objs14[i];
        if ((obj->flag3 & 1) != 0) {
            int active = 0;
            CbmBgQuad* q = (CbmBgQuad*)((char*)obj + 0x1c);
            while (q->type >= 0) {
                CbmBgVm* vm = &vms1c8[q->vmIdx6];
                vm->Update();
                if (vm->field424 != 0) {
                    ++active;
                }
                q = (CbmBgQuad*)((char*)q + q->size2);
            }
            if (active == 0) {
                obj->flag3 &= ~1;
            }
        }
    }
    return 0;
}

void __stdcall CbmAddPower(int delta) {
    int old = g_CbmPower;
    if (g_CbmPowerFlag != 0) {
        return;
    }
    int cur = old + delta;
    g_CbmPower = cur;
    if (cur / 10000 > old / 10000) {
        g_CbmSeMgr.PlaySe(0x12);
        CbmShowPopup(g_CbmGui, 4, 4, 0);
    }
    if (g_CbmPower > 100000) {
        g_CbmPower = 100000;
    } else if (g_CbmPower < 0) {
        g_CbmPowerFlag = 1;
    }
}

void CbmLaserMgr::SpawnNode(int listIdx) {
    CbmLaserNode* node = (CbmLaserNode*)::operator new(sizeof(CbmLaserNode));
    if (node != 0) {
        memset(node, 0, sizeof(CbmLaserNode));
        node->p18a0 = &node->sub151c;
        node->self1518 = node;
        node->entry.owner = node;
        node->entry.next = 0;
        node->entry.prev = 0;
    } else {
        node = 0;
    }
    CbmListEntry* tail = &lists[listIdx];
    while (tail->next != 0) {
        tail = tail->next;
    }
    CbmListEntry* entry = &node->entry;
    if (tail->next != 0) {
        entry->next = tail->next;
        tail->next->prev = entry;
    }
    tail->next = entry;
    entry->prev = tail;
    ++count134;
}

void CbmStrip::CopyStripVerts() {
    if (rows == 0) {
        return;
    }
    CbmVert28* src = srcVerts10;
    for (int i = 0; i < rows - 1; ++i) {
        CbmVert28* dst = rowObjsC[i]->verts4ac;
        for (int j = 0; j < cols; ++j) {
            dst[0] = *src;
            dst[1] = src[cols];
            dst += 2;
            ++src;
        }
    }
}

unsigned int CbmSumU32(const unsigned int* data, int byteLen) {
    unsigned int sum = 0;
    byteLen -= byteLen & 3;
    while (byteLen > 0) {
        sum += *data++;
        byteLen -= 4;
    }
    return sum;
}

std::string& CbmStringErase(std::string* s, size_t pos, size_t count) {
    return s->erase(pos, count);
}

} // namespace th128
