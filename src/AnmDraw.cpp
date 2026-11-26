#include "AnmDraw.hpp"
#include "AnmManager.hpp"
#include "AnmResource.hpp"
#include "AnmInterp.hpp"
#include "AnmSpriteRounding.hpp"
#include "AnmVmView.hpp"
#include "Supervisor.hpp"

#include <d3d9.h>

// Reviewed leaf boundaries. Their private register conventions and complete
// implementations remain separate reconstruction work; these are declarations.
namespace AnmDrawLeaf
{
    int __stdcall ProjectedSprite(AnmVm *, AnmManager *);          // 0x00460240
    int __stdcall WorldMatrixBatchSprite(AnmVm *, AnmManager *);   // 0x00460B50
    int __stdcall FogSprite(AnmVm *, AnmManager *);                // 0x00460270
    int __stdcall WorldFogSprite(AnmVm *, AnmManager *);           // 0x00460B80
    int __stdcall WorldMatrixSprite(AnmVm *, AnmManager *);        // 0x00460DD0
    int __stdcall TriangleStrip(AnmVm *, AnmManager *, const void *, int); // 0x004615A0
    int __stdcall TriangleFan(AnmVm *, AnmManager *, const void *, int);   // 0x00461A00
    void __stdcall Circle(AnmManager *, float, float, float, float, int, unsigned int, unsigned int); // 0x00462A00
    void __stdcall Line(AnmManager *, float, float, float, float, int, unsigned int); // 0x00462D60
}

AnmVmEffect10Vertex g_AnmSpriteVertices[4];

struct AnmSpriteViewport
{
    unsigned char unknown000[0xCC];
    D3DVIEWPORT9 viewport;
};

extern AnmSpriteViewport *g_AnmSpriteViewport;

namespace
{
    struct AnmPrimitiveVertex
    {
        float x, y, z, reciprocalW;
        unsigned int color;
    };

    // The two vertex stores and their counters/cursors are one manager
    // region. Sizes follow the observed reset bases and cursor offsets.
    struct AnmDrawBuffers
    {
        unsigned char unknown000[0x4E96EC];
        int spriteCount;
        AnmVmEffect10Vertex spriteVertices[0x20000];
        AnmVmEffect10Vertex *spriteCursor;
        AnmVmEffect10Vertex *spriteStart;
        int primitiveCount;
        AnmPrimitiveVertex primitiveVertices[0x4000];
        AnmPrimitiveVertex *primitiveCursor;
        AnmPrimitiveVertex *primitiveStart;
    };

    // Partial view of the manager's render-state cache. The receiver is the
    // manager supplied by the caller, including when it differs from the global.
    struct AnmSpriteState
    {
        unsigned char unknown000[0x4E9688];
        void **textureSlot;
        unsigned char blendMode;
        unsigned char unknown4E968D;
        unsigned char vertexFormatMode;
        unsigned char unknown4E968F[3];
        unsigned char samplerPointFilter;
    };

    union AnmSpriteColor
    {
        unsigned int value;
        struct
        {
            unsigned char blue, green, red, alpha;
        } channels;
    };

    inline ZunVec3 AddDrawVectors(const ZunVec3 &left, const ZunVec3 &right)
    {
        ZunVec3 result = {
            left.x + right.x,
            left.y + right.y,
            left.z + right.z
        };
        return result;
    }
}

void __stdcall AnmDrawLeaf::ResetBuffers()
{
    AnmDrawBuffers *buffers = reinterpret_cast<AnmDrawBuffers *>(g_AnmManager);
    buffers->spriteCursor = buffers->spriteVertices;
    buffers->spriteStart = buffers->spriteVertices;
    buffers->spriteCount = 0;
    buffers->primitiveCount = 0;
    buffers->primitiveCursor = buffers->primitiveVertices;
    buffers->primitiveStart = buffers->primitiveVertices;
}

void __stdcall AnmDrawLeaf::ApplyState(AnmVm *vm, AnmManager *manager)
{
    AnmSpriteState *state = reinterpret_cast<AnmSpriteState *>(manager);
    if (state->blendMode != ((vm->flags >> 5) & 7U))
    {
        manager->FlushSpriteBatch();
        state->blendMode = (unsigned char)((vm->flags >> 5) & 7U);
        switch (state->blendMode)
        {
        case 0:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        case 1:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        case 2:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
            break;
        case 3:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        case 4:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        case 6:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVSRCCOLOR);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        case 5:
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
            g_Supervisor.d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
            break;
        }
    }

    if (state->samplerPointFilter != ((vm->stateFlags >> 2) & 1U))
    {
        manager->FlushSpriteBatch();
        state->samplerPointFilter = (unsigned char)((vm->stateFlags >> 2) & 1U);
        if (state->samplerPointFilter == 0)
        {
            g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
            g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        }
        else
        {
            g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
            g_Supervisor.d3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        }
    }
    ++manager->stateCallCount;
}

