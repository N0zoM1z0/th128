#ifndef TH128_ONE_CALL_BATCH1_HPP
#define TH128_ONE_CALL_BATCH1_HPP

struct Oc1AnmVm;

struct Oc1AnmNode {
    Oc1AnmVm *vm;
    Oc1AnmNode *next;
};

struct Oc1AnmVm {
    unsigned char pad00[0x14];
    Oc1AnmNode *childNext;
    Oc1AnmNode *childPrev;
    unsigned char pad1c[0x4b0 - 0x1c];
    unsigned int flags;
};

struct Oc1AnmMgr {
    int dummy;
    Oc1AnmNode nodes[64];
    Oc1AnmVm vms[64];
};

struct Oc1AnmId {
    unsigned int id;
};

struct Oc1GuiPanel {
    unsigned char pad00[0x40];
    Oc1AnmId id40;
    Oc1AnmId id44;
    Oc1AnmId id48;
    Oc1AnmId id4c;
    Oc1AnmId id50;
    Oc1AnmId id54;
    Oc1AnmId id58;
    Oc1AnmId id5c;
    Oc1AnmId id60;
    Oc1AnmId id64;
    Oc1AnmId id68;
    Oc1AnmId id6c;
};

extern Oc1AnmMgr *g_Oc1AnmMgr;

Oc1AnmVm *__stdcall Oc1GetVmById(Oc1AnmMgr *mgr, Oc1AnmId id);
void Oc1HideGuiAnms(Oc1GuiPanel *panel);
void Oc1ShowGuiAnms(Oc1GuiPanel *panel);

#endif
