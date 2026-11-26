// Non-runnable VC10 /GL context for the real frame-entry and viewport code.
#include "../src/SupervisorFrame.hpp"
#include "../src/AnmViewport.hpp"

float (__stdcall *g_ProbeViewportTan)(float) = AnmViewportTan;

extern "C" int __stdcall SupervisorFrameProbe(
    SupervisorFrame *frame, AnmSpriteViewport *viewport, float angle)
{
    viewport->Set2DView();
    const float slope = AnmViewportTan(angle);
    if (slope > 0.0f)
        return RunAdaptiveFrame(frame);
    if (slope < 0.0f)
        return RunTimedFrame(frame);
    return RunVsyncFrame(frame);
}
