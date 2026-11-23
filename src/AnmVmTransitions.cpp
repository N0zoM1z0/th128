#include "AnmVm.hpp"


namespace
{
    double EaseTransition(unsigned int mode, double t)
    {
        const double inverse = 1.0 - t;
        switch (mode)
        {
        case 1: return t * t;
        case 2: return t * t * t;
        case 3: return t * t * t * t;
        case 4: return 1.0 - inverse * inverse;
        case 5: return 1.0 - inverse * inverse * inverse;
        case 6: return 1.0 - inverse * inverse * inverse * inverse;
        case 7:
        case 8:
            return t;
        case 9:
            return t < 0.5
                ? 2.0 * t * t
                : 1.0 - 2.0 * inverse * inverse;
        case 10:
            return t < 0.5
                ? 4.0 * t * t * t
                : 1.0 - 4.0 * inverse * inverse * inverse;
        case 11:
            return t < 0.5
                ? 8.0 * t * t * t * t
                : 1.0 - 8.0 * inverse * inverse * inverse * inverse;
        case 12:
        {
            const double x = t * 2.0 - 1.0;
            return t < 0.5
                ? 0.5 - 0.5 * (1.0 - 2.0 * t) * (1.0 - 2.0 * t)
                : 0.5 + 0.5 * x * x;
        }
        case 13:
        {
            const double x = t * 2.0 - 1.0;
            return t < 0.5
                ? 0.5 - 0.5 * (1.0 - 2.0 * t) *
                    (1.0 - 2.0 * t) * (1.0 - 2.0 * t)
                : 0.5 + 0.5 * x * x * x;
        }
        case 14:
        {
            const double x = t * 2.0 - 1.0;
            return t < 0.5
                ? 0.5 - 0.5 * (1.0 - 2.0 * t) *
                    (1.0 - 2.0 * t) * (1.0 - 2.0 * t) * (1.0 - 2.0 * t)
                : 0.5 + 0.5 * x * x * x * x;
        }
        case 15: return 0.0;
        case 16: return 1.0;
        default: return t;
        }
    }

    double Hermite(
        double start,
        double target,
        double startTangent,
        double targetTangent,
        double t)
    {
        const double t2 = t * t;
        const double t3 = t2 * t;
        const double h00 = (2.0 * t3) - (3.0 * t2) + 1.0;
        const double h01 = (-2.0 * t3) + (3.0 * t2);
        const double h10 = t3 - (2.0 * t2) + t;
        const double h11 = t3 - t2;
        return h00 * start + h01 * target +
            h10 * startTangent + h11 * targetTangent;
    }

    bool AdvanceTransition(
        ZunTimer *timer,
        unsigned int *duration,
        unsigned int mode)
    {
        if (*duration == 0)
        {
            return false;
        }

        timer->Increment();
        if (timer->current < (int)*duration)
        {
            return false;
        }

        timer->SetCurrent((int)*duration);
        *duration = 0;
        return true;
    }

