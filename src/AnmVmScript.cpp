#include "AnmVm.hpp"
#include "AnmVmGeometry.hpp"
#include "AnmVmView.hpp"
#include "AnmResource.hpp"
#include "AnmManager.hpp"
#include "AngleMath.hpp"
#include "Rng.hpp"

#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace
{
    typedef void (__stdcall *AnmVmChildCriticalSectionRoutine)(void *);

    class AnmVmChildCreationLock
    {
    public:
        AnmVmChildCreationLock()
        {
            if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
            {
                AnmVmChildCriticalSectionRoutine enter =
                    *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A088);
                enter(reinterpret_cast<void *>(0x004D3750));
                ++*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
            }
        }

        ~AnmVmChildCreationLock()
        {
            // The original child-creation routines re-read the global flag
            // at the exit edge rather than remembering the entry decision.
            if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
            {
                AnmVmChildCriticalSectionRoutine leave =
                    *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A08C);
                leave(reinterpret_cast<void *>(0x004D3750));
                --*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
            }
        }
    };

    unsigned int NextGlobalAnmRandomValue()
    {
        return reinterpret_cast<Rng *>(0x004D2AE0)->GetU32();
    }

    __forceinline const unsigned char *InstructionBytes(const AnmVmInstructionHeader *instruction)
    {
        return reinterpret_cast<const unsigned char *>(instruction);
    }

    __forceinline int ReadInstructionInteger(const AnmVmInstructionHeader *instruction, unsigned int offset)
    {
        return *reinterpret_cast<const int *>(InstructionBytes(instruction) + offset);
    }

    __forceinline unsigned short ReadInstructionUnsignedShort(
        const AnmVmInstructionHeader *instruction,
        unsigned int offset)
    {
        return *reinterpret_cast<const unsigned short *>(InstructionBytes(instruction) + offset);
    }

    __forceinline float ReadInstructionFloat(const AnmVmInstructionHeader *instruction, unsigned int offset)
    {
        return *reinterpret_cast<const float *>(InstructionBytes(instruction) + offset);
    }

    __forceinline int *IntegerDestination(AnmVm &vm, const AnmVmInstructionHeader *instruction)
    {
        int *destination = reinterpret_cast<int *>(
            const_cast<unsigned char *>(InstructionBytes(instruction) + 8));
        if ((instruction->flags & 1U) != 0)
        {
            destination = vm.ResolveIntegerOperandAddress(destination);
        }
        return destination;
    }

    __forceinline float *FloatDestination(AnmVm &vm, const AnmVmInstructionHeader *instruction)
    {
        float *destination = reinterpret_cast<float *>(
            const_cast<unsigned char *>(InstructionBytes(instruction) + 8));
        if ((instruction->flags & 1U) != 0)
        {
            destination = vm.ResolveFloatOperandAddress(destination);
        }
        return destination;
    }

    __forceinline unsigned short InstructionOperandFlags(
        const AnmVmInstructionHeader *instruction)
    {
        return ReadInstructionUnsignedShort(instruction, 6);
    }

    __forceinline int IntegerSource(
        AnmVm &vm,
        const AnmVmInstructionHeader *instruction,
        unsigned int offset,
        unsigned short flag)
    {
        if ((InstructionOperandFlags(instruction) & flag) != 0)
        {
            return vm.ResolveIntegerOperand(ReadInstructionInteger(instruction, offset));
        }
        return ReadInstructionInteger(instruction, offset);
    }

    __forceinline float FloatSource(
        AnmVm &vm,
        const AnmVmInstructionHeader *instruction,
        unsigned int offset,
        unsigned short flag)
    {
        if ((InstructionOperandFlags(instruction) & flag) != 0)
        {
            return vm.ResolveFloatOperand(ReadInstructionFloat(instruction, offset));
        }
        return ReadInstructionFloat(instruction, offset);
    }

    __forceinline unsigned int NextInstructionAddress(const AnmVmInstructionHeader *instruction)
    {
        return (unsigned int)(InstructionBytes(instruction) + instruction->size);
    }



    __declspec(noinline) unsigned int *__stdcall SpawnChildVm(
        unsigned int *childId,
        int scriptIndex,
        AnmVm *parent,
        unsigned int listMode)
    {
        AnmVmChildCreationLock lock;
        AnmResource *resource = parent->resource;
        ++resource->activeVmCount;

        AnmVm *child = AllocateAnmVm();
        child->stateFlags |= 2U;
        child->position.x = 0.0f;
        child->position.y = 0.0f;
        child->position.z = 0.0f;
        child->scriptState = parent->scriptState;
        resource->BindVmScript(child, scriptIndex);
        child->parentVm = parent;

        AnmManager *manager = GetGlobalAnmManager();
        if ((listMode & 4U) != 0)
        {
            *childId = (listMode & 2U) != 0
                ? manager->RegisterUiVmAtHead(child)
                : manager->RegisterUiVmAtTail(child);
        }
        else
        {
            *childId = (listMode & 2U) != 0
                ? manager->RegisterWorldVmAtHead(child)
                : manager->RegisterWorldVmAtTail(child);
        }

        AnmVmListNode *head = &parent->childNode;
        AnmVmListNode *node = &child->childNode;
        node->previous = head;
        node->next = head->next;
        if (head->next != 0)
        {
            head->next->previous = node;
        }
        head->next = node;
        return childId;
    }

    __declspec(noinline) unsigned int *__stdcall SpawnChildVmAtPosition(
        unsigned int *childId,
        int scriptIndex,
        AnmVm *parent)
    {
        AnmVmChildCreationLock lock;
        AnmResource *resource = parent->resource;
        ++resource->activeVmCount;

        AnmVm *child = AllocateAnmVm();
        child->stateFlags |= 2U;
        child->scriptState = parent->scriptState;
        child->position = parent->position;
        resource->BindVmScript(child, scriptIndex);
        child->unknown024 = parent->unknown024;
        child->unknown028 = parent->unknown028;
        child->unknown02C = parent->unknown02C;
        child->alternatePosition = parent->secondaryPosition;

        AnmManager *manager = GetGlobalAnmManager();
        *childId = manager->RegisterWorldVmAtTail(child);
        return childId;
    }

    __declspec(noinline) AnmVm *ResolveAnmVmId(unsigned int *id)
    {
        AnmManager *manager = GetGlobalAnmManager();
        AnmVm *vm = manager->GetVmById(*id);
        if (vm == 0)
        {
            *id = 0;
        }
        return vm;
    }

    __forceinline AnmResource *GetDefaultAnmResource()
    {
        void *globalState = *reinterpret_cast<void **>(0x004B8920);
        if (globalState == 0)
        {
            return 0;
        }

        return *reinterpret_cast<AnmResource **>(
            reinterpret_cast<unsigned char *>(globalState) + 0x1C894);
    }
}

__forceinline void AnmVm::UpdateTextureRectFromView()
{
    float corners[12];
    BuildAnmVmViewCorners(this, corners);
    float *textureCoordinates = &textureRect[0][0];
    NormalizeAnmCoordinates(textureCoordinates, corners);
    NormalizeAnmCoordinates(textureCoordinates + 2, corners + 3);
    NormalizeAnmCoordinates(textureCoordinates + 4, corners + 6);
    NormalizeAnmCoordinates(textureCoordinates + 6, corners + 9);
}

