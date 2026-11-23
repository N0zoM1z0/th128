#include "AnmVm.hpp"

void AnmVm::SetColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    unsigned int *values = reinterpret_cast<unsigned int *>(colorTransitionValues);
    unsigned int *offsets = reinterpret_cast<unsigned int *>(colorTransitionOffsets);

    colorTransitionDuration = duration;
    colorTransitionMode = mode;

    values[0] = current[0];
    values[1] = current[1];
    values[2] = current[2];
    values[3] = target[0];
    values[4] = target[1];
    values[5] = target[2];

    for (unsigned int i = 0; i != 6; ++i)
    {
        offsets[i] = 0;
    }

    timer120.Reset();
}

void AnmVm::SetScaleTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned char mode)
{
    scaleTransitionDuration = duration;
    scaleTransitionMode = mode;

    scaleTransitionStart[0] = current[0];
    scaleTransitionStart[1] = current[1];
    scaleTransitionTarget[0] = target[0];
    scaleTransitionTarget[1] = target[1];

    timer200.Reset();
}

void AnmVm::SetPositionTransition(
    const float *target,
    const float *current,
    int duration,
    unsigned char mode)
{
    positionTransitionDuration = duration;
    positionTransitionMode = mode;

    positionTransitionStart[0] = current[0];
    positionTransitionStart[1] = current[1];
    positionTransitionTarget[0] = target[0];
    positionTransitionTarget[1] = target[1];

    timer23C.Reset();
}

void AnmVm::SetSecondaryColorTransition(
    const unsigned char *target,
    const unsigned char *current,
    int duration,
    unsigned char mode)
{
    unsigned int *values =
        reinterpret_cast<unsigned int *>(secondaryColorTransitionValues);
    unsigned int *offsets =
        reinterpret_cast<unsigned int *>(secondaryColorTransitionOffsets);

    secondaryColorTransitionDuration = duration;
    secondaryColorTransitionMode = mode;

    values[0] = current[0];
    values[1] = current[1];
    values[2] = current[2];
    values[3] = target[0];
    values[4] = target[1];
    values[5] = target[2];

    for (unsigned int i = 0; i != 6; ++i)
    {
        offsets[i] = 0;
    }

    timer288.Reset();
}

void AnmVm::SetSecondaryEffectTransition(
    void *effectResource,
    unsigned int effectIndex,
    unsigned int duration,
    unsigned char mode)
{
    secondaryEffectResource = effectResource;
    secondaryEffectIndex = effectIndex & 0xFFU;
    secondaryEffectDuration = duration;
    secondaryEffectMode = mode;
    timer2B4.Reset();
}
