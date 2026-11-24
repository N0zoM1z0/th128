#pragma once

// Layout hypothesis from adjacent games. TH128 evidence: the current count at
// +4 and the bit-0 flag at +0x10 that embedded timers clear on construction.
extern float g_TimeScale;

static inline int ZunRoundToInt(double value)
{
    if (value != value || value >= 2147483648.0 || value < -2147483648.0)
    {
        return (int)0x80000000;
    }

    if (*reinterpret_cast<const volatile int *>(0x004DCD68) != 0)
    {
        return (int)value;
    }

    int integral = (int)value;
    const double fraction = value - (double)integral;
    if (fraction > 0.5 || (fraction == 0.5 && (integral & 1) != 0))
    {
        ++integral;
    }
    else if (fraction < -0.5 || (fraction == -0.5 && (integral & 1) != 0))
    {
        --integral;
    }
    return integral;
}

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
    void Increment();

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