int __stdcall AnmDrawLeaf::AppendSprite(
    AnmManager *manager, const AnmVmEffect10Vertex *vertices)
{
    // Re-read the cursor for each assignment: the source quad can alias the
    // manager, so caching the destination would give a stronger alias contract.
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[0] = vertices[0];
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[1] = vertices[1];
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[2] = vertices[2];
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[3] = vertices[1];
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[4] = vertices[2];
    reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase)[5] = vertices[3];
    manager->spriteVertexBufferBase =
        reinterpret_cast<AnmVmEffect10Vertex *>(manager->spriteVertexBufferBase) + 6;
    ++manager->spriteBatchCount;
    return 0;
}

int __stdcall AnmDrawLeaf::SubmitSprite(AnmManager *manager, AnmVm *vm, unsigned int flags)
{
    g_AnmSpriteVertices[0].x = g_AnmSpriteVertices[0].x + manager->viewOffsetX;
    g_AnmSpriteVertices[0].y = manager->viewOffsetY + g_AnmSpriteVertices[0].y;
    g_AnmSpriteVertices[1].x = g_AnmSpriteVertices[1].x + manager->viewOffsetX;
    g_AnmSpriteVertices[1].y = manager->viewOffsetY + g_AnmSpriteVertices[1].y;
    g_AnmSpriteVertices[2].x = g_AnmSpriteVertices[2].x + manager->viewOffsetX;
    g_AnmSpriteVertices[2].y = manager->viewOffsetY + g_AnmSpriteVertices[2].y;
    g_AnmSpriteVertices[3].x = g_AnmSpriteVertices[3].x + manager->viewOffsetX;
    g_AnmSpriteVertices[3].y = manager->viewOffsetY + g_AnmSpriteVertices[3].y;

    if ((flags & 1U) != 0)
    {
        const unsigned int roundingMode = _control87(0, 0);
        const float left = SnapAnmSpriteEdge(g_AnmSpriteVertices[0].x, roundingMode);
        const float right = SnapAnmSpriteEdge(g_AnmSpriteVertices[1].x, roundingMode);
        const float top = SnapAnmSpriteEdge(g_AnmSpriteVertices[0].y, roundingMode);
        const float bottom = SnapAnmSpriteEdge(g_AnmSpriteVertices[2].y, roundingMode);
        g_AnmSpriteVertices[2].y = g_AnmSpriteVertices[3].y = bottom;
        g_AnmSpriteVertices[0].y = g_AnmSpriteVertices[1].y = top;
        g_AnmSpriteVertices[1].x = g_AnmSpriteVertices[3].x = right;
        g_AnmSpriteVertices[0].x = g_AnmSpriteVertices[2].x = left;
    }

    vm->drawVertices[0] = *reinterpret_cast<const ZunVec3 *>(&g_AnmSpriteVertices[0].x);
    vm->drawVertices[1] = *reinterpret_cast<const ZunVec3 *>(&g_AnmSpriteVertices[1].x);
    vm->drawVertices[2] = *reinterpret_cast<const ZunVec3 *>(&g_AnmSpriteVertices[2].x);
    vm->drawVertices[3] = *reinterpret_cast<const ZunVec3 *>(&g_AnmSpriteVertices[3].x);

    const float (*rect)[4] = vm->textureRect;
    g_AnmSpriteVertices[0].u = rect[0][0] + vm->textureScrollX;
    g_AnmSpriteVertices[0].v = rect[0][1] + vm->textureScrollY;
    g_AnmSpriteVertices[1].u = (rect[0][2] - rect[0][0]) * vm->unknown058 +
        (rect[0][0] + vm->textureScrollX);
    g_AnmSpriteVertices[1].v = rect[0][3] + vm->textureScrollY;
    g_AnmSpriteVertices[2].u = vm->textureScrollX + rect[1][0];
    g_AnmSpriteVertices[2].v = (rect[1][1] - rect[0][1]) * vm->unknown05C +
        (rect[0][1] + vm->textureScrollY);
    g_AnmSpriteVertices[3].u = (rect[1][2] - rect[1][0]) * vm->unknown058 +
        (vm->textureScrollX + rect[1][0]);
    g_AnmSpriteVertices[3].v = (rect[1][3] - rect[0][3]) * vm->unknown05C +
        (rect[0][3] + vm->textureScrollY);

    float maxX = g_AnmSpriteVertices[1].x < g_AnmSpriteVertices[0].x
        ? g_AnmSpriteVertices[0].x : g_AnmSpriteVertices[1].x;
    if (maxX < g_AnmSpriteVertices[2].x) maxX = g_AnmSpriteVertices[2].x;
    if (maxX < g_AnmSpriteVertices[3].x) maxX = g_AnmSpriteVertices[3].x;
    float maxY = g_AnmSpriteVertices[1].y < g_AnmSpriteVertices[0].y
        ? g_AnmSpriteVertices[0].y : g_AnmSpriteVertices[1].y;
    if (maxY < g_AnmSpriteVertices[2].y) maxY = g_AnmSpriteVertices[2].y;
    if (maxY < g_AnmSpriteVertices[3].y) maxY = g_AnmSpriteVertices[3].y;
    float minX = g_AnmSpriteVertices[0].x < g_AnmSpriteVertices[1].x
        ? g_AnmSpriteVertices[0].x : g_AnmSpriteVertices[1].x;
    if (g_AnmSpriteVertices[2].x < minX) minX = g_AnmSpriteVertices[2].x;
    if (g_AnmSpriteVertices[3].x < minX) minX = g_AnmSpriteVertices[3].x;
    float minY = g_AnmSpriteVertices[0].y < g_AnmSpriteVertices[1].y
        ? g_AnmSpriteVertices[0].y : g_AnmSpriteVertices[1].y;
    if (g_AnmSpriteVertices[2].y < minY) minY = g_AnmSpriteVertices[2].y;
    if (g_AnmSpriteVertices[3].y < minY) minY = g_AnmSpriteVertices[3].y;

    const D3DVIEWPORT9 *viewport = &g_AnmSpriteViewport->viewport;
    if (maxX < viewport->X || maxY < viewport->Y ||
        (viewport->X + viewport->Width) < minX ||
        (viewport->Y + viewport->Height) < minY)
        return 0;

    AnmSpriteState *state = reinterpret_cast<AnmSpriteState *>(manager);
    if (state->textureSlot != vm->anmFileEntry->texture)
    {
        state->textureSlot = vm->anmFileEntry->texture;
        manager->FlushSpriteBatch();
        g_Supervisor.d3dDevice->SetTexture(0,
            static_cast<IDirect3DBaseTexture9 *>(*state->textureSlot));
    }
    if (state->vertexFormatMode != 1)
    {
        manager->FlushSpriteBatch();
        state->vertexFormatMode = 1;
    }

    if ((flags & 2U) == 0)
    {
        const unsigned int colorMode = (vm->flags >> 16) & 3U;
        AnmSpriteColor primary, secondary;
        switch (colorMode)
        {
        case 0:
        case 1:
            primary.value = colorMode == 0 ? vm->color : vm->secondaryColor;
            if (manager->colorModulationEnabled != 0)
            {
                primary.channels.red = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.red, manager->colorModulation[2]);
                primary.channels.green = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.green, manager->colorModulation[1]);
                primary.channels.blue = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.blue, manager->colorModulation[0]);
                primary.channels.alpha = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.alpha, manager->colorModulation[3]);
            }
            g_AnmSpriteVertices[2].color = primary.value;
            g_AnmSpriteVertices[3].color = primary.value;
            g_AnmSpriteVertices[1].color = primary.value;
            g_AnmSpriteVertices[0].color = primary.value;
            break;
        case 2:
        case 3:
            primary.value = vm->color;
            secondary.value = vm->secondaryColor;
            if (manager->colorModulationEnabled != 0)
            {
                primary.channels.red = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.red, manager->colorModulation[2]);
                primary.channels.green = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.green, manager->colorModulation[1]);
                primary.channels.blue = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.blue, manager->colorModulation[0]);
                primary.channels.alpha = (unsigned char)AnmInterpHelpers::BlendColorChannel(primary.channels.alpha, manager->colorModulation[3]);
                secondary.channels.red = (unsigned char)AnmInterpHelpers::BlendColorChannel(secondary.channels.red, manager->colorModulation[2]);
                secondary.channels.green = (unsigned char)AnmInterpHelpers::BlendColorChannel(secondary.channels.green, manager->colorModulation[1]);
                secondary.channels.blue = (unsigned char)AnmInterpHelpers::BlendColorChannel(secondary.channels.blue, manager->colorModulation[0]);
                secondary.channels.alpha = (unsigned char)AnmInterpHelpers::BlendColorChannel(secondary.channels.alpha, manager->colorModulation[3]);
            }
            g_AnmSpriteVertices[3].color = secondary.value;
            if (colorMode == 2)
            {
                g_AnmSpriteVertices[1].color = secondary.value;
                g_AnmSpriteVertices[2].color = primary.value;
            }
            else
            {
                g_AnmSpriteVertices[2].color = secondary.value;
                g_AnmSpriteVertices[1].color = primary.value;
            }
            g_AnmSpriteVertices[0].color = primary.value;
            break;
        }
    }
    ApplyState(vm, manager);
    AppendSprite(manager, g_AnmSpriteVertices);
    return 0;
}

