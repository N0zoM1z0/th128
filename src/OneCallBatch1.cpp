#include "OneCallBatch1.hpp"

Oc1AnmMgr *g_Oc1AnmMgr;

#define OC1_CLEAR_VIS(idVal) do { \
    Oc1AnmVm *vm = Oc1GetVmById(g_Oc1AnmMgr, (idVal)); \
    if (vm != 0) { \
        vm->flags &= ~2u; \
        if (vm->childPrev == 0) { \
            for (Oc1AnmNode *n = vm->childNext; n != 0; n = n->next) { \
                n->vm->flags &= ~2u; \
            } \
        } \
    } \
} while (0)

#define OC1_SET_VIS(idVal) do { \
    Oc1AnmVm *vm = Oc1GetVmById(g_Oc1AnmMgr, (idVal)); \
    if (vm != 0) { \
        vm->flags |= 2u; \
        if (vm->childPrev == 0) { \
            for (Oc1AnmNode *n = vm->childNext; n != 0; n = n->next) { \
                n->vm->flags |= 2u; \
            } \
        } \
    } \
} while (0)

void Oc1HideGuiAnms(Oc1GuiPanel *panel) {
    OC1_CLEAR_VIS(panel->id40);
    OC1_CLEAR_VIS(panel->id44);
    OC1_CLEAR_VIS(panel->id48);
    OC1_CLEAR_VIS(panel->id4c);
    OC1_CLEAR_VIS(panel->id50);
    OC1_CLEAR_VIS(panel->id54);
    OC1_CLEAR_VIS(panel->id58);
    OC1_CLEAR_VIS(panel->id5c);
    OC1_CLEAR_VIS(panel->id60);
    OC1_CLEAR_VIS(panel->id64);
    OC1_CLEAR_VIS(panel->id6c);
    OC1_CLEAR_VIS(panel->id68);
}

void Oc1ShowGuiAnms(Oc1GuiPanel *panel) {
    OC1_SET_VIS(panel->id40);
    OC1_SET_VIS(panel->id44);
    OC1_SET_VIS(panel->id48);
    OC1_SET_VIS(panel->id4c);
    OC1_SET_VIS(panel->id50);
    OC1_SET_VIS(panel->id54);
    OC1_SET_VIS(panel->id58);
    OC1_SET_VIS(panel->id5c);
    OC1_SET_VIS(panel->id60);
    OC1_SET_VIS(panel->id64);
    OC1_SET_VIS(panel->id6c);
    OC1_SET_VIS(panel->id68);
}
