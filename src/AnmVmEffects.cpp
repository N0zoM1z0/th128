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

struct AnmVmRendererState
{
    unsigned char unknown000[0xA8];
    unsigned int stateCallCount;            // +0x0A8
    unsigned int drawCallCount;             // +0x0AC
    unsigned char unknown0B0[0x4E9688 - 0x0B0];
    void *currentTexture;                   // +0x4E9688
    unsigned char blendMode;                // +0x4E968C
    unsigned char unknown4E968D;
    unsigned char vertexFormatMode;         // +0x4E968E
    unsigned char unknown4E968F[3];
    unsigned char samplerFilterMode;        // +0x4E9692
    unsigned char textureStageState;        // +0x4E9693
    unsigned char unknown4E9694[0x4E96EC - 0x4E9694];
    int pendingBatchCount;                  // +0x4E96EC
    unsigned char unknown4E96F0[0x8696F0 - 0x4E96F0];
    void *nextBatchVertex;                  // +0x8696F0
    void *batchVertices;                    // +0x8696F4
};

typedef char AnmVmRendererTextureOffset[
    (offsetof(AnmVmRendererState, currentTexture) == 0x4E9688) ? 1 : -1
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

void DrawAnmVmVertices(AnmVm *vm, const void *vertices, unsigned int vertexCount)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    if (renderer == 0 || device == 0 || vm == 0 || vertices == 0 ||
        vm->anmFileEntry == 0 || vertexCount < 3)
    {
        return;
    }

    if (renderer->pendingBatchCount != 0)
    {
        FlushAnmSpriteBatch();
    }

    if (renderer->vertexFormatMode != 3)
    {
        device->SetFVF(0x144);
        renderer->vertexFormatMode = 3;
    }

    ApplyAnmVmRenderState(vm);

    void **textureSlot = &vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            reinterpret_cast<IDirect3DBaseTexture9 *>(*textureSlot));
    }

    FlushAnmSpriteBatch();
    device->SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE)14, 0);

    if (renderer->textureStageState != 1)
    {
        device->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
        device->SetTextureStageState(0, D3DTSS_COLORARG2, 0);
        renderer->textureStageState = 1;
    }

    device->DrawPrimitiveUP(
        D3DPT_TRIANGLEFAN,
        vertexCount - 2,
        vertices,
        sizeof(AnmVmEffect10Vertex));
}


namespace
{
    float RoundAnmSpriteCoordinate(float value)
    {
        const float lower = floorf(value);
        const float fraction = value - lower;
        if (fraction > 0.5f ||
            (fraction == 0.5f && fmodf(lower, 2.0f) != 0.0f))
        {
            return lower + 1.0f;
        }
        return lower;
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

static int DrawAnmVmSpriteQuad(AnmVm *vm, bool pixelSnap, bool rotate)
{
    AnmVmRendererState *renderer = GetAnmVmRenderer();
    IDirect3DDevice9 *device = GetAnmVmDevice();
    AnmManager *manager = GetGlobalAnmManager();
    if (renderer == 0 || device == 0 || manager == 0 || vm == 0 ||
        vm->anmFileEntry == 0)
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
    float centerZ = vm->position.z +
        vm->secondaryPosition.z + vm->alternatePosition.z;
    float angle = rotate ? vm->unknown02C : 0.0f;

    if (inheritParent)
    {
        width *= parent->scaleX * parent->scale2X;
        height *= parent->scaleY * parent->scale2Y;
        centerX += parent->position.x +
            parent->secondaryPosition.x + parent->alternatePosition.x;
        centerY += parent->position.y +
            parent->secondaryPosition.y + parent->alternatePosition.y;
        if (!rotate)
        {
            centerZ += parent->position.z +
                parent->secondaryPosition.z + parent->alternatePosition.z;
        }
        if (rotate)
        {
            angle += parent->unknown02C;
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

    if (pixelSnap)
    {
        if (horizontalAlignment == 0U)
        {
            left = RoundAnmSpriteCoordinate(left);
            right = left + width;
        }
        if (verticalAlignment == 0U)
        {
            top = RoundAnmSpriteCoordinate(top);
            bottom = top + height;
        }
    }

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
        if (pixelSnap)
        {
            vertices[i].x =
                RoundAnmSpriteCoordinate(vertices[i].x) - 0.5f;
            vertices[i].y =
                RoundAnmSpriteCoordinate(vertices[i].y) - 0.5f;
        }
        vertices[i].z = centerZ;
        vertices[i].reciprocalW = 1.0f;
        vm->drawVertices[i].x = vertices[i].x;
        vm->drawVertices[i].y = vertices[i].y;
        vm->drawVertices[i].z = vertices[i].z;
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
    if (manager->colorModulationEnabled != 0)
    {
        primaryColor = ModulateAnmSpriteColor(
            primaryColor, manager->colorModulation);
        secondaryColor = ModulateAnmSpriteColor(
            secondaryColor, manager->colorModulation);
    }

    const unsigned int colorMode = (vm->flags >> 16) & 3U;
    if (colorMode == 0U)
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

    void **textureSlot = &vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        FlushAnmSpriteBatch();
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            reinterpret_cast<IDirect3DBaseTexture9 *>(*textureSlot));
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

void AnmVm::UpdateWorldMatrix()
{
    AnmManager *manager = GetGlobalAnmManager();
    if (manager == 0)
    {
        return;
    }

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

    memcpy(
        manager->vmWorldMatrix,
        textureTransformMatrix,
        sizeof(manager->vmWorldMatrix));

    manager->vmWorldMatrix[3][0] +=
        position.x + secondaryPosition.x + alternatePosition.x;
    manager->vmWorldMatrix[3][1] +=
        position.y + secondaryPosition.y + alternatePosition.y;
    manager->vmWorldMatrix[3][2] =
        position.z + secondaryPosition.z + alternatePosition.z;
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

    void **textureSlot = &vm->anmFileEntry->texture;
    if (renderer->currentTexture != textureSlot)
    {
        renderer->currentTexture = textureSlot;
        device->SetTexture(
            0,
            reinterpret_cast<IDirect3DBaseTexture9 *>(*textureSlot));
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
    case 2:
        return DrawAnmVmSpriteQuad(vm, false, false);
    case 1:
    case 3:
        return DrawAnmVmSpriteQuad(vm, false, true);
    case 9:
    case 12:
    case 13:
    case 14:
        return DrawAnmVmTriangleStrip(
            vm, vm->extraData, vertexCount);

    case 11:
        DrawAnmVmVertices(vm, vm->extraData, vertexCount);
        return 0;

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
        DrawAnmVmVertices(vm, data->vertices, 33);
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
