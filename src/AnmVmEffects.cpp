#include "AnmVm.hpp"
#include "AnmResource.hpp"
#include "AnmManager.hpp"
#include "AngleMath.hpp"

#include <d3d9.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct AnmVmEffect10Vertex
{
    float x;
    float y;
    float z;
    float reciprocalW;
    unsigned int color;
    float u;
    float v;

    void SetPolar(float angle, float radius);
};
typedef char AnmVmEffect10VertexSize[
    (sizeof(AnmVmEffect10Vertex) == 0x1C) ? 1 : -1
];

struct AnmVmEffect10Data
{
    float centerX;                          // +0x000
    float centerY;                          // +0x004
    float centerZ;                          // +0x008
    float centerW;                          // +0x00C
    unsigned int color;                     // +0x010
    float uvBaseU;                           // +0x014
    float uvBaseV;                           // +0x018
    AnmVmEffect10Vertex vertices[32];       // +0x01C
    float unknown231;                       // +0x39C
    float particleRadius[31];               // +0x3A0
    float unknown263[2];                    // +0x41C
    float particleRadialSpeed[31];          // +0x424
    float unknown296;                       // +0x4A0
    float uvDeltaU;                         // +0x4A4
    float uvDeltaV;                         // +0x4A8
    float unknown299;                       // +0x4AC
};
typedef char AnmVmEffect10DataSize[
    (sizeof(AnmVmEffect10Data) == 0x4B0) ? 1 : -1
];
typedef char AnmVmEffect10CenterVertexLayout[
    (offsetof(AnmVmEffect10Data, color) ==
         offsetof(AnmVmEffect10Vertex, color) &&
     offsetof(AnmVmEffect10Data, uvBaseU) ==
         offsetof(AnmVmEffect10Vertex, u) &&
     offsetof(AnmVmEffect10Data, uvBaseV) ==
         offsetof(AnmVmEffect10Vertex, v) &&
     offsetof(AnmVmEffect10Data, vertices) ==
         sizeof(AnmVmEffect10Vertex)) ? 1 : -1
];

struct AnmVmWorldInputVertex
{
    float x;
    float y;
    float z;
    float unknown0C;
    unsigned int color;
};
typedef char AnmVmWorldInputVertexSize[
    (sizeof(AnmVmWorldInputVertex) == 0x14) ? 1 : -1
];

struct AnmVmRendererState
{
    unsigned char unknown000[0xA8];
    unsigned int stateCallCount;            // +0x0A8
    unsigned int drawCallCount;             // +0x0AC
    unsigned char unknown0B0[0x4E9684 - 0x0B0];
    unsigned int currentTextureFactor;      // +0x4E9684
    void *currentTexture;                   // +0x4E9688
    unsigned char blendMode;                // +0x4E968C
    unsigned char unknown4E968D;
    unsigned char vertexFormatMode;         // +0x4E968E
    unsigned char unknown4E968F[3];
    unsigned char samplerFilterMode;        // +0x4E9692
    unsigned char textureStageState;        // +0x4E9693
    void *currentTextureTransformFile;      // +0x4E9694
    IDirect3DVertexBuffer9 *vertexBuffer;    // +0x4E9698
    AnmVmWorldInputVertex worldInputVertices[4]; // +0x4E969C
    int pendingBatchCount;                  // +0x4E96EC
    unsigned char unknown4E96F0[0x8696F0 - 0x4E96F0];
    void *nextBatchVertex;                  // +0x8696F0
    void *batchVertices;                    // +0x8696F4
};

typedef char AnmVmRendererTextureFactorOffset[
    (offsetof(AnmVmRendererState, currentTextureFactor) == 0x4E9684) ? 1 : -1
];
typedef char AnmVmRendererTextureOffset[
    (offsetof(AnmVmRendererState, currentTexture) == 0x4E9688) ? 1 : -1
];
typedef char AnmVmRendererTextureTransformFileOffset[
    (offsetof(AnmVmRendererState, currentTextureTransformFile) == 0x4E9694) ? 1 : -1
];
typedef char AnmVmRendererVertexBufferOffset[
    (offsetof(AnmVmRendererState, vertexBuffer) == 0x4E9698) ? 1 : -1
];
typedef char AnmVmRendererWorldInputVerticesOffset[
    (offsetof(AnmVmRendererState, worldInputVertices) == 0x4E969C) ? 1 : -1
];
typedef char AnmVmRendererPendingBatchOffset[
    (offsetof(AnmVmRendererState, pendingBatchCount) == 0x4E96EC) ? 1 : -1
];
typedef char AnmVmRendererBatchPointerOffset[
    (offsetof(AnmVmRendererState, batchVertices) == 0x8696F4) ? 1 : -1
];

namespace
{
    const float kEffect10Pi = 3.1415927410125732421875f;
    const float kEffect10AngleStep = 0.202683404f;
    const float kEffect10RadialSpeedLimit = 1.0f / 15.0f;

    AnmVmRendererState *GetAnmVmRenderer()
    {
        return *reinterpret_cast<AnmVmRendererState **>(0x004D2E50);
    }

    IDirect3DDevice9 *GetAnmVmDevice()
    {
        return *reinterpret_cast<IDirect3DDevice9 **>(0x004D2E70);
    }

    float NextSignedVmRandom(AnmVm *vm)
    {
        return vm->ResolveFloatOperand(10012.0f);
    }

    void WrapEffect10U(AnmVmEffect10Data *data)
    {
        data->uvBaseU += 1.0f;
        for (unsigned int i = 0; i != 32; ++i)
        {
            data->vertices[i].u += 1.0f;
        }
    }

    void WrapEffect10V(AnmVmEffect10Data *data)
    {
        data->uvBaseV += 1.0f;
        for (unsigned int i = 0; i != 32; ++i)
        {
            data->vertices[i].v += 1.0f;
        }
    }
}

void AnmVmEffect10Vertex::SetPolar(float angle, float radius)
{
    x = (float)(cos((double)angle) * (double)radius);
    y = (float)(sin((double)angle) * (double)radius);
}

void FlushAnmSpriteBatch()
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || renderer->pendingBatchCount == 0)
    {
        return;
    }

    if (renderer->textureStageState != 1)
    {
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
        device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
        renderer->textureStageState = 1;
    }

    device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
    device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
    device->SetFVF(0x144);
    device->DrawPrimitiveUP(
        D3DPT_TRIANGLELIST,
        (UINT)(renderer->pendingBatchCount * 2),
        renderer->batchVertices,
        0x1C);

    ++renderer->drawCallCount;
    renderer->batchVertices = renderer->nextBatchVertex;
    renderer->pendingBatchCount = 0;
}

void ApplyAnmVmRenderState(AnmVm *vm)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || vm == 0)
    {
        return;
    }

    const unsigned char blendMode = (unsigned char)((vm->flags >> 5) & 7U);
    if (renderer->blendMode != blendMode)
    {
        FlushAnmSpriteBatch();
        renderer->blendMode = blendMode;

        switch (blendMode)
        {
        case 0:
            device->SetRenderState(D3DRS_SRCBLEND, 5);
            device->SetRenderState(D3DRS_DESTBLEND, 6);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        case 1:
            device->SetRenderState(D3DRS_SRCBLEND, 5);
            device->SetRenderState(D3DRS_DESTBLEND, 2);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        case 2:
            device->SetRenderState(D3DRS_SRCBLEND, 5);
            device->SetRenderState(D3DRS_DESTBLEND, 2);
            device->SetRenderState(D3DRS_BLENDOP, 3);
            break;
        case 3:
            device->SetRenderState(D3DRS_SRCBLEND, 2);
            device->SetRenderState(D3DRS_DESTBLEND, 1);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        case 4:
            device->SetRenderState(D3DRS_SRCBLEND, 10);
            device->SetRenderState(D3DRS_DESTBLEND, 6);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        case 5:
            device->SetRenderState(D3DRS_SRCBLEND, 9);
            device->SetRenderState(D3DRS_DESTBLEND, 1);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        case 6:
            device->SetRenderState(D3DRS_SRCBLEND, 4);
            device->SetRenderState(D3DRS_DESTBLEND, 6);
            device->SetRenderState(D3DRS_BLENDOP, 1);
            break;
        default:
            break;
        }
    }

    const unsigned char samplerMode = (unsigned char)((vm->stateFlags >> 2) & 1U);
    if (renderer->samplerFilterMode != samplerMode)
    {
        FlushAnmSpriteBatch();
        renderer->samplerFilterMode = samplerMode;
        const DWORD filter = samplerMode == 0 ? 2 : 1;
        device->SetSamplerState(0, D3DSAMP_MINFILTER, filter);
        device->SetSamplerState(0, D3DSAMP_MAGFILTER, filter);
    }

    ++renderer->stateCallCount;
}

