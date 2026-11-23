#pragma once

#include <stddef.h>
#include "AnmVm.hpp"

// Animation manager (global pointer slot 0x004D2E50). The embedded pool is
// 0x1000 VMs; the used-byte table and rotating allocation cursor immediately
// follow it in the target object.
struct AnmManager
{
    unsigned char unknown000[0xA0];
    int activeVmCount;                      // +0x0A0
    unsigned char unknown0A4[0xB8 - 0xA4];
    int updateCount;                        // +0x0B8
    AnmVm vmPool[0x1000];                   // +0x0BC
    unsigned char vmPoolUsed[0x1000];       // +0x4E80BC
    int nextVmPoolIndex;                    // +0x4E90BC
    unsigned char unknown4E90C0[0x8B9704 - 0x4E90C0];
    AnmVmListNode *worldVms;                // +0x8B9704
    AnmVmListNode *worldVmsTail;            // +0x8B9708
    AnmVmListNode *uiVms;                   // +0x8B970C
    AnmVmListNode *uiVmsTail;               // +0x8B9710
    AnmVm renderListHeads[33];               // +0x8B9714, 31 world + 2 UI groups
    unsigned int nextVmId;                  // +0x8C38FC

    AnmVm *GetVmById(unsigned int id);
    unsigned int RegisterWorldVmAtTail(AnmVm *vm);
    unsigned int RegisterWorldVmAtHead(AnmVm *vm);
    unsigned int RegisterUiVmAtTail(AnmVm *vm);
    unsigned int RegisterUiVmAtHead(AnmVm *vm);
    void StopVmById(unsigned int id);
    void CleanupVm(AnmVm *vm);
    int UpdateWorldVms();
    int UpdateUiVms();
};
typedef char AnmManagerActiveCountOffset[(offsetof(AnmManager, activeVmCount) == 0xA0) ? 1 : -1];
typedef char AnmManagerUpdateCountOffset[(offsetof(AnmManager, updateCount) == 0xB8) ? 1 : -1];
typedef char AnmManagerPoolOffset[(offsetof(AnmManager, vmPool) == 0xBC) ? 1 : -1];
typedef char AnmManagerUsedOffset[(offsetof(AnmManager, vmPoolUsed) == 0x4E80BC) ? 1 : -1];
typedef char AnmManagerPoolCursorOffset[(offsetof(AnmManager, nextVmPoolIndex) == 0x4E90BC) ? 1 : -1];
typedef char AnmManagerWorldListOffset[(offsetof(AnmManager, worldVms) == 0x8B9704) ? 1 : -1];
typedef char AnmManagerRenderHeadsOffset[(offsetof(AnmManager, renderListHeads) == 0x8B9714) ? 1 : -1];
typedef char AnmManagerNextIdOffset[(offsetof(AnmManager, nextVmId) == 0x8C38FC) ? 1 : -1];

inline AnmManager *GetGlobalAnmManager()
{
    return *reinterpret_cast<AnmManager **>(0x004D2E50);
}

AnmVm *AllocateAnmVm();

extern AnmManager *g_AnmManager;
