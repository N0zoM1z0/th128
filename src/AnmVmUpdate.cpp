#include "AnmVm.hpp"

namespace
{
    bool IsScriptUpdateSuppressed(const AnmVm *vm)
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
}

// Main per-VM update entry at 0x00459D90. The script interpreter and
// transition stages are reconstructed here; renderer-specific work remains
// in the original large routine's still-unrecovered branches.
void AnmVm::Update()
{
    if (scriptDataOffsetCopy == 0 || (flags & 0x00080000U) != 0)
    {
        return;
    }

    float *gameSpeed = reinterpret_cast<float *>(0x004B6F58);
    const float savedGameSpeed = *gameSpeed;
    if ((stateFlags & 1U) != 0)
    {
        *gameSpeed = 1.0f;
    }

    if (IsScriptUpdateSuppressed(this))
    {
        *gameSpeed = savedGameSpeed;
        return;
    }

    ExecuteReadyInstructions();
    UpdateTransitions();

    if (frameCallback != 0 && frameCallback(this) != 0)
    {
        *gameSpeed = savedGameSpeed;
        return;
    }

    timer070.Increment();
    *gameSpeed = savedGameSpeed;
}
