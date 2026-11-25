#include "Input.hpp"

#include <mmsystem.h>
#include <string.h>

#include "GameErrorContext.hpp"
#include "Supervisor.hpp"

int __cdecl InitJoypad()
{
    JOYINFOEX joyInfo;
    joyInfo.dwSize = sizeof(JOYINFOEX);
    joyInfo.dwFlags = JOY_RETURNALL;
    if (joyGetPosEx(0, &joyInfo) != 0 && joyGetPosEx(1, &joyInfo) != 0)
    {
        g_GameErrorContext.Log("\x8e" "g\x82\xa6\x82\xe9\x83p\x83" "b\x83" "h\x82\xaa\x91\xb6\x8d\xdd\x82\xb5\x82\xc8\x82\xa2\x82\xe6\x82\xa4\x82\xc5\x82\xb7\x81" "A\x8e" "c\x94O\r\n");
        return 1;
    }
    joyGetDevCapsA(0, &g_JoyCaps, sizeof(JOYCAPSA));
    return 0;
}

unsigned int __stdcall MapJoyButtonByte(unsigned int *outMask, unsigned int bit, const unsigned char *buttons, short index)
{
    if (index < 0)
    {
        return 0;
    }
    *outMask |= (buttons[index] & 0x80) != 0 ? bit : 0;
    return (buttons[index] & 0x80) != 0 ? bit : 0;
}

unsigned int __stdcall MapJoyButtonBit(unsigned int *outMask, short index, unsigned int bit, unsigned int rawButtons)
{
    if (index < 0)
    {
        return 0;
    }
    unsigned int pressed = (rawButtons & (1u << index)) != 0 ? bit : 0;
    *outMask |= pressed;
    return pressed;
}

unsigned int __stdcall UpdateInputSlot(int slot, unsigned int buttons)
{
    InputState *state = &g_InputStates[slot];
    state->previous = state->current;
    state->current = buttons;
    state->UpdateTimers();
    return buttons;
}

int __stdcall PollKeyboardState(unsigned char *keyState)
{
    memset(keyState, 0, 0x100);
    if (g_WindowActive != 0)
    {
        if ((g_Supervisor.flags & 0x400u) == 0)
        {
            GetKeyboardState(keyState);
            return 2;
        }
        HRESULT hr = g_DirectInputKeyboard->GetDeviceState(0x100, keyState);
        if (hr == (HRESULT)0x8007001E || hr != 0)
        {
            g_DirectInputKeyboard->Acquire();
        }
        return 1;
    }
    return 0;
}

void __cdecl ClearKeyboardState()
{
    unsigned char state[256];
    GetKeyboardState(state);
    for (int i = 0; i < 256; ++i)
    {
        state[i] &= 0x7F;
    }
    SetKeyboardState(state);
}

void InputState::UpdateTimers()
{
    unsigned int bits = current;
    repeatMask = 0;
    heldMask = 0;
    unsigned int mask = 1;
    for (int i = 0; i < 32; ++i)
    {
        if ((bits & 1u) != 0)
        {
            ++repeatTimers[i];
            ++holdTimers[i];
            if (repeatTimers[i] >= 8u)
            {
                heldMask |= mask;
            }
            if (repeatTimers[i] >= 26u)
            {
                repeatMask |= mask;
                repeatTimers[i] -= 8u;
            }
        }
        else
        {
            repeatTimers[i] = 0;
            holdTimers[i] = 0;
        }
        bits >>= 1;
        mask <<= 1;
    }
    unsigned int changed = previous ^ current;
    triggerMask = changed & current;
    releaseMask = changed & ~current;
}

void InputState::ResetDefaultConfig()
{
    padButtons[4] = 4;
    padButtons[5] = -1;
    padButtons[6] = -1;
    padButtons[0] = 0;
    padButtons[1] = 1;
    padButtons[2] = 3;
    padButtons[3] = 2;
    padButtons[7] = -1;
    padButtons[8] = -1;
}
