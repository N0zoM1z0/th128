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

inline void ZunTimer::Initialize()
{
    if (!initialized)
    {
        currentF = 0.0f;
        current = 0;
        previous = -999999;
        gameSpeed = &g_TimeScale;
        initialized = 1;
    }
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