int DrawAnmVmVertices(
    AnmVm *vm,
    const void *vertices,
    unsigned int vertexCount)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || vm == 0 || vertices == 0 ||
        vm->anmFileEntry == 0 || vm->anmFileEntry->texture == 0 ||
        vertexCount < 3)
    {
        return -1;
    }

    if (renderer->pendingBatchCount != 0)
    {
        FlushAnmSpriteBatch();
    }

    void *textureSlot = vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            *reinterpret_cast<IDirect3DBaseTexture9 **>(textureSlot));
    }

    if (renderer->vertexFormatMode != 3)
    {
        device->SetFVF(0x144);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
        renderer->vertexFormatMode = 3;
    }

    ApplyAnmVmRenderState(vm);

    if (renderer->textureStageState != 1)
    {
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
        device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
        renderer->textureStageState = 1;
    }

    device->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN,
        vertexCount - 2,
        vertices,
        sizeof(AnmVmEffect10Vertex));
    return 0;
}


namespace
{
    float SnapAnmSpriteCoordinate(float value)
    {
        // FUN_0048FDF0 is the target's floor(double) helper. E920 subtracts
        // the D3D half-pixel offset after flooring each selected edge.
        return floorf(value) - 0.5f;
    }

    unsigned int ModulateAnmSpriteColor(
        unsigned int color,
        const unsigned char modulation[4])
    {
        unsigned int result = 0;
        for (unsigned int channel = 0; channel != 4; ++channel)
        {
            unsigned int value =
                ((color >> (channel * 8)) & 0xFFU) * modulation[channel] >> 7;
            if (value > 0xFFU)
            {
                value = 0xFFU;
            }
            result |= value << (channel * 8);
        }
        return result;
    }

    void AppendAnmVmSpriteQuad(
        AnmVmRendererState *renderer,
        const AnmVmEffect10Vertex vertices[4])
    {
        AnmVmEffect10Vertex *destination =
            reinterpret_cast<AnmVmEffect10Vertex *>(renderer->nextBatchVertex);
        destination[0] = vertices[0];
        destination[1] = vertices[1];
        destination[2] = vertices[2];
        destination[3] = vertices[1];
        destination[4] = vertices[2];
        destination[5] = vertices[3];
        renderer->nextBatchVertex = destination + 6;
        ++renderer->pendingBatchCount;
    }
}

static int DrawAnmVmSpriteQuad(
    AnmVm *vm,
    bool pixelSnap,
    bool rotate,
    bool applyColorModulation = true,
    const unsigned int *vertexColorOverride = 0)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    AnmManager *manager = GetGlobalAnmManager();
    if (renderer == 0 || device == 0 || manager == 0 || vm == 0 ||
        vm->anmFileEntry == 0 || vm->anmFileEntry->texture == 0)
    {
        return -1;
    }

    AnmVm *parent = vm->parentVm;
    const bool inheritParent =
        parent != 0 && (vm->stateFlags & 0x100U) == 0;

    float width = vm->uvScrollX * vm->scaleX * vm->scale2X;
    float height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
    float centerX = vm->position.x +
        vm->secondaryPosition.x + vm->alternatePosition.x;
    float centerY = vm->position.y +
        vm->secondaryPosition.y + vm->alternatePosition.y;
    const float centerZ = vm->position.z +
        vm->secondaryPosition.z + vm->alternatePosition.z;
    float angle = rotate ? vm->unknown02C : 0.0f;
    float parentOffsetX = 0.0f;
    float parentOffsetY = 0.0f;

    if (inheritParent)
    {
        width *= parent->scaleX * parent->scale2X;
        height *= parent->scaleY * parent->scale2Y;
        const float parentX = parent->position.x +
            parent->secondaryPosition.x + parent->alternatePosition.x;
        const float parentY = parent->position.y +
            parent->secondaryPosition.y + parent->alternatePosition.y;
        if (rotate)
        {
            centerX += parentX;
            centerY += parentY;
            angle += parent->unknown02C;
        }
        else
        {
            parentOffsetX = parentX;
            parentOffsetY = parentY;
        }
    }

    const unsigned int horizontalAlignment = (vm->flags >> 20) & 3U;
    const unsigned int verticalAlignment = (vm->flags >> 22) & 3U;
    float left = centerX;
    float right = centerX;
    float top = centerY;
    float bottom = centerY;

    switch (horizontalAlignment)
    {
    case 0:
        left -= width * 0.5f;
        right = left + width;
        break;
    case 1:
        right += width;
        break;
    case 2:
        left -= width;
        break;
    default:
        break;
    }

    switch (verticalAlignment)
    {
    case 0:
        top -= height * 0.5f;
        bottom = top + height;
        break;
    case 1:
        bottom += height;
        break;
    case 2:
        top -= height;
        break;
    default:
        break;
    }

    left += parentOffsetX;
    right += parentOffsetX;
    top += parentOffsetY;
    bottom += parentOffsetY;

    const float localX[4] = {left, right, left, right};
    const float localY[4] = {top, top, bottom, bottom};
    const float cosine = rotate ? (float)cos((double)angle) : 1.0f;
    const float sine = rotate ? (float)sin((double)angle) : 0.0f;
    const float offsetX = manager->viewOffsetX;
    const float offsetY = manager->viewOffsetY;

    AnmVmEffect10Vertex vertices[4];
    for (unsigned int i = 0; i != 4; ++i)
    {
        const float relativeX = localX[i] - centerX;
        const float relativeY = localY[i] - centerY;
        vertices[i].x = centerX +
            cosine * relativeX - sine * relativeY + offsetX;
        vertices[i].y = centerY +
            sine * relativeX + cosine * relativeY + offsetY;
        vertices[i].z = centerZ;
        vertices[i].reciprocalW = 1.0f;
        vm->drawVertices[i].x = vertices[i].x;
        vm->drawVertices[i].y = vertices[i].y;
        vm->drawVertices[i].z = vertices[i].z;
    }

    if (pixelSnap)
    {
        // The target snaps the first corner on each axis, then propagates
        // those rounded edges. Rounding all four vertices independently
        // changes the sprite dimensions when its size is fractional.
        vertices[0].x =
            SnapAnmSpriteCoordinate(vertices[0].x);
        vertices[1].x =
            SnapAnmSpriteCoordinate(vertices[1].x);
        vertices[0].y =
            SnapAnmSpriteCoordinate(vertices[0].y);
        vertices[2].y =
            SnapAnmSpriteCoordinate(vertices[2].y);

        vertices[2].x = vertices[0].x;
        vertices[3].x = vertices[1].x;
        vertices[1].y = vertices[0].y;
        vertices[3].y = vertices[2].y;

        for (unsigned int i = 0; i != 4; ++i)
        {
            vm->drawVertices[i].x = vertices[i].x;
            vm->drawVertices[i].y = vertices[i].y;
        }
    }

    const float (*rect)[4] = vm->textureRect;
    vertices[0].u = rect[0][0] + vm->textureScrollX;
    vertices[0].v = rect[0][1] + vm->textureScrollY;
    vertices[1].u = rect[0][0] +
        (rect[0][2] - rect[0][0]) * vm->unknown058 +
        vm->textureScrollX;
    vertices[1].v = rect[0][3] + vm->textureScrollY;
    vertices[2].u = rect[1][0] + vm->textureScrollX;
    vertices[2].v = rect[0][1] +
        (rect[1][1] - rect[0][1]) * vm->unknown05C +
        vm->textureScrollY;
    vertices[3].u = rect[1][0] +
        (rect[1][2] - rect[1][0]) * vm->unknown058 +
        vm->textureScrollX;
    vertices[3].v = rect[0][3] +
        (rect[1][3] - rect[0][3]) * vm->unknown05C +
        vm->textureScrollY;

    unsigned int primaryColor = vm->color;
    unsigned int secondaryColor = vm->secondaryColor;
    if (applyColorModulation && manager->colorModulationEnabled != 0)
    {
        primaryColor = ModulateAnmSpriteColor(
            primaryColor, manager->colorModulation);
        secondaryColor = ModulateAnmSpriteColor(
            secondaryColor, manager->colorModulation);
    }

    const unsigned int colorMode = (vm->flags >> 16) & 3U;
    if (vertexColorOverride != 0)
    {
        for (unsigned int i = 0; i != 4; ++i)
        {
            vertices[i].color = vertexColorOverride[i];
        }
    }
    else if (colorMode == 0U)
    {
        for (unsigned int i = 0; i != 4; ++i)
        {
            vertices[i].color = primaryColor;
        }
    }
    else if (colorMode == 1U)
    {
        for (unsigned int i = 0; i != 4; ++i)
        {
            vertices[i].color = secondaryColor;
        }
    }
    else if (colorMode == 2U)
    {
        vertices[0].color = primaryColor;
        vertices[1].color = secondaryColor;
        vertices[2].color = primaryColor;
        vertices[3].color = secondaryColor;
    }
    else
    {
        vertices[0].color = primaryColor;
        vertices[1].color = primaryColor;
        vertices[2].color = secondaryColor;
        vertices[3].color = secondaryColor;
    }

    const unsigned char *viewport =
        *reinterpret_cast<const unsigned char *const *>(0x004D33B4);
    if (viewport != 0)
    {
        const unsigned int leftEdge =
            (unsigned int)*reinterpret_cast<const int *>(viewport + 0xCC);
        const unsigned int topEdge =
            (unsigned int)*reinterpret_cast<const int *>(viewport + 0xD0);
        const unsigned int rightEdge = leftEdge +
            (unsigned int)*reinterpret_cast<const int *>(viewport + 0xD4);
        const unsigned int bottomEdge = topEdge +
            (unsigned int)*reinterpret_cast<const int *>(viewport + 0xD8);
        const float clipLeft = (float)leftEdge;
        const float clipTop = (float)topEdge;
        const float clipRight = (float)rightEdge;
        const float clipBottom = (float)bottomEdge;
        float minX = vertices[0].x;
        float maxX = vertices[0].x;
        float minY = vertices[0].y;
        float maxY = vertices[0].y;
        for (unsigned int i = 1; i != 4; ++i)
        {
            if (vertices[i].x < minX) minX = vertices[i].x;
            if (vertices[i].x > maxX) maxX = vertices[i].x;
            if (vertices[i].y < minY) minY = vertices[i].y;
            if (vertices[i].y > maxY) maxY = vertices[i].y;
        }
        if (maxX < clipLeft || maxY < clipTop ||
            clipRight < minX || clipBottom < minY)
        {
            return 0;
        }
    }

    void *textureSlot = vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        FlushAnmSpriteBatch();
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            *reinterpret_cast<IDirect3DBaseTexture9 **>(textureSlot));
    }

    if (renderer->vertexFormatMode != 1)
    {
        FlushAnmSpriteBatch();
        renderer->vertexFormatMode = 1;
    }

    ApplyAnmVmRenderState(vm);
    AppendAnmVmSpriteQuad(renderer, vertices);
    return 0;
}


