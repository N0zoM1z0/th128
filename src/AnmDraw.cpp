#include "AnmDraw.hpp"
#include "AnmManager.hpp"
#include "Supervisor.hpp"

#include <d3d9.h>

// Reviewed leaf boundaries. Their private register conventions and complete
// implementations remain separate reconstruction work; these are declarations.
namespace AnmDrawLeaf
{
    int __stdcall PixelAlignedSprite(AnmVm *, AnmManager *);       // 0x0045F5D0
    int __stdcall RotatedSprite(AnmVm *, AnmManager *);            // 0x0045FD40
    int __stdcall ProjectedSprite(AnmVm *, AnmManager *);          // 0x00460240
    int __stdcall WorldMatrixBatchSprite(AnmVm *, AnmManager *);   // 0x00460B50
    int __stdcall FogSprite(AnmVm *, AnmManager *);                // 0x00460270
    int __stdcall WorldFogSprite(AnmVm *, AnmManager *);           // 0x00460B80
    int __stdcall WorldMatrixSprite(AnmVm *, AnmManager *);        // 0x00460DD0
    int __stdcall TriangleStrip(AnmVm *, AnmManager *, const void *, int); // 0x004615A0
    int __stdcall TriangleFan(AnmVm *, AnmManager *, const void *, int);   // 0x00461A00
    int __stdcall UnrotatedSprite(AnmVm *, AnmManager *);          // 0x0045FA10
    int __stdcall AlternateRotatedSprite(AnmVm *, AnmManager *);   // 0x0045FD70
    void __stdcall ApplyState(AnmVm *, AnmManager *);              // 0x0045E770
    void __stdcall Rectangle(AnmManager *, float, float, float, float, float, unsigned int); // 0x004620E0
    void __stdcall GradientRectangle(AnmManager *, float, float, float, float, float, unsigned int, unsigned int); // 0x004626B0
    void __stdcall OutlinedRectangle(AnmManager *, float, float, float, float, float, unsigned int); // 0x004628D0
    void __stdcall OutlinedGradientRectangle(AnmManager *, float, float, float, float, float, unsigned int, unsigned int); // 0x00462960
    void __stdcall Circle(AnmManager *, float, float, float, float, int, unsigned int, unsigned int); // 0x00462A00
    void __stdcall Line(AnmManager *, float, float, float, float, int, unsigned int); // 0x00462D60
}

namespace
{
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
        return AnmDrawLeaf::UnrotatedSprite(vm, manager);
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
