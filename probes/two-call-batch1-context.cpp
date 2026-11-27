#include "TwoCallBatch1.hpp"
#include <string.h>

Tc1AnmVm *__stdcall Tc1GetVmById(Tc1AnmMgr *mgr, Tc1AnmId id) {
    if (id.id == 0) {
        return 0;
    }
    for (Tc1AnmNode *n = mgr->list0.next; n != 0; n = n->next) {
        if (n->vm->id.id == id.id) {
            return n->vm;
        }
    }
    for (Tc1AnmNode *n = mgr->list1.next; n != 0; n = n->next) {
        if (n->vm->id.id == id.id) {
            return n->vm;
        }
    }
    return 0;
}

int Tc1AnmVm::BindAnmFile(Tc1AnmFile *file, int spriteId) {
    if (file->f108 == 0 || file->f124 != 0) {
        return -1;
    }
    scriptId = (unsigned short)spriteId;
    const float *entry = (const float *)(file->sprites + spriteId * 72);
    float *dst = (float *)pad01c;
    dst[0] = entry[0];
    dst[1] = entry[0];
    dst[2] = entry[1];
    dst[3] = entry[1];
    dst[4] = entry[2];
    dst[5] = entry[2];
    dst[6] = entry[3];
    dst[7] = entry[3];
    dst[8] = 0.0f;
    dst[9] = 0.0f;
    dst[10] = 1.0f;
    dst[11] = 1.0f;
    return 0;
}

extern "C" int __stdcall TwoCallBatch1Probe(Tc1AnmMgr *mgr, Tc1MenuObj *obj, int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Tc1AnmMgr = mgr;
    }
    if (mode & 2) {
        Tc1UpdateDigitSprites(obj);
    }
    if (mode & 4) {
        Tc1UpdateDigitSprites(obj);
    }
    if (mode & 8) {
        Tc1AnmVm *v = Tc1GetVmById(mgr, obj->rootId);
        if (v != 0) {
            sum += v->BindAnmFile(v->anmFile, mode);
            sum += v->scriptId;
        }
    }
    return sum + obj->values[0] + obj->values[1];
}