namespace
{
    void MultiplyAnmVmMatrix(
        float destination[4][4],
        const float left[4][4],
        const float right[4][4])
    {
        float product[4][4];
        for (unsigned int row = 0; row != 4; ++row)
        {
            for (unsigned int column = 0; column != 4; ++column)
            {
                float value = 0.0f;
                for (unsigned int inner = 0; inner != 4; ++inner)
                {
                    value += left[row][inner] * right[inner][column];
                }
                product[row][column] = value;
            }
        }
        memcpy(destination, product, sizeof(product));
    }

    void MakeAnmVmRotationX(float matrix[4][4], float angle)
    {
        const float cosine = (float)cos((double)angle);
        const float sine = (float)sin((double)angle);
        memset(matrix, 0, sizeof(float) * 16);
        matrix[0][0] = 1.0f;
        matrix[1][1] = cosine;
        matrix[1][2] = sine;
        matrix[2][1] = -sine;
        matrix[2][2] = cosine;
        matrix[3][3] = 1.0f;
    }

    void MakeAnmVmRotationY(float matrix[4][4], float angle)
    {
        const float cosine = (float)cos((double)angle);
        const float sine = (float)sin((double)angle);
        memset(matrix, 0, sizeof(float) * 16);
        matrix[0][0] = cosine;
        matrix[0][2] = -sine;
        matrix[1][1] = 1.0f;
        matrix[2][0] = sine;
        matrix[2][2] = cosine;
        matrix[3][3] = 1.0f;
    }

    void MakeAnmVmRotationZ(float matrix[4][4], float angle)
    {
        const float cosine = (float)cos((double)angle);
        const float sine = (float)sin((double)angle);
        memset(matrix, 0, sizeof(float) * 16);
        matrix[0][0] = cosine;
        matrix[0][1] = sine;
        matrix[1][0] = -sine;
        matrix[1][1] = cosine;
        matrix[2][2] = 1.0f;
        matrix[3][3] = 1.0f;
    }
}

void AnmVm::UpdateWorldMatrix(float matrix[4][4], bool applyAlignment)
{
    if ((flags & 0x8000U) == 0 && (flags & 0x0CU) != 0)
    {
        memcpy(
            textureTransformMatrix,
            transformMatrix,
            sizeof(textureTransformMatrix));

        textureTransformMatrix[0][0] *= scaleX * scale2X;
        textureTransformMatrix[1][1] *= scaleY * scale2Y;
        flags &= ~8U;

        float rotation[4][4];
        if (unknown024 != 0.0f)
        {
            MakeAnmVmRotationX(rotation, unknown024);
            MultiplyAnmVmMatrix(
                textureTransformMatrix, textureTransformMatrix, rotation);
        }
        if (unknown028 != 0.0f)
        {
            MakeAnmVmRotationY(rotation, unknown028);
            MultiplyAnmVmMatrix(
                textureTransformMatrix, textureTransformMatrix, rotation);
        }
        if (unknown02C != 0.0f)
        {
            MakeAnmVmRotationZ(rotation, unknown02C);
            MultiplyAnmVmMatrix(
                textureTransformMatrix, textureTransformMatrix, rotation);
        }
        flags &= ~4U;
    }

    memcpy(matrix, textureTransformMatrix, sizeof(textureTransformMatrix));

    float pivotX =
        position.x + secondaryPosition.x + alternatePosition.x;
    float pivotY =
        position.y + secondaryPosition.y + alternatePosition.y;
    const float width = fabsf(uvScrollX * scaleX * scale2X) * 0.5f;
    const float height = fabsf(uvScrollY * scaleY * scale2Y) * 0.5f;

    if (applyAlignment)
    {
        switch ((flags >> 20) & 3U)
        {
        case 1:
            pivotX -= width;
            break;
        case 2:
            pivotX += width;
            break;
        default:
            break;
        }

        switch ((flags >> 22) & 3U)
        {
        case 1:
            pivotY -= height;
            break;
        case 2:
            pivotY += height;
            break;
        default:
            break;
        }
    }

    matrix[3][0] = pivotX;
    matrix[3][1] = pivotY;
    matrix[3][2] =
        position.z + secondaryPosition.z + alternatePosition.z;
    matrix[3][3] = 1.0f;
}



