#include "TitleBatch1.hpp"

Tb1AnmMgr *g_Tb1AnmMgr;
Tb1SoundMgr g_Tb1SoundMgr;
Tb1Config g_Tb1Cfg;
int g_Tb1BgmVol;
int g_Tb1SeVol;
int g_Tb1SeDb;

static inline Tb1AnmId FindChildIdByScript(Tb1AnmMgr *mgr, Tb1AnmId *rootId, unsigned short scriptId) {
    Tb1AnmVm *root = Tb1GetVmById(mgr, *rootId);
    if (root == 0) {
        rootId->id = 0;
    }
    for (Tb1AnmNode *n = &root->childNode; n != 0; n = n->next) {
        if (n->vm->scriptId == scriptId) {
            return n->vm->id;
        }
    }
    Tb1AnmId zero;
    zero.id = 0;
    return zero;
}

void Tb1UpdateOptionsVolume(Tb1MenuObj *obj) {
    g_Tb1BgmVol = g_Tb1Cfg.bgmVol;
    g_Tb1SoundMgr.PostCommand("SetVol", 8, 0);
    g_Tb1SeVol = g_Tb1Cfg.seVol;
    if (g_Tb1SeVol != 0) {
        float v = (float)(g_Tb1BgmVol / 100.0);
        v = 1.0f - v;
        v = v * v;
        v = v * v;
        v = 1.0f - v;
        g_Tb1SeDb = -5000 - (int)(v * -5000.0);
    } else {
        g_Tb1SeDb = -10000;
    }

    Tb1AnmMgr *mgr = g_Tb1AnmMgr;
    Tb1AnmVm *vm;

#define SET_DIGIT(scriptId, expr, base) \
    vm = Tb1GetVmById(mgr, FindChildIdByScript(mgr, &obj->rootId, (scriptId))); \
    if (vm != 0) { \
        vm->BindAnmFile(vm->anmFile, (expr) + (base)); \
    }

    SET_DIGIT(0x1b, g_Tb1Cfg.bgmVol / 100, 0x2a);
    SET_DIGIT(0x1c, (g_Tb1Cfg.bgmVol / 10) % 10, 0x2a);
    SET_DIGIT(0x1d, g_Tb1Cfg.bgmVol % 10, 0x2a);
    SET_DIGIT(0x1f, g_Tb1Cfg.bgmVol / 100, 0x35);
    SET_DIGIT(0x20, (g_Tb1Cfg.bgmVol / 10) % 10, 0x35);
    SET_DIGIT(0x21, g_Tb1Cfg.bgmVol % 10, 0x35);

    SET_DIGIT(0x23, g_Tb1Cfg.seVol / 100, 0x2a);
    SET_DIGIT(0x24, (g_Tb1Cfg.seVol / 10) % 10, 0x2a);
    SET_DIGIT(0x25, g_Tb1Cfg.seVol % 10, 0x2a);
    SET_DIGIT(0x27, g_Tb1Cfg.seVol / 100, 0x35);
    SET_DIGIT(0x28, (g_Tb1Cfg.seVol / 10) % 10, 0x35);
    SET_DIGIT(0x29, g_Tb1Cfg.seVol % 10, 0x35);

#undef SET_DIGIT

#define HIDE_DIGIT(scriptId) \
    Tb1GetVmById(mgr, FindChildIdByScript(mgr, &obj->rootId, (scriptId)))->flags &= ~2u
#define SHOW_DIGIT(scriptId) \
    Tb1GetVmById(mgr, FindChildIdByScript(mgr, &obj->rootId, (scriptId)))->flags |= 2u

    if (g_Tb1Cfg.bgmVol < 10) {
        HIDE_DIGIT(0x1b);
        HIDE_DIGIT(0x1c);
        HIDE_DIGIT(0x1f);
        HIDE_DIGIT(0x20);
    } else if (g_Tb1Cfg.bgmVol < 100) {
        HIDE_DIGIT(0x1b);
        SHOW_DIGIT(0x1c);
        HIDE_DIGIT(0x1f);
        SHOW_DIGIT(0x20);
    } else {
        SHOW_DIGIT(0x1b);
        SHOW_DIGIT(0x1c);
        SHOW_DIGIT(0x1f);
        SHOW_DIGIT(0x20);
    }

    if (g_Tb1Cfg.seVol < 10) {
        HIDE_DIGIT(0x23);
        HIDE_DIGIT(0x24);
        HIDE_DIGIT(0x27);
        HIDE_DIGIT(0x28);
    } else if (g_Tb1Cfg.seVol < 100) {
        HIDE_DIGIT(0x23);
        SHOW_DIGIT(0x24);
        HIDE_DIGIT(0x27);
        SHOW_DIGIT(0x28);
    } else {
        SHOW_DIGIT(0x23);
        SHOW_DIGIT(0x24);
        SHOW_DIGIT(0x27);
        SHOW_DIGIT(0x28);
    }

#undef HIDE_DIGIT
#undef SHOW_DIGIT
}
