#pragma once

#include <stddef.h>
#include "CardInf.hpp"

// The target's FPS doubles have four-byte member alignment.
#pragma pack(push, 4)
struct PostPresentFrameRateState
{
    unsigned char unknown000[0x14];
    double sampleStart;                // +0x14
    int over65Windows;                 // +0x1C
    unsigned int displayedFrames;      // +0x20, unsigned conversion in target
    double accountedFrames;            // +0x24
    double nominalFrames;              // +0x2C
    float framesPerSecond;             // +0x34
};
#pragma pack(pop)

typedef char PostPresentSampleOffset[(offsetof(PostPresentFrameRateState, sampleStart) == 0x14) ? 1 : -1];
typedef char PostPresentRateOffset[(offsetof(PostPresentFrameRateState, framesPerSecond) == 0x34) ? 1 : -1];

extern void *g_PostPresentTimingState;  // 0x004B894C
extern void *g_ReplayPauseStorage;      // 0x004B8A84
extern int g_CurrentReplayStage;       // 0x004B4D14

int __cdecl UpdatePostPresentTiming();
void __cdecl UpdatePostPresentReplay();