struct AnmVmProjectedPoint
{
    float x;
    float y;
    float z;
};

typedef AnmVmProjectedPoint *(__stdcall *AnmD3DXVec3ProjectFn)(
    AnmVmProjectedPoint *out,
    const AnmVmProjectedPoint *source,
    const D3DVIEWPORT9 *viewport,
    const D3DMATRIX *projection,
    const D3DMATRIX *view,
    const D3DMATRIX *world);

static int ProjectAnmVmSprite(
    const AnmVm *vm,
    AnmVmProjectedPoint *projectedCenter,
    float *projectedScale)
{
    const unsigned char *camera =
        *reinterpret_cast<const unsigned char *const *>(0x004D33B4);
    // The original D3DXVec3Project import thunk is at 0x0049A2D4.
    AnmD3DXVec3ProjectFn project =
        *reinterpret_cast<AnmD3DXVec3ProjectFn *>(0x0049A2D4);
    if (camera == 0 || project == 0 || vm == 0 ||
        projectedCenter == 0 || projectedScale == 0)
    {
        return -1;
    }

    const AnmVmProjectedPoint reference =
        *reinterpret_cast<const AnmVmProjectedPoint *>(camera + 0x30);
    const AnmVmProjectedPoint worldPosition =
    {
        vm->position.x + vm->secondaryPosition.x + vm->alternatePosition.x,
        vm->position.y + vm->secondaryPosition.y + vm->alternatePosition.y,
        vm->position.z + vm->secondaryPosition.z + vm->alternatePosition.z
    };

    D3DMATRIX world;
    memset(&world, 0, sizeof(world));
    world._11 = 1.0f;
    world._22 = 1.0f;
    world._33 = 1.0f;
    world._44 = 1.0f;

    AnmVmProjectedPoint projectedReference;
    const D3DVIEWPORT9 *viewport =
        reinterpret_cast<const D3DVIEWPORT9 *>(camera + 0xCC);
    const D3DMATRIX *view =
        reinterpret_cast<const D3DMATRIX *>(camera + 0x4C);
    const D3DMATRIX *projection =
        reinterpret_cast<const D3DMATRIX *>(camera + 0x8C);

    project(projectedCenter, &worldPosition, viewport, projection, view, &world);
    if (projectedCenter->z < 0.0f || projectedCenter->z > 1.0f)
    {
        return -1;
    }

    project(
        &projectedReference, &reference,
        viewport, projection, view, &world);

    const float deltaX = projectedReference.x - projectedCenter->x;
    const float deltaY = projectedReference.y - projectedCenter->y;
    const float deltaZ = projectedReference.z - projectedCenter->z;
    // FUN_0045fda0 multiplies the projected reference distance by
    // DAT_004A65A8 (0.5), then applies the VM's width/height scales.
    *projectedScale = (float)(
        sqrt((double)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ)) *
        0.5);
    return 0;
}

static void PrepareAnmVmProjectedSprite(
    AnmVm *projectedVm,
    const AnmVm *sourceVm,
    const AnmVmProjectedPoint &center,
    float projectedScale)
{
    *projectedVm = *sourceVm;
    projectedVm->parentVm = 0;
    projectedVm->stateFlags |= 0x100U;
    projectedVm->position.x = center.x;
    projectedVm->position.y = center.y;
    projectedVm->position.z = center.z;
    projectedVm->secondaryPosition.x = 0.0f;
    projectedVm->secondaryPosition.y = 0.0f;
    projectedVm->secondaryPosition.z = 0.0f;
    projectedVm->alternatePosition.x = 0.0f;
    projectedVm->alternatePosition.y = 0.0f;
    projectedVm->alternatePosition.z = 0.0f;
    projectedVm->scaleX = 1.0f;
    projectedVm->scaleY = 1.0f;
    projectedVm->scale2X = 1.0f;
    projectedVm->scale2Y = 1.0f;
    projectedVm->uvScrollX = sourceVm->uvScrollX *
        sourceVm->scaleX * sourceVm->scale2X * projectedScale;
    projectedVm->uvScrollY = sourceVm->uvScrollY *
        sourceVm->scaleY * sourceVm->scale2Y * projectedScale;
}

static int DrawAnmVmProjectedSprite(AnmVm *vm)
{
    AnmVmProjectedPoint center;
    float projectedScale;
    if (ProjectAnmVmSprite(vm, &center, &projectedScale) != 0)
    {
        return -1;
    }

    AnmVm projectedVm = *vm;
    PrepareAnmVmProjectedSprite(&projectedVm, vm, center, projectedScale);

    const int result = DrawAnmVmSpriteQuad(&projectedVm, false, true);
    memcpy(vm->drawVertices, projectedVm.drawVertices, sizeof(vm->drawVertices));
    return result;
}

static unsigned int FadeAnmVmFogColor(unsigned int color, float fadeFactor)
{
    const float *fogColor = reinterpret_cast<const float *>(0x004D3170);
    unsigned int result = 0;

    for (unsigned int channel = 0; channel != 3; ++channel)
    {
        const int fog = (int)floorf(fogColor[channel] + 0.5f);
        const int source = (int)((color >> (channel * 8)) & 0xFFU);
        const int adjustment = (int)((float)(source - fog) * fadeFactor);
        const unsigned int faded =
            (unsigned int)(source - adjustment) & 0xFFU;
        result |= faded << (channel * 8);
    }

    const int alpha = (int)(float)(color >> 24) * (1.0f - fadeFactor);
    result |= ((unsigned int)alpha & 0xFFU) << 24;
    return result;
}

static int DrawAnmVmFogSprite(AnmVm *vm)
{
    AnmManager *manager = GetGlobalAnmManager();
    const unsigned char *camera =
        *reinterpret_cast<const unsigned char *const *>(0x004D33B4);
    if (manager == 0 || camera == 0 || vm == 0)
    {
        return -1;
    }

    AnmVmProjectedPoint center;
    float projectedScale;
    if (ProjectAnmVmSprite(vm, &center, &projectedScale) != 0)
    {
        return -1;
    }

    const float worldX =
        vm->position.x + vm->secondaryPosition.x + vm->alternatePosition.x;
    const float worldY =
        vm->position.y + vm->secondaryPosition.y + vm->alternatePosition.y;
    const float worldZ =
        vm->position.z + vm->secondaryPosition.z + vm->alternatePosition.z;
    const float deltaX =
        worldX - *reinterpret_cast<const float *>(camera + 0x00);
    const float deltaY =
        worldY - *reinterpret_cast<const float *>(camera + 0x04);
    const float deltaZ =
        worldZ - *reinterpret_cast<const float *>(camera + 0x08);
    const float distance = (float)sqrt(
        (double)(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ));
    const float fogStart = *reinterpret_cast<const float *>(0x004D3168);
    const float fogRange =
        *reinterpret_cast<const float *>(camera + 0xFC) -
        *reinterpret_cast<const float *>(camera + 0x100);

    float fadeFactor = 0.0f;
    if (distance > fogStart)
    {
        fadeFactor = (fogStart - distance) / fogRange;
        if (fadeFactor >= 1.0f)
        {
            return -1;
        }
    }

    AnmVm projectedVm = *vm;
    PrepareAnmVmProjectedSprite(&projectedVm, vm, center, projectedScale);

    const unsigned int colorMode = (vm->flags >> 16) & 3U;
    if (manager->colorModulationEnabled != 0)
    {
        if (colorMode == 0U)
        {
            projectedVm.color = ModulateAnmSpriteColor(
                projectedVm.color, manager->colorModulation);
        }
        else if (colorMode == 1U)
        {
            projectedVm.secondaryColor = ModulateAnmSpriteColor(
                projectedVm.secondaryColor, manager->colorModulation);
        }
        else
        {
            projectedVm.color = ModulateAnmSpriteColor(
                projectedVm.color, manager->colorModulation);
            projectedVm.secondaryColor = ModulateAnmSpriteColor(
                projectedVm.secondaryColor, manager->colorModulation);
        }
    }

    if (distance > fogStart)
    {
        if (colorMode == 0U)
        {
            projectedVm.color = FadeAnmVmFogColor(
                projectedVm.color, fadeFactor);
        }
        else if (colorMode == 1U)
        {
            projectedVm.secondaryColor = FadeAnmVmFogColor(
                projectedVm.secondaryColor, fadeFactor);
        }
        else
        {
            projectedVm.color = FadeAnmVmFogColor(
                projectedVm.color, fadeFactor);
            projectedVm.secondaryColor = FadeAnmVmFogColor(
                projectedVm.secondaryColor, fadeFactor);
        }
    }

    const int result =
        DrawAnmVmSpriteQuad(&projectedVm, false, true, false);
    memcpy(vm->drawVertices, projectedVm.drawVertices, sizeof(vm->drawVertices));
    return result;
}

