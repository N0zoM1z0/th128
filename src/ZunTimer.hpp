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

    bool operator<(int value) const;
    bool operator<=(int value) const;
    bool operator>=(int value) const;
    bool operator==(int value) const;
    bool operator>(int value) const;
};

int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor);