int __stdcall AnmDrawLeaf::PixelAlignedSprite(AnmVm *vm, AnmManager *manager)
{
    BuildAnmVmViewCornersMode0(vm, &g_AnmSpriteVertices[0].x,
        &g_AnmSpriteVertices[1].x, &g_AnmSpriteVertices[2].x, &g_AnmSpriteVertices[3].x);
    return SubmitSprite(manager, vm, 1);
}

int __stdcall AnmDrawLeaf::UnrotatedSprite(AnmManager *manager, AnmVm *vm)
{
    BuildAnmVmViewCornersMode23(vm, &g_AnmSpriteVertices[0].x,
        &g_AnmSpriteVertices[1].x, &g_AnmSpriteVertices[2].x, &g_AnmSpriteVertices[3].x);
    return SubmitSprite(manager, vm, 0);
}

int __stdcall AnmDrawLeaf::RotatedSprite(AnmVm *vm, AnmManager *manager)
{
    BuildAnmVmViewCornersMode1(vm, &g_AnmSpriteVertices[0].x,
        &g_AnmSpriteVertices[1].x, &g_AnmSpriteVertices[2].x, &g_AnmSpriteVertices[3].x);
    return SubmitSprite(manager, vm, 0);
}

int __stdcall AnmDrawLeaf::AlternateRotatedSprite(AnmVm *vm, AnmManager *manager)
{
    BuildAnmVmViewCornersMode1(vm, &g_AnmSpriteVertices[0].x,
        &g_AnmSpriteVertices[1].x, &g_AnmSpriteVertices[2].x, &g_AnmSpriteVertices[3].x);
    return SubmitSprite(manager, vm, 0);
}

