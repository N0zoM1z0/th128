// Non-runnable VC10 /GL context for Input subsystem.
#include "../src/GameErrorContext.hpp"
#include "../src/Input.hpp"
#include "../src/Supervisor.hpp"

Supervisor g_Supervisor;
GameErrorContext g_GameErrorContext;
JOYCAPSA g_JoyCaps;
InputState g_InputStates[4];
int g_WindowActive = 0;
DirectInputDeviceView *g_DirectInputKeyboard = 0;

unsigned int *g_ProbeSupFlags = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeSupCs = g_Supervisor.criticalSections;
unsigned char *g_ProbeSupDepth = g_Supervisor.lockDepths;
int *g_ProbeWinActive = &g_WindowActive;
DirectInputDeviceView **g_ProbeDiKbd = &g_DirectInputKeyboard;

int (__cdecl *g_ProbeInitJoypad)() = InitJoypad;
void (__cdecl *g_ProbeClearKbd)() = ClearKeyboardState;

extern "C" unsigned int __stdcall InputProbe(
    unsigned int *out1, unsigned int *out2,
    const unsigned char *btns1, const unsigned char *btns2,
    unsigned char *kbd1, unsigned char *kbd2,
    short idx1, short idx2, unsigned int bit1, unsigned int bit2,
    unsigned int raw1, unsigned int raw2, int slot1, int slot2,
    InputState *st1, InputState *st2)
{
    unsigned int r = 0;
    r += (unsigned int)InitJoypad();
    r += MapJoyButtonByte(out1, bit1, btns1, idx1);
    r += MapJoyButtonByte(out2, bit2, btns2, idx2);
    r += MapJoyButtonBit(out1, idx1, bit1, raw1);
    r += MapJoyButtonBit(out2, idx2, bit2, raw2);
    r += UpdateInputSlot(slot1, raw1);
    r += UpdateInputSlot(slot2, raw2);
    r += (unsigned int)PollKeyboardState(kbd1);
    r += (unsigned int)PollKeyboardState(kbd2);
    ClearKeyboardState();
    st1->UpdateTimers();
    st2->UpdateTimers();
    st1->ResetDefaultConfig();
    st2->ResetDefaultConfig();
    return r;
}
