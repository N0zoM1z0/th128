#pragma once

#include <windows.h>

namespace th128 {

struct GlwChainElem {
    unsigned int pad0;
    unsigned int flags4;
};

struct GlwChainMgr {
    int dummy;
    void Cut(GlwChainElem *elem);
};

struct GlwSpellCard {
    unsigned char pad0[0xa8];
    int encodedA8;
};

struct GlwAnmMgr {
    unsigned char pad0[0x4e912c];
    void *slot27_4e912c;
};

struct GlwGui {
    unsigned char pad0[0x5904];
    int field5904;
    unsigned char pad5908[0x5a1c - 0x5908];
    void *msgObj5a1c;
    void *msgBuf5a20;

    static void ReleaseMsgAndSlot27();
};

struct GlwPopupMgr {
    int dummy;
    void __stdcall Show(int kind, int value);
};

struct GlwConfig {
    unsigned int version0;         // +0x00
    unsigned int keys4[4];         // +0x04..+0x13
    unsigned short key14;          // +0x14
    unsigned short w16;            // +0x16 (0x258)
    unsigned short h18;            // +0x18 (0x258)
    unsigned char f1a;             // +0x1A (0)
    unsigned char f1b;             // +0x1B (1)
    unsigned char f1c;             // +0x1C (1)
    unsigned char f1d;             // +0x1D (0)
    unsigned char f1e;             // +0x1E (0)
    unsigned char f1f;             // +0x1F (2)
    unsigned char f20;             // +0x20 (0x64)
    unsigned char f21;             // +0x21 (0x50)
    unsigned char f22;             // +0x22 (0)
    unsigned char f23;             // +0x23 (2)
    unsigned int pad24;
    int posX28;                    // +0x28 (0x80000000)
    int posY2c;                    // +0x2C (0x80000000)
    unsigned int pad30[2];
    unsigned int flags38;          // +0x38 (|= 0x100)

    void ResetDefaults();
}; // 0x3C

struct GlwLaserElem {
    unsigned char pad0[0xa40];
    ~GlwLaserElem();
};

struct GlwLaserItemMgr {
    unsigned char pad0[8];
    GlwChainElem *chain8;
    GlwChainElem *chainC;
    int pad10;
    GlwLaserElem elems14[0x2cc];

    ~GlwLaserItemMgr();
};

struct GlwLaserBase {
    virtual void V00() = 0;
    virtual void V01() = 0;
    virtual void V02() = 0;
    virtual void __thiscall Init(void *arg) = 0; // +0x0c
    GlwLaserBase *prev4;
    GlwLaserBase *next8;
    unsigned char padC[0x80 - 0x0c];
    int id80;
};

struct GlwLaserType0 : GlwLaserBase {
    unsigned char pad84[0x17cc - 0x84];
    GlwLaserType0();
    virtual void V00() {}
    virtual void V01() {}
    virtual void V02() {}
    virtual void __thiscall Init(void *) {}
};

struct GlwLaserType1 : GlwLaserBase {
    unsigned char pad84[0x1308 - 0x84];
    GlwLaserType1();
    virtual void V00() {}
    virtual void V01() {}
    virtual void V02() {}
    virtual void __thiscall Init(void *) {}
};

struct GlwLaserType2 : GlwLaserBase {
    unsigned char pad84[0x1e00 - 0x84];
    GlwLaserType2();
    virtual void V00() {}
    virtual void V01() {}
    virtual void V02() {}
    virtual void __thiscall Init(void *) {}
};

struct GlwLaserType3 : GlwLaserBase {
    unsigned char pad84[0x1328 - 0x84];
    GlwLaserType3();
    virtual void V00() {}
    virtual void V01() {}
    virtual void V02() {}
    virtual void __thiscall Init(void *) {}
};

struct GlwLaserMgr {
    unsigned char pad0[0x5d0];
    GlwLaserBase *tail5d0;
    int count5d4;
    int nextId5d8;

    static int Spawn(int type, void *initArg);
};

struct GlwLaserStep {
    unsigned char pad0[0x10];
    int param10;
    unsigned char pad14[0x20 - 0x14];
    int cmd20;
    int flag24;
    int pad28;
}; // 0x2C

struct GlwLaserInfinite {
    unsigned char pad4[0xc - 4];
    int stateC;
    unsigned char pad10[0x594 - 0x10];
    int stepIdx594;
    int wait598;
    unsigned char pad59c[0x5b4 - 0x59c];
    int param5b4;
    unsigned char pad5b8[0x61c - 0x5b8];
    GlwLaserStep steps61c[0x12];
    unsigned char pad934[0xde8 - (0x61c + 0x12 * 0x2c)];
    unsigned int flagsDe8;

    virtual void Slot01();
};

extern unsigned int g_GlwSupFlags;
extern CRITICAL_SECTION g_GlwSupCs0;
extern unsigned char g_GlwSupCsDepth0;
extern GlwChainMgr *g_GlwChainMgr;
extern GlwSpellCard *g_GlwSpellCard;
extern GlwGui *g_GlwGui;
extern unsigned int g_GlwGameFlags;
extern GlwAnmMgr *g_GlwAnmMgr;
extern int g_GlwFreezeGauge;
extern int g_GlwFreezeEmpty;
extern int g_GlwMotivGauge;
extern GlwPopupMgr *g_GlwPopupMgr;
extern unsigned int g_GlwInputKeys[5];
extern GlwLaserItemMgr *g_GlwLaserItemMgr;
extern GlwLaserMgr *g_GlwLaserMgr;

void __fastcall GlwChainCutImpl(GlwChainMgr *mgr, GlwChainElem *elem);
void GlwDestroyMsgObj(void *obj);
void GlwReleaseAnm(void *anm);
void GlwPlaySfx(int id);
void __stdcall GlwGuiUpdateMeterAnms(GlwPopupMgr *gui);
void __stdcall GlwDecodeSpellBonus(int *outHi, int *outLo);
void __stdcall GlwAddFreezeGauge(int delta);
void __stdcall GlwAddMotivGauge(int delta);

} // namespace th128
