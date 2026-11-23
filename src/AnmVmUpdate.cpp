#include "AnmVm.hpp"
#include "AngleMath.hpp"

namespace
{
    bool IsFrameUpdateSuppressedByStage(const AnmVm *vm)
    {
        if (vm->currentFrame != 0 || (vm->stateFlags & 0xC0U) != 0x40U)
        {
            return false;
        }

        const void *stageState =
            *reinterpret_cast<void *const *>(0x004B8954);
        if (stageState == 0)
        {
            return false;
        }

        return (*reinterpret_cast<const unsigned char *>(
            reinterpret_cast<const unsigned char *>(stageState) + 0x60) & 2U) != 0;
    }

    void UpdateContinuousMotion(AnmVm *vm, float gameSpeed)
    {
        if (vm->scriptVector.x != 0.0f)
        {
            vm->unknown024 = AddAngles(
                vm->unknown024, gameSpeed * vm->scriptVector.x);
            vm->flags |= 4U;
        }

        if (vm->scriptVector.y != 0.0f)
        {
            vm->unknown028 = AddAngles(
                vm->unknown028, gameSpeed * vm->scriptVector.y);
            vm->flags |= 4U;
        }

        if (vm->scriptVector.z != 0.0f)
        {
            vm->unknown02C = AddAngles(
                vm->unknown02C, gameSpeed * vm->scriptVector.z);
            vm->flags |= 4U;
        }

        if (vm->unknown054 != 0.0f)
        {
            vm->scaleY += gameSpeed * vm->unknown054;
            vm->flags |= 8U;
        }

        if (vm->unknown050 != 0.0f)
        {
            vm->scaleX += vm->unknown050 * gameSpeed;
            vm->flags |= 0x0CU;
        }

        float scroll = vm->textureScrollX + gameSpeed * vm->textureOffsetX;
        if (scroll >= 1.0f)
        {
            scroll -= 1.0f;
        }
        else if (scroll < 0.0f)
        {
            scroll += 1.0f;
        }
        vm->textureScrollX = scroll;

        scroll = vm->textureScrollY + vm->textureOffsetY * gameSpeed;
        if (scroll >= 1.0f)
        {
            scroll -= 1.0f;
        }
        else if (scroll < 0.0f)
        {
            scroll += 1.0f;
        }
        vm->textureScrollY = scroll;

        if ((vm->flags & 0x4000U) != 0)
        {
            const float *stageOffset =
                reinterpret_cast<const float *>(0x004D338C);
            vm->position.x += stageOffset[0];
            vm->position.y += stageOffset[1];
            vm->position.z += stageOffset[2];
        }
    }
}

// Partial reconstruction of the 13,604-byte per-VM update owner at
// 0x00459D90. This source mapping is not an exact match: the remaining target
// branches, ordering and side effects still require full control-flow review.
int AnmVm::Update()
{
    if (scriptDataOffsetCopy == 0)
    {
        return 1;
    }
    if ((flags & 0x00080000U) != 0)
    {
        return 0;
    }

    float *gameSpeed = reinterpret_cast<float *>(0x004B6F58);
    const float savedGameSpeed = *gameSpeed;
    if ((stateFlags & 1U) != 0)
    {
        *gameSpeed = 1.0f;
    }

    if (IsFrameUpdateSuppressedByStage(this))
    {
        *gameSpeed = savedGameSpeed;
        return 0;
    }

    ExecuteReadyInstructions();
    if (scriptDataOffsetCopy == 0)
    {
        *gameSpeed = savedGameSpeed;
        return 1;
    }

    UpdateContinuousMotion(this, *gameSpeed);
    if ((stateFlags & 0x20U) != 0)
    {
        UpdateTextureRectFromView();
    }

    UpdateTransitions();
    UpdateCustomGeometry();

    if (frameCallback != 0 && frameCallback(this) != 0)
    {
        return 1;
    }

    timer070.Increment();
    *gameSpeed = savedGameSpeed;
    return 0;
}
