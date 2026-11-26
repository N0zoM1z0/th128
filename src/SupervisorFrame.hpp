#pragma once

#include <stddef.h>

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

    void PresentAdaptive();            // 0x00454940; not yet reconstructed
    void PresentImmediate();           // 0x00454D90; SupervisorPresent.cpp
    void PresentWithSleep();           // 0x00454F70; SupervisorPresent.cpp
    void HandleScreenshot();           // 0x004550B0; SupervisorScreenshot.cpp
};
typedef char SupervisorFrameTimeOffset[(offsetof(SupervisorFrame, currentTime) == 0x2040) ? 1 : -1];

int __stdcall RunAdaptiveFrame(SupervisorFrame *frame);
int __stdcall RunTimedFrame(SupervisorFrame *frame);
int __stdcall RunVsyncFrame(SupervisorFrame *frame);
