#include "AnmVm.hpp"
#include "AngleMath.hpp"


void AnmVm::SetColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    colorTransition.duration = (unsigned int)duration;
    colorTransition.mode = mode;

    for (unsigned int i = 0; i != 3; ++i)
    {
        colorTransition.values[i] = current[i];
        colorTransition.values[i + 3] = target[i];
        colorTransition.values[i + 6] = 0;
        colorTransition.values[i + 9] = 0;
    }

    colorTransition.timer.Reset();
}

void AnmVm::SetScaleTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned char mode)
{
    scaleTransition.duration = (unsigned int)duration;
    scaleTransition.mode = mode;

    scaleTransition.values[0] = current[0];
    scaleTransition.values[1] = current[1];
    scaleTransition.values[2] = target[0];
    scaleTransition.values[3] = target[1];
    scaleTransition.timer.Reset();
}

void AnmVm::SetSecondaryScaleTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned char mode)
{
    secondaryScaleTransition.duration = (unsigned int)duration;
    secondaryScaleTransition.mode = mode;

    secondaryScaleTransition.values[0] = current[0];
    secondaryScaleTransition.values[1] = current[1];
    secondaryScaleTransition.values[2] = target[0];
    secondaryScaleTransition.values[3] = target[1];
    secondaryScaleTransition.timer.Reset();
}

void AnmVm::SetEffectState(
    unsigned int duration,
    unsigned char mode,
    unsigned char parameter0,
    unsigned char parameter1)
{
    effectTransition.duration = duration;
    effectTransition.mode = mode;
    effectTransition.values[0] = parameter0;
    effectTransition.values[1] = parameter1;
    effectTransition.values[2] = 0;
    effectTransition.values[3] = 0;
    effectTransition.timer.Reset();
}

void AnmVm::SetSecondaryEffectState(
    unsigned int duration,
    unsigned int mode,
    unsigned char parameter0,
    unsigned char parameter1)
{
    secondaryEffectTransition.duration = duration;
    secondaryEffectTransition.mode = mode & 0xFFU;
    secondaryEffectTransition.values[0] = parameter0;
    secondaryEffectTransition.values[1] = parameter1;
    secondaryEffectTransition.timer.Reset();
}

void AnmVm::SetPositionTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned int mode)
{
    positionTransition.duration = (unsigned int)duration;
    positionTransition.mode = mode;

    for (unsigned int i = 0; i != 3; ++i)
    {
        positionTransition.values[i] = current[i];
        positionTransition.values[i + 3] = target[i];
    }

    const float *reference = reinterpret_cast<const float *>(0x004D2E54);
    for (unsigned int i = 0; i != 3; ++i)
    {
        positionTransition.values[i + 6] = reference[i];
        positionTransition.values[i + 9] = reference[i];
    }

    positionTransition.timer.Reset();
}

void AnmVm::SetSecondaryColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    secondaryColorTransition.duration = (unsigned int)duration;
    secondaryColorTransition.mode = mode;

    for (unsigned int i = 0; i != 3; ++i)
    {
        secondaryColorTransition.values[i] = current[i];
        secondaryColorTransition.values[i + 3] = target[i];
        secondaryColorTransition.values[i + 6] = 0;
        secondaryColorTransition.values[i + 9] = 0;
    }

    secondaryColorTransition.timer.Reset();
}
