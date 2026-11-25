#include "ZunTimer.hpp"

__declspec(noinline) void ZunTimer::Increment()
{
    int nextCurrent = current;
    float *const speed = gameSpeed;
    previous = nextCurrent;
    if (*speed > 0.99 && *speed < 1.01f)
    {
        float nextCurrentF = currentF;
        ++nextCurrent;
        nextCurrentF += 1.0f;
        current = nextCurrent;
        currentF = nextCurrentF;
    }
    else
    {
        const float nextCurrentF = *speed + currentF;
        currentF = nextCurrentF;
        current = ZunRoundToInt((double)nextCurrentF);
    }
}

__declspec(noinline) void ZunTimer::Reset()
{
    Initialize();
    currentF = 0.0f;
    current = 0;
    previous = -1;
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

// The ANM wait path passes a complete timer value by value; the target
// consumes only its integer current field and rebuilds the destination's
// previous/currentF values through the same lazy initialization path.
void ZunTimer::SetCurrent(ZunTimer value)
{
    Initialize();
    current = value.current;
    previous = current - 1;
    currentF = (float)current;
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