int __stdcall AnmDrawLeaf::Rectangle(AnmManager *manager,
    float x, float y, float width, float height, float angle, unsigned int color)
{
    AnmDrawBuffers *buffers = reinterpret_cast<AnmDrawBuffers *>(manager);
    AnmPrimitiveVertex *vertices = buffers->primitiveCursor;
    float cosine = 1.0f;
    float sine = 0.0f;
    if (angle != 0.0f)
    {
        cosine = (float)cos((double)angle);
        sine = (float)sin((double)angle);
    }
    width = (float)(width * 0.5);
    height = (float)(height * 0.5);
    vertices[0].x = (width * cosine - height * sine) + x;
    vertices[0].y = (width * sine + height * cosine) + y;
    vertices[0].z = 0.0f;
    vertices[1].x = x - (width * cosine + height * sine);
    vertices[1].y = (height * cosine - width * sine) + y;
    vertices[1].z = 0.0f;
    vertices[2].x = (width * cosine + height * sine) + x;
    vertices[2].y = (width * sine - height * cosine) + y;
    vertices[2].z = 0.0f;
    vertices[3].x = (height * sine - width * cosine) + x;
    vertices[3].y = y - (width * sine + height * cosine);
    vertices[3].z = 0.0f;
    vertices[3].color = color;
    vertices[2].color = color;
    vertices[1].color = color;
    vertices[0].color = color;
    vertices[3].reciprocalW = vertices[2].reciprocalW =
        vertices[1].reciprocalW = vertices[0].reciprocalW = 1.0f;

    g_AnmManager->FlushSpriteBatch();
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    if (g_AnmManager->spriteStageMode != 0)
    {
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        g_AnmManager->spriteStageMode = 0;
    }
    AnmSpriteState *state = reinterpret_cast<AnmSpriteState *>(manager);
    if (state->vertexFormatMode != 1)
    {
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        state->vertexFormatMode = 1;
    }
    g_Supervisor.d3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
        buffers->primitiveCursor, sizeof(AnmPrimitiveVertex));
    buffers->primitiveCursor += 4;
    ++manager->drawCallCount;
    return 0;
}

