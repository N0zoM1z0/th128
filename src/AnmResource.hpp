#pragma once

#include <stddef.h>
#include "AnmVm.hpp"

void NormalizeAnmCoordinates(float *normalized, const float *pixels);

// One animation-file entry occupies 0x48 bytes. The dimension/rectangle
// values used by VM binding are observed directly at +0x1C..+0x40.
struct AnmFileEntry
{
    unsigned char unknown000[8];
    void **texture;                         // +0x08, address of the D3D texture slot
    unsigned char unknown00C[0x1C - 0x0C];
    float field01C;                         // +0x1C
    float field020;                         // +0x20
    float field024;                         // +0x24
    float field028;                         // +0x28
    float field02C;                         // +0x2C
    float field030;                         // +0x30
    float field034;                         // +0x34
    float field038;                         // +0x38
    float field03C;                         // +0x3C
    float field040;                         // +0x40
    unsigned int unknown044;                // +0x44
};
typedef char AnmFileEntrySize[(sizeof(AnmFileEntry) == 0x48) ? 1 : -1];
typedef char AnmFileEntryTextureOffset[(offsetof(AnmFileEntry, texture) == 0x08) ? 1 : -1];

struct AnmTextureEntry
{
    void *texture;        // +0x00 (IDirect3DTexture9 *)
    void *rawData;        // +0x04
    unsigned int field08; // +0x08
    unsigned int field0C; // +0x0C
    unsigned int field10; // +0x10

    void Release();
};
typedef char AnmTextureEntrySize[(sizeof(AnmTextureEntry) == 0x14) ? 1 : -1];

struct AnmResource
{
    unsigned short fileId;                  // +0x000
    unsigned char unknown002[0x108 - 2];
    void *anmFileHeader;                    // +0x108, required for binding
    int textureCount;                       // +0x10C
    unsigned char unknown110[0x118 - 0x110];
    AnmFileEntry *anmFiles;                 // +0x118
    const unsigned int *scriptOffsets;      // +0x11C
    AnmTextureEntry *textures;              // +0x120
    unsigned int isInvalid;                 // +0x124
    unsigned int loadPending;               // +0x128
    unsigned int unknown12C;                // +0x12C
    int activeVmCount;                      // +0x130
    void *extraData134;                     // +0x134

    AnmResource();
    void Release();
    void BindVmScript(AnmVm *vm, int scriptIndex);
    void BindVmAtPosition(AnmVm *vm, int scriptIndex, const ZunVec3 *position);
    unsigned int *CreateVm(
        const ZunVec3 *position,
        unsigned int *outId,
        int scriptIndex,
        int initialState,
        unsigned int flags);
};
typedef char AnmResourceHeaderOffset[
    (offsetof(AnmResource, anmFileHeader) == 0x108) ? 1 : -1
];
typedef char AnmResourceFilesOffset[
    (offsetof(AnmResource, anmFiles) == 0x118) ? 1 : -1
];
typedef char AnmResourceScriptTableOffset[
    (offsetof(AnmResource, scriptOffsets) == 0x11C) ? 1 : -1
];
typedef char AnmResourceInvalidFlagOffset[
    (offsetof(AnmResource, isInvalid) == 0x124) ? 1 : -1
];
typedef char AnmResourceActiveCountOffset[
    (offsetof(AnmResource, activeVmCount) == 0x130) ? 1 : -1
];
