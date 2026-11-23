#pragma once

#include <stddef.h>
#include "AnmVm.hpp"

// Animation resource metadata consumed by the VM factory. The script offset
// table and active-VM counter are directly observed at +0x11C and +0x130.
struct AnmResource
{
    unsigned short fileId;                  // +0x000
    unsigned char unknown002[0x11C - 2];
    const unsigned int *scriptOffsets;      // +0x11C
    unsigned int unknown120;
    unsigned int isInvalid;                 // +0x124
    unsigned char unknown128[8];
    int activeVmCount;                      // +0x130

    void BindVmScript(AnmVm *vm, int scriptIndex);
    void BindVmAtPosition(AnmVm *vm, int scriptIndex, const ZunVec3 *position);
    unsigned int *CreateVm(
        const ZunVec3 *position,
        unsigned int *outId,
        int scriptIndex,
        int initialState,
        unsigned int flags);
};
typedef char AnmResourceScriptTableOffset[
    (offsetof(AnmResource, scriptOffsets) == 0x11C) ? 1 : -1
];
typedef char AnmResourceInvalidFlagOffset[
    (offsetof(AnmResource, isInvalid) == 0x124) ? 1 : -1
];
typedef char AnmResourceActiveCountOffset[
    (offsetof(AnmResource, activeVmCount) == 0x130) ? 1 : -1
];
