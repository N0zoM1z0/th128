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

    void Initialize();
    void SetCurrent(int value);
    void SetCurrentF(float value);
    void Add(float value);

    bool operator<(int value) const;
    bool operator<=(int value) const;
    bool operator>=(int value) const;
    bool operator==(int value) const;
    bool operator>(int value) const;
};

extern float g_TimeScale;

int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor);
