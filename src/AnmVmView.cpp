#include "AnmVm.hpp"
#include "AnmVmView.hpp"

#include <math.h>

namespace
{
    struct AnmVmViewMetrics
    {
        float width;
        float height;
        float centerX;
        float centerY;
        float centerZ;
        float parentX;
        float parentY;
        float angle;
        unsigned int horizontalAnchor;
        unsigned int verticalAnchor;
        bool inheritParent;
    };

    __forceinline AnmVmViewMetrics GetAnmVmViewMetrics(const AnmVm *vm)
    {
        AnmVmViewMetrics metrics;
        const AnmVm *parent = vm->parentVm;
        metrics.inheritParent = parent != 0 && (vm->stateFlags & 0x100U) == 0;

        metrics.width = vm->uvScrollX * vm->scaleX * vm->scale2X;
        metrics.height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
        metrics.centerX = vm->position.x +
            vm->secondaryPosition.x + vm->alternatePosition.x;
        metrics.centerY = vm->position.y +
            vm->secondaryPosition.y + vm->alternatePosition.y;
        metrics.centerZ = vm->position.z +
            vm->secondaryPosition.z + vm->alternatePosition.z;
        metrics.parentX = 0.0f;
        metrics.parentY = 0.0f;
        metrics.angle = vm->unknown02C;

        if (metrics.inheritParent)
        {
            metrics.width = parent->scale2X * parent->scaleX * metrics.width;
            metrics.height = parent->scale2Y * parent->scaleY * metrics.height;
            metrics.parentX = parent->position.x +
                parent->secondaryPosition.x + parent->alternatePosition.x;
            metrics.parentY = parent->position.y +
                parent->secondaryPosition.y + parent->alternatePosition.y;
            // The target's three view helpers add the parent XY translation,
            // but all four output Z coordinates are overwritten with the
            // child's own Z sum at the end of each helper.
            metrics.angle += parent->unknown02C;
        }

        metrics.horizontalAnchor = (vm->flags >> 20) & 3U;
        metrics.verticalAnchor = (vm->flags >> 22) & 3U;
        return metrics;
    }

    __forceinline void WriteAxisAlignedCorners(
        const AnmVmViewMetrics &metrics,
        float centerX,
        float centerY,
        bool floorCenteredEdges,
        float *corners)
    {
        float left;
        float right;
        float top;
        float bottom;

        switch (metrics.horizontalAnchor)
        {
        case 0:
        {
            const float centeredLeft = centerX - metrics.width * 0.5f;
            left = floorCenteredEdges ? (float)floor((double)centeredLeft) : centeredLeft;
            right = left + metrics.width;
            break;
        }
        case 1:
            left = centerX;
            right = centerX + metrics.width;
            break;
        case 2:
            left = centerX - metrics.width;
            right = centerX;
            break;
        default:
            left = right = 0.0f;
            break;
        }

        switch (metrics.verticalAnchor)
        {
        case 0:
        {
            const float centeredTop = centerY - metrics.height * 0.5f;
            top = floorCenteredEdges ? (float)floor((double)centeredTop) : centeredTop;
            bottom = top + metrics.height;
            break;
        }
        case 1:
            top = centerY;
            bottom = centerY + metrics.height;
            break;
        case 2:
            top = centerY - metrics.height;
            bottom = centerY;
            break;
        default:
            top = bottom = 0.0f;
            break;
        }

        left += metrics.parentX;
        right += metrics.parentX;
        top += metrics.parentY;
        bottom += metrics.parentY;

        corners[0] = left;
        corners[1] = top;
        corners[2] = metrics.centerZ;
        corners[3] = right;
        corners[4] = top;
        corners[5] = metrics.centerZ;
        corners[6] = left;
        corners[7] = bottom;
        corners[8] = metrics.centerZ;
        corners[9] = right;
        corners[10] = bottom;
        corners[11] = metrics.centerZ;
    }
}

void BuildAnmVmViewCornersMode0(const AnmVm *vm, float *corners)
{
    const AnmVmViewMetrics metrics = GetAnmVmViewMetrics(vm);
    WriteAxisAlignedCorners(metrics, metrics.centerX, metrics.centerY, true, corners);
}

void BuildAnmVmViewCornersMode23(const AnmVm *vm, float *corners)
{
    const AnmVmViewMetrics metrics = GetAnmVmViewMetrics(vm);
    WriteAxisAlignedCorners(metrics, metrics.centerX, metrics.centerY, false, corners);
}

void BuildAnmVmViewCornersMode1(const AnmVm *vm, float *corners)
{
    const AnmVmViewMetrics metrics = GetAnmVmViewMetrics(vm);
    const float cosine = (float)cos((double)metrics.angle);
    const float sine = (float)sin((double)metrics.angle);
    const float originX = metrics.centerX + metrics.parentX;
    const float originY = metrics.centerY + metrics.parentY;

    float left;
    float right;
    float top;
    float bottom;
    switch (metrics.horizontalAnchor)
    {
    case 0:
        left = -metrics.width * 0.5f;
        right = metrics.width * 0.5f;
        break;
    case 1:
        left = 0.0f;
        right = metrics.width;
        break;
    case 2:
        left = -metrics.width;
        right = 0.0f;
        break;
    default:
        left = right = 0.0f;
        break;
    }

    switch (metrics.verticalAnchor)
    {
    case 0:
        top = -metrics.height * 0.5f;
        bottom = metrics.height * 0.5f;
        break;
    case 1:
        top = 0.0f;
        bottom = metrics.height;
        break;
    case 2:
        top = -metrics.height;
        bottom = 0.0f;
        break;
    default:
        top = bottom = 0.0f;
        break;
    }

    const float xOffsets[4] = {left, right, left, right};
    const float yOffsets[4] = {top, top, bottom, bottom};
    for (unsigned int i = 0; i != 4; ++i)
    {
        corners[i * 3] = originX +
            cosine * xOffsets[i] - sine * yOffsets[i];
        corners[i * 3 + 1] = originY +
            cosine * yOffsets[i] + sine * xOffsets[i];
        corners[i * 3 + 2] = metrics.centerZ;
    }
}

void BuildAnmVmViewCorners(const AnmVm *vm, float *corners)
{
    switch ((vm->flags >> 24) & 0x1FU)
    {
    case 0:
        BuildAnmVmViewCornersMode0(vm, corners);
        break;
    case 1:
        BuildAnmVmViewCornersMode1(vm, corners);
        break;
    case 2:
    case 3:
        BuildAnmVmViewCornersMode23(vm, corners);
        break;
    }
}
