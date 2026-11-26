#pragma once

#include <stddef.h>
#include "Chain.hpp"
#include "ZunTimer.hpp"

struct AnmResource;

// Identified by card.anm and src\game\card.cpp in the original TH128 target.
struct CardInf
{
    unsigned int lifecycleFlags;      // +0x00
    unsigned int unknown004;
    ChainElem *calcElement;            // +0x08
    ChainElem *drawElement;            // +0x0C
    int unknown010;
    unsigned int vmIds[3];             // +0x14, released through DeleteVmByIdPtr
    int unknown020;
    ZunTimer timer;                    // +0x24
    unsigned char unknown038[0x40];
    int unknown078;
    unsigned int flags;                // +0x7C, bit 0 paused / bit 7 timed
    unsigned char unknown080[0x0C];
    int pauseSampleIndex;              // +0x8C
    int frameMarker;                   // +0x90
    int previousFrameMarker;           // +0x94
    double pauseStarted;               // +0x98
    double duration;                   // +0xA0
    int encodedDuration;               // +0xA8
    unsigned char unknown0AC[0x0C];
    AnmResource *resource;             // +0xB8, ANM slot 37
    unsigned int unknown0BC;

    static void EnableChains();
    int HasInvalidDurationCode();
};

typedef char CardInfSize[(sizeof(CardInf) == 0xC0) ? 1 : -1];
typedef char CardInfTimerOffset[(offsetof(CardInf, timer) == 0x24) ? 1 : -1];
typedef char CardInfPauseStartOffset[(offsetof(CardInf, pauseStarted) == 0x98) ? 1 : -1];
typedef char CardInfCodeOffset[(offsetof(CardInf, encodedDuration) == 0xA8) ? 1 : -1];
typedef char CardInfResourceOffset[(offsetof(CardInf, resource) == 0xB8) ? 1 : -1];

extern CardInf *g_CardInf;              // 0x004B8934
