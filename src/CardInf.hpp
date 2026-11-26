#pragma once

#include <stddef.h>
#include "Chain.hpp"
#include "ZunTimer.hpp"
#include "AnmManager.hpp"
#include <string.h>

struct AnmResource;

// Identified by card.anm and src\game\card.cpp in the original TH128 target.
struct CardInf
{
    unsigned int lifecycleFlags;      // +0x00
    unsigned int unknown004;
    ChainElem *calcElement;            // +0x08
    ChainElem *drawElement;            // +0x0C
    AnmId auxiliaryVmId;              // +0x10
    AnmId vmIds[3];                   // +0x14, released through DeleteVmByIdPtr
    AnmId positionVmId;               // +0x20
    ZunTimer timer;                    // +0x24
    char name[0x40];                  // +0x38
    int unknown078;
    unsigned int flags;                // +0x7C, bit 0 active / bit 7 clock started
    int bonus;                        // +0x80
    int initialBonus;                 // +0x84
    int bonusDurationFrames;          // +0x88
    int durationSampleIndex;          // +0x8C
    int activeFrameCount;             // +0x90
    int completedFrameCount;           // +0x94
    double activeStartedAt;           // +0x98
    double duration;                   // +0xA0
    int encodedDuration;               // +0xA8
    ZunVec3 trackedPosition;           // +0xAC
    AnmResource *resource;             // +0xB8, ANM slot 37
    unsigned int unknown0BC;

    CardInf();
    static CardInf *Create();
    static void Destroy();
    int Initialize();
    void Shutdown();
    int Update();
    int Draw();
    static int __fastcall OnTick(void *card);
    static int __fastcall OnDraw(void *card);
    static void EnableChains();
    int HasInvalidDurationCode();
};

typedef char CardInfSize[(sizeof(CardInf) == 0xC0) ? 1 : -1];
typedef char CardInfTimerOffset[(offsetof(CardInf, timer) == 0x24) ? 1 : -1];
typedef char CardInfActiveStartOffset[(offsetof(CardInf, activeStartedAt) == 0x98) ? 1 : -1];
typedef char CardInfCodeOffset[(offsetof(CardInf, encodedDuration) == 0xA8) ? 1 : -1];
typedef char CardInfResourceOffset[(offsetof(CardInf, resource) == 0xB8) ? 1 : -1];

extern CardInf *g_CardInf;              // 0x004B8934

inline CardInf::CardInf()
{
    memset(this, 0, sizeof(*this));
    lifecycleFlags |= 2U;
    g_CardInf = this;
}
