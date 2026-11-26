#pragma once

#include <stddef.h>
#include "AnmVm.hpp"

struct AnmId
{
    unsigned int id;
};

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
    AnmResource *loadedAnms[38];            // +0x4E90C0
    float vmWorldMatrix[4][4];              // +0x4E9158
    unsigned char unknown4E9198[0x4E9693 - 0x4E9198];
    unsigned char spriteStageMode;          // +0x4E9693
    unsigned char unknown4E9694[0x4E96EC - 0x4E9694];
    int spriteBatchCount;                   // +0x4E96EC
    unsigned char unknown4E96F0[0x8696F0 - 0x4E96F0];
    void *spriteVertexBufferBase;           // +0x8696F0
    void *spriteVertexBufferStart;          // +0x8696F4
    unsigned char unknown8696F8[0x8B96FC - 0x8696F8];
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
    AnmVm *GetVmById(AnmId id);
    void __fastcall UpdateVm(unsigned int stateFlags, AnmVm *vm);
    void StopVmById(AnmId id, short interrupt);
    void SetInterruptAndUpdate(AnmId id, short interrupt);
    void MarkResourceVmsForDelete(AnmResource *resource);
    void DeleteVmById(unsigned int *id);
    unsigned int *FindChildVmByScript(unsigned int *outId, unsigned int *rootId, int scriptIndex);
    int CleanupVm(AnmVm *vm);
    int UpdateWorldVms();
    int UpdateUiVms();
    int OnTickWorldCallback();
    int OnTickUiCallback();
    int DrawVmGroup(unsigned int group);
    int DrawLayer0();
    int DrawLayer1();
    int DrawLayer2();
    int DrawLayer3();
    int DrawLayer5();
    int DrawLayer6();
    int DrawLayer7();
    int DrawLayer8();
    int DrawLayer9();
    int DrawLayer10();
    int DrawLayer11();
    int DrawLayer12();
    int DrawLayer13();
    int DrawLayer14();
    int DrawLayer15();
    int DrawLayer16();
    int DrawLayer17();
    int DrawLayer18();
    int DrawLayer19();
    int DrawLayer22();
    int DrawLayer23();
    int DrawLayer25();
    int DrawLayer26();
    int AreAllAnmsLoaded();
    void ReleaseAnm(unsigned int slot);
    bool IsPoolVm(AnmVm *vm);
    static AnmResource *PreloadAnm(int slot, const char *filename);
    void FlushSpriteBatch();
};

// The original VM lookup helper uses the manager in EDX and leaves the VM id
// on the stack. Its unused ECX slot is part of the target's private fastcall.
AnmVm *__fastcall GetVmByIdTargetAbi(
    void *unused, AnmManager *manager, volatile unsigned int id);

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
