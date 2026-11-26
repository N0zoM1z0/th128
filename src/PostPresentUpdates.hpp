#pragma once

#include <stddef.h>

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

struct ReplayPauseState
{
    unsigned char unknown000[0x7C];
    unsigned int flags;                // +0x7C, bit 0 paused / bit 7 timed
    unsigned char unknown080[0x0C];
    int pauseSampleIndex;              // +0x8C
    int frameMarker;                   // +0x90
    int previousFrameMarker;           // +0x94
    double pauseStarted;               // +0x98
    double duration;                   // +0xA0
    int encodedDuration;               // +0xA8

    int HasInvalidDurationCode();
};

typedef char PostPresentSampleOffset[(offsetof(PostPresentFrameRateState, sampleStart) == 0x14) ? 1 : -1];
typedef char PostPresentRateOffset[(offsetof(PostPresentFrameRateState, framesPerSecond) == 0x34) ? 1 : -1];
typedef char ReplayPauseStartOffset[(offsetof(ReplayPauseState, pauseStarted) == 0x98) ? 1 : -1];
typedef char ReplayPauseCodeOffset[(offsetof(ReplayPauseState, encodedDuration) == 0xA8) ? 1 : -1];

extern void *g_PostPresentTimingState;  // 0x004B894C
extern void *g_PostPresentReplayState;  // 0x004B8934
extern void *g_ReplayPauseStorage;      // 0x004B8A84
extern int g_CurrentReplayStage;       // 0x004B4D14

int __cdecl UpdatePostPresentTiming();
void __cdecl UpdatePostPresentReplay();
