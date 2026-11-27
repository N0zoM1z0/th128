#ifndef TH128_INPUT_BATCH1_HPP
#define TH128_INPUT_BATCH1_HPP

#include <windows.h>

namespace th128 {

struct Ib1DirectInputDeviceVtbl {
    void* methods[7];
    HRESULT (__stdcall* Acquire)(void* self);
    void* method8;
    HRESULT (__stdcall* GetDeviceState)(void* self, DWORD cbData, void* lpvData);
};

struct Ib1DirectInputDevice {
    Ib1DirectInputDeviceVtbl* vtbl;
};

struct Ib1InputState {
    unsigned int curr;
    unsigned int prev;
    unsigned char pad[0x240];

    void Update();
};

extern int g_Ib1WindowActive;
extern unsigned int g_Ib1SupervisorFlags;
extern Ib1DirectInputDevice* g_Ib1KeyboardDevice;
extern Ib1InputState g_Ib1InputStates[4];

unsigned int __fastcall Ib1MergeJoyInput(unsigned int mask);

unsigned int Ib1PollInputRaw();
unsigned int Ib1PollInputMain();
unsigned int __stdcall Ib1PollInputSlot(int slot);

} // namespace th128

#endif
