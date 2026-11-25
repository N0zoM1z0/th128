#include "AnmVm.hpp"
#include "AnmVmView.hpp"

#include <math.h>

void BuildAnmVmViewCornersMode0(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight)
{
    const AnmVm *parent = vm->parentVm;
    const bool inheritParent = parent != 0 && (vm->stateFlags & 0x100U) == 0;

    float width = vm->uvScrollX * vm->scaleX * vm->scale2X;
    float height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
    if (inheritParent)
    {
        width = parent->scale2X * parent->scaleX * width;
        height = parent->scale2Y * parent->scaleY * height;
    }

    const float centerX =
        vm->position.x + vm->secondaryPosition.x + vm->alternatePosition.x;
    const float centerY =
        vm->position.y + vm->secondaryPosition.y + vm->alternatePosition.y;

    // Anchor value 3 is a no-write mode in the target; do not replace the
    // caller's existing corner coordinates with a synthetic zero rectangle.
    const unsigned int horizontalAnchor = (vm->flags >> 20) & 3U;
    if (horizontalAnchor != 3U)
    {
        float left;
        float right;
        switch (horizontalAnchor)
        {
        case 0:
            left = (float)floor((double)(centerX - width * 0.5f));
            right = left + width;
            break;
        case 1:
            left = centerX;
            right = centerX + width;
            break;
        default:
            left = centerX - width;
            right = centerX;
            break;
        }

        topLeft[0] = left;
        topRight[0] = right;
        bottomLeft[0] = left;
        bottomRight[0] = right;
    }

    const unsigned int verticalAnchor = (vm->flags >> 22) & 3U;
    if (verticalAnchor != 3U)
    {
        float top;
        float bottom;
        switch (verticalAnchor)
        {
        case 0:
            top = (float)floor((double)(centerY - height * 0.5f));
            bottom = top + height;
            break;
        case 1:
            top = centerY;
            bottom = centerY + height;
            break;
        default:
            top = centerY - height;
            bottom = centerY;
            break;
        }

        topLeft[1] = top;
        topRight[1] = top;
        bottomLeft[1] = bottom;
        bottomRight[1] = bottom;
    }

    if (inheritParent)
    {
        // Preserve the target's per-corner x87 addition order. The original
        // materializes each parent offset as a float before adding it to a
        // corner, and swaps the first two X operands for the middle corners.
        volatile float topLeftX =
            parent->secondaryPosition.x + parent->position.x;
        topLeftX = parent->alternatePosition.x + topLeftX;
        topLeft[0] = topLeft[0] + topLeftX;
        volatile float topLeftY =
            parent->secondaryPosition.y + parent->position.y;
        topLeftY = parent->alternatePosition.y + topLeftY;
        topLeft[1] = topLeft[1] + topLeftY;

        volatile float topRightX =
            parent->position.x + parent->secondaryPosition.x;
        topRightX = parent->alternatePosition.x + topRightX;
        topRight[0] = topRight[0] + topRightX;
        volatile float topRightY =
            parent->secondaryPosition.y + parent->position.y;
        topRightY = parent->alternatePosition.y + topRightY;
        topRight[1] = topRight[1] + topRightY;

        volatile float bottomLeftX =
            parent->position.x + parent->secondaryPosition.x;
        bottomLeftX = parent->alternatePosition.x + bottomLeftX;
        bottomLeft[0] = bottomLeft[0] + bottomLeftX;
        volatile float bottomLeftY =
            parent->secondaryPosition.y + parent->position.y;
        bottomLeftY = parent->alternatePosition.y + bottomLeftY;
        bottomLeft[1] = bottomLeft[1] + bottomLeftY;

        volatile float bottomRightX =
            parent->secondaryPosition.x + parent->position.x;
        bottomRightX = parent->alternatePosition.x + bottomRightX;
        bottomRight[0] = bottomRight[0] + bottomRightX;
        volatile float bottomRightY =
            parent->secondaryPosition.y + parent->position.y;
        bottomRightY = parent->alternatePosition.y + bottomRightY;
        bottomRight[1] = bottomRight[1] + bottomRightY;

        // The target also translates each temporary corner Z by the parent,
        // then replaces all four Z values with the child's center Z below.
        volatile float topLeftZ =
            parent->secondaryPosition.z + parent->position.z;
        topLeftZ = parent->alternatePosition.z + topLeftZ;
        topLeft[2] = topLeft[2] + topLeftZ;
        volatile float topRightZ =
            parent->secondaryPosition.z + parent->position.z;
        topRightZ = parent->alternatePosition.z + topRightZ;
        topRight[2] = topRight[2] + topRightZ;
        volatile float bottomLeftZ =
            parent->secondaryPosition.z + parent->position.z;
        bottomLeftZ = parent->alternatePosition.z + bottomLeftZ;
        bottomLeft[2] = bottomLeft[2] + bottomLeftZ;
        volatile float bottomRightZ =
            parent->secondaryPosition.z + parent->position.z;
        bottomRightZ = parent->alternatePosition.z + bottomRightZ;
        bottomRight[2] = bottomRight[2] + bottomRightZ;
    }

    const float centerZ =
        vm->position.z + vm->secondaryPosition.z + vm->alternatePosition.z;
    topLeft[2] = centerZ;
    topRight[2] = centerZ;
    bottomLeft[2] = centerZ;
    bottomRight[2] = centerZ;
}

