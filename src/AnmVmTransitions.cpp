#include "AnmVm.hpp"
#include "AngleMath.hpp"


namespace
{
struct AnmTransitionColor
{
    int r;
    int g;
    int b;

    AnmTransitionColor(int red, int green, int blue)
        : r(red), g(green), b(blue) {}

    AnmTransitionColor(const unsigned char *rgb)
    {
        b = rgb[2];
        g = rgb[1];
        r = rgb[0];
    }
};
}

void AnmVm::SetColorTransition(
    const unsigned char *current,
    const unsigned char *target,
    int duration,
    unsigned char mode)
{
    colorTransition.duration = duration;
    AnmTransitionColor zero(0, 0, 0);
    *(AnmTransitionColor *)&colorTransition.values[6] = zero;
    *(AnmTransitionColor *)&colorTransition.values[9] = zero;
    colorTransition.mode = mode;
    // Snapshot both byte triples before writing either endpoint.
    AnmTransitionColor start(current);
    AnmTransitionColor goal(target);
    *(AnmTransitionColor *)&colorTransition.values[0] = start;
    *(AnmTransitionColor *)&colorTransition.values[3] = goal;
    colorTransition.timer.Initialize();
    colorTransition.timer.currentF = 0.0f;
    colorTransition.timer.current = 0;
    colorTransition.timer.previous = -1;
}

namespace
{
struct AnmScalePair
{
    float x;
    float y;
};
}

void AnmVm::SetScaleTransition(
    const float *current,
    const float *target,
    int duration,
    unsigned char mode)
{
    scaleTransition.duration = duration;
    scaleTransition.mode = mode;

    *(AnmScalePair *)&scaleTransition.values[0] = *(const AnmScalePair *)current;
    *(AnmScalePair *)&scaleTransition.values[2] = *(const AnmScalePair *)target;
    scaleTransition.timer.Initialize();
    scaleTransition.timer.currentF = 0.0f;
    scaleTransition.timer.current = 0;
    scaleTransition.timer.previous = -1;
}

void AnmVm::SetSecondaryScaleTransition(
    const float *current,
    const float *target,
    int duration,
    unsigned char mode)
{
    secondaryScaleTransition.duration = duration;
    secondaryScaleTransition.mode = mode;

    *(AnmScalePair *)&secondaryScaleTransition.values[0] = *(const AnmScalePair *)current;
    *(AnmScalePair *)&secondaryScaleTransition.values[2] = *(const AnmScalePair *)target;
    secondaryScaleTransition.timer.Initialize();
    secondaryScaleTransition.timer.currentF = 0.0f;
    secondaryScaleTransition.timer.current = 0;
    secondaryScaleTransition.timer.previous = -1;
}

void AnmVm::SetEffectState(
    unsigned int duration,
    unsigned int mode,
    unsigned char parameter0,
    unsigned char parameter1)
{
    effectTransition.duration = (int)duration;
    effectTransition.mode = mode & 0xFFU;
    effectTransition.values[0] = parameter0;
    effectTransition.values[1] = parameter1;
    effectTransition.values[2] = 0;
    effectTransition.values[3] = 0;
    effectTransition.timer.Initialize();
    effectTransition.timer.currentF = 0.0f;
    effectTransition.timer.current = 0;
    effectTransition.timer.previous = -1;
}

void AnmVm::SetSecondaryEffectState(
    unsigned int duration,
    unsigned char mode,
    unsigned char parameter0,
    unsigned char parameter1)
{
    secondaryEffectTransition.duration = (int)duration;
    secondaryEffectTransition.mode = mode;
    secondaryEffectTransition.values[0] = parameter0;
    secondaryEffectTransition.values[1] = parameter1;
    secondaryEffectTransition.timer.Initialize();
    secondaryEffectTransition.timer.currentF = 0.0f;
    secondaryEffectTransition.timer.current = 0;
    secondaryEffectTransition.timer.previous = -1;
}


void AnmVm::SetSecondaryColorTransition(
    const unsigned char *current,
    const unsigned char *target,
    int duration,
    unsigned char mode)
{
    secondaryColorTransition.duration = duration;
    AnmTransitionColor zero(0, 0, 0);
    *(AnmTransitionColor *)&secondaryColorTransition.values[6] = zero;
    *(AnmTransitionColor *)&secondaryColorTransition.values[9] = zero;
    secondaryColorTransition.mode = mode;
    // Snapshot both byte triples before writing either endpoint.
    AnmTransitionColor start(current);
    AnmTransitionColor goal(target);
    *(AnmTransitionColor *)&secondaryColorTransition.values[0] = start;
    *(AnmTransitionColor *)&secondaryColorTransition.values[3] = goal;
    secondaryColorTransition.timer.Initialize();
    secondaryColorTransition.timer.currentF = 0.0f;
    secondaryColorTransition.timer.current = 0;
    secondaryColorTransition.timer.previous = -1;
}
