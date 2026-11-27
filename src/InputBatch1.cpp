#include "InputBatch1.hpp"

namespace th128 {

static __forceinline unsigned int Ib1VirtualKeyButtonsRaw(const unsigned char* keys) {
    return ((keys[0x5a] & 0x80) >> 7) |
        ((keys[0x58] & 0x80) >> 6) |
        ((keys[0x10] & 0x80) >> 4) |
        (((keys[0x68] | keys[0x26]) & 0x80) >> 3) |
        (((keys[0x62] | keys[0x28]) & 0x80) >> 2) |
        (((keys[0x64] | keys[0x25]) & 0x80) >> 1) |
        ((keys[0x66] | keys[0x27]) & 0x80) |
        ((keys[0x1b] & 0x80) << 1) |
        ((keys[0x11] & 0x80) << 2) |
        ((keys[0x51] & 0x80) << 9) |
        ((keys[0x53] & 0x80) << 10) |
        (((keys[0x50] | keys[0x24]) & 0x80) << 11) |
        ((keys[0x0d] & 0x80) << 12) |
        ((keys[0x44] & 0x80) << 13) |
        ((keys[0x52] & 0x80) << 14) |
        ((keys[0x63] & 0x80) ? 0xa0 : 0) |
        ((keys[0x69] & 0x80) ? 0x90 : 0) |
        ((keys[0x61] & 0x80) ? 0x60 : 0) |
        ((keys[0x67] & 0x80) ? 0x50 : 0);
}

static __forceinline unsigned int Ib1ScanCodeButtonsRaw(const unsigned char* keys) {
    return ((keys[0x2c] & 0x80) >> 7) |
        ((keys[0x2d] & 0x80) >> 6) |
        ((keys[0x2a] & 0x80) >> 4) |
        (((keys[0x48] | keys[0xc8]) & 0x80) >> 3) |
        (((keys[0xd0] | keys[0x50]) & 0x80) >> 2) |
        (((keys[0x4b] | keys[0xcb]) & 0x80) >> 1) |
        ((keys[0x4d] | keys[0xcd]) & 0x80) |
        ((keys[0x01] & 0x80) << 1) |
        ((keys[0x1d] & 0x80) << 2) |
        ((keys[0x10] & 0x80) << 9) |
        ((keys[0x1f] & 0x80) << 10) |
        (((keys[0x19] | keys[0xc7]) & 0x80) << 11) |
        ((keys[0x1c] & 0x80) << 12) |
        ((keys[0x20] & 0x80) << 13) |
        ((keys[0x13] & 0x80) << 14) |
        ((keys[0x49] & 0x80) ? 0x90 : 0) |
        ((keys[0x4f] & 0x80) ? 0x60 : 0) |
        ((keys[0x47] & 0x80) ? 0x50 : 0) |
        ((keys[0x51] & 0x80) ? 0xa0 : 0);
}

static __forceinline unsigned int Ib1VirtualKeyButtonsMain(const unsigned char* keys) {
    return ((keys[0x5a] & 0x80) >> 7) |
        ((keys[0x58] & 0x80) >> 6) |
        ((keys[0x10] & 0x80) >> 4) |
        (((keys[0x68] | keys[0x26]) & 0x80) >> 3) |
        (((keys[0x62] | keys[0x28]) & 0x80) >> 2) |
        (((keys[0x64] | keys[0x25]) & 0x80) >> 1) |
        ((keys[0x66] | keys[0x27]) & 0x80) |
        ((keys[0x1b] & 0x80) << 1) |
        (((keys[0x43] | keys[0x11]) & 0x80) << 2) |
        ((keys[0x51] & 0x80) << 9) |
        ((keys[0x53] & 0x80) << 10) |
        (((keys[0x50] | keys[0x24]) & 0x80) << 11) |
        ((keys[0x0d] & 0x80) << 12) |
        ((keys[0x44] & 0x80) << 13) |
        ((keys[0x52] & 0x80) << 14) |
        ((keys[0x79] & 0x80) << 16) |
        ((keys[0x63] & 0x80) ? 0xa0 : 0) |
        ((keys[0x69] & 0x80) ? 0x90 : 0) |
        ((keys[0x61] & 0x80) ? 0x60 : 0) |
        ((keys[0x67] & 0x80) ? 0x50 : 0);
}

static __forceinline unsigned int Ib1ScanCodeButtonsMain(const unsigned char* keys) {
    return ((keys[0x2c] & 0x80) >> 7) |
        ((keys[0x2d] & 0x80) >> 6) |
        ((keys[0x2a] & 0x80) >> 4) |
        (((keys[0x48] | keys[0xc8]) & 0x80) >> 3) |
        (((keys[0xd0] | keys[0x50]) & 0x80) >> 2) |
        (((keys[0x4b] | keys[0xcb]) & 0x80) >> 1) |
        ((keys[0x4d] | keys[0xcd]) & 0x80) |
        ((keys[0x01] & 0x80) << 1) |
        (((keys[0x2e] | keys[0x1d]) & 0x80) << 2) |
        ((keys[0x10] & 0x80) << 9) |
        ((keys[0x1f] & 0x80) << 10) |
        (((keys[0x19] | keys[0xc7]) & 0x80) << 11) |
        ((keys[0x1c] & 0x80) << 12) |
        ((keys[0x20] & 0x80) << 13) |
        ((keys[0x13] & 0x80) << 14) |
        ((keys[0x44] & 0x80) << 16) |
        ((keys[0x49] & 0x80) ? 0x90 : 0) |
        ((keys[0x4f] & 0x80) ? 0x60 : 0) |
        ((keys[0x47] & 0x80) ? 0x50 : 0) |
        ((keys[0x51] & 0x80) ? 0xa0 : 0);
}

static __forceinline unsigned int Ib1VirtualKeyButtonsSlot(const unsigned char* keys) {
    return ((keys[0x58] & 0x80) >> 6) |
        ((keys[0x5a] & 0x80) >> 7) |
        ((keys[0x10] & 0x80) >> 4) |
        (((keys[0x68] | keys[0x26]) & 0x80) >> 3) |
        (((keys[0x62] | keys[0x28]) & 0x80) >> 2) |
        (((keys[0x64] | keys[0x25]) & 0x80) >> 1) |
        ((keys[0x66] | keys[0x27]) & 0x80) |
        ((keys[0x1b] & 0x80) << 1) |
        (((keys[0x43] | keys[0x11]) & 0x80) << 2) |
        ((keys[0x51] & 0x80) << 9) |
        ((keys[0x53] & 0x80) << 10) |
        (((keys[0x50] | keys[0x24]) & 0x80) << 11) |
        ((keys[0x0d] & 0x80) << 12) |
        ((keys[0x44] & 0x80) << 13) |
        ((keys[0x52] & 0x80) << 14) |
        ((keys[0x63] & 0x80) ? 0xa0 : 0) |
        ((keys[0x69] & 0x80) ? 0x90 : 0) |
        ((keys[0x61] & 0x80) ? 0x60 : 0) |
        ((keys[0x67] & 0x80) ? 0x50 : 0);
}

static __forceinline unsigned int Ib1ScanCodeButtonsSlot(const unsigned char* keys) {
    return ((keys[0x2c] & 0x80) >> 7) |
        ((keys[0x2d] & 0x80) >> 6) |
        ((keys[0x2a] & 0x80) >> 4) |
        (((keys[0x48] | keys[0xc8]) & 0x80) >> 3) |
        (((keys[0xd0] | keys[0x50]) & 0x80) >> 2) |
        (((keys[0x4b] | keys[0xcb]) & 0x80) >> 1) |
        ((keys[0x4d] | keys[0xcd]) & 0x80) |
        ((keys[0x01] & 0x80) << 1) |
        (((keys[0x2e] | keys[0x1d]) & 0x80) << 2) |
        ((keys[0x10] & 0x80) << 9) |
        ((keys[0x1f] & 0x80) << 10) |
        (((keys[0x19] | keys[0xc7]) & 0x80) << 11) |
        ((keys[0x1c] & 0x80) << 12) |
        ((keys[0x20] & 0x80) << 13) |
        ((keys[0x13] & 0x80) << 14) |
        ((keys[0x49] & 0x80) ? 0x90 : 0) |
        ((keys[0x4f] & 0x80) ? 0x60 : 0) |
        ((keys[0x47] & 0x80) ? 0x50 : 0) |
        ((keys[0x51] & 0x80) ? 0xa0 : 0);
}

unsigned int Ib1PollInputRaw() {
    unsigned char keys[256];
    unsigned int mask = 0;

    if (g_Ib1WindowActive != 0) {
        if ((g_Ib1SupervisorFlags & 0x400) == 0) {
            GetKeyboardState(keys);
            mask = Ib1VirtualKeyButtonsRaw(keys);
        } else {
            HRESULT hr = g_Ib1KeyboardDevice->vtbl->GetDeviceState(g_Ib1KeyboardDevice, 256, keys);
            mask = 0;
            if (hr == (HRESULT)0x8007001e)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else if (hr != 0)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else
                mask = Ib1ScanCodeButtonsRaw(keys);
        }
    }

    return Ib1MergeJoyInput(mask);
}

unsigned int Ib1PollInputMain() {
    unsigned char keys[256];
    unsigned int mask = 0;

    if (g_Ib1WindowActive != 0) {
        if ((g_Ib1SupervisorFlags & 0x400) == 0) {
            GetKeyboardState(keys);
            mask = Ib1VirtualKeyButtonsMain(keys);
        } else {
            HRESULT hr = g_Ib1KeyboardDevice->vtbl->GetDeviceState(g_Ib1KeyboardDevice, 256, keys);
            mask = 0;
            if (hr == (HRESULT)0x8007001e)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else if (hr != 0)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else
                mask = Ib1ScanCodeButtonsMain(keys);
        }
    }

    mask = Ib1MergeJoyInput(mask);
    g_Ib1InputStates[0].prev = g_Ib1InputStates[0].curr;
    g_Ib1InputStates[0].curr = mask;
    g_Ib1InputStates[0].Update();
    return mask;
}

unsigned int __stdcall Ib1PollInputSlot(int slot) {
    unsigned char keys[256];
    Ib1InputState* state = &g_Ib1InputStates[slot];
    unsigned int mask = 0;

    if (g_Ib1WindowActive != 0) {
        if ((g_Ib1SupervisorFlags & 0x400) == 0) {
            GetKeyboardState(keys);
            mask = Ib1VirtualKeyButtonsSlot(keys);
        } else {
            HRESULT hr = g_Ib1KeyboardDevice->vtbl->GetDeviceState(g_Ib1KeyboardDevice, 256, keys);
            mask = 0;
            if (hr == (HRESULT)0x8007001e)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else if (hr != 0)
                g_Ib1KeyboardDevice->vtbl->Acquire(g_Ib1KeyboardDevice);
            else
                mask = Ib1ScanCodeButtonsSlot(keys);
        }
    }

    mask = Ib1MergeJoyInput(mask);
    state->prev = state->curr;
    state->curr = mask;
    state->Update();
    return mask;
}

} // namespace th128
