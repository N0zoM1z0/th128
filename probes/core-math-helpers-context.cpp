#include "../src/CoreMathHelpers.hpp"

unsigned int g_CoreRuntimeFlags;
CRITICAL_SECTION g_CoreRngLock;
unsigned char g_CoreRngLockDepth;
CoreTimeScale g_CoreDefaultTimeScale;

__declspec(noinline) unsigned short CoreRng::GetU16()
{
    seed ^= 0x1234;
    return seed;
}

__declspec(noinline) void CoreMotionState::QuantizePosition()
{
    pos.x += 1.0f;
}

typedef void (CoreTimer::*ResetScaleFn)();
typedef void (CoreFloat2::*FromPolarFn)(float, float);

extern "C" int __stdcall CoreMathHelpersProbe(
    CoreRng *rng, CoreFloat2 *out2, const CoreFloat2 *in2, float angle,
    CoreTimer *timer, char *dst, const char *src, int len,
    const unsigned char *b, const unsigned short *w, const unsigned int *d,
    CoreMotionState *ms)
{
    int acc = 0;
    acc += (int)rng->NextU32();
    acc += (int)rng->NextU32Skip2();
    out2->RotateFrom(in2, angle);
    FromPolarFn pPolar = &CoreFloat2::FromPolar;
    (out2->*pPolar)(angle, 1.0f);
    timer->ClearFlag0();
    ResetScaleFn pReset = &CoreTimer::ResetScale;
    (timer->*pReset)();
    acc += timer->Advance();
    acc += (int)timer->GetScale();
    acc += (int)CoreReadLineSjis(dst, src, len);
    acc += (int)CoreChecksumBytes((unsigned int)len, b);
    acc += (int)CoreChecksumWords(w, len);
    acc += (int)CoreChecksumDwords(d, len);
    ms->UpdateVelocity();
    ms->StepPosition();
    ms->ApplyPosition();
    return acc;
}