int __stdcall AnmDrawLeaf::GradientRectangle(AnmManager *manager,
    float x, float y, float width, float height, float angle, unsigned int leftColor, unsigned int rightColor)
{
    AnmDrawBuffers *buffers = reinterpret_cast<AnmDrawBuffers *>(manager);
    AnmPrimitiveVertex *vertices = buffers->primitiveCursor;
    float cosine = 1.0f;
    float sine = 0.0f;
    if (angle != 0.0f)
    {
        cosine = (float)cos((double)angle);
        sine = (float)sin((double)angle);
    }
    width = (float)(width * 0.5);
    height = (float)(height * 0.5);
    vertices[0].x = (width * cosine - height * sine) + x;
    vertices[0].y = (width * sine + height * cosine) + y;
    vertices[0].z = 0.0f;
    vertices[1].x = x - (width * cosine + height * sine);
    vertices[1].y = (height * cosine - width * sine) + y;
    vertices[1].z = 0.0f;
    vertices[2].x = (width * cosine + height * sine) + x;
    vertices[2].y = (width * sine - height * cosine) + y;
    vertices[2].z = 0.0f;
    vertices[3].x = (height * sine - width * cosine) + x;
    vertices[3].y = y - (width * sine + height * cosine);
    vertices[3].z = 0.0f;
    vertices[2].color = leftColor;
    vertices[0].color = leftColor;
    vertices[3].color = rightColor;
    vertices[1].color = rightColor;
    vertices[3].reciprocalW = vertices[2].reciprocalW =
        vertices[1].reciprocalW = vertices[0].reciprocalW = 1.0f;

    g_AnmManager->FlushSpriteBatch();
    g_Supervisor.d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    if (g_AnmManager->spriteStageMode != 0)
    {
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        g_AnmManager->spriteStageMode = 0;
    }
    AnmSpriteState *state = reinterpret_cast<AnmSpriteState *>(manager);
    if (state->vertexFormatMode != 1)
    {
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        g_Supervisor.d3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        state->vertexFormatMode = 1;
    }
    g_Supervisor.d3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
    g_Supervisor.d3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2,
        buffers->primitiveCursor, sizeof(AnmPrimitiveVertex));
    buffers->primitiveCursor += 4;
    ++manager->drawCallCount;
    return 0;
}

int __stdcall AnmDrawLeaf::OutlinedRectangle(AnmManager *manager,
    float x, float y, float width, float height, float angle, unsigned int color)
{
    const unsigned int borderColor = (color & 0x00FFFFFFU) | ((color >> 1) & 0x7F000000U);
    Rectangle(manager, x, y, width + 1.0f, height + 1.0f, angle, borderColor);
    Rectangle(manager, x, y, width, height, angle, color);
    return 0;
}

int __stdcall AnmDrawLeaf::OutlinedGradientRectangle(AnmManager *manager,
    float x, float y, float width, float height, float angle,
    unsigned int leftColor, unsigned int rightColor)
{
    const unsigned int borderLeft = (leftColor & 0x00FFFFFFU) | ((leftColor >> 1) & 0x7F000000U);
    const unsigned int borderRight = (rightColor & 0x00FFFFFFU) | ((rightColor >> 1) & 0x7F000000U);
    GradientRectangle(manager, x, y, width + 1.0f, height + 1.0f, angle, borderLeft, borderRight);
    GradientRectangle(manager, x, y, width, height, angle, leftColor, rightColor);
    return 0;
}

ZunVec3 __stdcall GetAnmVmDrawPosition(const AnmVm *vm)
{
    return AddDrawVectors(AddDrawVectors(vm->secondaryPosition, vm->position),
        vm->alternatePosition);
}

long AnmFogState::Enable()
{
    if (enabled != 1)
    {
        g_AnmManager->FlushSpriteBatch();
        enabled = 1;
        return device->SetRenderState(D3DRS_FOGENABLE, TRUE);
    }
    return 0;
}

