#include "AnmDraw.hpp"
#include "AnmManager.hpp"
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
    void __stdcall Rectangle(AnmManager *, float, float, float, float, float, unsigned int); // 0x004620E0
    void __stdcall GradientRectangle(AnmManager *, float, float, float, float, float, unsigned int, unsigned int); // 0x004626B0
    void __stdcall OutlinedRectangle(AnmManager *, float, float, float, float, float, unsigned int); // 0x004628D0
    void __stdcall OutlinedGradientRectangle(AnmManager *, float, float, float, float, float, unsigned int, unsigned int); // 0x00462960
    void __stdcall Circle(AnmManager *, float, float, float, float, int, unsigned int, unsigned int); // 0x00462A00
    void __stdcall Line(AnmManager *, float, float, float, float, int, unsigned int); // 0x00462D60
}

AnmVmEffect10Vertex g_AnmSpriteVertices[4];

namespace
{
    // Partial view of the manager's render-state cache. The receiver is the
    // manager supplied by the caller, including when it differs from the global.
    struct AnmSpriteState
    {
        unsigned char unknown000[0x4E968C];
        unsigned char blendMode;
        unsigned char unknown4E968D[0x4E9692 - 0x4E968D];
        unsigned char samplerPointFilter;
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
