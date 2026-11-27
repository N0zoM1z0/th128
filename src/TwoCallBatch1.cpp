#include "TwoCallBatch1.hpp"

Tc1AnmMgr *g_Tc1AnmMgr;

static inline Tc1AnmId FindChildIdByScript(Tc1AnmMgr *mgr, Tc1AnmId *rootId, unsigned short scriptId) {
    Tc1AnmVm *root = Tc1GetVmById(mgr, *rootId);
    if (root == 0) {
        rootId->id = 0;
    }
    for (Tc1AnmNode *n = &root->childNode; n != 0; n = n->next) {
        if (n->vm->scriptId == scriptId) {
            return n->vm->id;
        }
    }
    Tc1AnmId zero;
    zero.id = 0;
    return zero;
}

void Tc1UpdateDigitSprites(Tc1MenuObj *obj) {
    Tc1AnmMgr *mgr = g_Tc1AnmMgr;
    Tc1AnmVm *vm;

#define SET_DIGIT(scriptId, expr) \
    vm = Tc1GetVmById(mgr, FindChildIdByScript(mgr, &obj->rootId, (scriptId))); \
    if (vm != 0) { \
        vm->BindAnmFile(vm->anmFile, (expr) + 0x2a); \
    }

    SET_DIGIT(0x39, obj->values[0] / 10);
    SET_DIGIT(0x3a, obj->values[0] % 10);
    SET_DIGIT(0x43, obj->values[0] / 10);
    SET_DIGIT(0x44, obj->values[0] % 10);

    SET_DIGIT(0x3b, obj->values[1] / 10);
    SET_DIGIT(0x3c, obj->values[1] % 10);
    SET_DIGIT(0x45, obj->values[1] / 10);
    SET_DIGIT(0x46, obj->values[1] % 10);

    SET_DIGIT(0x3d, obj->values[2] / 10);
    SET_DIGIT(0x3e, obj->values[2] % 10);
    SET_DIGIT(0x47, obj->values[2] / 10);
    SET_DIGIT(0x48, obj->values[2] % 10);

    SET_DIGIT(0x3f, obj->values[3] / 10);
    SET_DIGIT(0x40, obj->values[3] % 10);
    SET_DIGIT(0x49, obj->values[3] / 10);
    SET_DIGIT(0x4a, obj->values[3] % 10);

    SET_DIGIT(0x41, obj->values[4] / 10);
    SET_DIGIT(0x42, obj->values[4] % 10);
    SET_DIGIT(0x4b, obj->values[4] / 10);
    SET_DIGIT(0x4c, obj->values[4] % 10);

#undef SET_DIGIT
}
