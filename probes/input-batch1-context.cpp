#include "InputBatch1.hpp"

namespace th128 {

int g_Ib1WindowActive;
unsigned int g_Ib1SupervisorFlags;
Ib1DirectInputDevice* g_Ib1KeyboardDevice;
Ib1InputState g_Ib1InputStates[4];

static volatile unsigned int g_Ib1Sink;

unsigned int __fastcall Ib1MergeJoyInput(unsigned int mask) {
    unsigned int t = GetTickCount();
    unsigned int r = (mask ^ t) + g_Ib1Sink;
    g_Ib1Sink = r + 1;
    return r;
}

void Ib1InputState::Update() {
    unsigned int t = GetTickCount();
    g_Ib1Sink ^= (curr ^ prev ^ t);
}

typedef unsigned int (__fastcall* MergeFn)(unsigned int);
typedef void (Ib1InputState::*UpdateFn)();

volatile MergeFn g_Ib1MergePtr = &Ib1MergeJoyInput;
volatile UpdateFn g_Ib1UpdatePtr = &Ib1InputState::Update;

} // namespace th128

extern "C" int __cdecl main(int argc, char** argv) {
    unsigned int r = 0;
    if (argc & 1) {
        r ^= th128::Ib1PollInputRaw();
    }
    if (argc & 2) {
        r ^= th128::Ib1PollInputMain();
    }
    if (argc & 4) {
        r ^= th128::Ib1PollInputSlot(argc >> 3);
    }
    if (argc & 8) {
        r ^= th128::g_Ib1MergePtr(r);
        (th128::g_Ib1InputStates[0].*th128::g_Ib1UpdatePtr)();
    }
    return (int)r;
}
