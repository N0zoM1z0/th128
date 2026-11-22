#pragma once

// Layout hypothesis from adjacent games. TH128 evidence: the current count at
// +4 and the bit-0 flag at +0x10 that embedded timers clear on construction.
struct ZunTimer
{
    int previous;
    int current;
    float currentF;
    float *gameSpeed;
    unsigned int initialized : 1;  // +0x10 bit 0, cleared on construction
    unsigned int controlFlags : 31;

    ZunTimer() { initialized = 0; }

    bool operator<(int value) const;
    bool operator<=(int value) const;
    bool operator>=(int value) const;
    bool operator==(int value) const;
    bool operator>(int value) const;
};

int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor);
