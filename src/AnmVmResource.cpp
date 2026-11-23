#include "AnmResource.hpp"

#include <string.h>

int AnmVm::BindAnmFile(int fileIndex, AnmResource *resource)
{
    if (resource->anmFileHeader == 0 || resource->isInvalid != 0)
    {
        return -1;
    }

    AnmFileEntry *entry = &resource->anmFiles[fileIndex];
    anmFileIndex = (short)fileIndex;
    anmFileEntry = entry;

    textureRect[0][0] = entry->field024;
    textureRect[0][1] = entry->field028;
    textureRect[0][2] = entry->field02C;
    textureRect[0][3] = entry->field030;
    textureRect[1][0] = entry->field024;
    textureRect[1][1] = entry->field028;
    textureRect[1][2] = entry->field02C;
    textureRect[1][3] = entry->field030;

    uvScrollX = entry->field038;
    uvScrollY = entry->field034;

    memset(transformMatrix, 0, sizeof(transformMatrix));
    transformMatrix[0][0] = 1.0f;
    transformMatrix[1][1] = 1.0f;
    transformMatrix[2][2] = 1.0f;
    transformMatrix[3][3] = 1.0f;

    memset(spriteScaleMatrix, 0, sizeof(spriteScaleMatrix));
    spriteScaleMatrix[0][0] = 1.0f;
    spriteScaleMatrix[1][1] = 1.0f;
    spriteScaleMatrix[2][2] = 1.0f;
    spriteScaleMatrix[3][3] = 1.0f;

    transformMatrix[0][0] = uvScrollX * (1.0f / 256.0f);
    transformMatrix[1][1] = uvScrollY * (1.0f / 256.0f);
    memcpy(textureTransformMatrix, transformMatrix, sizeof(textureTransformMatrix));

    spriteScaleMatrix[0][0] =
        (uvScrollX / entry->field020) * entry->field03C;
    spriteScaleMatrix[1][1] =
        (uvScrollY / entry->field01C) * entry->field040;

    return 0;
}

// Converts the VM's pixel-space coordinates to normalized texture space.
// The target divides X by 640 and Y by 480, then clamps only the lower bound.
void NormalizeAnmCoordinates(float *normalized, const float *pixels)
{
    normalized[0] = pixels[0] / 640.0f;
    normalized[1] = pixels[1] / 480.0f;

    if (normalized[0] < 0.0f)
    {
        normalized[0] = 0.0f;
    }
    if (normalized[1] < 0.0f)
    {
        normalized[1] = 0.0f;
    }
}
