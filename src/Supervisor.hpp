#ifndef TH128_SUPERVISOR_HPP
#define TH128_SUPERVISOR_HPP

#include <windows.h>

#include "ThreadInf.hpp"

// Partial physical layout of the global Supervisor singleton rooted at
// 0x004D2E60 (provisional base; reviewed offsets anchored against TH128.exe):
//   +0x0018 (0x004D2E78): RECT mainWindowRect
//   +0x0060 (0x004D2EC0): HWND deviceWindow
//   +0x011C (0x004D2F7C): int cursorAlwaysVisible / windowed
//   +0x01ED (0x004D304D): unsigned char configWindowMode
//   +0x01EE (0x004D304E): unsigned char configFlag304E
//   +0x01F3 (0x004D3053): unsigned char configFrameSkip
//   +0x01F8 (0x004D3058): int configWindowX
//   +0x01FC (0x004D305C): int configWindowY
//   +0x0598 (0x004D33F8): unsigned int flags (bit 0x8000 = critical sections active)
//   +0x07F8 (0x004D3658): ThreadInf thread
//   +0x0818 (0x004D3678): CRITICAL_SECTION criticalSections[12] (stride 0x18)
//   +0x0938 (0x004D3798): unsigned char lockDepths[12]
//   +0x0B10 (0x004D3970): HWND mainWindow
//   +0x0B1C (0x004D397C): WPARAM appActiveParam
//   +0x0B20 (0x004D3980): int appInactive
struct Supervisor
{
    unsigned char pad0000[0x18];
    RECT mainWindowRect;                // +0x0018 (0x004D2E78)
    unsigned char pad0028[0x38];
    HWND deviceWindow;                  // +0x0060 (0x004D2EC0)
    unsigned char pad0064[0xB8];
    int cursorAlwaysVisible;            // +0x011C (0x004D2F7C)
    unsigned char pad0120[0xCD];
    unsigned char configWindowMode;     // +0x01ED (0x004D304D)
    unsigned char configFlag304E;       // +0x01EE (0x004D304E)
    unsigned char pad01EF[0x04];
    unsigned char configFrameSkip;      // +0x01F3 (0x004D3053)
    unsigned char pad01F4[0x04];
    int configWindowX;                  // +0x01F8 (0x004D3058)
    int configWindowY;                  // +0x01FC (0x004D305C)
    unsigned char pad0200[0x398];
    unsigned int flags;                 // +0x0598 (0x004D33F8)
    unsigned char pad059C[0x25C];
    ThreadInf thread;                   // +0x07F8 (0x004D3658)
    unsigned char pad0814[0x04];
    CRITICAL_SECTION criticalSections[12]; // +0x0818 (0x004D3678)
    unsigned char lockDepths[12];       // +0x0938 (0x004D3798)
    unsigned char pad0944[0x1CC];
    HWND mainWindow;                    // +0x0B10 (0x004D3970)
    unsigned char pad0B14[0x08];
    WPARAM appActiveParam;              // +0x0B1C (0x004D397C)
    int appInactive;                    // +0x0B20 (0x004D3980)

    void EnterCriticalSection(int index)
    {
        if ((flags & 0x8000u) != 0)
        {
            ::EnterCriticalSection(&criticalSections[index]);
            ++lockDepths[index];
        }
    }

    void LeaveCriticalSection(int index)
    {
        if ((flags & 0x8000u) != 0)
        {
            ::LeaveCriticalSection(&criticalSections[index]);
            --lockDepths[index];
        }
    }
};

extern Supervisor g_Supervisor;

#endif
