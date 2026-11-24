// Non-runnable compiler context: independent calls let VC10 choose the
// private convention (generator in ESI in the original).
#include "../src/Rng.hpp"

// The game takes the flags' address elsewhere, so they may alias.
extern unsigned int g_GameFlags;
unsigned int *g_ProbeFlagsAddress = &g_GameFlags;

extern "C" float __stdcall RngProbe(Rng *a, Rng *b)
{
    unsigned int value = a->GetU32() + b->GetU16() + a->GetU16();
    return a->GetFloat() + a->GetFloat(1.0f) + b->GetFloatSigned() + a->GetAngle() + b->GetFloat() + (float)value;
}
