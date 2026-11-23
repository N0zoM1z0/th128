#include "AnmVm.hpp"
#include "AngleMath.hpp"


void AnmVm::SetColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    colorTransition.duration = duration;
    colorTransition.values[6] = 0;
    colorTransition.values[9] = 0;
    colorTransition.values[7] = 0;
    colorTransition.values[10] = 0;
    colorTransition.values[8] = 0;
    colorTransition.values[11] = 0;
    colorTransition.mode = mode;
    colorTransition.values[0] = current[0];
    colorTransition.values[3] = target[0];
    colorTransition.values[1] = current[1];
    colorTransition.values[4] = target[1];
    colorTransition.values[2] = current[2];
    colorTransition.values[5] = target[2];
    colorTransition.timer.Reset();
}

void AnmVm::SetScaleTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned char mode)
{
    scaleTransition.duration = duration;
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
    secondaryScaleTransition.duration = duration;
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
    effectTransition.duration = (int)duration;
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
    secondaryEffectTransition.duration = (int)duration;
    secondaryEffectTransition.mode = mode & 0xFFU;
    secondaryEffectTransition.values[0] = parameter0;
    secondaryEffectTransition.values[1] = parameter1;
    secondaryEffectTransition.timer.Reset();
}


void AnmVm::SetSecondaryColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    secondaryColorTransition.duration = duration;
    secondaryColorTransition.values[6] = 0;
    secondaryColorTransition.values[9] = 0;
    secondaryColorTransition.values[7] = 0;
    secondaryColorTransition.values[10] = 0;
    secondaryColorTransition.values[8] = 0;
    secondaryColorTransition.values[11] = 0;
    secondaryColorTransition.mode = mode;
    secondaryColorTransition.values[0] = current[0];
    secondaryColorTransition.values[3] = target[0];
    secondaryColorTransition.values[1] = current[1];
    secondaryColorTransition.values[4] = target[1];
    secondaryColorTransition.values[2] = current[2];
    secondaryColorTransition.values[5] = target[2];
    secondaryColorTransition.timer.Reset();
}
