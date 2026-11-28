#pragma once

#include <windows.h>

namespace th128 {

extern float g_LfbTimeScale;

struct LfbTimer {
    int prev;
    int cur;
    float curF;
    float *scale;
    unsigned int flags;

    LfbTimer() { flags &= ~1u; }

    inline void ResetToMinusOne() {
        if (!(this->flags & 1)) {
            this->curF = 0.0f;
            this->cur = 0;
            this->prev = -999999;
            this->scale = &g_LfbTimeScale;
            this->flags |= 1;
        }
        this->curF = 0.0f;
        this->cur = 0;
        this->prev = -1;
    }

    inline void SetInt(int val) {
        if (!(this->flags & 1)) {
            this->curF = 0.0f;
            this->cur = 0;
            this->prev = -999999;
            this->scale = &g_LfbTimeScale;
            this->flags |= 1;
        }
        this->cur = val;
        this->prev = val - 1;
        this->curF = (float)val;
    }

    void Increment();
};

struct LfbEffectNode {
    unsigned char pad00[0x04];
    LfbEffectNode *next04;
    LfbEffectNode *prev08;
    int active0c;
    unsigned char pad10[0x10];
    LfbTimer timer20;
    unsigned char pad34[0x1c];
    int vmId50;
    int vmId54;
    unsigned char pad58[0x08];
    LfbEffectNode *next60;
    LfbEffectNode *prev64;

    void Release();
};

struct LfbGameState {
    unsigned char pad00[0x60];
    unsigned int flags60;
    unsigned char pad64[0x10];
    int isReplay74;
};

struct LfbNameEntry {
    unsigned char pad00[0x04];
    int mode04;
    unsigned char pad08[0x08];
    LfbTimer timer10;
    unsigned char pad24[0x1c4];
    int vmId1e8;
    int vmId1ec;
    unsigned char pad1f0[0xe8];
    float savedTimeScale2d8;

    void EnterMode15();
    void EnterMode1C();
};

struct LfbGui {
    unsigned char pad00[0x58cc];
    int vmId58cc;
    int vmId58d0;
    unsigned char pad58d4[0x130];
    unsigned int flags5a04;
    LfbTimer timer5a08;
};

struct LfbLaserSub {
    unsigned char pad00[0x34];
    LfbTimer timer;
};

struct LfbLaserBase {
    virtual ~LfbLaserBase();
    unsigned char pad04[0x04];
    LfbLaserBase *next08;
    unsigned char pad0c[0x08];
    LfbTimer timer14;
    LfbTimer timer28;
    LfbTimer timer3c;
    LfbLaserSub subs[18];
    unsigned char pad560[0x40];
    LfbTimer timer5a0;
    int f5b4;
    unsigned char pad5b8[0x08];

    LfbLaserBase();
};

struct LfbLaserCancelObj {
    virtual void S0() = 0;
    virtual void S1() = 0;
    virtual void S2() = 0;
    virtual void S3() = 0;
    virtual void S4() = 0;
    virtual void S5() = 0;
    virtual void S6() = 0;
    virtual void Cancel(int a, int b) = 0; // +0x1c
    unsigned char pad04[0x04];
    LfbLaserCancelObj *next08;
    unsigned char pad0c[0x594];
    LfbTimer timer5a0;
    int f5b4;
};

struct LfbLaserMgr {
    unsigned char pad00[0x18];
    LfbLaserCancelObj *head18;
};

struct LfbBgCmd {
    int time;
    short opcode;
    short size;
    int arg0;
};

struct LfbBgMgr {
    unsigned char pad00[0x1c];
    LfbBgCmd *scriptStart;
    unsigned char pad20[0x18];
    LfbTimer timer38;
    LfbBgCmd *curCmd;
};

struct LfbScreenFade {
    unsigned char pad00[0x18];
    int alpha18;
    int duration1c;
    unsigned char pad20[0x10];
    LfbTimer timer30;

    int OnCalc();
};

struct LfbScoreMgr {
    unsigned char pad00[0x33b0];
    __int64 playTimeMs;
};

struct LfbPlayTimeObj {
    unsigned char pad00[0x88];
    double lastTime88;

    void AccumulatePlayTime();
};

extern LfbGameState *g_LfbGameState;
extern double g_LfbPauseStartTime;
extern LfbGui *g_LfbGui;
extern LfbLaserMgr *g_LfbLaserMgr;
extern LfbBgMgr *g_LfbBgMgr;
extern int g_LfbSkipFade;
extern int g_LfbSceneState;
extern LfbScoreMgr *g_LfbScoreMgr;

void LfbDeleteVmByIdPtr(int *idPtr);
void __stdcall LfbStopVmById(int id, int flag);
double LfbGetElapsedSeconds();

void LfbHideBossBar();
int LfbCancelAllLasers();
void LfbBgJumpToLabel(int label);

} // namespace th128