static int DrawAnmVmWorldMatrixSprite(AnmVm *vm)
{
    AnmManager *manager = GetGlobalAnmManager();
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (manager == 0 || renderer == 0 || device == 0 || vm == 0 ||
        vm->anmFileEntry == 0 || vm->anmFileEntry->texture == 0 ||
        renderer->vertexBuffer == 0)
    {
        return -1;
    }

    if ((vm->color & 0xFF000000U) == 0)
    {
        return -1;
    }

    if (renderer->pendingBatchCount != 0)
    {
        FlushAnmSpriteBatch();
    }

    float worldMatrix[4][4];
    vm->UpdateWorldMatrix(worldMatrix);
    ApplyAnmVmRenderState(vm);

    unsigned int color =
        (vm->flags & 0x00030000U) == 0 ? vm->color : vm->secondaryColor;
    if (manager->colorModulationEnabled != 0)
    {
        color = ModulateAnmSpriteColor(color, manager->colorModulation);
    }

    if (renderer->currentTextureFactor != color)
    {
        FlushAnmSpriteBatch();
        renderer->currentTextureFactor = color;
        device->SetRenderState(D3DRS_TEXTUREFACTOR, color);
    }

    device->SetTransform(
        D3DTS_WORLD,
        reinterpret_cast<const D3DMATRIX *>(worldMatrix));

    void *textureSlot = vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            *reinterpret_cast<IDirect3DBaseTexture9 **>(textureSlot));
    }

    if (renderer->currentTextureTransformFile != vm->anmFileEntry ||
        vm->textureScrollX != 0.0f || vm->textureScrollY != 0.0f ||
        vm->unknown058 != 1.0f || vm->unknown05C != 1.0f)
    {
        renderer->currentTextureTransformFile = vm->anmFileEntry;

        float textureMatrix[4][4];
        memcpy(textureMatrix, vm->spriteScaleMatrix, sizeof(textureMatrix));
        textureMatrix[0][0] *= vm->unknown058;
        textureMatrix[1][1] *= vm->unknown05C;
        textureMatrix[3][0] = vm->textureRect[0][0] + vm->textureScrollX;
        textureMatrix[3][1] = vm->textureRect[0][1] + vm->textureScrollY;

        device->SetTransform(
            D3DTS_TEXTURE0,
            reinterpret_cast<const D3DMATRIX *>(textureMatrix));
    }

    if (renderer->vertexFormatMode != 2)
    {
        device->SetStreamSource(0, renderer->vertexBuffer, 0, 0x14);
        device->SetFVF(0x102);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 3);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, 3);
        renderer->vertexFormatMode = 2;
    }

    if (renderer->textureStageState != 1)
    {
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
        device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
        renderer->textureStageState = 1;
    }

    device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
    return 0;
}

static unsigned int FadeAnmVmWorldVertexColor(
    unsigned int sourceColor,
    float fadeFactor,
    unsigned int fullFogColor)
{
    if (fadeFactor >= 1.0f)
    {
        return (fullFogColor & 0x00FFFFFFU) |
            (sourceColor & 0xFF000000U);
    }

    const float *fogChannels =
        reinterpret_cast<const float *>(0x004D3170);
    unsigned int result = sourceColor & 0xFF000000U;
    for (unsigned int channel = 0; channel != 3; ++channel)
    {
        const float source =
            (float)((sourceColor >> (channel * 8)) & 0xFFU);
        const int adjustment =
            (int)((source - fogChannels[channel]) * fadeFactor);
        result |= ((unsigned int)(source - adjustment) & 0xFFU)
            << (channel * 8);
    }
    return result;
}

static void ResetAnmVmSharedSpriteReciprocalW()
{
    *reinterpret_cast<float *>(0x004DAF34) = 1.0f;
    *reinterpret_cast<float *>(0x004DAF50) = 1.0f;
    *reinterpret_cast<float *>(0x004DAF6C) = 1.0f;
    *reinterpret_cast<float *>(0x004DAF88) = 1.0f;
}

static int DrawAnmVmWorldMatrixBatchSprite(AnmVm *vm)
{
    AnmManager *manager = GetGlobalAnmManager();
    if (manager == 0 || vm == 0)
    {
        return -1;
    }

    vm->UpdateWorldMatrix(manager->vmWorldMatrix, false);

    AnmVm drawVm = *vm;
    drawVm.parentVm = 0;
    drawVm.stateFlags |= 0x100U;
    const int result = DrawAnmVmSpriteQuad(&drawVm, false, false);
    memcpy(vm->drawVertices, drawVm.drawVertices, sizeof(vm->drawVertices));
    ResetAnmVmSharedSpriteReciprocalW();
    return result;
}

static int DrawAnmVmWorldFogSprite(AnmVm *vm)
{
    AnmManager *manager = GetGlobalAnmManager();
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    const unsigned char *camera =
        *reinterpret_cast<const unsigned char *const *>(0x004D33B4);
    if (manager == 0 || renderer == 0 || camera == 0 || vm == 0)
    {
        return -1;
    }

    vm->UpdateWorldMatrix(manager->vmWorldMatrix, false);

    const unsigned int sourceColor =
        (vm->flags & 0x00030000U) == 0 ? vm->color : vm->secondaryColor;

    const float fogStart = *reinterpret_cast<const float *>(0x004D3168);
    const float fogRange =
        *reinterpret_cast<const float *>(camera + 0xFC) -
        *reinterpret_cast<const float *>(camera + 0x100);
    const float cameraX = *reinterpret_cast<const float *>(0x004D306C);
    const float cameraY = *reinterpret_cast<const float *>(0x004D3070);
    const float cameraZ = *reinterpret_cast<const float *>(0x004D3074);
    const unsigned int fullFogColor =
        *reinterpret_cast<const unsigned int *>(camera + 0x114);

    unsigned int vertexColors[4];
    const float (*matrix)[4] = manager->vmWorldMatrix;
    for (unsigned int i = 0; i != 4; ++i)
    {
        const AnmVmWorldInputVertex *source =
            &renderer->worldInputVertices[i];
        const float worldX =
            source->x * matrix[0][0] +
            source->y * matrix[1][0] +
            source->z * matrix[2][0] + matrix[3][0];
        const float worldY =
            source->x * matrix[0][1] +
            source->y * matrix[1][1] +
            source->z * matrix[2][1] + matrix[3][1];
        const float worldZ =
            source->x * matrix[0][2] +
            source->y * matrix[1][2] +
            source->z * matrix[2][2] + matrix[3][2];

        const float deltaX = worldX - cameraX;
        const float deltaY = worldY - cameraY;
        const float deltaZ = worldZ - cameraZ;
        const float distance = (float)sqrt(
            (double)(deltaX * deltaX +
                     deltaY * deltaY +
                     deltaZ * deltaZ));

        if (distance <= fogStart)
        {
            vertexColors[i] = sourceColor;
        }
        else
        {
            const float fadeFactor = (fogStart - distance) / fogRange;
            vertexColors[i] = FadeAnmVmWorldVertexColor(
                sourceColor, fadeFactor, fullFogColor);
        }
    }

    AnmVm drawVm = *vm;
    drawVm.parentVm = 0;
    drawVm.stateFlags |= 0x100U;
    const int result = DrawAnmVmSpriteQuad(
        &drawVm, false, true, false, vertexColors);
    memcpy(vm->drawVertices, drawVm.drawVertices, sizeof(vm->drawVertices));
    ResetAnmVmSharedSpriteReciprocalW();
    return result;
}