void BuildAnmVmViewCornersMode23(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight)
{
    const AnmVm *parent = vm->parentVm;
    const bool inheritParent = parent != 0 && (vm->stateFlags & 0x100U) == 0;

    float width = vm->uvScrollX * vm->scaleX * vm->scale2X;
    float height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
    if (inheritParent)
    {
        width = parent->scale2X * parent->scaleX * width;
        height = parent->scale2Y * parent->scaleY * height;
    }

    const float centerX =
        vm->position.x + vm->secondaryPosition.x + vm->alternatePosition.x;
    const float centerY =
        vm->position.y + vm->secondaryPosition.y + vm->alternatePosition.y;

    // Mode 2/3 shares the target's no-write handling for anchor value 3.
    const unsigned int horizontalAnchor = (vm->flags >> 20) & 3U;
    if (horizontalAnchor != 3U)
    {
        float left;
        float right;
        switch (horizontalAnchor)
        {
        case 0:
            left = centerX - width * 0.5f;
            right = left + width;
            break;
        case 1:
            left = centerX;
            right = centerX + width;
            break;
        default:
            left = centerX - width;
            right = centerX;
            break;
        }

        topLeft[0] = left;
        topRight[0] = right;
        bottomLeft[0] = left;
        bottomRight[0] = right;
    }

    const unsigned int verticalAnchor = (vm->flags >> 22) & 3U;
    if (verticalAnchor != 3U)
    {
        float top;
        float bottom;
        switch (verticalAnchor)
        {
        case 0:
            top = centerY - height * 0.5f;
            bottom = top + height;
            break;
        case 1:
            top = centerY;
            bottom = centerY + height;
            break;
        default:
            top = centerY - height;
            bottom = centerY;
            break;
        }

        topLeft[1] = top;
        topRight[1] = top;
        bottomLeft[1] = bottom;
        bottomRight[1] = bottom;
    }

    if (inheritParent)
    {
        // The non-rotated target path materializes a 3D parent offset for
        // each corner. Its X operand order differs for the right corners.
        volatile float topLeftX =
            parent->secondaryPosition.x + parent->position.x;
        topLeftX = parent->alternatePosition.x + topLeftX;
        topLeft[0] = topLeft[0] + topLeftX;
        volatile float topLeftY =
            parent->secondaryPosition.y + parent->position.y;
        topLeftY = parent->alternatePosition.y + topLeftY;
        topLeft[1] = topLeft[1] + topLeftY;
        volatile float topLeftZ =
            parent->secondaryPosition.z + parent->position.z;
        topLeftZ = parent->alternatePosition.z + topLeftZ;
        topLeft[2] = topLeft[2] + topLeftZ;

        volatile float topRightX =
            parent->position.x + parent->secondaryPosition.x;
        topRightX = parent->alternatePosition.x + topRightX;
        topRight[0] = topRight[0] + topRightX;
        volatile float topRightY =
            parent->secondaryPosition.y + parent->position.y;
        topRightY = parent->alternatePosition.y + topRightY;
        topRight[1] = topRight[1] + topRightY;
        volatile float topRightZ =
            parent->secondaryPosition.z + parent->position.z;
        topRightZ = parent->alternatePosition.z + topRightZ;
        topRight[2] = topRight[2] + topRightZ;

        volatile float bottomLeftX =
            parent->secondaryPosition.x + parent->position.x;
        bottomLeftX = parent->alternatePosition.x + bottomLeftX;
        bottomLeft[0] = bottomLeft[0] + bottomLeftX;
        volatile float bottomLeftY =
            parent->secondaryPosition.y + parent->position.y;
        bottomLeftY = parent->alternatePosition.y + bottomLeftY;
        bottomLeft[1] = bottomLeft[1] + bottomLeftY;
        volatile float bottomLeftZ =
            parent->secondaryPosition.z + parent->position.z;
        bottomLeftZ = parent->alternatePosition.z + bottomLeftZ;
        bottomLeft[2] = bottomLeft[2] + bottomLeftZ;

        volatile float bottomRightX =
            parent->position.x + parent->secondaryPosition.x;
        bottomRightX = parent->alternatePosition.x + bottomRightX;
        bottomRight[0] = bottomRight[0] + bottomRightX;
        volatile float bottomRightY =
            parent->secondaryPosition.y + parent->position.y;
        bottomRightY = parent->alternatePosition.y + bottomRightY;
        bottomRight[1] = bottomRight[1] + bottomRightY;
        volatile float bottomRightZ =
            parent->secondaryPosition.z + parent->position.z;
        bottomRightZ = parent->alternatePosition.z + bottomRightZ;
        bottomRight[2] = bottomRight[2] + bottomRightZ;
    }

    const float centerZ =
        vm->position.z + vm->secondaryPosition.z + vm->alternatePosition.z;
    topLeft[2] = centerZ;
    topRight[2] = centerZ;
    bottomLeft[2] = centerZ;
    bottomRight[2] = centerZ;
}

