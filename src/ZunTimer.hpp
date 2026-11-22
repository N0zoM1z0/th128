#pragma once

// Layout hypothesis from adjacent games; TH128 evidence so far covers only
// the current count at +4.
struct ZunTimer
{
    int previous;
    int current;
    float currentF;
    float *gameSpeed;
    int control;
};

int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor);
