#pragma once

#include <stddef.h>
#include <string.h>
#include "AnmVm.hpp"

void NormalizeAnmCoordinates(float *normalized, const float *pixels);

// The v7 entry header is followed by sprite offsets and eight-byte script
// descriptors. Entries link through nextOffset, not through sizeof(header).
struct AnmArchiveEntry
{
    unsigned int version;                  // +0x00
    unsigned short spriteCount;            // +0x04
    unsigned short scriptCount;            // +0x06
    unsigned short unknown008;
    unsigned short width;                  // +0x0A
    unsigned short height;                 // +0x0C
    unsigned short format;                 // +0x0E
    unsigned int nameOffset;               // +0x10
    short offsetX;                         // +0x14
    short offsetY;                         // +0x16
    unsigned int unknown018;
    unsigned int textureOffset;            // +0x1C
    unsigned char hasEmbeddedTexture;      // +0x20
    unsigned char unknown021[3];
    unsigned int nextOffset;               // +0x24
    unsigned char unknown028[0x18];
};

struct AnmArchiveSprite
{
    unsigned int id;
    float x;
    float y;
    float width;
    float height;
};

struct AnmEmbeddedTexture;

// One resolved sprite occupies 0x48 bytes.
struct AnmFileEntry
{
    int fileId;                            // +0x00
    int textureIndex;                      // +0x04
    void **texture;                         // +0x08, address of the D3D texture slot
    float left;                            // +0x0C
    float top;                             // +0x10
    float right;                           // +0x14
    float bottom;                          // +0x18
    float textureHeight;                   // +0x1C
    float textureWidth;                    // +0x20
    float uvLeft;                          // +0x24
    float uvTop;                           // +0x28
    float uvRight;                         // +0x2C
    float uvBottom;                        // +0x30
    float spriteHeight;                    // +0x34
    float spriteWidth;                     // +0x38
    float scaleX;                          // +0x3C
    float scaleY;                          // +0x40
    unsigned int unknown044;                // +0x44
};
typedef char AnmFileEntrySize[(sizeof(AnmFileEntry) == 0x48) ? 1 : -1];
typedef char AnmFileEntryTextureOffset[(offsetof(AnmFileEntry, texture) == 0x08) ? 1 : -1];

struct AnmTextureEntry
{
    void *texture;        // +0x00 (IDirect3DTexture9 *)
    void *rawData;        // +0x04
    unsigned int rawDataSize; // +0x08
    unsigned int field0C; // +0x0C
    unsigned int field10; // +0x10

    void Release();
    int LoadExternalTexture(int format, int width, int height, int offsetX, int offsetY);
    int LoadEmbeddedTexture(const AnmEmbeddedTexture *data, int format, int width, int height);
    int CreateEmptyTexture(int width, int height, int format);
    int CreateRenderTarget();
};
typedef char AnmTextureEntrySize[(sizeof(AnmTextureEntry) == 0x14) ? 1 : -1];

struct AnmResource
{
    int fileId;                             // +0x000
    char filename[260];                     // +0x004
    void *anmFileHeader;                    // +0x108, required for binding
    int textureCount;                       // +0x10C
    int scriptCount;                        // +0x110
    int spriteCount;                        // +0x114
    AnmFileEntry *anmFiles;                 // +0x118
    unsigned int *scriptOffsets;            // +0x11C, absolute script addresses
    AnmTextureEntry *textures;              // +0x120
    unsigned int finalizeCursor;            // +0x124, one-based entry index; zero when idle
    unsigned int releaseRequested;          // +0x128
    unsigned int textureMemoryBytes;        // +0x12C
    int activeVmCount;                      // +0x130
    void *extraData134;                     // +0x134

    AnmResource() { memset(this, 0, sizeof(AnmResource)); }
    void Release();
    int CacheExternalTexture(const AnmArchiveEntry *entry, int textureIndex);
    AnmResource *FinalizeNextEntry();
    void SetSprite(int spriteIndex, const AnmFileEntry *sprite);
    AnmId SpawnChildVm(int scriptIndex, AnmVm *parent, unsigned int listMode);
    AnmId SpawnChildVmAtPosition(int scriptIndex, AnmVm *parent);
    void BindVmScript(AnmVm *vm, int scriptIndex);
    void BindVmAtPosition(AnmVm *vm, int scriptIndex, const ZunVec3 *position);
    AnmId CreateVm(
        const ZunVec3 *position,
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
typedef char AnmResourceFinalizeCursorOffset[
    (offsetof(AnmResource, finalizeCursor) == 0x124) ? 1 : -1
];
typedef char AnmResourceActiveCountOffset[
    (offsetof(AnmResource, activeVmCount) == 0x130) ? 1 : -1
];
