#include "ZeroCallMedium.hpp"

#include <math.h>
#include <string.h>

namespace th128 {

int *ZcEnemy::GetIntVarPtr(int varId) {
    switch (varId) {
    case -9985: return &intVars[0];
    case -9984: return &intVars[1];
    case -9983: return &intVars[2];
    case -9982: return &intVars[3];
    case -9949: return &g_ZcEnemyMgr->intVars10[0];
    case -9948: return &g_ZcEnemyMgr->intVars10[1];
    case -9947: return &g_ZcEnemyMgr->intVars10[2];
    case -9943: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->intVars[0] : &intVars[0];
    case -9942: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->intVars[1] : &intVars[1];
    case -9941: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->intVars[2] : &intVars[2];
    case -9940: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->intVars[3] : &intVars[3];
    case -9926: return &g_ZcEnemyMgr->intVars2c[0];
    case -9925: return &g_ZcEnemyMgr->intVars2c[1];
    case -9924: return &g_ZcEnemyMgr->intVars2c[2];
    case -9923: return &g_ZcEnemyMgr->intVars2c[3];
    default: return 0;
    }
}

float *ZcEnemy::GetFloatVarPtr(int varId) {
    switch (varId) {
    case -9981: return &floatVars[0];
    case -9980: return &floatVars[1];
    case -9979: return &floatVars[2];
    case -9978: return &floatVars[3];
    case -9935: return &floatVars[4];
    case -9934: return &floatVars[5];
    case -9933: return &floatVars[6];
    case -9932: return &floatVars[7];
    case -9939: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->floatVars[0] : &floatVars[0];
    case -9938: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->floatVars[1] : &floatVars[1];
    case -9937: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->floatVars[2] : &floatVars[2];
    case -9936: return g_ZcEnemyMgr->boss ? &g_ZcEnemyMgr->boss->floatVars[3] : &floatVars[3];
    case -9922: return &g_ZcEnemyMgr->floatVars3c[0];
    case -9921: return &g_ZcEnemyMgr->floatVars3c[1];
    case -9920: return &g_ZcEnemyMgr->floatVars3c[2];
    case -9919: return &g_ZcEnemyMgr->floatVars3c[3];
    case -9918: return &g_ZcEnemyMgr->floatVars3c[4];
    case -9917: return &g_ZcEnemyMgr->floatVars3c[5];
    case -9916: return &g_ZcEnemyMgr->floatVars3c[6];
    case -9915: return &g_ZcEnemyMgr->floatVars3c[7];
    case -9995: return &fvar1230;
    case -9994: return &fvar1234;
    case -9993: return &fvar1264;
    case -9992: return &fvar1268;
    default: return 0;
    }
}

int ZcEnemy::UpdateInterrupts() {
    hpDisplay = hp;
    hpBase = 0;
    for (unsigned int i = 0; i < 8; ++i) {
        if (interrupts[i].hpThreshold >= 0) {
            hpDisplay = hp - interrupts[i].hpThreshold;
            hpBase = interrupts[i].hpThreshold;
            if (hp <= interrupts[i].hpThreshold) {
                hp = interrupts[i].hpThreshold;
                interrupts[i].hpThreshold = -1;
                if ((timer1434.flags & 1) == 0) {
                    timer1434.flags |= 1;
                    timer1434.curF = 0.0f;
                    timer1434.cur = 0;
                    timer1434.prev = -999999;
                    timer1434.speedPtr = &g_ZcGameSpeed;
                }
                timer1434.cur = 0;
                timer1434.curF = 0.0f;
                timer1434.prev = -1;
                flags &= ~0x02000000u;
                return interrupts[i].subHp;
            }
            break;
        }
    }
    for (unsigned int i = 0; i < 8; ++i) {
        if (interrupts[i].hpThreshold >= 0 && interrupts[i].timeThreshold > 0) {
            int rem = interrupts[i].timeThreshold - timer1434.cur;
            int sec = rem / 60;
            int centi = ((rem % 60) * 100) / 60;
            g_ZcGui->spellSec = (sec > 99) ? 99 : sec;
            g_ZcGui->spellCenti = (sec > 99) ? 99 : centi;
            if (timer1434.cur >= interrupts[i].timeThreshold) {
                hp = interrupts[i].hpThreshold;
                interrupts[i].hpThreshold = -1;
                if ((timer1434.flags & 1) == 0) {
                    timer1434.curF = 0.0f;
                    timer1434.flags |= 1;
                    timer1434.cur = 0;
                    timer1434.prev = -999999;
                    timer1434.speedPtr = &g_ZcGameSpeed;
                }
                timer1434.curF = 0.0f;
                timer1434.cur = 0;
                timer1434.prev = -1;
                flags |= 0x02000000u;
                if ((g_ZcSpellCard->flags & 0x10) == 0) {
                    if (g_ZcSpellCard->flags & 1) {
                        if (g_ZcSpellCard->timer >= 60) {
                            g_ZcSpellCard->bonus = 0;
                            g_ZcSpellCard->flags &= ~0x46u;
                        } else if (g_ZcStage->field3c != 0) {
                            g_ZcSpellCard->flags |= 0x40;
                        }
                    }
                    g_ZcEnemyMgr->intVars10[2] = 0;
                }
                return interrupts[i].subTime;
            }
            break;
        }
    }
    return 0;
}

void __stdcall ZcSetGaugeIconsA(ZcGaugeGui *gui, int fullCount, int partialState) {
    unsigned int i = 0;
    for (; (int)i < fullCount; ++i) {
        gui->gaugeA[i].Interrupt(2);
    }
    if (i < 8) {
        short state = (short)(partialState + 7);
        gui->gaugeA[i].Interrupt(state);
        ++i;
        for (; i < 8; ++i) {
            gui->gaugeA[i].Interrupt(3);
        }
    }
}

void __stdcall ZcSetGaugeIconsB(ZcGaugeGui *gui, int fullCount, int partialState) {
    unsigned int i = 0;
    for (; (int)i < fullCount; ++i) {
        gui->gaugeB[i].Interrupt(2);
    }
    if (i < 8) {
        short state = (short)(partialState + 7);
        gui->gaugeB[i].Interrupt(state);
        ++i;
        for (; i < 8; ++i) {
            gui->gaugeB[i].Interrupt(3);
        }
    }
}

static inline ZcAnmVm *ZcFindVmById(int id) {
    if (id == 0) return 0;
    for (ZcAnmListNode *n = g_ZcAnmMgr->worldListHead; n != 0; n = n->next) {
        if (n->vm->id == id) return n->vm;
    }
    for (ZcAnmListNode *n = g_ZcAnmMgr->uiListHead; n != 0; n = n->next) {
        if (n->vm->id == id) return n->vm;
    }
    return 0;
}

static inline void ZcInterruptVmAndChildren(ZcAnmVm *vm, short intr) {
    if (vm == 0) return;
    vm->Interrupt(intr);
    if (vm->childNode.prev == 0) {
        for (ZcAnmListNode *c = vm->childNode.next; c != 0; c = c->next) {
            c->vm->Interrupt(intr);
        }
    }
}

void ZcPlayerStopAllOptionAnms() {
    ZcPlayer *player = g_ZcPlayer;
    player->flags |= 0x10;
    for (int i = 0; i < 8; ++i) {
        ZcInterruptVmAndChildren(ZcFindVmById(player->options[i].anmIdA), 3);
        ZcInterruptVmAndChildren(ZcFindVmById(player->options[i].anmIdB), 3);
    }
    player->field438f0 = 0;
}

static inline ZcAnmVm *ZcFindChildByScript(ZcAnmVm *root, int scriptIdx) {
    for (ZcAnmListNode *n = &root->childNode; n != 0; n = n->next) {
        if (n->vm->scriptIdx == scriptIdx || (scriptIdx == -1 && n->vm != root)) {
            return n->vm;
        }
    }
    return 0;
}

void __stdcall ZcGuiUpdateMeterAnms(ZcGui *gui) {
    for (int i = 0; i < 3; ++i) {
        if (g_ZcFreezeMeter >= (i + 1) * 10000) {
            ZcAnmVm *vm = ZcFindVmById(gui->meterAnmId);
            if (vm == 0) gui->meterAnmId = 0;
            ZcFindChildByScript(vm, i + 12)->flags0 |= 2;
        } else {
            ZcAnmVm *vm = ZcFindVmById(gui->meterAnmId);
            if (vm == 0) gui->meterAnmId = 0;
            ZcFindChildByScript(vm, i + 12)->flags0 &= ~2u;
        }
    }
}

int ZcLaserHit::HitTestPointA(const ZcVec3 *p, float radius) {
    float dx = p->x - pos.x;
    float dy = p->y - pos.y;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float ry = s * dx + c * dy;
    float rx = c * dx - s * dy;
    float minX = rx - radius;
    float minY = ry - radius;
    float maxX = rx + radius;
    float maxY = ry + radius;
    if (minX <= length && minY <= width * 0.5 && maxX >= 0.0f && maxY >= -width * 0.5) {
        return 2;
    }
    return 0;
}

int ZcLaserHit::HitTestPointB(const ZcVec3 *p, float radius) {
    float dx = p->x - pos.x;
    float dy = p->y - pos.y;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float ry = s * dx + c * dy;
    float rx = c * dx - s * dy;
    float minX = rx - radius;
    float minY = ry - radius;
    float maxX = rx + radius;
    float maxY = ry + radius;
    if (minX <= length && minY <= width * 0.5 && maxX >= 0.0f && maxY >= -width * 0.5) {
        return 2;
    }
    return 0;
}

int ZcLaserHit::HitTestPointC(const ZcVec3 *p, float radius) {
    float dx = p->x - pos.x;
    float dy = p->y - pos.y;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float ry = s * dx + c * dy;
    float rx = c * dx - s * dy;
    float minX = rx - radius;
    float minY = ry - radius;
    float maxX = rx + radius;
    float maxY = ry + radius;
    if (minX <= length && minY <= width * 0.5 && maxX >= 0.0f && maxY >= -width * 0.5) {
        return 2;
    }
    return 0;
}

inline void ZcAnmVm::ResetForLaser() {
    ZcVec3 savedPos = pos;
    int savedLayer = layer;
    memset(this, 0, sizeof(*this));
    scaleX = 1.0f;
    scaleY = 1.0f;
    scaleZ = 1.0f;
    field4c = 1.0f;
    field58 = 1.0f;
    field5c = 1.0f;
    pos = savedPos;
    layer = savedLayer;
    color = 0xffffffffu;
    for (int i = 0; i < 16; ++i) {
        mat[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }
    *(unsigned short *)&flags0 = 7;
    timerCur = 0;
    timerCurF = 0.0f;
    timerPrev = -999999;
    interp0 = 0;
    interp1 = 0;
    interp2 = 0;
    interp3 = 0;
    interp4 = 0;
    interp5 = 0;
    interp6 = 0;
    interp7 = 0;
    interp8 = 0;
    interp9 = 0;
    flags1 = (flags1 & ~0x80u) | 0x40u;
    *(ZcAnmVm **)((unsigned char *)this + 4) = this;
    *(int *)((unsigned char *)this + 8) = 0;
    *(int *)((unsigned char *)this + 0xc) = 0;
    childNode.vm = this;
    childNode.next = 0;
    childNode.prev = 0;
    flags0 = (flags0 & ~0xc0u) | 0x20u;
}

int ZcLaserBeam::InitFromData(const ZcLaserData *src) {
    data = *src;
    length = data.length;
    angle = data.angle;
    field0c = 3;
    field10 = 3;
    pos = data.pos;
    field5bc = data.field2c;
    field80 = data.field28;
    for (int i = 0; i < 512; ++i) {
        lengths[i] = length;
    }
    width = 1.0f;
    fielddfc = 0.0f;
    vm.ResetForLaser();
    return 0;
}

} // namespace th128
