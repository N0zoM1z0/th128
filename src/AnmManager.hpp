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
    unsigned char unknown0A4[0xA8 - 0xA4];
    unsigned int stateCallCount;            // +0x0A8
    unsigned int drawCallCount;             // +0x0AC
    float viewOffsetX;                      // +0x0B0
    float viewOffsetY;                      // +0x0B4
    int updateCount;                        // +0x0B8
    AnmVm vmPool[0x1000];                   // +0x0BC
    unsigned char vmPoolUsed[0x1000];       // +0x4E80BC
    int nextVmPoolIndex;                    // +0x4E90BC
    unsigned char unknown4E90C0[0x4E9158 - 0x4E90C0];
    float vmWorldMatrix[4][4];              // +0x4E9158
    unsigned char unknown4E9198[0x8B96FC - 0x4E9198];
    float *primitiveVertexCursor;           // +0x8B96FC
    unsigned int unknown8B9700;
    AnmVmListNode *worldVms;                // +0x8B9704
    AnmVmListNode *worldVmsTail;            // +0x8B9708
    AnmVmListNode *uiVms;                   // +0x8B970C
    AnmVmListNode *uiVmsTail;               // +0x8B9710
    AnmVm renderListHeads[33];               // +0x8B9714, 31 world + 2 UI groups
    unsigned int nextVmId;                  // +0x8C38FC
    unsigned char colorModulation[4];        // +0x8C3900
    unsigned int colorModulationEnabled;     // +0x8C3904

    AnmVm *GetVmById(unsigned int id);
    unsigned int RegisterWorldVmAtTail(AnmVm *vm);
    unsigned int RegisterWorldVmAtHead(AnmVm *vm);
    unsigned int RegisterUiVmAtTail(AnmVm *vm);
    unsigned int RegisterUiVmAtHead(AnmVm *vm);
    void __fastcall UpdateVm(unsigned int stateFlags, AnmVm *vm);
    void StopVmById(unsigned int id);
    void CleanupVm(AnmVm *vm);
    int UpdateWorldVms();
    int UpdateUiVms();
    int DrawVmGroup(unsigned int group);
};
typedef char AnmManagerActiveCountOffset[(offsetof(AnmManager, activeVmCount) == 0xA0) ? 1 : -1];
typedef char AnmManagerViewOffsetX[(offsetof(AnmManager, viewOffsetX) == 0xB0) ? 1 : -1];
typedef char AnmManagerViewOffsetY[(offsetof(AnmManager, viewOffsetY) == 0xB4) ? 1 : -1];
typedef char AnmManagerUpdateCountOffset[(offsetof(AnmManager, updateCount) == 0xB8) ? 1 : -1];
typedef char AnmManagerPoolOffset[(offsetof(AnmManager, vmPool) == 0xBC) ? 1 : -1];
typedef char AnmManagerUsedOffset[(offsetof(AnmManager, vmPoolUsed) == 0x4E80BC) ? 1 : -1];
typedef char AnmManagerPoolCursorOffset[(offsetof(AnmManager, nextVmPoolIndex) == 0x4E90BC) ? 1 : -1];
typedef char AnmManagerVmWorldMatrixOffset[(offsetof(AnmManager, vmWorldMatrix) == 0x4E9158) ? 1 : -1];
typedef char AnmManagerPrimitiveVertexCursorOffset[(offsetof(AnmManager, primitiveVertexCursor) == 0x8B96FC) ? 1 : -1];
typedef char AnmManagerWorldListOffset[(offsetof(AnmManager, worldVms) == 0x8B9704) ? 1 : -1];
typedef char AnmManagerRenderHeadsOffset[(offsetof(AnmManager, renderListHeads) == 0x8B9714) ? 1 : -1];
typedef char AnmManagerNextIdOffset[(offsetof(AnmManager, nextVmId) == 0x8C38FC) ? 1 : -1];
typedef char AnmManagerColorModulationOffset[(offsetof(AnmManager, colorModulation) == 0x8C3900) ? 1 : -1];
typedef char AnmManagerColorModulationEnabledOffset[(offsetof(AnmManager, colorModulationEnabled) == 0x8C3904) ? 1 : -1];

inline AnmManager *GetGlobalAnmManager()
{
    return *reinterpret_cast<AnmManager **>(0x004D2E50);
}

AnmVm *AllocateAnmVm();

extern AnmManager *g_AnmManager;