long AnmFogState::Disable()
{
    if (enabled != 0)
    {
        g_AnmManager->FlushSpriteBatch();
        enabled = 0;
        return device->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
    return 0;
}

int AnmVm::Draw(AnmManager *manager)
{
    AnmVm *vm = this;
    if ((vm->flags & 1U) == 0 || (vm->flags & 2U) == 0 ||
        (vm->flags & 0x20000000U) != 0)
        return -1;

    switch ((vm->flags >> 24) & 0x1FU)
    {
    case 0:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::PixelAlignedSprite(vm, manager);
    case 1:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::RotatedSprite(vm, manager);
    case 4:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::ProjectedSprite(vm, manager);
    case 5:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::WorldMatrixBatchSprite(vm, manager);
    case 6:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::FogSprite(vm, manager);
    case 7:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::WorldFogSprite(vm, manager);
    case 8:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::WorldMatrixSprite(vm, manager);
    case 18:
    {
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        AnmFogState *fog = reinterpret_cast<AnmFogState *>(
            reinterpret_cast<unsigned char *>(&g_Supervisor) + 8);
        fog->Enable();
        AnmDrawLeaf::WorldMatrixSprite(vm, manager);
        fog->Disable();
        return 0;
    }
    case 9:
    case 12:
    case 13:
    case 14:
        return AnmDrawLeaf::TriangleStrip(vm, manager, vm->extraData,
            vm->integerVariables[0] * 2);
    case 11:
        return AnmDrawLeaf::TriangleFan(vm, manager, vm->extraData,
            vm->integerVariables[0] * 2);
    case 2:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::UnrotatedSprite(manager, vm);
    case 3:
        if ((vm->color & 0xFF000000U) == 0 &&
            (vm->secondaryColor & 0xFF000000U) == 0) return -1;
        return AnmDrawLeaf::AlternateRotatedSprite(vm, manager);
    case 15:
    case 19:
    case 20:
    case 21:
    {
        float angle = vm->unknown02C;
        float width = vm->uvScrollX * vm->scaleX;
        float height = vm->uvScrollY * vm->scaleY;
        ZunVec3 center = GetAnmVmDrawPosition(vm);
        AnmVm *parent = vm->parentVm;
        if (parent != 0 && (vm->stateFlags & 0x100U) == 0)
        {
            ZunVec3 parentPosition = GetAnmVmDrawPosition(parent);
            center.x = parentPosition.x + center.x;
            center.y = parentPosition.y + center.y;
            parent = vm->parentVm;
            width = parent->scaleX * width;
            height = parent->scaleY * height;
            angle = parent->unknown02C + angle;
        }
        AnmDrawLeaf::ApplyState(vm, manager);
        switch ((vm->flags >> 24) & 0x1FU)
        {
        case 15:
            AnmDrawLeaf::Rectangle(manager, center.x, center.y, width, height, angle, vm->color);
            break;
        case 19:
            AnmDrawLeaf::GradientRectangle(manager, center.x, center.y, width, height, angle, vm->color, vm->secondaryColor);
            break;
        case 20:
            AnmDrawLeaf::OutlinedRectangle(manager, center.x, center.y, width, height, angle, vm->color);
            break;
        case 21:
            AnmDrawLeaf::OutlinedGradientRectangle(manager, center.x, center.y, width, height, angle, vm->color, vm->secondaryColor);
            break;
        }
        return 0;
    }
    case 16:
    case 17:
    {
        float angle = vm->unknown02C;
        float radius = vm->uvScrollX * vm->scaleX;
        ZunVec3 center = GetAnmVmDrawPosition(vm);
        AnmVm *parent = vm->parentVm;
        if (parent != 0 && (vm->stateFlags & 0x100U) == 0)
        {
            ZunVec3 parentPosition = GetAnmVmDrawPosition(parent);
            center.x = parentPosition.x + center.x;
            center.y = parentPosition.y + center.y;
            parent = vm->parentVm;
            radius = parent->scaleX * radius;
            angle = parent->unknown02C + angle;
        }
        AnmDrawLeaf::ApplyState(vm, manager);
        switch ((vm->flags >> 24) & 0x1FU)
        {
        case 17:
            AnmDrawLeaf::Line(manager, center.x, center.y, radius, angle,
                vm->integerVariables[0], vm->color);
            break;
        case 16:
            AnmDrawLeaf::Circle(manager, center.x, center.y, radius, angle,
                vm->integerVariables[0], vm->color, vm->secondaryColor);
            break;
        }
        return 0;
    }
    default:
        return 0;
    }
}