static int DrawAnmVmFogEnabledWorldMatrixSprite(AnmVm *vm)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || vm == 0)
    {
        return -1;
    }

    // The mode-18 wrapper toggles D3DRS_FOGENABLE on the shared device
    // context, flushing queued 2D quads before each state transition.
    unsigned int *fogState =
        reinterpret_cast<unsigned int *>(0x004D2E68 + 0x990);
    if (*fogState != 1U)
    {
        FlushAnmSpriteBatch();
        *fogState = 1U;
        device->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }

    const int result = DrawAnmVmWorldMatrixSprite(vm);

    if (*fogState != 0U)
    {
        FlushAnmSpriteBatch();
        *fogState = 0U;
        device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
    return result;
}

int DrawAnmVmTriangleStrip(
    AnmVm *vm,
    const void *vertices,
    unsigned int vertexCount)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || vm == 0 || vertices == 0 ||
        vm->anmFileEntry == 0 || vertexCount < 3 ||
        (vm->flags & 3U) != 3U || (vm->color & 0xFF000000U) == 0)
    {
        return -1;
    }

    if (renderer->pendingBatchCount != 0)
    {
        FlushAnmSpriteBatch();
    }

    void *textureSlot = vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            *reinterpret_cast<IDirect3DBaseTexture9 **>(textureSlot));
    }

    if (renderer->vertexFormatMode != 3)
    {
        device->SetFVF(0x144);
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
        renderer->vertexFormatMode = 3;
    }

    ApplyAnmVmRenderState(vm);

    if (renderer->textureStageState != 1)
    {
        device->SetTextureStageState(0, D3DTSS_ALPHAOP, 4);
        device->SetTextureStageState(0, D3DTSS_COLOROP, 4);
        renderer->textureStageState = 1;
    }

    device->DrawPrimitiveUP(
        D3DPT_TRIANGLESTRIP,
        vertexCount - 2,
        vertices,
        sizeof(AnmVmEffect10Vertex));
    return 0;
}


namespace
{
    struct AnmVmPrimitiveVertex
    {
        float x;
        float y;
        float z;
        float reciprocalW;
        unsigned int color;
    };
    typedef char AnmVmPrimitiveVertexSize[
        (sizeof(AnmVmPrimitiveVertex) == 0x14) ? 1 : -1
    ];

    void SetAnmVmPrimitiveDrawState(
        AnmVmRendererState *renderer,
        IDirect3DDevice9 *device)
    {
        FlushAnmSpriteBatch();
        device->SetRenderState(D3DRS_ZWRITEENABLE, 0);

        if (renderer->textureStageState != 0)
        {
            device->SetTextureStageState(0, D3DTSS_ALPHAOP, 3);
            device->SetTextureStageState(0, D3DTSS_COLOROP, 3);
            renderer->textureStageState = 0;
        }

        if (renderer->vertexFormatMode != 1)
        {
            device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
            device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
            renderer->vertexFormatMode = 1;
        }

        device->SetFVF(0x44);
    }

    void WriteAnmVmPrimitiveQuad(
        AnmVmPrimitiveVertex *vertices,
        float centerX,
        float centerY,
        float width,
        float height,
        float angle,
        unsigned int color0,
        unsigned int color1,
        unsigned int color2,
        unsigned int color3)
    {
        float cosine = 1.0f;
        float sine = 0.0f;
        if (angle != 0.0f)
        {
            cosine = (float)cos((double)angle);
            sine = (float)sin((double)angle);
        }

        const float halfWidth = width * 0.5f;
        const float halfHeight = height * 0.5f;
        const float x[4] =
        {
            centerX + halfWidth * cosine - halfHeight * sine,
            centerX - (halfHeight * sine + halfWidth * cosine),
            centerX + halfHeight * sine + halfWidth * cosine,
            centerX + halfHeight * sine - halfWidth * cosine
        };
        const float y[4] =
        {
            centerY + halfHeight * cosine + halfWidth * sine,
            centerY + halfHeight * cosine - halfWidth * sine,
            centerY + halfWidth * sine - halfHeight * cosine,
            centerY - (halfWidth * sine + halfHeight * cosine)
        };
        const unsigned int colors[4] =
        {
            color0, color1, color2, color3
        };

        for (unsigned int i = 0; i != 4; ++i)
        {
            vertices[i].x = x[i];
            vertices[i].y = y[i];
            vertices[i].z = 0.0f;
            vertices[i].reciprocalW = 1.0f;
            vertices[i].color = colors[i];
        }
    }

    void DrawAnmVmPrimitiveQuad(
        AnmManager *manager,
        AnmVmRendererState *renderer,
        IDirect3DDevice9 *device,
        float centerX,
        float centerY,
        float width,
        float height,
        float angle,
        unsigned int color0,
        unsigned int color1,
        unsigned int color2,
        unsigned int color3)
    {
        if (manager->primitiveVertexCursor == 0)
        {
            return;
        }

        AnmVmPrimitiveVertex *vertices =
            reinterpret_cast<AnmVmPrimitiveVertex *>(
                manager->primitiveVertexCursor);
        WriteAnmVmPrimitiveQuad(
            vertices, centerX, centerY, width, height, angle,
            color0, color1, color2, color3);

        SetAnmVmPrimitiveDrawState(renderer, device);
        device->DrawPrimitiveUP(
            D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(AnmVmPrimitiveVertex));
        manager->primitiveVertexCursor += 0x50 / sizeof(float);
        ++manager->drawCallCount;
    }

    float AdvanceAnmPrimitiveAngle(float angle, float offset)
    {
        const float twoPi = 6.283185482025146484375f;
        angle += offset;

        int corrections = 0;
        while (angle > twoPi)
        {
            angle -= twoPi;
            if (corrections++ > 32)
            {
                break;
            }
        }

        while (angle < 0.0f)
        {
            angle += twoPi;
            if (corrections++ > 32)
            {
                break;
            }
        }
        return angle;
    }


    void DrawAnmVmPrimitiveLine(
        AnmManager *manager,
        AnmVmRendererState *renderer,
        IDirect3DDevice9 *device,
        float centerX,
        float centerY,
        float radius,
        float startAngle,
        int segmentCount,
        unsigned int color)
    {
        if (manager->primitiveVertexCursor == 0 || segmentCount <= 0)
        {
            return;
        }

        const unsigned int cursorAddress =
            (unsigned int)manager->primitiveVertexCursor;
        const unsigned int limitAddress =
            (unsigned int)&manager->primitiveVertexCursor;
        if (cursorAddress >= limitAddress)
        {
            return;
        }
        const unsigned long long requiredBytes =
            ((unsigned long long)(unsigned int)segmentCount + 1ULL) *
            sizeof(AnmVmPrimitiveVertex);
        if (requiredBytes >=
            (unsigned long long)(limitAddress - cursorAddress))
        {
            return;
        }

        AnmVmPrimitiveVertex *vertices =
            reinterpret_cast<AnmVmPrimitiveVertex *>(
                manager->primitiveVertexCursor);
        const float angleStep = 6.283185482025146484375f /
            (float)segmentCount;
        float angle = startAngle;
        for (int i = 0; i <= segmentCount; ++i)
        {
            AnmVmPrimitiveVertex *vertex = &vertices[i];
            vertex->x = centerX + (float)cos((double)angle) * radius;
            vertex->y = centerY + (float)sin((double)angle) * radius;
            vertex->z = 0.0f;
            vertex->reciprocalW = 1.0f;
            vertex->color = color;
            angle = AdvanceAnmPrimitiveAngle(angle, angleStep);
        }

        SetAnmVmPrimitiveDrawState(renderer, device);
        device->DrawPrimitiveUP(
            D3DPT_LINESTRIP,
            (UINT)segmentCount,
            vertices,
            sizeof(AnmVmPrimitiveVertex));
        manager->primitiveVertexCursor += (segmentCount + 1) * 5;
        ++manager->drawCallCount;
    }

