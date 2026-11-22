#include "ZunTimer.hpp"

int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor)
{
    return timer->current % divisor;
}
