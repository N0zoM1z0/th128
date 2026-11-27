#include "OneCallBatch1.hpp"

Oc1AnmVm *__stdcall Oc1GetVmById(Oc1AnmMgr *mgr, Oc1AnmId id) {
    if (id.id == 0) {
        return 0;
    }
    for (Oc1AnmNode *n = mgr->nodes[0].next; n != 0; n = n->next) {
        if (n->vm->flags == id.id) {
            return n->vm;
        }
    }
    for (Oc1AnmNode *n = mgr->nodes[1].next; n != 0; n = n->next) {
        if (n->vm->flags == id.id) {
            return n->vm;
        }
    }
    return 0;
}

extern "C" int __stdcall OneCallBatch1Probe(Oc1AnmMgr *mgr, Oc1GuiPanel *panel, int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Oc1AnmMgr = mgr;
    }
    if (mode & 2) {
        Oc1HideGuiAnms(panel);
    }
    if (mode & 4) {
        Oc1ShowGuiAnms(panel);
    }
    if (mode & 8) {
        Oc1AnmId id;
        id.id = (unsigned int)mode;
        Oc1AnmVm *v = Oc1GetVmById(mgr, id);
        if (v != 0) {
            sum += v->flags;
        }
    }
    return sum;
}
