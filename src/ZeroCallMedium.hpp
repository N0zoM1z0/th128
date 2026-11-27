#pragma once

#include <stddef.h>

namespace th128 {

struct ZcVec3 {
    float x, y, z;
};

struct ZcZunTimer {
    int prev;
    int cur;
    float curF;
    float *speedPtr;
    unsigned int flags;
};

struct ZcInterrupt {
    int hpThreshold;
    int timeThreshold;
    int subHp;
    int subTime;
};

struct ZcEnemy {
    unsigned char pad00[0x1230];
    float fvar1230;
    float fvar1234;
    unsigned char pad1238[0x1264 - 0x1238];
    float fvar1264;
    float fvar1268;
    unsigned char pad126c[0x1404 - 0x126c];
    int intVars[4];
    float floatVars[8];
    ZcZunTimer timer1434;
    unsigned char pad1448[0x5004 - 0x1448];
    int hp;
    int pad5008;
    int hpDisplay;
    int pad5010;
    int hpBase;
    unsigned char pad5018[0x50ac - 0x5018];
    unsigned int flags;
    unsigned char pad50b0[0x50c4 - 0x50b0];
    ZcInterrupt interrupts[8];

    int *GetIntVarPtr(int varId);
    float *GetFloatVarPtr(int varId);
    int UpdateInterrupts();
};

struct ZcEnemyMgr {
    unsigned int pad00[4];
    int intVars10[3];
    unsigned int pad1c[4];
    int intVars2c[4];
    float floatVars3c[8];
    ZcEnemy *boss;
};

struct ZcGui {
    unsigned char pad00[0x58dc];
    int meterAnmId;
    unsigned char pad58e0[0x5a24 - 0x58e0];
    int spellSec;
    int spellCenti;
};

struct ZcSpellCard {
    unsigned char pad00[0x28];
    int timer;
    unsigned char pad2c[0x7c - 0x2c];
    unsigned int flags;
    int bonus;
};

struct ZcStage {
    unsigned char pad00[0x3c];
    int field3c;
};

struct ZcAnmVm;

struct ZcAnmListNode {
    ZcAnmVm *vm;
    ZcAnmListNode *next;
    ZcAnmListNode *prev;
};

struct ZcAnmVm {
    int id;
    unsigned char pad04[0x0c];
    ZcAnmListNode childNode;
    unsigned int pad1c;
    int layer;
    unsigned char pad24[0x40 - 0x24];
    float scaleX;
    float scaleY;
    float scaleZ;
    float field4c;
    unsigned int pad50[2];
    float field58;
    float field5c;
    unsigned int pad60[4];
    int timerPrev;
    int timerCur;
    float timerCurF;
    unsigned char pad7c[0xe8 - 0x7c];
    int interp0;
    unsigned char padec[0x134 - 0xec];
    int interp1;
    unsigned char pad138[0x160 - 0x138];
    int interp2;
    unsigned char pad164[0x1ac - 0x164];
    int interp3;
    unsigned char pad1b0[0x214 - 0x1b0];
    int interp4;
    unsigned char pad218[0x250 - 0x218];
    int interp5;
    unsigned char pad254[0x29c - 0x254];
    int interp6;
    unsigned char pad2a0[0x2c8 - 0x2a0];
    int interp7;
    unsigned char pad2cc[0x2f4 - 0x2cc];
    int interp8;
    unsigned char pad2f8[0x320 - 0x2f8];
    int interp9;
    unsigned char pad324[0x330 - 0x324];
    float mat[16];
    unsigned char pad370[0x3f0 - 0x370];
    unsigned int color;
    unsigned int pad3f4;
    short pendingInterrupt;
    unsigned char pad3fa[0x41e - 0x3fa];
    short scriptIdx;
    unsigned char pad420[0x464 - 0x420];
    ZcVec3 pos;
    unsigned char pad470[0x4b0 - 0x470];
    unsigned int flags0;
    unsigned int flags1;
    unsigned char pad4b8[0x4c8 - 0x4b8];
    void (__fastcall *onInterrupt)(ZcAnmVm *vm, int interrupt);
    unsigned char pad4cc[0x4e8 - 0x4cc];

    inline void Interrupt(short intr) {
        if (onInterrupt != 0) {
            onInterrupt(this, intr);
        }
        pendingInterrupt = intr;
    }

    inline void ResetForLaser();
};

struct ZcGaugeGui {
    unsigned char pad00[0x10];
    ZcAnmVm gaugeA[8];
    ZcAnmVm gaugeB[8];
};

struct ZcAnmMgr {
    unsigned char pad00[0x8b9704];
    ZcAnmListNode *worldListHead;
    unsigned int pad8b9708;
    ZcAnmListNode *uiListHead;
};

struct ZcPlayerOption {
    int anmIdA;
    int anmIdB;
    unsigned char pad08[0xe4 - 8];
};

struct ZcPlayer {
    unsigned char pad00[0x8864];
    ZcPlayerOption options[8];
    unsigned char pad_after[0x438ec - (0x8864 + 8 * 0xe4)];
    unsigned int flags;
    int field438f0;
};

struct ZcLaserHit {
    unsigned char pad00[0x50];
    ZcVec3 pos;
    unsigned char pad5c[0x68 - 0x5c];
    float angle;
    float length;
    float width;

    int HitTestPointA(const ZcVec3 *p, float radius);
    int HitTestPointB(const ZcVec3 *p, float radius);
    int HitTestPointC(const ZcVec3 *p, float radius);
};

struct ZcLaserData {
    ZcVec3 pos;
    unsigned int pad0c[3];
    float angle;
    unsigned int pad1c;
    float length;
    unsigned int pad24;
    int field28;
    int field2c;
    unsigned char pad30[0x354 - 0x30];
};

struct ZcLaserBeam {
    unsigned int pad00[3];
    int field0c;
    int field10;
    unsigned char pad14[0x50 - 0x14];
    ZcVec3 pos;
    unsigned char pad5c[0x68 - 0x5c];
    float angle;
    float length;
    float width;
    unsigned char pad74[0x80 - 0x74];
    int field80;
    unsigned char pad84[0x5bc - 0x84];
    int field5bc;
    ZcLaserData data;
    ZcAnmVm vm;
    float fielddfc;
    unsigned int pad_e00;
    float lengths[512];

    int InitFromData(const ZcLaserData *src);
};

extern ZcEnemyMgr *g_ZcEnemyMgr;
extern ZcGui *g_ZcGui;
extern ZcSpellCard *g_ZcSpellCard;
extern ZcStage *g_ZcStage;
extern ZcAnmMgr *g_ZcAnmMgr;
extern ZcPlayer *g_ZcPlayer;
extern float g_ZcGameSpeed;
extern int g_ZcFreezeMeter;

void __stdcall ZcSetGaugeIconsA(ZcGaugeGui *gui, int fullCount, int partialState);
void __stdcall ZcSetGaugeIconsB(ZcGaugeGui *gui, int fullCount, int partialState);
void ZcPlayerStopAllOptionAnms();
void __stdcall ZcGuiUpdateMeterAnms(ZcGui *gui);

} // namespace th128