__forceinline bool AnmVm::ExecuteWaitCommand(bool clearActiveFlag)
{
    if (clearActiveFlag)
    {
        flags &= ~1U;
    }

    if (currentFrame == 0)
    {
        flags |= 0x2000U;
        timer070.Add(-1.0f);
        return false;
    }

    const AnmVmInstructionHeader *match = 0;
    const AnmVmInstructionHeader *fallback = 0;
    const AnmVmInstructionHeader *marker =
        reinterpret_cast<const AnmVmInstructionHeader *>(scriptDataOffset);

    while (marker->opcode != 0xFFFFU)
    {
        if (marker->opcode == 0x40U)
        {
            // Opcode 0x40's first dword operand selects the wait target.
            // The header frame is a separate scheduled-frame value restored
            // into timer070 after the marker is selected.
            const int targetFrame = ReadInstructionInteger(marker, 8);
            if (targetFrame == currentFrame)
            {
                match = marker;
                break;
            }
            if (targetFrame == -1)
            {
                fallback = marker;
            }
        }

        marker = reinterpret_cast<const AnmVmInstructionHeader *>(
            InstructionBytes(marker) + marker->size);
    }

    if (match == 0)
    {
        match = fallback;
    }

    flags &= ~0x2000U;
    currentFrame = 0;
    if (match == 0)
    {
        timer070.Add(-1.0f);
        return false;
    }

    // Save the active script timer before moving it to the wait marker's
    // frame. The target snapshots timer070.current into timer3FC here.
    timer3FC.SetCurrent(timer070);
    waitScriptAddress = scriptDataOffsetCopy;
    timer070.SetCurrent(match->frame);
    flags |= 1U;
    scriptDataOffsetCopy = NextInstructionAddress(match);
    return true;
}


namespace
{
    // Modes 1..16 mirror the target's 16-entry easing jump table at 0x0046B428.
    float EaseTransition(unsigned int mode, float t)
    {
        const float inverse = 1.0f - t;
        switch (mode)
        {
        case 1: return t * t;
        case 2: return t * t * t;
        case 3: return t * t * t * t;
        case 4: return 1.0f - inverse * inverse;
        case 5: return 1.0f - inverse * inverse * inverse;
        case 6: return 1.0f - inverse * inverse * inverse * inverse;
        case 7:
        case 8:
            return t;
        case 9:
            return t < 0.5f
                ? 2.0f * t * t
                : 1.0f - 2.0f * inverse * inverse;
        case 10:
            return t < 0.5f
                ? 4.0f * t * t * t
                : 1.0f - 4.0f * inverse * inverse * inverse;
        case 11:
            return t < 0.5f
                ? 8.0f * t * t * t * t
                : 1.0f - 8.0f * inverse * inverse * inverse * inverse;
        case 12:
        {
            const float x = t * 2.0f - 1.0f;
            return t < 0.5f
                ? 0.5f - 0.5f * (1.0f - 2.0f * t) * (1.0f - 2.0f * t)
                : 0.5f + 0.5f * x * x;
        }
        case 13:
        {
            const float x = t * 2.0f - 1.0f;
            return t < 0.5f
                ? 0.5f - 0.5f * (1.0f - 2.0f * t) *
                    (1.0f - 2.0f * t) * (1.0f - 2.0f * t)
                : 0.5f + 0.5f * x * x * x;
        }
        case 14:
        {
            const float x = t * 2.0f - 1.0f;
            return t < 0.5f
                ? 0.5f - 0.5f * (1.0f - 2.0f * t) *
                    (1.0f - 2.0f * t) * (1.0f - 2.0f * t) * (1.0f - 2.0f * t)
                : 0.5f + 0.5f * x * x * x * x;
        }
        case 15: return 0.0f;
        case 16: return 1.0f;
        default: return t;
        }
    }

    __forceinline bool AdvanceTransition(
        ZunTimer *timer,
        int *duration,
        unsigned int mode)
    {
        if (*duration <= 0)
        {
            return false;
        }

        timer->Increment();
        if (timer->current < *duration)
        {
            return false;
        }

        timer->Initialize();
        timer->previous = *duration - 1;
        timer->current = *duration;
        timer->currentF = (float)*duration;
        *duration = 0;
        return true;
    }

