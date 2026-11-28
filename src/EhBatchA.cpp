#include "EhBatchA.hpp"
#include <stdlib.h>
#include <string.h>

EbaBulletMgr *g_EbaBulletMgr;
EbaLaserMgr *g_EbaLaserMgr;
EbaScoreMgr *g_EbaScoreMgr;
int g_EbaReplayMgr;

// 1. 0x00464240 (223B)
EbaAnmMgr::~EbaAnmMgr() {
    EbaListNode *n = list1;
    while (n != 0) {
        void *vm = n->data;
        EbaListNode *next = n->next;
        EbaReleaseVm(vm);
        n = next;
    }
    n = list2;
    while (n != 0) {
        void *vm = n->data;
        EbaListNode *next = n->next;
        EbaReleaseVm(vm);
        n = next;
    }
    // vms2 destroyed automatically, then buf4e9644?
    // Wait: in Target, vms2 is destroyed BEFORE free(buf4e9644), which means
    // buf4e9644 is freed in a member destructor OR ~EbaAnmMgr has a sub-object!
    // Let's check: in Target +070: vms2 dtor, +08c: free(buf4e9644), +09f: vms1 dtor!
    // So buf4e9644 is a member struct between vms1 and vms2!
}

// 2. 0x00408450 (197B)
inline EbaBulletMgr::EbaBulletMgr() {
    memset(this, 0, sizeof(EbaBulletMgr));
    g_EbaBulletMgr = this;
}
EbaBulletMgr *EbaCreateBulletMgr() {
    EbaBulletMgr *p = new EbaBulletMgr();
    if (EbaInitBulletMgr(p) != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 3. 0x00441000 (196B)
inline EbaReplayObj::EbaReplayObj() {
    memset(this, 0, sizeof(EbaReplayObj));
}
EbaReplayObj *__stdcall EbaCreateReplayA(int arg) {
    EbaReplayObj *p = new EbaReplayObj();
    if (EbaInitReplayA(p, &g_EbaReplayMgr, arg) != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 4. 0x00428500 (196B)
inline EbaLaserMgr::EbaLaserMgr() {
    memset(this, 0, sizeof(EbaLaserMgr));
    flags |= 2;
    g_EbaLaserMgr = this;
}
EbaLaserMgr *EbaCreateLaserMgr() {
    EbaLaserMgr *p = new EbaLaserMgr();
    if (EbaInitLaserMgr(p) != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 5. 0x00403990 (155B)
EbaStageObj *__stdcall EbaCreateStage(int arg) {
    EbaStageObj *p = new EbaStageObj();
    if (EbaInitStage(p, arg) != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 6. 0x0043aea0 (147B)
EbaPlayerObj *EbaCreatePlayer() {
    EbaPlayerObj *p = new EbaPlayerObj();
    if (EbaInitPlayer(p) != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 7. 0x00420fd0 (146B)
EbaGuiObj *EbaCreateGui() {
    EbaGuiObj *p = new EbaGuiObj();
    if (EbaInitGui() != 0) {
        delete p;
        return 0;
    }
    return p;
}

// 8. 0x00442d90 (95B)
void EbaCreateScoreMgr() {
    g_EbaScoreMgr = new EbaScoreMgr();
}
