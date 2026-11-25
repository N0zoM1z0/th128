#ifndef TH128_INPUT_HPP
#define TH128_INPUT_HPP

#include <windows.h>

struct InputConfig
{
    short pad00[4];       // +0x00
    short keyShot;        // +0x08 (at +0x234 in InputState)
    short keyBomb;        // +0x0A
    short keySlow;        // +0x0C
    short keySkip;        // +0x0E
    short keyPause;       // +0x10
    short keyUnused1;     // +0x12
    short keyUnused2;     // +0x14
    short keyUnused3;     // +0x16
    short keyUnused4;     // +0x18
};

struct InputState
{
    unsigned int current;       // +0x000
    unsigned int previous;      // +0x004
    unsigned int repeatMask;    // +0x008
    unsigned int triggerMask;   // +0x00C
    unsigned int releaseMask;   // +0x010
    unsigned int repeatTimers[32]; // +0x014
    unsigned int pad094[32];    // +0x094
    unsigned int holdTimers[32];   // +0x114
    unsigned int pad194[38];    // +0x194
    unsigned int heldMask;      // +0x22C
    unsigned int field230;      // +0x230
    short padButtons[9];        // +0x234..0x245
    unsigned short pad246;      // +0x246

    void UpdateTimers();
    void ResetDefaultConfig();
};

struct DirectInputDeviceView
{
    virtual HRESULT __stdcall Slot00() = 0; // +0x00
    virtual HRESULT __stdcall Slot01() = 0; // +0x04
    virtual HRESULT __stdcall Slot02() = 0; // +0x08
    virtual HRESULT __stdcall Slot03() = 0; // +0x0C
    virtual HRESULT __stdcall Slot04() = 0; // +0x10
    virtual HRESULT __stdcall Slot05() = 0; // +0x14
    virtual HRESULT __stdcall Slot06() = 0; // +0x18
    virtual HRESULT __stdcall Acquire() = 0; // +0x1C
    virtual HRESULT __stdcall Slot08() = 0; // +0x20
    virtual HRESULT __stdcall GetDeviceState(DWORD cbData, LPVOID lpvData) = 0; // +0x24
};

extern JOYCAPSA g_JoyCaps;
extern InputState g_InputStates[4];
extern int g_WindowActive;
extern DirectInputDeviceView *g_DirectInputKeyboard;

int __cdecl InitJoypad();
unsigned int __stdcall MapJoyButtonByte(unsigned int *outMask, unsigned int bit, const unsigned char *buttons, short index);
unsigned int __stdcall MapJoyButtonBit(unsigned int *outMask, short index, unsigned int bit, unsigned int rawButtons);
unsigned int __stdcall UpdateInputSlot(int slot, unsigned int buttons);
int __stdcall PollKeyboardState(unsigned char *keyState);
void __cdecl ClearKeyboardState();

#endif