    void InterpolateColorVector(AnmVmColorTransition *transition, int result[3])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            const unsigned int endpoint = (mode == 7) ? 0 : 3;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            result[2] = transition->values[endpoint + 2];
            return;
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[3];
            transition->values[1] += transition->values[4];
            transition->values[2] += transition->values[5];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            result[2] = transition->values[2];
            return;
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[9];
            transition->values[1] += transition->values[10];
            transition->values[2] += transition->values[11];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            result[2] = transition->values[2];
            transition->values[9] += transition->values[3];
            transition->values[10] += transition->values[4];
            transition->values[11] += transition->values[5];
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float h00 = (float)((t + t + 1.0) * tMinusOne * tMinusOne);
            const float h01 = (float)(t * t * (3.0 - (t + t)));
            const float h10 = (float)((1.0 - t) * (1.0 - t) * t);
            const float h11 = (float)(tMinusOne * t * t);
            result[0] =
                ZunRoundToInt((double)h00 * transition->values[0]) +
                ZunRoundToInt((double)h01 * transition->values[3]) +
                ZunRoundToInt((double)h10 * transition->values[6]) +
                ZunRoundToInt((double)h11 * transition->values[9]);
            result[1] =
                ZunRoundToInt((double)h00 * transition->values[1]) +
                ZunRoundToInt((double)h01 * transition->values[4]) +
                ZunRoundToInt((double)h10 * transition->values[7]) +
                ZunRoundToInt((double)h11 * transition->values[10]);
            result[2] =
                ZunRoundToInt((double)h00 * transition->values[2]) +
                ZunRoundToInt((double)h01 * transition->values[5]) +
                ZunRoundToInt((double)h10 * transition->values[8]) +
                ZunRoundToInt((double)h11 * transition->values[11]);
            return;
        }

        const float eased = EaseTransition(mode, (float)t);
        const int delta0 = transition->values[3] - transition->values[0];
        const int delta1 = transition->values[4] - transition->values[1];
        const int delta2 = transition->values[5] - transition->values[2];
        result[0] = transition->values[0] + ZunRoundToInt((double)delta0 * eased);
        result[1] = transition->values[1] + ZunRoundToInt((double)delta1 * eased);
        result[2] = transition->values[2] + ZunRoundToInt((double)delta2 * eased);
    }


    int InterpolateInteger(AnmVmIntegerTransition *transition)
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            return transition->values[(mode == 7) ? 0 : 1];
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[1];
            return transition->values[0];
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[3];
            transition->values[3] += transition->values[1];
            return transition->values[0];
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float h00 = (float)((t + t + 1.0) * tMinusOne * tMinusOne);
            const float h01 = (float)(t * t * (3.0 - (t + t)));
            const float h10 = (float)((1.0 - t) * (1.0 - t) * t);
            const float h11 = (float)(tMinusOne * t * t);
            const double value =
                (double)h00 * transition->values[0] +
                (double)h01 * transition->values[1] +
                (double)h10 * transition->values[2] +
                (double)h11 * transition->values[3];
            return ZunRoundToInt(value);
        }

        const float eased = EaseTransition(mode, (float)t);
        const int delta = transition->values[1] - transition->values[0];
        return ZunRoundToInt((double)delta * eased + transition->values[0]);
    }

    // The target's three-component vector helper uses mode 7 for relative updates.
    void InterpolateVector(AnmVmVectorTransition *transition, float result[3])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            const unsigned int endpoint = (mode == 7) ? 0 : 3;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            result[2] = transition->values[endpoint + 2];
            return;
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[3];
            transition->values[1] += transition->values[4];
            transition->values[2] += transition->values[5];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            result[2] = transition->values[2];
            return;
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[9];
            transition->values[1] += transition->values[10];
            transition->values[2] += transition->values[11];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            result[2] = transition->values[2];
            transition->values[9] += transition->values[3];
            transition->values[10] += transition->values[4];
            transition->values[11] += transition->values[5];
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float h00 = (float)((t + t + 1.0) * tMinusOne * tMinusOne);
            const float h01 = (float)(t * t * (3.0 - (t + t)));
            const float h10 = (float)((1.0 - t) * (1.0 - t) * t);
            const float h11 = (float)(tMinusOne * t * t);
            result[0] = h00 * transition->values[0] +
                h01 * transition->values[3] +
                h10 * transition->values[6] +
                h11 * transition->values[9];
            result[1] = h00 * transition->values[1] +
                h01 * transition->values[4] +
                h10 * transition->values[7] +
                h11 * transition->values[10];
            result[2] = h00 * transition->values[2] +
                h01 * transition->values[5] +
                h10 * transition->values[8] +
                h11 * transition->values[11];
            return;
        }

        const float eased = EaseTransition(mode, (float)t);
        result[0] = eased *
            (transition->values[3] - transition->values[0]) + transition->values[0];
        result[1] = eased *
            (transition->values[4] - transition->values[1]) + transition->values[1];
        result[2] = eased *
            (transition->values[5] - transition->values[2]) + transition->values[2];
    }


    void InterpolateScale(AnmVmScaleTransition *transition, float result[2])
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            // The target's two-component scale helper also uses mode 7 for
            // its relative endpoint.
            const unsigned int endpoint = (mode == 7) ? 0 : 2;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            return;
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[2];
            transition->values[1] += transition->values[3];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            return;
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[6];
            transition->values[1] += transition->values[7];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            transition->values[6] += transition->values[2];
            transition->values[7] += transition->values[3];
            return;
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float h00 = (float)((t + t + 1.0) * tMinusOne * tMinusOne);
            const float h01 = (float)(t * t * (3.0 - (t + t)));
            const float h10 = (float)((1.0 - t) * (1.0 - t) * t);
            const float h11 = (float)(tMinusOne * t * t);
            result[0] = h00 * transition->values[0] +
                h01 * transition->values[2] +
                h10 * transition->values[4] +
                h11 * transition->values[6];
            result[1] = h00 * transition->values[1] +
                h01 * transition->values[3] +
                h10 * transition->values[5] +
                h11 * transition->values[7];
            return;
        }

        const float eased = EaseTransition(mode, (float)t);
        result[0] = eased *
            (transition->values[2] - transition->values[0]) + transition->values[0];
        result[1] = eased *
            (transition->values[3] - transition->values[1]) + transition->values[1];
    }


    // The view rotation is an angular scalar: relative and eased paths wrap,
    // and ordinary interpolation follows the shortest signed arc.
    float InterpolateAngle(AnmVmScalarTransition *transition)
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            return transition->values[(mode == 7) ? 0 : 1];
        }

        if (mode == 7)
        {
            transition->values[0] = NormalizeAngle(
                transition->values[0] + transition->values[1]);
            return transition->values[0];
        }

        if (mode == 17)
        {
            transition->values[0] = NormalizeAngle(
                transition->values[0] + transition->values[3]);
            transition->values[3] = NormalizeAngle(
                transition->values[3] + transition->values[1]);
            return transition->values[0];
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float targetTerm = NormalizeAngle(
                (float)(t * t * (3.0 - (t + t)) * transition->values[1]));
            const float startTerm = NormalizeAngle(
                (float)(tMinusOne * tMinusOne * (t + t + 1.0) *
                transition->values[0]));
            float result = NormalizeAngle(startTerm + targetTerm);
            const float startTangentTerm = NormalizeAngle(
                (float)((1.0 - t) * (1.0 - t) * t * transition->values[2]));
            result = NormalizeAngle(startTangentTerm + result);
            const float targetTangentTerm = NormalizeAngle(
                (float)(tMinusOne * t * t * transition->values[3]));
            return NormalizeAngle(targetTangentTerm + result);
        }

        const float eased = EaseTransition(mode, (float)t);
        const float difference = ShortestAngleDifference(
            transition->values[1], transition->values[0]);
        const float normalizedDifference = NormalizeAngle(difference);
        const float scaledDifference = NormalizeAngle(
            normalizedDifference * eased);
        return NormalizeAngle(scaledDifference + transition->values[0]);
    }

    // Texture scroll offsets use the ordinary scalar helper at 0x0040CCF0.
    // Its Hermite terms are accumulated in the target's evaluation order,
    // without angle normalization or shortest-arc interpolation.
    float InterpolateLinearScalar(AnmVmScalarTransition *transition)
    {
        const unsigned int mode = transition->mode;
        if (AdvanceTransition(&transition->timer, &transition->duration, mode))
        {
            return transition->values[(mode == 7) ? 0 : 1];
        }

        if (mode == 7)
        {
            transition->values[0] += transition->values[1];
            return transition->values[0];
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[3];
            transition->values[3] += transition->values[1];
            return transition->values[0];
        }

        const double t = (double)transition->timer.currentF /
            (double)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = t - 1.0;
            const float targetTerm =
                (float)(t * t * (3.0 - (t + t)) * transition->values[1]);
            const float startTerm =
                (float)(tMinusOne * tMinusOne * (t + t + 1.0) *
                transition->values[0]);
            float result = startTerm + targetTerm;
            const float startTangentTerm =
                (float)((1.0 - t) * (1.0 - t) * t * transition->values[2]);
            result += startTangentTerm;
            const float targetTangentTerm =
                (float)(tMinusOne * t * t * transition->values[3]);
            return result + targetTangentTerm;
        }

        const float eased = EaseTransition(mode, (float)t);
        return (transition->values[1] - transition->values[0]) * eased +
            transition->values[0];
    }

}



namespace
{
    // The target divides its binary64 2*pi constant by a float-rounded
    // segment count, then stores the quotient as float.
    const double kAnmVmGeometryTwoPi = 6.28318530717958647692;


}




// Keep the frame owner in this translation unit with the script interpreter.
namespace
{



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

    if (currentFrame != 0)
    {
        goto AnmVmUpdateWaitScan;
    }

    if ((stateFlags & 0xC0U) == 0x40U)
    {
        const void *stageState =
            *reinterpret_cast<void *const *>(0x004B8954);
        if (stageState != 0 &&
            (*reinterpret_cast<const unsigned char *>(
                reinterpret_cast<const unsigned char *>(stageState) + 0x60) & 2U) != 0)
        {
            // The target branches directly to 0x0045D0BA, past the game-speed
            // restore at 0x0045D0B0. Preserve the temporary speed on this edge.
            return 0;
        }
    }