    void DrawAnmVmPrimitiveFan(
        AnmManager *manager,
        AnmVmRendererState *renderer,
        IDirect3DDevice9 *device,
        float centerX,
        float centerY,
        float radius,
        float startAngle,
        int segmentCount,
        unsigned int centerColor,
        unsigned int edgeColor)
    {
        if (manager->primitiveVertexCursor == 0 || segmentCount <= 0)
        {
            return;
        }

        AnmVmPrimitiveVertex *vertices =
            reinterpret_cast<AnmVmPrimitiveVertex *>(
                manager->primitiveVertexCursor);
        vertices[0].x = centerX;
        vertices[0].y = centerY;
        vertices[0].z = 0.0f;
        vertices[0].reciprocalW = 1.0f;
        vertices[0].color = centerColor;

        const float angleStep = 6.283185482025146484375f /
            (float)segmentCount;
        float angle = startAngle;
        for (int i = 0; i <= segmentCount; ++i)
        {
            AnmVmPrimitiveVertex *vertex = &vertices[i + 1];
            vertex->x = centerX + (float)cos((double)angle) * radius;
            vertex->y = centerY + (float)sin((double)angle) * radius;
            vertex->z = 0.0f;
            vertex->reciprocalW = 1.0f;
            vertex->color = edgeColor;
            angle = AdvanceAnmPrimitiveAngle(angle, angleStep);
        }

        SetAnmVmPrimitiveDrawState(renderer, device);
        device->DrawPrimitiveUP(
            D3DPT_TRIANGLEFAN,
            (UINT)segmentCount,
            vertices,
            sizeof(AnmVmPrimitiveVertex));
        manager->primitiveVertexCursor += (segmentCount + 2) * 5;
        ++manager->drawCallCount;
    }

    void GetAnmVmEffectCenter(
        const AnmVm *vm,
        float *centerX,
        float *centerY,
        float *width,
        float *height,
        float *angle)
    {
        *centerX = vm->position.x +
            vm->secondaryPosition.x + vm->alternatePosition.x;
        *centerY = vm->position.y +
            vm->secondaryPosition.y + vm->alternatePosition.y;
        *width = vm->uvScrollX * vm->scaleX;
        *height = vm->uvScrollY * vm->scaleY;
        *angle = vm->unknown02C;

        AnmVm *parent = vm->parentVm;
        if (parent != 0 && (vm->stateFlags & 0x100U) == 0)
        {
            *centerX += parent->position.x +
                parent->secondaryPosition.x + parent->alternatePosition.x;
            *centerY += parent->position.y +
                parent->secondaryPosition.y + parent->alternatePosition.y;
            *width *= parent->scaleX;
            *height *= parent->scaleY;
            *angle += parent->unknown02C;
        }
    }

    unsigned int HalfAlphaColor(unsigned int color)
    {
        return (color & 0x00FFFFFFU) |
            (((color >> 24) >> 1) << 24);
    }

    int DrawAnmVmPrimitiveEffect(AnmVm *vm, unsigned int mode)
    {
        AnmManager *manager = GetGlobalAnmManager();
        AnmVmRendererState *renderer = GetAnmVmRenderer();
        IDirect3DDevice9 *device = GetAnmVmDevice();
        if (manager == 0 || renderer == 0 || device == 0 ||
            manager->primitiveVertexCursor == 0)
        {
            return -1;
        }

        float centerX;
        float centerY;
        float width;
        float height;
        float angle;
        GetAnmVmEffectCenter(
            vm, &centerX, &centerY, &width, &height, &angle);
        ApplyAnmVmRenderState(vm);

        switch (mode)
        {
        case 15:
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width, height, angle,
                vm->color, vm->color, vm->color, vm->color);
            return 0;

        case 16:
            DrawAnmVmPrimitiveFan(
                manager, renderer, device, centerX, centerY,
                width, angle, vm->integerVariables[0],
                vm->color, vm->secondaryColor);
            return 0;

        case 17:
            DrawAnmVmPrimitiveLine(
                manager, renderer, device, centerX, centerY,
                width, angle, vm->integerVariables[0], vm->color);
            return 0;

        case 19:
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width, height, angle,
                vm->color, vm->secondaryColor,
                vm->color, vm->secondaryColor);
            return 0;

        case 20:
        {
            const unsigned int outline = HalfAlphaColor(vm->color);
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width + 1.0f, height + 1.0f, angle,
                outline, outline, outline, outline);
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width, height, angle,
                vm->color, vm->color, vm->color, vm->color);
            return 0;
        }

        case 21:
        {
            const unsigned int outlineColor0 =
                vm->secondaryColor & 0x00FFFFFFU;
            const unsigned int outlineColor1 =
                HalfAlphaColor(vm->color);
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width + 1.0f, height + 1.0f, angle,
                outlineColor0, outlineColor1,
                outlineColor0, outlineColor1);
            DrawAnmVmPrimitiveQuad(
                manager, renderer, device, centerX, centerY,
                width, height, angle,
                vm->secondaryColor, vm->color,
                vm->secondaryColor, vm->color);
            return 0;
        }

        default:
            return 0;
        }
    }
}

int DrawAnmVm(AnmVm *vm)
{
    if (vm == 0 || (vm->flags & 3U) != 3U ||
        (vm->flags & 0x20000000U) != 0)
    {
        return -1;
    }

    const unsigned int mode = (vm->flags >> 24) & 0x1FU;
    const unsigned int vertexCount =
        (unsigned int)vm->integerVariables[0] * 2U;
    if (mode <= 8U &&
        (vm->color & 0xFF000000U) == 0 &&
        (vm->secondaryColor & 0xFF000000U) == 0)
    {
        return -1;
    }

    switch (mode)
    {
    case 0:
        return DrawAnmVmSpriteQuad(vm, true, false);
    case 1:
    case 3:
        return DrawAnmVmSpriteQuad(vm, false, true);
    case 2:
        return DrawAnmVmSpriteQuad(vm, false, false);
    case 4:
        return DrawAnmVmProjectedSprite(vm);
    case 5:
        return DrawAnmVmWorldMatrixBatchSprite(vm);
    case 6:
        return DrawAnmVmFogSprite(vm);
    case 7:
        return DrawAnmVmWorldFogSprite(vm);
    case 8:
        return DrawAnmVmWorldMatrixSprite(vm);
    case 18:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0)
        {
            return -1;
        }
        return DrawAnmVmFogEnabledWorldMatrixSprite(vm);
    case 9:
    case 12:
    case 13:
    case 14:
        return DrawAnmVmTriangleStrip(
            vm, vm->extraData, vertexCount);

    case 11:
        return DrawAnmVmVertices(vm, vm->extraData, vertexCount);

    case 15:
    case 16:
    case 17:
    case 19:
    case 20:
    case 21:
        return DrawAnmVmPrimitiveEffect(vm, mode);

    default:
        return 0;
    }
}

