#include "AngleMath.hpp"

namespace
{
// These match the target's double constants at 0x004A6580 and 0x004A6578.
const double kPi = 3.14159265358979323846;
const double kTwoPi = 6.28318530717958647692;
}

float __stdcall NormalizeAngle(float angle)
{
    int corrections = 0;
    while (angle > kPi)
    {
        angle = static_cast<float>(angle - kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    // The target lower bound is -float(pi) widened to double, not -kPi.
    while (angle < -3.1415927410125732421875)
    {
        angle = static_cast<float>(angle + kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    return angle;
}

float __stdcall AddAngles(float angle, float offset)
{
    // Keep the initial sum in the x87 register. The target only rounds it
    // through the float parameter slot when a wraparound adjustment occurs.
    float value = angle;
    value += offset;
    int corrections = 0;
    while (value > kPi)
    {
        value = (float)(value - kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    // Match the same float-rounded lower bound in the angle-add helper.
    while (value < -3.1415927410125732421875)
    {
        value = (float)(value + kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    return value;
}


float __stdcall ShortestAngleDifference(float target, float current)
{
    const double difference = (double)target - (double)current;
    if (kPi < difference)
    {
        return (float)((double)target - ((double)current + kTwoPi));
    }
    if (kPi < (double)current - (double)target)
    {
        return (float)((double)target - ((double)current - kTwoPi));
    }
    return (float)difference;
}
