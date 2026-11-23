#include "AnmVm.hpp"
#include "AnmResource.hpp"
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

    switch (mode)
    {
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