int __fastcall UpdateAnmVmEffect10(AnmVm *vm)
{
    AnmVmEffect10Data *data = (AnmVmEffect10Data *)vm->extraData;
    if (data == 0)
    {
        return 0;
    }

    data->centerX = vm->secondaryPosition.x + vm->position.x;
    data->centerY = vm->secondaryPosition.y + vm->position.y;
    data->centerZ = vm->secondaryPosition.z + vm->position.z;
    data->color = vm->color;

    data->uvBaseU += data->uvDeltaU;
    if (data->uvBaseU < 0.0f)
    {
        WrapEffect10U(data);
    }

    data->uvBaseV += data->uvDeltaU;
    if (data->uvBaseV < 0.0f)
    {
        WrapEffect10V(data);
    }

    float angle = -kEffect10Pi;
    for (unsigned int i = 0; i != 31; ++i)
    {
        AnmVmEffect10Vertex *vertex = &data->vertices[i];

        vertex->u += data->uvDeltaU;
        if (vertex->u < 0.0f)
        {
            WrapEffect10U(data);
        }

        vertex->v += data->uvDeltaU;
        if (vertex->v < 0.0f)
        {
            WrapEffect10V(data);
        }

        vertex->color = vm->color & 0x00FFFFFFU;

        data->particleRadius[i] += data->particleRadialSpeed[i];
        vertex->SetPolar(angle, data->particleRadius[i]);
        vertex->x += data->centerX;
        vertex->y += data->centerY;
        vertex->z += data->centerZ;

        angle += kEffect10AngleStep;
    }

    data->vertices[31] = data->vertices[0];
    return 0;
}

void __fastcall DrawAnmVmEffect10(AnmVm *vm)
{
    AnmVmEffect10Data *data = (AnmVmEffect10Data *)vm->extraData;
    if (data != 0)
    {
        // The leading 0x1C bytes are the center vertex; vertices[32] are
        // the perimeter, so the fan consumes the whole data block.
        DrawAnmVmVertices(vm, data, 33);
    }
}

namespace
{
    const float kAnmVmGeometryTwoPi = 6.283185482025146484375f;

    void WriteAnmVmGeometryVertexPair(
        AnmVmEffect10Vertex *vertices,
        float centerX,
        float centerY,
        float centerZ,
        float angle,
        float outerRadius,
        float innerRadius,
        unsigned int color,
        float outerU,
        float innerU,
        float v)
    {
        vertices[0].SetPolar(angle, outerRadius);
        vertices[0].x += centerX;
        vertices[0].y += centerY;
        vertices[0].z = centerZ;
        vertices[0].reciprocalW = 1.0f;
        vertices[0].color = color;
        vertices[0].u = outerU;
        vertices[0].v = v;

        vertices[1].SetPolar(angle, innerRadius);
        vertices[1].x += centerX;
        vertices[1].y += centerY;
        vertices[1].z = centerZ;
        vertices[1].reciprocalW = 1.0f;
        vertices[1].color = color;
        vertices[1].u = innerU;
        vertices[1].v = v;
    }
}

void AnmVm::UpdateCustomGeometry()
{
    const unsigned int geometryMode = (flags >> 24) & 0x1FU;
    if ((geometryMode != 9U && geometryMode != 13U && geometryMode != 14U) ||
        extraData == 0)
    {
        return;
    }

    AnmVmEffect10Vertex *vertices =
        reinterpret_cast<AnmVmEffect10Vertex *>(extraData);
    const unsigned int count = integerVariables[0] < 0
        ? 0U
        : (unsigned int)integerVariables[0];
    if (count == 0)
    {
        return;
    }

    float centerX = position.x + secondaryPosition.x + alternatePosition.x;
    float centerY = position.y + secondaryPosition.y + alternatePosition.y;
    float centerZ = position.z + secondaryPosition.z + alternatePosition.z;
    if (parentVm != 0)
    {
        centerX += parentVm->position.x +
            parentVm->secondaryPosition.x + parentVm->alternatePosition.x;
        centerY += parentVm->position.y +
            parentVm->secondaryPosition.y + parentVm->alternatePosition.y;
        centerZ += parentVm->position.z +
            parentVm->secondaryPosition.z + parentVm->alternatePosition.z;
    }

    const unsigned int vertexColor =
        (flags & 0x00030000U) == 0 ? color : secondaryColor;
    const float outerRadius = scaleX * 0.5f + scaleY;
    const float innerRadius = scaleY - scaleX * 0.5f;
    const float outerU = textureRect[0][0] + textureScrollX;
    const float innerU = textureRect[0][2] + textureScrollX;

    if (geometryMode == 9U)
    {
        const unsigned int segmentCount = count - 1U;
        if (segmentCount == 0)
        {
            vertices[0].v = textureScrollY;
            vertices[1].v = textureScrollY;
            return;
        }

        const float angleStep = kAnmVmGeometryTwoPi /
            (float)segmentCount;
        const float vStep = (float)integerVariables[1] /
            (float)segmentCount;
        float angle = unknown02C;
        float v = 0.0f;

        for (unsigned int i = 0; i != segmentCount; ++i)
        {
            WriteAnmVmGeometryVertexPair(
                vertices + i * 2,
                centerX,
                centerY,
                centerZ,
                angle,
                outerRadius,
                innerRadius,
                vertexColor,
                outerU,
                innerU,
                v + textureScrollY);
            v += vStep;
            angle = AddAngles(angle, angleStep);
        }

        AnmVmEffect10Vertex *closingVertices = vertices + segmentCount * 2;
        closingVertices[0] = vertices[0];
        closingVertices[1] = vertices[1];
        closingVertices[0].v = v + textureScrollY;
        closingVertices[1].v = v + textureScrollY;
        return;
    }

    const unsigned int divisor = count - 1U;
    const float angleStep = divisor != 0
        ? unknown024 / (float)divisor
        : 0.0f;
    const float vStep = divisor != 0
        ? (float)integerVariables[1] / (float)divisor
        : 0.0f;
    float angle = geometryMode == 13U
        ? NormalizeAngle(unknown02C - unknown024 * 0.5f)
        : NormalizeAngle(unknown02C);
    float v = 0.0f;

    for (unsigned int i = 0; i != count; ++i)
    {
        WriteAnmVmGeometryVertexPair(
            vertices + i * 2,
            centerX,
            centerY,
            centerZ,
            angle,
            outerRadius,
            innerRadius,
            vertexColor,
            outerU,
            innerU,
            v + textureScrollY);
        v += vStep;
        angle = AddAngles(angle, angleStep);
    }
}

void AnmVm::StartEffect10()
{
    if (extraData != 0)
    {
        free(extraData);
        extraData = 0;
    }

    AnmVmEffect10Data *data =
        (AnmVmEffect10Data *)malloc(sizeof(AnmVmEffect10Data));
    extraData = data;
    extraDataUpdateCallback = UpdateAnmVmEffect10;
    extraDataDrawCallback = DrawAnmVmEffect10;

    if (data == 0)
    {
        return;
    }

    data->uvDeltaU = NextSignedVmRandom(this) * (1.0f / 120.0f);
    data->uvDeltaV = NextSignedVmRandom(this) * (1.0f / 120.0f);

    data->centerX = secondaryPosition.x + position.x;
    data->centerY = secondaryPosition.y + position.y;
    data->centerZ = secondaryPosition.z + position.z;
    data->centerW = 1.0f;
    data->uvBaseU = 0.5f;
    data->uvBaseV = 0.5f;

    float angle = -kEffect10Pi;
    float radialSpeed = NextSignedVmRandom(this) * kEffect10RadialSpeedLimit;

    for (unsigned int i = 0; i != 31; ++i)
    {
        AnmVmEffect10Vertex *vertex = &data->vertices[i];
        vertex->SetPolar(angle, 0.5f);
        vertex->z = 0.0f;
        vertex->reciprocalW = 1.0f;
        vertex->u = vertex->x + 0.5f;
        vertex->v = vertex->y + 0.5f;

        data->particleRadius[i] =
            NextSignedVmRandom(this) * 8.0f + 80.0f;
        data->particleRadialSpeed[i] = radialSpeed;

        radialSpeed += NextSignedVmRandom(this) * (1.0f / 30.0f);
        if (radialSpeed > kEffect10RadialSpeedLimit)
        {
            radialSpeed = kEffect10RadialSpeedLimit;
        }
        else if (radialSpeed < -kEffect10RadialSpeedLimit)
        {
            radialSpeed = -kEffect10RadialSpeedLimit;
        }

        vertex->x += data->centerX;
        vertex->y += data->centerY;
        vertex->z += data->centerZ;
        angle += kEffect10AngleStep;
    }
}