    void InterpolateColorVector(AnmVmColorTransition *transition, int result[3])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            const unsigned int endpoint = (mode == 7) ? 0 : 3;
            for (unsigned int i = 0; i != 3; ++i)
            {
                result[i] = transition->values[endpoint + i];
            }
            return;
        }

        if (mode == 7)
        {
            for (unsigned int i = 0; i != 3; ++i)
            {
                transition->values[i] += transition->values[i + 3];
                result[i] = transition->values[i];
            }
            return;
        }

        if (mode == 17)
        {
            for (unsigned int i = 0; i != 3; ++i)
            {
                transition->values[i] += transition->values[i + 9];
                result[i] = transition->values[i];
                transition->values[i + 9] += transition->values[i + 3];
            }
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        for (unsigned int i = 0; i != 3; ++i)
        {
            double value;
            if (mode == 8)
            {
                value = Hermite(
                    transition->values[i],
                    transition->values[i + 3],
                    transition->values[i + 6],
                    transition->values[i + 9],
                    t);
            }
            else
            {
                const double eased = EaseTransition(mode, t);
                value = (double)transition->values[i] +
                    ((double)transition->values[i + 3] -
                     (double)transition->values[i]) * eased;
            }
            result[i] = ZunRoundToInt(value);
        }
    }

    int InterpolateInteger(AnmVmIntegerTransition *transition)
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            return transition->values[(mode == 7) ? 0 : 1];
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[1];
            return transition->values[0];
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[3];
            transition->values[3] += transition->values[1];
            return transition->values[0];
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        double value;
        if (mode == 8)
        {
            value = Hermite(
                transition->values[0],
                transition->values[1],
                transition->values[2],
                transition->values[3],
                t);
        }
        else
        {
            const double eased = EaseTransition(mode, t);
            value = (double)transition->values[0] +
                ((double)transition->values[1] -
                 (double)transition->values[0]) * eased;
        }
        return ZunRoundToInt(value);
    }

    void InterpolateVector(AnmVmVectorTransition *transition, float result[3])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            const unsigned int endpoint = (mode == 7) ? 0 : 3;
            for (unsigned int i = 0; i != 3; ++i)
            {
                result[i] = transition->values[endpoint + i];
            }
            return;
        }

        if (mode == 7)
        {
            for (unsigned int i = 0; i != 3; ++i)
            {
                transition->values[i] += transition->values[i + 3];
                result[i] = transition->values[i];
            }
            return;
        }

        if (mode == 17)
        {
            for (unsigned int i = 0; i != 3; ++i)
            {
                transition->values[i] += transition->values[i + 9];
                result[i] = transition->values[i];
                transition->values[i + 9] += transition->values[i + 3];
            }
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        for (unsigned int i = 0; i != 3; ++i)
        {
            double value;
            if (mode == 8)
            {
                value = Hermite(
                    transition->values[i],
                    transition->values[i + 3],
                    transition->values[i + 6],
                    transition->values[i + 9],
                    t);
            }
            else
            {
                const double eased = EaseTransition(mode, t);
                value = (double)transition->values[i] +
                    ((double)transition->values[i + 3] -
                     (double)transition->values[i]) * eased;
            }
            result[i] = (float)value;
        }
    }

    void InterpolateScale(AnmVmScaleTransition *transition, float result[2])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            const unsigned int endpoint = (mode == 7) ? 0 : 2;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            return;
        }

        if (mode == 7)
        {
            for (unsigned int i = 0; i != 2; ++i)
            {
                transition->values[i] += transition->values[i + 2];
                result[i] = transition->values[i];
            }
            return;
        }

        if (mode == 17)
        {
            for (unsigned int i = 0; i != 2; ++i)
            {
                transition->values[i] += transition->values[i + 6];
                result[i] = transition->values[i];
                transition->values[i + 6] += transition->values[i + 2];
            }
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        for (unsigned int i = 0; i != 2; ++i)
        {
            double value;
            if (mode == 8)
            {
                value = Hermite(
                    transition->values[i],
                    transition->values[i + 2],
                    transition->values[i + 4],
                    transition->values[i + 6],
                    t);
            }
            else
            {
                const double eased = EaseTransition(mode, t);
                value = (double)transition->values[i] +
                    ((double)transition->values[i + 2] -
                     (double)transition->values[i]) * eased;
            }
            result[i] = (float)value;
        }
    }

    float InterpolateScalar(AnmVmScalarTransition *transition)
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            return transition->values[(mode == 7) ? 0 : 1];
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[1];
            return transition->values[0];
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[3];
            transition->values[3] += transition->values[1];
            return transition->values[0];
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            return (float)Hermite(
                transition->values[0],
                transition->values[1],
                transition->values[2],
                transition->values[3],
                t);
        }

        const double eased = EaseTransition(mode, t);
        return (float)((double)transition->values[0] +
            ((double)transition->values[1] -
             (double)transition->values[0]) * eased);
    }
}

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
    secondaryEffectTransition.values[2] = 0;
    secondaryEffectTransition.values[3] = 0;
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

void AnmVm::UpdateTransitions()
{
    if (colorTransition.duration != 0)
    {
        int result[3];
        InterpolateColorVector(&colorTransition, result);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&color);
        channels[0] = (unsigned char)result[0];
        channels[1] = (unsigned char)result[1];
        channels[2] = (unsigned char)result[2];
    }

    if (effectTransition.duration != 0)
    {
        unsigned char *channels = reinterpret_cast<unsigned char *>(&color);
        channels[3] = (unsigned char)InterpolateInteger(&effectTransition);
    }

    if (scaleTransition.duration != 0)
    {
        float result[2];
        InterpolateScale(&scaleTransition, result);
        scaleX = result[0];
        scaleY = result[1];
        flags |= 8U;
    }

    if (secondaryScaleTransition.duration != 0)
    {
        float result[2];
        InterpolateScale(&secondaryScaleTransition, result);
        scale2X = result[0];
        scale2Y = result[1];
        flags |= 0x10U;
    }

    if (positionTransition.duration != 0)
    {
        float result[3];
        InterpolateVector(&positionTransition, result);
        unknown024 = result[0];
        unknown028 = result[1];
        unknown02C = result[2];
        flags |= 4U;
    }

    if (viewTransition.duration != 0)
    {
        unknown02C = InterpolateScalar(&viewTransition);
        flags |= 4U;
    }

    if (secondaryColorTransition.duration != 0)
    {
        int result[3];
        InterpolateColorVector(&secondaryColorTransition, result);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&secondaryColor);
        channels[0] = (unsigned char)result[0];
        channels[1] = (unsigned char)result[1];
        channels[2] = (unsigned char)result[2];
    }

    if (secondaryEffectTransition.duration != 0)
    {
        unsigned char *channels = reinterpret_cast<unsigned char *>(&secondaryColor);
        channels[3] = (unsigned char)InterpolateInteger(&secondaryEffectTransition);
    }

    if (textureOffsetXTransition.duration != 0)
    {
        textureOffsetX = InterpolateScalar(&textureOffsetXTransition);
    }

    if (textureOffsetYTransition.duration != 0)
    {
        textureOffsetY = InterpolateScalar(&textureOffsetYTransition);
    }
}
