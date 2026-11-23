#include "ZunTimer.hpp"


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
    current = ZunRoundToInt((double)value);
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
        current = ZunRoundToInt((double)currentF);
    }
    else
    {
        value *= *gameSpeed;
        currentF += value;
        current = ZunRoundToInt((double)currentF);
    }
}

// Advances by one frame scaled by the game speed.
void ZunTimer::Increment()
{
    previous = current;

    if (*gameSpeed > 0.99f && *gameSpeed < 1.01f)
    {
        ++current;
        currentF += 1.0f;
    }
    else
    {
        currentF += *gameSpeed;
        current = ZunRoundToInt((double)currentF);
    }
}

