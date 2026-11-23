#include "ZunTimer.hpp"

#include <math.h>

namespace
{
    int RoundTimerValueToNearestEven(float value)
    {
        const double lower = floor((double)value);
        const double fraction = (double)value - lower;
        double rounded = lower;

        if (fraction > 0.5 || (fraction == 0.5 && fmod(lower, 2.0) != 0.0))
        {
            rounded += 1.0;
        }

        return (int)rounded;
    }
}


int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor)
{
    return timer->current % divisor;
}

bool ZunTimer::operator<(int value) const
{
    return current < value;
}

bool ZunTimer::operator<=(int value) const
{
    return current <= value;
}

bool ZunTimer::operator>=(int value) const
{
    return current >= value;
}

bool ZunTimer::operator==(int value) const
{
    return current == value;
}

bool ZunTimer::operator>(int value) const
{
    return current > value;
}

void ZunTimer::SetCurrent(int value)
{
    Initialize();
    current = value;
    previous = value - 1;
    currentF = (float)value;
}

void ZunTimer::SetCurrentF(float value)
{
    Initialize();
    current = (int)value;
    currentF = value;
    previous = current - 1;
}

// Advances by `value` scaled by the game speed; the speed is ignored when it
// is within (0.99, 1.01).
void ZunTimer::Add(float value)
{
    previous = current;
    if (*gameSpeed > 0.99 && *gameSpeed < 1.01f)
    {
        currentF += value;
        current = (int)currentF;
    }
    else
    {
        value *= *gameSpeed;
        currentF += value;
        current = (int)currentF;
    }
}

// Advances by one frame scaled by the game speed.
void ZunTimer::Increment()
{
    previous = current;
    if (*gameSpeed > 0.99 && *gameSpeed < 1.01f)
    {
        current++;
        currentF += 1.0;
    }
    else
    {
        currentF += *gameSpeed;
        current = (int)currentF;
    }
}

// Target helper FUN_00402AC0: update the integer frame with the x87
// round-to-nearest-even conversion when the global speed is not 1.0.
void ZunTimer::AdvanceFrame()
{
    Initialize();
    previous = current;

    if (*gameSpeed > 0.99f && *gameSpeed < 1.01f)
    {
        ++current;
        currentF += 1.0f;
    }
    else
    {
        currentF += *gameSpeed;
        current = RoundTimerValueToNearestEven(currentF);
    }
}