    register const AnmVmInstructionHeader *instruction;
AnmVmUpdateScriptLoop:
    for (;;)
    {
            instruction =
                reinterpret_cast<const AnmVmInstructionHeader *>(scriptDataOffsetCopy);
            if (instruction->frame > timer070.current)
            {
                break;
            }

    int *integerDestination;
    float *floatDestination;
    int integerValue;
    float floatValue;

    const short opcode = static_cast<short>(instruction->opcode);
    switch (opcode)
    {
    case 3:
    {
        flags |= 1U;
        int fileIndex = ReadInstructionInteger(instruction, 8);
        if (anmFileResolver == 0)
        {
            if ((instruction->flags & 1U) != 0)
            {
                fileIndex = ResolveIntegerOperand(fileIndex);
            }
        }
        else
        {
            // The target consumes an indirect operand before invoking the
            // resolver callback, even though the callback supplies the index.
            if ((instruction->flags & 1U) != 0)
            {
                ResolveIntegerOperand(fileIndex);
            }
            fileIndex = anmFileResolver(this);
        }

        AnmResource *fileResource = resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }
        BindAnmFile(fileIndex, fileResource);
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 4:
        timer070.SetCurrent(ReadInstructionInteger(instruction, 12));
        scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 8);
        continue;

    case 5:
        integerDestination = IntegerDestination(*this, instruction);
        --(*integerDestination);
        integerValue = IntegerSource(*this, instruction, 8, 1U);
        if (integerValue > 0)
        {
            timer070.SetCurrent(ReadInstructionInteger(instruction, 16));
            scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 12);
        }
        else
        {
            scriptDataOffsetCopy = NextInstructionAddress(instruction);
        }
        continue;

