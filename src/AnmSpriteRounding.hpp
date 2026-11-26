#pragma once

#include <float.h>
#include <math.h>

// Mathematical sprite-edge rounding for the x87 mode read by the caller.
// This natural C++ implementation is a codegen frontier for target FRNDINT.
inline float SnapAnmSpriteEdge(float value, unsigned int roundingMode)
{
    if (!_finite(value))
        return value - 0.5f;

    double rounded;
    switch (roundingMode & _MCW_RC)
    {
    case _RC_DOWN:
        rounded = floor((double)value);
        break;
    case _RC_UP:
        rounded = ceil((double)value);
        break;
    case _RC_CHOP:
        rounded = value < 0.0f ? ceil((double)value) : floor((double)value);
        break;
    default:
        rounded = floor((double)value);
        const double fraction = (double)value - rounded;
        if (fraction > 0.5 ||
            (fraction == 0.5 && fmod(rounded, 2.0) != 0.0))
            rounded += 1.0;
        break;
    }
    return (float)(rounded - 0.5);
}
