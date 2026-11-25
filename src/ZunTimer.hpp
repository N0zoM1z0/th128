#pragma once

// Layout hypothesis from adjacent games. TH128 evidence: the current count at
// +4 and the bit-0 flag at +0x10 that embedded timers clear on construction.
extern float g_TimeScale;

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
    void Reset();
    void SetCurrent(int value);
    void SetCurrent(ZunTimer value);
    void SetCurrentF(float value);
    void Add(float value);
    __declspec(noinline) void Increment();

    bool operator<(int value) const;
    bool operator<=(int value) const;
    bool operator>=(int value) const;
    bool operator==(int value) const;
    bool operator>(int value) const;
};


int __stdcall ZunTimerModulo(ZunTimer *timer, int divisor);

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