void BuildAnmVmViewCornersMode1(
    const AnmVm *vm,
    float *topLeft,
    float *topRight,
    float *bottomLeft,
    float *bottomRight)
{
    const AnmVm *parent = vm->parentVm;
    const bool inheritParent = parent != 0 && (vm->stateFlags & 0x100U) == 0;

    float angle = vm->unknown02C;
    if (inheritParent)
    {
        angle += parent->unknown02C;
    }
    const float cosine = (float)cos((double)angle);
    const float sine = (float)sin((double)angle);

    float width = vm->uvScrollX * vm->scaleX * vm->scale2X;
    float height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
    if (inheritParent)
    {
        width = parent->scale2X * parent->scaleX * width;
        height = parent->scale2Y * parent->scaleY * height;
    }

    float originX =
        vm->position.x + vm->secondaryPosition.x + vm->alternatePosition.x;
    float originY =
        vm->position.y + vm->secondaryPosition.y + vm->alternatePosition.y;
    if (inheritParent)
    {
        originX += parent->position.x +
            parent->secondaryPosition.x + parent->alternatePosition.x;
        originY += parent->position.y +
            parent->secondaryPosition.y + parent->alternatePosition.y;
    }

    const float centerZ =
        vm->position.z + vm->secondaryPosition.z + vm->alternatePosition.z;

    float left = 0.0f;
    float right = 0.0f;
    switch ((vm->flags >> 20) & 3U)
    {
    case 0:
        left = -width * 0.5f;
        right = width * 0.5f;
        break;
    case 1:
        right = width;
        break;
    case 2:
        left = -width;
        break;
    }

    float top = 0.0f;
    float bottom = 0.0f;
    switch ((vm->flags >> 22) & 3U)
    {
    case 0:
        top = -height * 0.5f;
        bottom = height * 0.5f;
        break;
    case 1:
        bottom = height;
        break;
    case 2:
        top = -height;
        break;
    }

    topLeft[0] = originX + cosine * left - sine * top;
    topLeft[1] = originY + cosine * top + sine * left;
    topRight[0] = originX + cosine * right - sine * top;
    topRight[1] = originY + cosine * top + sine * right;
    bottomLeft[0] = originX + cosine * left - sine * bottom;
    bottomLeft[1] = originY + cosine * bottom + sine * left;
    bottomRight[0] = originX + cosine * right - sine * bottom;
    bottomRight[1] = originY + cosine * bottom + sine * right;

    topLeft[2] = centerZ;
    topRight[2] = centerZ;
    bottomLeft[2] = centerZ;
    bottomRight[2] = centerZ;
}

void BuildAnmVmViewCorners(const AnmVm *vm, float *corners)
{
    float *topLeft = corners;
    float *topRight = corners + 3;
    float *bottomLeft = corners + 6;
    float *bottomRight = corners + 9;

    switch ((vm->flags >> 24) & 0x1FU)
    {
    case 0:
        BuildAnmVmViewCornersMode0(vm, topLeft, topRight, bottomLeft, bottomRight);
        break;
    case 1:
        BuildAnmVmViewCornersMode1(vm, topLeft, topRight, bottomLeft, bottomRight);
        break;
    case 2:
    case 3:
        BuildAnmVmViewCornersMode23(vm, topLeft, topRight, bottomLeft, bottomRight);
        break;
    }
}
