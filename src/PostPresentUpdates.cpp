#include "PostPresentUpdates.hpp"
#include "Supervisor.hpp"

#include <math.h>

double __cdecl GetElapsedSeconds();
struct AnmPauseStateView;
extern AnmPauseStateView *g_AnmPauseState; // 0x004B8954

namespace
{
    struct PostPresentGameplayState
    {
        unsigned char unknown000[0x60];
        unsigned int flags;
        unsigned char unknown064[0x10];
        int replayMode;               // +0x74
    };

    inline PostPresentGameplayState *GameplayState()
    {
        return reinterpret_cast<PostPresentGameplayState *>(g_AnmPauseState);
    }

    struct ReplayPausePlaybackSlot
    {
        unsigned char *stream;
        unsigned char unknown004[0x20];
    };
}

int __cdecl UpdatePostPresentTiming()
{
    PostPresentFrameRateState *state = static_cast<PostPresentFrameRateState *>(g_PostPresentTimingState);
    const double now = GetElapsedSeconds();
    if (now < state->sampleStart)
        state->sampleStart = now;
    const double elapsed = now - state->sampleStart;
    if (elapsed >= 1.0)
    {
        state->sampleStart += elapsed;
        const float rate = (float)(state->displayedFrames / elapsed);
        state->framesPerSecond = rate;
        if (rate > 65.0f)
            ++state->over65Windows;
        else
            state->over65Windows = 0;
        PostPresentGameplayState *game = GameplayState();
        if (game != 0)
        {
            if ((game->flags & 0x14U) == 0)
            {
                state->nominalFrames += 60.0;
                state->accountedFrames += rate > 57.0 ? 60.0 : rate;
            }
            game->flags &= ~0x80U;
        }
        state->displayedFrames = 0;
    }
    state->displayedFrames += g_Supervisor.configFlag304E + 1;
    return 1;
}

int ReplayPauseState::HasInvalidDurationCode()
{
    const int seconds = ((encodedDuration / 100) % 1000 + 934) % 1000;
    const int hundredths = (encodedDuration % 100 + 67) % 100;
    return encodedDuration / 100000 - 22 != seconds + hundredths;
}

void __cdecl UpdatePostPresentReplay()
{
    ReplayPauseState *state = static_cast<ReplayPauseState *>(g_PostPresentReplayState);
    if ((state->flags & 1U) != 0)
    {
        if ((state->flags & 0x80U) == 0)
        {
            state->pauseStarted = GetElapsedSeconds();
            state->flags |= 0x80U;
        }
        return;
    }
    if ((state->flags & 0x80U) == 0)
        return;

    state->previousFrameMarker = state->frameMarker;
    const double elapsed = GetElapsedSeconds() - state->pauseStarted;
    state->duration = elapsed;
    const double remainder = fmod(elapsed, 0.0167);
    state->duration = elapsed - remainder;
    if (remainder >= 0.00835)
        state->duration += 0.0167;
    const double integral = floor(state->duration);
    int seconds = (int)integral;
    if (seconds >= 1000)
        seconds = 999;
    const int hundredths = (int)((state->duration - integral) * 100.0);
    state->duration = 0.0;
    state->flags &= ~0x80U;
    state->encodedDuration = ((seconds + hundredths + 22) * 1000 + (seconds + 66) % 1000) * 100
        + (hundredths + 33) % 100;

    unsigned char *storage = static_cast<unsigned char *>(g_ReplayPauseStorage);
    if (GameplayState()->replayMode == 0)
    {
        unsigned char *stream = reinterpret_cast<unsigned char **>(
            storage + 0x20)[g_CurrentReplayStage];
        reinterpret_cast<int *>(stream + 0x30)[state->pauseSampleIndex] = state->encodedDuration;
        ++state->pauseSampleIndex;
        return;
    }
    ReplayPausePlaybackSlot *slots = reinterpret_cast<ReplayPausePlaybackSlot *>(
        storage + 0x148);
    state->encodedDuration = reinterpret_cast<int *>(slots[g_CurrentReplayStage].stream + 0x30)[state->pauseSampleIndex];
    if (state->HasInvalidDurationCode())
        state->encodedDuration = 0x06AD1584; // Encoded 999.99-second fallback.
    ++state->pauseSampleIndex;
}