    case 6:
        *IntegerDestination(*this, instruction) =
            IntegerSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 7:
        *FloatDestination(*this, instruction) =
            FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 8:
    {
        const int operand = IntegerSource(*this, instruction, 12, 2U);
        integerDestination = IntegerDestination(*this, instruction);
        *integerDestination += operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 9:
    {
        const float operand = FloatSource(*this, instruction, 12, 2U);
        floatDestination = FloatDestination(*this, instruction);
        *floatDestination += operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 10:
    {
        const int operand = IntegerSource(*this, instruction, 12, 2U);
        integerDestination = IntegerDestination(*this, instruction);
        *integerDestination -= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 11:
    {
        const float operand = FloatSource(*this, instruction, 12, 2U);
        floatDestination = FloatDestination(*this, instruction);
        *floatDestination -= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 12:
    {
        const int operand = IntegerSource(*this, instruction, 12, 2U);
        integerDestination = IntegerDestination(*this, instruction);
        *integerDestination *= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 13:
    {
        const float operand = FloatSource(*this, instruction, 12, 2U);
        floatDestination = FloatDestination(*this, instruction);
        *floatDestination *= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 14:
    {
        const int operand = IntegerSource(*this, instruction, 12, 2U);
        integerDestination = IntegerDestination(*this, instruction);
        *integerDestination /= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 15:
    {
        const float operand = FloatSource(*this, instruction, 12, 2U);
        floatDestination = FloatDestination(*this, instruction);
        *floatDestination /= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 16:
    {
        const int operand = IntegerSource(*this, instruction, 12, 2U);
        integerDestination = IntegerDestination(*this, instruction);
        *integerDestination %= operand;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 17:
    {
        const float right = FloatSource(*this, instruction, 12, 2U);
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float result = (float)fmod((double)left, (double)right);
        *FloatDestination(*this, instruction) = result;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 18:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left + right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 19:
        *FloatDestination(*this, instruction) =
            FloatSource(*this, instruction, 12, 2U) +
            FloatSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 20:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left - right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 21:
        *FloatDestination(*this, instruction) =
            FloatSource(*this, instruction, 12, 2U) -
            FloatSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 22:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left * right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 23:
        *FloatDestination(*this, instruction) =
            FloatSource(*this, instruction, 12, 2U) *
            FloatSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 24:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left / right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 25:
        *FloatDestination(*this, instruction) =
            FloatSource(*this, instruction, 12, 2U) /
            FloatSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 26:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left % right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 27:
        *FloatDestination(*this, instruction) =
            (float)fmod(
                (double)FloatSource(*this, instruction, 12, 2U),
                (double)FloatSource(*this, instruction, 16, 4U));
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 28:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left == right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 29:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left == right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 30:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left != right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 31:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left != right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 32:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left < right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 33:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left < right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 34:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left <= right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 35:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left <= right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 36:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left > right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 37:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left > right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 38:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left >= right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 39:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left >= right) goto AnmVmUpdateConditionalBranch;
        break;
    }

    case 40:
    {
        const unsigned int limit = (unsigned int)IntegerSource(*this, instruction, 12, 2U);
        const unsigned int value = (limit == 0) ? 0 : NextRandomValue() % limit;
        *IntegerDestination(*this, instruction) = (int)value;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 41:
    {
        const float limit = FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)(RandomUnit() * limit);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 42:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)sin(value);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 43:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)cos(value);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 44:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)tan(value);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 45:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)asin(value);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 46:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        *FloatDestination(*this, instruction) = (float)atan(value);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 47:
    {
        const float angle = FloatSource(*this, instruction, 8, 1U);
        *FloatDestination(*this, instruction) = NormalizeAngle(angle);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 48:
        if ((flags & 0x200U) == 0)
        {
            const float z = FloatSource(*this, instruction, 16, 4U);
            const float y = FloatSource(*this, instruction, 12, 2U);
            const float x = FloatSource(*this, instruction, 8, 1U);
            secondaryPosition.x = x;
            secondaryPosition.y = y;
            secondaryPosition.z = z;
        }
        else
        {
            const float z = FloatSource(*this, instruction, 16, 4U);
            const float y = FloatSource(*this, instruction, 12, 2U);
            const float x = FloatSource(*this, instruction, 8, 1U);
            alternatePosition.x = x;
            alternatePosition.y = y;
            alternatePosition.z = z;
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 49:
        unknown024 = FloatSource(*this, instruction, 8, 1U);
        unknown028 = FloatSource(*this, instruction, 12, 2U);
        unknown02C = FloatSource(*this, instruction, 16, 4U);
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 50:
        scaleX = FloatSource(*this, instruction, 8, 1U);
        scaleY = FloatSource(*this, instruction, 12, 2U);
        flags |= 8U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 51:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&color);
        colorBytes[3] = (unsigned char)IntegerSource(*this, instruction, 8, 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 52:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&color);
        colorBytes[2] = (unsigned char)IntegerSource(*this, instruction, 8, 1U);
        colorBytes[1] = (unsigned char)IntegerSource(*this, instruction, 12, 2U);
        colorBytes[0] = (unsigned char)IntegerSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 53:
        scriptVector.x = FloatSource(*this, instruction, 8, 1U);
        scriptVector.y = FloatSource(*this, instruction, 12, 2U);
        scriptVector.z = FloatSource(*this, instruction, 16, 4U);
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 54:
        unknown050 = FloatSource(*this, instruction, 8, 1U);
        unknown054 = FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 70:
        textureOffsetX = FloatSource(*this, instruction, 8, 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 71:
        textureOffsetY = FloatSource(*this, instruction, 8, 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 72:
        flags = (flags & ~0x00000001U) |
            ((unsigned int)ReadInstructionInteger(instruction, 8) & 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 73:
        flags = (flags & ~0x00001000U) |
            (((unsigned int)ReadInstructionInteger(instruction, 8) << 12) & 0x00001000U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 74:
        flags = (flags & ~0x00004000U) |
            (((unsigned int)ReadInstructionInteger(instruction, 8) << 14) & 0x00004000U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 80:
        flags = (flags & ~0x00030000U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 16) & 0x00030000U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 82:
        flags = (flags & ~0x40000000U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 30) & 0x40000000U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 83:
    {
        const ZunVec3 oldPosition = position;
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        secondaryPosition = oldPosition;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 85:
        flags = (flags & 0x7FFFFFFFU) |
            ((unsigned int)InstructionBytes(instruction)[8] << 31);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 86:
        stateFlags = (stateFlags & ~1U) |
            ((unsigned int)IntegerSource(*this, instruction, 8, 1U) & 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 87:
        stateFlags = (stateFlags & ~2U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 1) & 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 89:
        stateFlags = (stateFlags & ~4U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 2) & 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 99:
        stateFlags = (stateFlags & ~0x20U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 5) & 0x20U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 55:
    {
        const unsigned int duration = (unsigned int)IntegerSource(*this, instruction, 12, 2U);
        const unsigned char parameter1 = InstructionBytes(instruction)[8];
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(duration, 0, alpha, parameter1);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 58:
    {
        const unsigned int duration = (unsigned int)IntegerSource(*this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        const unsigned char parameter1 = (unsigned char)IntegerSource(*this, instruction, 16, 4U);
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(duration, mode, alpha, parameter1);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 56:
    {
        viewMode = IntegerSource(*this, instruction, 8, 1U);
        viewFlags = ReadInstructionInteger(instruction, 12);

        viewReferenceA.x = *reinterpret_cast<const float *>(0x004D2E54);
        viewReferenceA.y = *reinterpret_cast<const float *>(0x004D2E58);
        viewReferenceA.z = *reinterpret_cast<const float *>(0x004D2E5C);
        viewReferenceB = viewReferenceA;

        if ((flags & 0x200U) == 0)
        {
            viewPosition = secondaryPosition;
        }
        else
        {
            viewPosition = alternatePosition;
        }

        viewTarget.x = FloatSource(*this, instruction, 16, 4U);
        viewTarget.y = FloatSource(*this, instruction, 20, 8U);
        viewTarget.z = FloatSource(*this, instruction, 24, 0x10U);
        timer0D4.Reset();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 57:
    {
        const unsigned char sourceBlue = (unsigned char)color;
        const unsigned char sourceGreen = (unsigned char)(color >> 8);
        const unsigned char sourceRed = (unsigned char)(color >> 16);
        const unsigned char targetAlpha =
            (unsigned char)IntegerSource(*this, instruction, 24, 0x10U);
        const unsigned char targetGreen =
            (unsigned char)IntegerSource(*this, instruction, 20, 8U);
        const unsigned char targetBlue =
            (unsigned char)IntegerSource(*this, instruction, 16, 4U);

        const unsigned char target[3] = {targetAlpha, targetGreen, targetBlue};
        const unsigned char current[3] = {sourceBlue, sourceGreen, sourceRed};
        SetColorTransition(
            target, current,
            IntegerSource(*this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 59:
    {
        const float target[3] = {
            FloatSource(*this, instruction, 16, 4U),
            FloatSource(*this, instruction, 20, 8U),
            FloatSource(*this, instruction, 24, 0x10U)
        };
        positionTransition.values[0] = unknown024;
        positionTransition.values[1] = unknown028;
        positionTransition.values[2] = unknown02C;
        positionTransition.values[3] = target[0];
        positionTransition.values[4] = target[1];
        positionTransition.values[5] = target[2];
        const float *reference = reinterpret_cast<const float *>(0x004D2E54);
        positionTransition.values[6] = reference[0];
        positionTransition.values[7] = reference[1];
        positionTransition.values[8] = reference[2];
        positionTransition.values[9] = reference[0];
        positionTransition.values[10] = reference[1];
        positionTransition.values[11] = reference[2];
        positionTransition.duration =
            IntegerSource(*this, instruction, 8, 1U);
        positionTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        positionTransition.timer.Reset();
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 60:
    {
        const float target[2] = {
            FloatSource(*this, instruction, 16, 4U),
            FloatSource(*this, instruction, 20, 8U)
        };
        const float current[2] = { scaleX, scaleY };
        const int duration = IntegerSource(*this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        SetScaleTransition(target, current, duration, mode);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 61:
        flags ^= 0x400U;
        flags |= 8U;
        scaleX *= -1.0f;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 62:
        flags ^= 0x800U;
        flags |= 8U;
        scaleY *= -1.0f;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 65:
    {
        flags = (flags & ~0x00300000U) |
            (((unsigned int)ReadInstructionUnsignedShort(instruction, 8) << 20) & 0x00300000U);
        flags = (flags & ~0x00C00000U) |
            (((unsigned int)ReadInstructionUnsignedShort(instruction, 10) << 22) & 0x00C00000U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 66:
        flags ^= (((unsigned int)ReadInstructionInteger(instruction, 8) << 5) ^ flags) & 0x000000E0U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 67:
        flags ^= (((unsigned int)ReadInstructionInteger(instruction, 8) << 24) ^ flags) & 0x1F000000U;
        if ((flags & 0x1F000000U) == 0x0A000000U)
        {
            StartEffect10();
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 63:
        if (currentFrame == 0)
        {
            goto AnmVmUpdateWaitNoFrame;
        }
        goto AnmVmUpdateWaitScan;

    case 68:
        scriptState = (int)InstructionBytes(instruction)[8];
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 69:
        flags &= ~1U;
        if (currentFrame == 0)
        {
            goto AnmVmUpdateWaitNoFrame;
        }
        goto AnmVmUpdateWaitScan;

    case 81:
        // Restore the saved active timer, then resume after the wait marker.
        timer070.SetCurrent(timer3FC);
        scriptDataOffsetCopy = waitScriptAddress;
        continue;

    case 75:
    {
        const int frameOffset = IntegerSource(*this, instruction, 8, 1U);
        timer070.Add((float)-frameOffset);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 76:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&secondaryColor);
        colorBytes[2] = (unsigned char)IntegerSource(*this, instruction, 8, 1U);
        colorBytes[1] = (unsigned char)IntegerSource(*this, instruction, 12, 2U);
        colorBytes[0] = (unsigned char)IntegerSource(*this, instruction, 16, 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 77:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&secondaryColor);
        colorBytes[3] = (unsigned char)IntegerSource(*this, instruction, 8, 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 78:
    {
        const unsigned char sourceBlue = (unsigned char)secondaryColor;
        const unsigned char sourceGreen = (unsigned char)(secondaryColor >> 8);
        const unsigned char sourceRed = (unsigned char)(secondaryColor >> 16);
        const unsigned char targetAlpha =
            (unsigned char)IntegerSource(*this, instruction, 24, 0x10U);
        const unsigned char targetGreen =
            (unsigned char)IntegerSource(*this, instruction, 20, 8U);
        const unsigned char targetBlue =
            (unsigned char)IntegerSource(*this, instruction, 16, 4U);

        const unsigned char target[3] = {targetAlpha, targetGreen, targetBlue};
        const unsigned char current[3] = {sourceBlue, sourceGreen, sourceRed};
        SetSecondaryColorTransition(
            target, current,
            IntegerSource(*this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 79:
    {
        const unsigned int duration = (unsigned int)IntegerSource(*this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        const unsigned char parameter1 = (unsigned char)IntegerSource(*this, instruction, 16, 4U);
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&secondaryColor)[3];
        SetSecondaryEffectState(duration, mode, alpha, parameter1);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 84:
    {
        flags = (flags & 0xE9FFFFFFU) | 0x09000000U;
        const int vertexCount = IntegerSource(*this, instruction, 8, 1U);
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 101:
    {
        flags = (flags & 0xEDFFFFFFU) | 0x0D000000U;
        const int vertexCount = IntegerSource(*this, instruction, 8, 1U);
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 88:
    {
        unsigned int childId;
        SpawnChildVm(&childId, IntegerSource(*this, instruction, 8, 1U), this, 0);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 90:
    {
        unsigned int childId;
        SpawnChildVm(&childId, IntegerSource(*this, instruction, 8, 1U), this, 4);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 91:
    {
        unsigned int childId;
        SpawnChildVm(&childId, IntegerSource(*this, instruction, 8, 1U), this, 2);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 92:
    {
        unsigned int childId;
        SpawnChildVm(&childId, IntegerSource(*this, instruction, 8, 1U), this, 6);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 95:
    {
        unsigned int childId;
        SpawnChildVmAtPosition(&childId, IntegerSource(*this, instruction, 8, 1U), this);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 96:
    {
        unsigned int childId;
        SpawnChildVm(&childId, IntegerSource(*this, instruction, 8, 1U), this, 0);
        AnmVm *child = ResolveAnmVmId(&childId);
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(*this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(*this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 97:
    {
        unsigned int childId;
        SpawnChildVmAtPosition(&childId, IntegerSource(*this, instruction, 8, 1U), this);
        AnmVm *child = ResolveAnmVmId(&childId);
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(*this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(*this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 98:
        UpdateTextureRectFromView();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 100:
    {
        const float referenceAX =
            FloatSource(*this, instruction, 12, 2U);
        const float referenceAY =
            FloatSource(*this, instruction, 16, 4U);
        const float referenceAZ =
            FloatSource(*this, instruction, 20, 8U);
        const float referenceBX =
            FloatSource(*this, instruction, 36, 0x80U);
        const float referenceBY =
            FloatSource(*this, instruction, 40, 0x100U);
        const float referenceBZ =
            FloatSource(*this, instruction, 44, 0x200U);
        const int duration = IntegerSource(*this, instruction, 8, 1U);

        viewMode = duration;
        viewReferenceA.x = referenceAX;
        viewReferenceA.y = referenceAY;
        viewReferenceA.z = referenceAZ;
        viewReferenceB.x = referenceBX;
        viewReferenceB.y = referenceBY;
        viewReferenceB.z = referenceBZ;
        viewFlags = 8;
        viewPosition = (flags & 0x200U) != 0
            ? alternatePosition
            : secondaryPosition;

        viewTarget.x = FloatSource(*this, instruction, 24, 0x10U);
        viewTarget.y = FloatSource(*this, instruction, 28, 0x20U);
        viewTarget.z = FloatSource(*this, instruction, 32, 0x40U);
        timer0D4.Reset();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 102:
    {
        flags |= 1U;
        int fileIndex;
        if (anmFileResolver == 0)
        {
            const int firstIndex = IntegerSource(*this, instruction, 8, 1U);
            const unsigned int variantCount =
                (unsigned int)IntegerSource(*this, instruction, 12, 2U);
            const unsigned int randomValue = NextGlobalAnmRandomValue();
            fileIndex = firstIndex + (int)(randomValue % variantCount);
        }
        else
        {
            // The target consumes both encoded operands and one value from
            // the shared global RNG before invoking the resolver callback.
            (void)IntegerSource(*this, instruction, 8, 1U);
            (void)IntegerSource(*this, instruction, 12, 2U);
            NextGlobalAnmRandomValue();
            fileIndex = anmFileResolver(this);
        }

        AnmResource *fileResource = resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }

        BindAnmFile(fileIndex, fileResource);
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 93:
    {
        textureOffsetXTransition.duration =
            IntegerSource(*this, instruction, 8, 1U);
        textureOffsetXTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        textureOffsetXTransition.values[0] = textureOffsetX;
        textureOffsetXTransition.values[1] =
            FloatSource(*this, instruction, 16, 4U);
        textureOffsetXTransition.values[2] = 0.0f;
        textureOffsetXTransition.values[3] = 0.0f;
        textureOffsetXTransition.timer.Reset();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 94:
    {
        textureOffsetYTransition.duration =
            IntegerSource(*this, instruction, 8, 1U);
        textureOffsetYTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        textureOffsetYTransition.values[0] = textureOffsetY;
        textureOffsetYTransition.values[1] =
            FloatSource(*this, instruction, 16, 4U);
        textureOffsetYTransition.values[2] = 0.0f;
        textureOffsetYTransition.values[3] = 0.0f;
        textureOffsetYTransition.timer.Reset();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 103:
        flags = (flags & 0xEFFFFFFFU) | 0x0F000000U;
        floatValue = ReadInstructionFloat(instruction, 8);
        if ((instruction->flags & 1U) != 0)
        {
            floatValue = ResolveFloatOperand(floatValue);
        }
        uvScrollX = floatValue;
        floatValue = ReadInstructionFloat(instruction, 12);
        if ((instruction->flags & 2U) != 0)
        {
            floatValue = ResolveFloatOperand(floatValue);
        }
        uvScrollY = floatValue;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 104:
        flags = (flags & 0xF0FFFFFFU) | 0x10000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        integerVariables[0] = IntegerSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 105:
        flags = (flags & 0xF1FFFFFFU) | 0x11000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        integerVariables[0] = IntegerSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 106:
        unknown058 = FloatSource(*this, instruction, 8, 1U);
        unknown05C = FloatSource(*this, instruction, 12, 2U);
        flags |= 0x10U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 107:
    {
        const float target[2] = {
            FloatSource(*this, instruction, 16, 4U),
            FloatSource(*this, instruction, 20, 8U)
        };
        const float current[2] = {unknown058, unknown05C};
        SetSecondaryScaleTransition(
            target,
            current,
            IntegerSource(*this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        flags |= 0x10U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 108:
        flags = (flags & 0xF3FFFFFFU) | 0x13000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        uvScrollY = FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 109:
        flags = (flags & 0xF4FFFFFFU) | 0x14000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        uvScrollY = FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 110:
        flags = (flags & 0xF5FFFFFFU) | 0x15000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        uvScrollY = FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 111:
        stateFlags = (stateFlags & ~0xC0U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 6) & 0xC0U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 112:
        stateFlags = (stateFlags & ~0x100U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 8) & 0x100U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 114:
    {
        flags = (flags & 0xEEFFFFFFU) | 0x0E000000U;
        const int vertexCount = IntegerSource(*this, instruction, 8, 1U);
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 113:
        viewTransition.duration =
            IntegerSource(*this, instruction, 8, 1U);
        viewTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        viewTransition.values[0] = NormalizeAngle(unknown02C);
        viewTransition.values[1] =
            NormalizeAngle(FloatSource(*this, instruction, 16, 4U));
        viewTransition.values[2] = 0.0f;
        viewTransition.values[3] = 0.0f;
        viewTransition.timer.Reset();
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case -1:
    case 1:
        flags &= ~1U;
        // Fall through to the shared script-completion case.
    case 2:
        scriptDataOffsetCopy = 0;
        goto AnmVmUpdateAfterScript;

    default:
        break;
    }

            scriptDataOffsetCopy = NextInstructionAddress(instruction);
            continue;

AnmVmUpdateConditionalBranch:
            timer070.SetCurrent(ReadInstructionInteger(instruction, 20));
            scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 16);
            continue;
    }
    goto AnmVmUpdateAfterScript;

AnmVmUpdateWaitNoFrame:
    flags |= 0x2000U;
    timer070.Add(-1.0f);
    goto AnmVmUpdateAfterScript;

AnmVmUpdateWaitScan:
    {
        const AnmVmInstructionHeader *fallback = 0;
        const AnmVmInstructionHeader *marker =
            reinterpret_cast<const AnmVmInstructionHeader *>(scriptDataOffset);

        for (;;)
        {
            if (marker->opcode == 0x40U &&
                ReadInstructionInteger(marker, 8) == currentFrame)
            {
                break;
            }
            if (marker->opcode == 0xFFFFU)
            {
                break;
            }
            if (marker->opcode == 0x40U &&
                ReadInstructionInteger(marker, 8) == -1)
            {
                fallback = marker;
            }

            marker = reinterpret_cast<const AnmVmInstructionHeader *>(
                InstructionBytes(marker) + marker->size);
        }

        flags &= ~0x2000U;
        currentFrame = 0;
        if (marker->opcode != 0x40U)
        {
            if (fallback == 0)
            {
                timer070.Add(-1.0f);
                goto AnmVmUpdateAfterScript;
            }
            marker = fallback;
        }

        timer3FC.SetCurrent(timer070);
        waitScriptAddress = scriptDataOffsetCopy;
        timer070.SetCurrent(marker->frame);
        flags |= 1U;
        scriptDataOffsetCopy = NextInstructionAddress(marker);
    }
    goto AnmVmUpdateScriptLoop;

AnmVmUpdateAfterScript:
    if (scriptDataOffsetCopy == 0)
    {
        *gameSpeed = savedGameSpeed;
        return 1;
    }

    {
        const float frameGameSpeed = *gameSpeed;
        if (scriptVector.x != 0.0f)
        {
            unknown024 = AddAngles(
                unknown024, frameGameSpeed * scriptVector.x);
            flags |= 4U;
        }

        if (scriptVector.y != 0.0f)
        {
            unknown028 = AddAngles(
                unknown028, frameGameSpeed * scriptVector.y);
            flags |= 4U;
        }

        if (scriptVector.z != 0.0f)
        {
            unknown02C = AddAngles(
                unknown02C, frameGameSpeed * scriptVector.z);
            flags |= 4U;
        }

        if (unknown054 != 0.0f)
        {
            scaleY += frameGameSpeed * unknown054;
            flags |= 8U;
        }

        if (unknown050 != 0.0f)
        {
            scaleX += unknown050 * frameGameSpeed;
            flags |= 0x0CU;
        }

        float scroll = textureScrollX + frameGameSpeed * textureOffsetX;
        if (scroll >= 1.0f)
        {
            scroll -= 1.0f;
        }
        else if (scroll < 0.0f)
        {
            scroll += 1.0f;
        }
        textureScrollX = scroll;

        scroll = textureScrollY + textureOffsetY * frameGameSpeed;
        if (scroll < 1.0f)
        {
            if (scroll < 0.0f)
            {
                scroll += 1.0f;
            }
        }
        else
        {
            // Unlike the X coordinate's ordered comparisons, the target's
            // Y path sends unordered values through the subtract-one branch.
            scroll -= 1.0f;
        }
        textureScrollY = scroll;

        if ((flags & 0x4000U) != 0)
        {
            const float *stageOffset =
                reinterpret_cast<const float *>(0x004D338C);
            position.x += stageOffset[0];
            position.y += stageOffset[1];
            position.z += stageOffset[2];
        }
        }
    if ((stateFlags & 0x20U) != 0)
    {

        float corners[12];
        bool cornersValid = true;
        switch ((flags >> 24) & 0x1FU)
        {
        case 0:
            BuildAnmVmViewCornersMode0(this, corners, corners + 3, corners + 6, corners + 9);
            break;
        case 1:
            BuildAnmVmViewCornersMode1(this, corners, corners + 3, corners + 6, corners + 9);
            break;
        case 2:
        case 3:
            BuildAnmVmViewCornersMode23(this, corners, corners + 3, corners + 6, corners + 9);
            break;
        default:
            cornersValid = false;
            break;
        }

        if (cornersValid)
        {
        float *textureCoordinates = &textureRect[0][0];
        for (unsigned int i = 0; i != 4; ++i)
        {
            const float pixels[2] = {corners[i * 3], corners[i * 3 + 1]};
            float normalized[2];
            NormalizeAnmCoordinates(normalized, pixels);
            textureCoordinates[i * 2] = normalized[0];
            textureCoordinates[i * 2 + 1] = normalized[1];
        }
        }

    }


    if (viewMode != 0)
    {
        float result[3];
        AnmVmVectorTransition *transition =
            reinterpret_cast<AnmVmVectorTransition *>(&viewPosition);
        InterpolateVector(transition, result);

        ZunVec3 *viewOffset = (flags & 0x200U) != 0
            ? &alternatePosition
            : &secondaryPosition;
        viewOffset->x = result[0];
        viewOffset->y = result[1];
        viewOffset->z = result[2];
    }

    if (colorTransition.duration != 0)
    {
        int result[3];
        InterpolateColorVector(&colorTransition, result);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&color);
        channels[0] = (unsigned char)result[0];
        channels[1] = (unsigned char)result[1];
        channels[2] = (unsigned char)result[2];
    }

    if (effectTransition.duration != 0)
    {
        unsigned char *channels = reinterpret_cast<unsigned char *>(&color);
        channels[3] = (unsigned char)InterpolateInteger(&effectTransition);
    }

    if (scaleTransition.duration != 0)
    {
        float result[2];
        InterpolateScale(&scaleTransition, result);
        scaleX = result[0];
        scaleY = result[1];
        flags |= 8U;
    }

    if (secondaryScaleTransition.duration != 0)
    {
        float result[2];
        InterpolateScale(&secondaryScaleTransition, result);
        scale2X = result[0];
        scale2Y = result[1];
        flags |= 0x10U;
    }

    if (positionTransition.duration != 0)
    {
        float result[3];
        InterpolateVector(&positionTransition, result);
        unknown024 = result[0];
        unknown028 = result[1];
        unknown02C = result[2];
        flags |= 4U;
    }

    if (viewTransition.duration != 0)
    {
        unknown02C = InterpolateAngle(&viewTransition);
        flags |= 4U;
    }

    if (secondaryColorTransition.duration != 0)
    {
        int result[3];
        InterpolateColorVector(&secondaryColorTransition, result);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&secondaryColor);
        channels[0] = (unsigned char)result[0];
        channels[1] = (unsigned char)result[1];
        channels[2] = (unsigned char)result[2];
    }

    if (secondaryEffectTransition.duration != 0)
    {
        unsigned char *channels = reinterpret_cast<unsigned char *>(&secondaryColor);
        channels[3] = (unsigned char)InterpolateInteger(&secondaryEffectTransition);
    }

    if (textureOffsetXTransition.duration != 0)
    {
        textureOffsetX = InterpolateLinearScalar(&textureOffsetXTransition);
    }

    if (textureOffsetYTransition.duration != 0)
    {
        textureOffsetY = InterpolateLinearScalar(&textureOffsetYTransition);
    }

    do
    {
    const unsigned int geometryMode = (flags >> 24) & 0x1FU;
    if ((geometryMode != 9U && geometryMode != 13U && geometryMode != 14U) ||
        extraData == 0)
    {
        break;
    }

    AnmVmEffect10Vertex *vertices =
        reinterpret_cast<AnmVmEffect10Vertex *>(extraData);
    const unsigned int count = integerVariables[0] < 0
        ? 0U
        : (unsigned int)integerVariables[0];
    if (count == 0)
    {
        if (geometryMode == 9U)
        {
            // The target still writes the seam V coordinate when the ring
            // has no segments; its zero-initialized loop accumulator is used.
            vertices[0].v = textureScrollY;
            vertices[1].v = textureScrollY;
        }
        break;
    }

    float centerX;
    float centerY;
    float centerZ;
    if (geometryMode == 9U)
    {
        centerX = alternatePosition.x + secondaryPosition.x;
        centerY = alternatePosition.y + secondaryPosition.y;
        centerZ = alternatePosition.z + secondaryPosition.z;
        centerX = centerX + position.x;
        centerY = centerY + position.y;
        centerZ = centerZ + position.z;
    }
    else
    {
        centerX = secondaryPosition.x + alternatePosition.x;
        centerY = secondaryPosition.y + alternatePosition.y;
        centerZ = secondaryPosition.z + alternatePosition.z;
        centerX = position.x + centerX;
        centerY = position.y + centerY;
        centerZ = position.z + centerZ;
    }

    if (parentVm != 0)
    {
        float parentCenterX =
            parentVm->alternatePosition.x + parentVm->secondaryPosition.x;
        float parentCenterY =
            parentVm->alternatePosition.y + parentVm->secondaryPosition.y;
        float parentCenterZ =
            parentVm->alternatePosition.z + parentVm->secondaryPosition.z;
        if (geometryMode == 9U)
        {
            parentCenterX = parentCenterX + parentVm->position.x;
            parentCenterY = parentCenterY + parentVm->position.y;
            parentCenterZ = parentCenterZ + parentVm->position.z;
            centerX = centerX + parentCenterX;
            centerY = centerY + parentCenterY;
            centerZ = centerZ + parentCenterZ;
        }
        else
        {
            parentCenterX = parentVm->position.x + parentCenterX;
            parentCenterY = parentVm->position.y + parentCenterY;
            parentCenterZ = parentVm->position.z + parentCenterZ;
            centerX = parentCenterX + centerX;
            centerY = parentCenterY + centerY;
            centerZ = parentCenterZ + centerZ;
        }
    }

    const unsigned int vertexColor =
        (flags & 0x00030000U) == 0 ? color : secondaryColor;

    if (geometryMode == 9U)
    {
        const unsigned int segmentCount = count - 1U;
        if (segmentCount == 0)
        {
            vertices[0].v = textureScrollY;
            vertices[1].v = textureScrollY;
            break;
        }

        const float segmentCountAsFloat = (float)segmentCount;
        const float angleStep = (float)(
            kAnmVmGeometryTwoPi / (double)segmentCountAsFloat);
        const float vStep = integerVariables[1] / segmentCountAsFloat;
        float angle = unknown02C;
        float v = 0.0f;

        for (unsigned int i = 0; i != segmentCount; ++i)
        {
            AnmVmEffect10Vertex *pairVertices = vertices + i * 2;
            pairVertices[0].reciprocalW = 1.0f;
            pairVertices[0].color = vertexColor;
            pairVertices[0].u = textureRect[0][0] + textureScrollX;
            pairVertices[0].v = v + textureScrollY;
            pairVertices[0].SetPolarForUpdate(angle, scaleX * 0.5f + scaleY);
            pairVertices[0].x += centerX;
            pairVertices[0].y += centerY;
            pairVertices[0].z = centerZ;

            pairVertices[1].reciprocalW = 1.0f;
            pairVertices[1].color = vertexColor;
            pairVertices[1].u = textureRect[0][2] + textureScrollX;
            pairVertices[1].v = v + textureScrollY;
            pairVertices[1].SetPolarForUpdate(angle, scaleY - scaleX * 0.5f);
            pairVertices[1].x += centerX;
            pairVertices[1].y += centerY;
            pairVertices[1].z = centerZ;
            v += vStep;
            angle = AddAngles(angle, angleStep);
        }

        AnmVmEffect10Vertex *closingVertices = vertices + segmentCount * 2;
        closingVertices[0] = vertices[0];
        closingVertices[1] = vertices[1];
        closingVertices[0].v = v + textureScrollY;
        closingVertices[1].v = v + textureScrollY;
        break;
    }

    const unsigned int divisor = count - 1U;
    const float divisorAsFloat = (float)divisor;
    const float angleStep = unknown024 / divisorAsFloat;
    const float vStep = integerVariables[1] / divisorAsFloat;
    const float initialAngle = geometryMode == 13U
        ? unknown02C - unknown024 * 0.5f
        : unknown02C;
    float angle = NormalizeAngle(initialAngle);
    float v = 0.0f;

    for (unsigned int i = 0; i != count; ++i)
    {
        AnmVmEffect10Vertex *pairVertices = vertices + i * 2;
        pairVertices[0].reciprocalW = 1.0f;
        pairVertices[0].color = vertexColor;
        pairVertices[0].u = textureRect[0][0] + textureScrollX;
        pairVertices[0].v = v + textureScrollY;
        pairVertices[0].SetPolarForUpdate(angle, scaleX * 0.5f + scaleY);
        pairVertices[0].x += centerX;
        pairVertices[0].y += centerY;
        pairVertices[0].z = centerZ;

        pairVertices[1].reciprocalW = 1.0f;
        pairVertices[1].color = vertexColor;
        pairVertices[1].u = textureRect[0][2] + textureScrollX;
        pairVertices[1].v = v + textureScrollY;
        pairVertices[1].SetPolarForUpdate(angle, scaleY - scaleX * 0.5f);
        pairVertices[1].x += centerX;
        pairVertices[1].y += centerY;
        pairVertices[1].z = centerZ;
        v += vStep;
        angle = AddAngles(angle, angleStep);
    }
    } while (0);

    if (frameCallback != 0)
    {
        const int callbackResult = frameCallback(this);
        if (callbackResult != 0)
        {
            // The target branches to its early epilogue, which explicitly
            // returns one for both script completion and callback termination.
            // This edge also skips the timer increment and speed restoration.
            return 1;
        }
    }

    timer070.previous = timer070.current;
    if (*timer070.gameSpeed > 0.99 && *timer070.gameSpeed < 1.01f)
    {
        ++timer070.current;
        timer070.currentF += 1.0f;
    }
    else
    {
        timer070.currentF += *timer070.gameSpeed;
        timer070.current = (int)timer070.currentF;
    }
    *gameSpeed = savedGameSpeed;
    return 0;
}
