#pragma once

#include <stddef.h>

struct SupervisorSleepProfile
{
    int maximumMilliseconds;
    int sleepMilliseconds;
    int stableFrameCount;
};

// Partial main-window/frame-clock view rooted at Supervisor + 0xB10
// (0x004D3970). The frame entry points receive this view on the stack.
struct SupervisorFrame
{
    unsigned char unknown000[0x14];
    signed char framesSinceDraw;       // +0x0014
    unsigned char unknown015[0x202B];
    double currentTime;               // +0x2040
    double previousTime;              // +0x2048
    double nextTickTime;              // +0x2050
    double timeOrigin;                // +0x2058 (GetElapsedSeconds' global)
    double lastPresentTime;           // +0x2060
    double lastRasterTime;            // +0x2068
    unsigned char unknown2070[4];
    int estimatedSleepMilliseconds;   // +0x2074
    int sleepProfileIndex;            // +0x2078
    SupervisorSleepProfile sleepProfiles[4]; // +0x207C; initial sleeps 15/12/12/8

    void PresentAdaptive();            // 0x00454940; SupervisorPresent.cpp
    void PresentImmediate();           // 0x00454D90; SupervisorPresent.cpp
    void PresentWithSleep();           // 0x00454F70; SupervisorPresent.cpp
    void HandleScreenshot();           // 0x004550B0; SupervisorScreenshot.cpp
};
typedef char SupervisorFrameTimeOffset[(offsetof(SupervisorFrame, currentTime) == 0x2040) ? 1 : -1];
typedef char SupervisorFrameSleepOffset[(offsetof(SupervisorFrame, estimatedSleepMilliseconds) == 0x2074) ? 1 : -1];
typedef char SupervisorFrameProfileOffset[(offsetof(SupervisorFrame, sleepProfileIndex) == 0x2078) ? 1 : -1];
typedef char SupervisorFrameProfilesOffset[(offsetof(SupervisorFrame, sleepProfiles) == 0x207C) ? 1 : -1];

int __stdcall RunAdaptiveFrame(SupervisorFrame *frame);
int __stdcall RunTimedFrame(SupervisorFrame *frame);
int __stdcall RunVsyncFrame(SupervisorFrame *frame);
