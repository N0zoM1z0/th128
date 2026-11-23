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
    while (angle < -kPi)
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
    // FUN_0046A660 keeps the initial sum in the x87 extended register, but
    // rounds each wraparound adjustment back to float before comparing again.
    double value = (double)angle + (double)offset;
    int corrections = 0;
    while (value > kPi)
    {
        value = (double)(float)(value - kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    while (value < -kPi)
    {
        value = (double)(float)(value + kTwoPi);
        if (corrections++ > 32)
        {
            break;
        }
    }
    return (float)value;
}
