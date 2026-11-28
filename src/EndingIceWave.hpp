#pragma once

#include <windows.h>

namespace th128 {

struct EiwVec3 {
    float x;
    float y;
    float z;
};

extern float g_EiwTimeScale;

struct EiwTimer {
    int previous;
    int current;
    float currentF;
    float *gameSpeed;
    unsigned int flags;

    void Initialize() {
        if ((flags & 1) == 0) {
            currentF = 0.0f;
            current = 0;
            previous = -999999;
            gameSpeed = &g_EiwTimeScale;
            flags |= 1u;
        }
    }
    void SetOne() {
        Initialize();
        current = 1;
        currentF = 1.0f;
        previous = 0;
    }

    static float &TimeScale();
};

extern float g_EiwTimeScale;

struct EiwEffectData {
    float pts[16][2];       // +0x00..+0x7F
    unsigned int colors[16];// +0x80..+0xBF
    float angleC0;          // +0xC0
    int prevC4;             // +0xC4
    int curC8;              // +0xC8
    float curFCC;           // +0xCC
    float *gameSpeedD0;     // +0xD0
    unsigned int flagsD4;   // +0xD4
};

struct EiwAnmVm {
    unsigned char pad0[0x20];
    int mode20;             // +0x20
    unsigned char pad24[0x464 - 0x24];
    EiwVec3 pos464;         // +0x464
    unsigned char pad470[0x4ac - 0x470];
    void *extraData4ac;     // +0x4AC
};

struct EiwRng {
    unsigned short seed;
    unsigned short pad2;
    unsigned int count;

    unsigned int GetU32();
    float GetFloatSigned() {
        return (float)((double)(float)GetU32() * (1.0 / 2147483648.0) - 1.0);
    }
};

struct EiwChainElem;
struct EiwChainMgr {
    int dummy;
    void Cut(EiwChainElem *elem);
};

void EiwReleaseAnm(void *anm);

struct EiwAnmMgr {
    unsigned char pad0[0x4e9140];
    void *res0;
    void *res1;
    void *res2;
    void *res3;

    void ReleaseSlot(void *&slot) {
        if (slot != NULL) {
            EiwReleaseAnm(slot);
            ::operator delete(slot);
            slot = NULL;
        }
    }
};

struct EiwEndingSub {
    int dummy;
    ~EiwEndingSub();
};

struct EiwEnding {
    unsigned char pad0[8];
    EiwChainElem *chain8;
    EiwChainElem *chainC;
    int pad10;
    void *buf14;
    EiwEndingSub *sub18;

    ~EiwEnding();
};

struct EiwIceGrid {
    unsigned char pad0[0x65de4];
    unsigned int bits[448][12]; // +0x65DE4 (0x19779 * 4 = 0x65DE4!)

    void ClearIceMask(const EiwVec3 *pos, const unsigned int *mask, int height, int width);
    void SetIceMask(const EiwVec3 *pos, const unsigned int *mask, int height, int width);
};

extern EiwRng g_EiwRng;
extern EiwChainMgr *g_EiwChainMgr;
extern EiwAnmMgr *g_EiwAnmMgr;
extern EiwEnding *g_EiwEnding;
extern void *g_EiwGlobalAux;
extern unsigned int g_EiwGameFlags;
extern CRITICAL_SECTION g_EiwChainCs;
extern unsigned char g_EiwChainLockDepth;

void __fastcall EiwChainCutInternal(EiwChainMgr *mgr, EiwChainElem *elem);

int __fastcall EiwStartEffectBlue(EiwAnmVm *vm, const EiwVec3 *pos);
int __fastcall EiwStartEffectGreen(EiwAnmVm *vm, const EiwVec3 *pos);

} // namespace th128
