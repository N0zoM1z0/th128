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

    // The original transition owner reset receives the owner object, not the
    // embedded ZunTimer. Every owner used here stores its timer at +0x30.
    // Keeping that boundary also preserves the target's shared call edge.
    __declspec(noinline) void ResetTransitionTimer(void *owner)
    {
        ZunTimer *timer = reinterpret_cast<ZunTimer *>(
            reinterpret_cast<unsigned char *>(owner) +
            offsetof(AnmVmVectorTransition, timer));
        if (!timer->initialized)
        {
            timer->currentF = 0.0f;
            timer->current = 0;
            timer->previous = -999999;
            timer->gameSpeed = &g_TimeScale;
            timer->initialized = 1;
        }
        timer->currentF = 0.0f;
        timer->current = 0;
        timer->previous = -1;
    }

    // Scalar transition owners place the same timer immediately after their
    // four float lanes, at +0x10 rather than the vector owner's +0x30.
    __declspec(noinline) void ResetScalarTransitionTimer(void *owner)
    {
        ZunTimer *timer = reinterpret_cast<ZunTimer *>(
            reinterpret_cast<unsigned char *>(owner) +
            offsetof(AnmVmScalarTransition, timer));
        if (!timer->initialized)
        {
            timer->currentF = 0.0f;
            timer->current = 0;
            timer->previous = -999999;
            timer->gameSpeed = &g_TimeScale;
            timer->initialized = 1;
        }
        timer->currentF = 0.0f;
        timer->current = 0;
        timer->previous = -1;
    }

    // The view-angle transition has its own target helper copy at 0x45DAD0;
    // keep its owner-relative timer reset as a distinct natural C++ call edge.
    __declspec(noinline) void ResetViewTransitionTimer(void *owner)
    {
        ZunTimer *timer = reinterpret_cast<ZunTimer *>(
            reinterpret_cast<unsigned char *>(owner) +
            offsetof(AnmVmScalarTransition, timer));
        if (!timer->initialized)
        {
            timer->currentF = 0.0f;
            timer->current = 0;
            timer->previous = -999999;
            timer->gameSpeed = &g_TimeScale;
            timer->initialized = 1;
        }
        timer->currentF = 0.0f;
        timer->current = 0;
        timer->previous = -1;
    }

    // Update has a second target copy of SetCurrent at 0x00403280. Keep the
    // private EAX/stack ABI as a separate natural C++ call boundary.
    __declspec(noinline) void __stdcall SetUpdateTimerCurrent(
        ZunTimer *timer,
        int value)
    {
        timer->Initialize();
        timer->current = value;
        timer->previous = value - 1;
        timer->currentF = (float)value;
    }


    __declspec(noinline) unsigned int *__stdcall SpawnChildVm(
        AnmResource *resource,
        unsigned int *childId,
        int scriptIndex,
        AnmVm *parent,
        unsigned int listMode)
    {
        const unsigned int increment = 1;
        if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
        {
            AnmVmChildCriticalSectionRoutine enter =
                *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A088);
            enter(reinterpret_cast<void *>(0x004D3750));
            *reinterpret_cast<volatile unsigned char *>(0x004D37A1) +=
                static_cast<unsigned char>(increment);
        }
        resource->activeVmCount += increment;

        AnmVm *child = AllocateAnmVm();
        const ZunVec3 initialPosition = {0.0f, 0.0f, 0.0f};
        child->stateFlags |= 2U;
        child->position.x = initialPosition.x;
        child->scriptState = parent->scriptState;
        child->position.y = initialPosition.y;
        child->position.z = initialPosition.z;
        resource->BindVmScript(child, scriptIndex);
        child->parentVm = parent;
        AnmId registeredId;
        if ((listMode & 4U) != 0)
        {
            if ((listMode & 2U) != 0)
                registeredId = child->RegisterUiVmAtHead();
            else
                registeredId = child->RegisterUiVmAtTail();
        }
        else
        {
            if ((listMode & 2U) != 0)
                registeredId = child->RegisterWorldVmAtHead();
            else
                registeredId = child->RegisterWorldVmAtTail();
        }
        *childId = registeredId.id;

        AnmVmListNode *head = &parent->childNode;
        AnmVmListNode *node = &child->childNode;
        node->next = head->next;
        if (node->next != 0)
        {
            node->next->previous = node;
        }
        head->next = node;
        node->previous = head;

        if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
        {
            AnmVmChildCriticalSectionRoutine leave =
                *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A08C);
            leave(reinterpret_cast<void *>(0x004D3750));
            --*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
        }
        return childId;
    }

    __declspec(noinline) unsigned int *__stdcall SpawnChildVmAtPosition(
        AnmResource *resource,
        unsigned int *childId,
        int scriptIndex,
        AnmVm *parent)
    {
        const unsigned int increment = 1;
        if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
        {
            AnmVmChildCriticalSectionRoutine enter =
                *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A088);
            enter(reinterpret_cast<void *>(0x004D3750));
            *reinterpret_cast<volatile unsigned char *>(0x004D37A1) +=
                static_cast<unsigned char>(increment);
        }
        resource->activeVmCount += increment;

        AnmVm *child = AllocateAnmVm();
        child->stateFlags |= 2U;
        child->scriptState = parent->scriptState;
        resource->BindVmScript(child, scriptIndex);
        const unsigned int *sourcePosition =
            reinterpret_cast<const unsigned int *>(&parent->position);
        unsigned int *destinationPosition =
            reinterpret_cast<unsigned int *>(&child->position);
        destinationPosition[0] = sourcePosition[0];
        destinationPosition[1] = sourcePosition[1];
        destinationPosition[2] = sourcePosition[2];
        const unsigned int *sourceScriptFloats =
            reinterpret_cast<const unsigned int *>(&parent->unknown024);
        unsigned int *destinationScriptFloats =
            reinterpret_cast<unsigned int *>(&child->unknown024);
        destinationScriptFloats[0] = sourceScriptFloats[0];
        destinationScriptFloats[1] = sourceScriptFloats[1];
        destinationScriptFloats[2] = sourceScriptFloats[2];
        child->alternatePosition = parent->secondaryPosition;
        const AnmId registeredId = child->RegisterWorldVmAtTail();
        *childId = registeredId.id;

        if ((*reinterpret_cast<volatile unsigned int *>(0x004D33F8) & 0x8000U) != 0)
        {
            AnmVmChildCriticalSectionRoutine leave =
                *reinterpret_cast<AnmVmChildCriticalSectionRoutine *>(0x0049A08C);
            leave(reinterpret_cast<void *>(0x004D3750));
            --*reinterpret_cast<volatile unsigned char *>(0x004D37A1);
        }
        return childId;
    }

    __declspec(noinline) AnmVm *ResolveAnmVmId(unsigned int *id)
    {
        AnmManager *manager = GetGlobalAnmManager();
        AnmVm *vm = GetVmByIdTargetAbi(0, manager, *id);
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
    // Callers validate mode 1..16 and pass the target's zero-based table index.
    float __stdcall EaseTransition(unsigned int modeIndex, float t)
    {
        __assume(modeIndex < 16U);
        switch (modeIndex)
        {
        case 0: return t * t;
        case 3: return 1.0f - (1.0f - t) * (1.0f - t);
        case 1: return t * t * t;
        case 4: return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t);
        case 2: return t * t * t * t;
        case 5: return 1.0f - (1.0f - t) * (1.0f - t) * (1.0f - t) * (1.0f - t);
        case 8:
            t *= 2.0;
            if (t < 1.0f)
                return (float)((double)t * t * 0.5);
            {
                const double delta = 2.0 - (double)t;
                return (float)((2.0 - delta * delta) * 0.5);
            }
        case 11:
            t += t;
            if (t < 1.0f)
            {
                const double delta = 1.0 - t;
                const double power = delta * delta;
                const double half = 0.5;
                return (float)(half - half * power);
            }
            else
            {
                const double delta = t - 1.0;
                const double power = delta * delta;
                const double half = 0.5;
                return (float)(half + half * power);
            }
        case 9:
            t *= 2.0;
            if (t < 1.0f)
                return (float)((double)t * t * t * 0.5);
            {
                double delta = 2.0 - (double)t;
                delta *= delta;
                delta *= 2.0 - (double)t;
                return (float)((2.0 - delta) * 0.5);
            }
        case 12:
            t += t;
            if (t < 1.0f)
            {
                double delta = 1.0 - t;
                delta *= delta;
                delta *= 1.0 - t;
                const double half = 0.5;
                return (float)(half - half * delta);
            }
            else
            {
                double delta = t - 1.0;
                delta *= delta;
                delta *= t - 1.0;
                const double half = 0.5;
                return (float)(half + half * delta);
            }
        case 10:
            t *= 2.0;
            if (t < 1.0f)
                return (float)((double)t * t * t * t * 0.5);
            {
                double delta = 2.0 - (double)t;
                delta *= delta;
                delta *= 2.0 - (double)t;
                delta *= 2.0 - (double)t;
                return (float)((2.0 - delta) * 0.5);
            }
        case 13:
            t += t;
            if (t < 1.0f)
            {
                double delta = 1.0 - t;
                delta *= delta;
                delta *= 1.0 - t;
                delta *= 1.0 - t;
                const double half = 0.5;
                return (float)(half - half * delta);
            }
            else
            {
                double delta = t - 1.0;
                delta *= delta;
                delta *= t - 1.0;
                delta *= t - 1.0;
                const double half = 0.5;
                return (float)(half + half * delta);
            }
        case 14: return 0.0f;
        case 15: return 1.0f;
        case 6:
        case 7:
            return t;
        }
        __assume(0);
        return 0.0f;
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

    int *InterpolateColorVector(AnmVmColorTransition *transition, int result[3])
    {
        volatile int *const durationValue = &transition->duration;
        if (*durationValue > 0)
        {
            transition->timer.Increment();
            const int duration = *durationValue;
            if (transition->timer.current >= duration)
            {
                transition->timer.Initialize();
                transition->timer.previous = duration - 1;
                transition->timer.current = duration;
                transition->timer.currentF = (float)duration;
                *durationValue = 0;
                volatile unsigned int *const transitionMode = &transition->mode;
                if (*transitionMode != 7)
                {
                    result[0] = transition->values[3];
                    result[1] = transition->values[4];
                    result[2] = transition->values[5];
                }
                else
                {
                    result[0] = transition->values[0];
                    result[1] = transition->values[1];
                    result[2] = transition->values[2];
                }
                return result;
            }
        }

        unsigned int mode = transition->mode;
        if (mode == 7)
        {
            const int current0 = transition->values[0];
            const int current1 = transition->values[1];
            const int current2 = transition->values[2];
            const int delta0 = transition->values[3];
            const int delta1 = transition->values[4];
            const int delta2 = transition->values[5];
            const int next0 = current0 + delta0;
            const int next1 = current1 + delta1;
            const int next2 = current2 + delta2;
            transition->values[0] = next0;
            transition->values[1] = next1;
            transition->values[2] = next2;
            result[0] = next0;
            result[1] = next1;
            result[2] = next2;
            return result;
        }

        if (mode == 17)
        {
            const int current0 = transition->values[0];
            const int current1 = transition->values[1];
            const int current2 = transition->values[2];
            const int future0 = transition->values[9];
            const int future1 = transition->values[10];
            const int future2 = transition->values[11];
            const int next0 = future0 + current0;
            const int next1 = future1 + current1;
            const int next2 = future2 + current2;
            transition->values[0] = next0;
            transition->values[1] = next1;
            transition->values[2] = next2;
            result[0] = next0;
            result[1] = next1;
            result[2] = next2;
            volatile int nextFuture0 = future0 + transition->values[3];
            volatile int nextFuture1 = future1 + transition->values[4];
            volatile int nextFuture2 = future2 + transition->values[5];
            transition->values[9] = nextFuture0;
            transition->values[10] = nextFuture1;
            transition->values[11] = nextFuture2;
            return result;
        }

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const double tMinusOne = (double)t - 1.0;
            volatile float coefficients[4];
            coefficients[0] = (float)((t + t + 1.0) * tMinusOne * tMinusOne);
            coefficients[1] = (float)(t * t * (3.0 - (t + t)));
            coefficients[2] = (float)((1.0 - t) * (1.0 - t) * t);
            coefficients[3] = (float)(tMinusOne * t * t);
            volatile int terms[12];
            terms[0] = (int)((double)coefficients[3] * transition->values[9]);
            terms[1] = (int)((double)coefficients[3] * transition->values[10]);
            terms[2] = (int)((double)coefficients[3] * transition->values[11]);
            terms[3] = (int)((double)coefficients[2] * transition->values[6]);
            terms[4] = (int)((double)coefficients[2] * transition->values[7]);
            terms[5] = (int)((double)coefficients[2] * transition->values[8]);
            terms[6] = (int)((double)coefficients[1] * transition->values[3]);
            terms[7] = (int)((double)coefficients[1] * transition->values[4]);
            terms[8] = (int)((double)coefficients[1] * transition->values[5]);
            terms[9] = (int)((double)coefficients[0] * transition->values[0]);
            terms[10] = (int)((double)coefficients[0] * transition->values[1]);
            terms[11] = (int)((double)coefficients[0] * transition->values[2]);
            result[0] = terms[9] + terms[6] + terms[3] + terms[0];
            result[1] = terms[10] + terms[7] + terms[4] + terms[1];
            result[2] = terms[11] + terms[8] + terms[5] + terms[2];
            return result;
        }

        --mode;
        float eased = t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, t);
        }
        const int delta0 = transition->values[3] - transition->values[0];
        const int delta1 = transition->values[4] - transition->values[1];
        const int delta2 = transition->values[5] - transition->values[2];
        const int value0 = transition->values[0] +
            (int)((double)delta0 * eased);
        const int value1 = transition->values[1] +
            (int)((double)delta1 * eased);
        const int value2 = transition->values[2] +
            (int)((double)delta2 * eased);
        result[0] = value0;
        result[1] = value1;
        result[2] = value2;
        return result;
}


    int InterpolateInteger(AnmVmIntegerTransition *transition)
    {
        if (AdvanceTransition(&transition->timer, &transition->duration,
            transition->mode))
        {
            return transition->values[(transition->mode == 7) ? 0 : 1];
        }

        unsigned int mode = transition->mode;
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
            const double h00Extended = (double)h00;
            const double h01Extended = (double)h01;
            const double h10Extended = (double)h10;
            const double h11Extended = (double)h11;
            const double value =
                h00Extended * transition->values[0] +
                h01Extended * transition->values[1] +
                h10Extended * transition->values[2] +
                h11Extended * transition->values[3];
            return (int)value;
        }

        --mode;
        float eased = (float)t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, eased);
        }
        const int delta = transition->values[1] - transition->values[0];
        return (int)((double)delta * eased + transition->values[0]);
    }

    // The target's three-component vector helper uses mode 7 for relative updates.
    float *InterpolateVector(AnmVmVectorTransition *transition, float result[3])
    {
        if (AdvanceTransition(&transition->timer, &transition->duration,
            transition->mode))
        {
            const unsigned int endpoint = (transition->mode == 7) ? 0 : 3;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            result[2] = transition->values[endpoint + 2];
            return result;
        }

        unsigned int mode = transition->mode;
        if (mode == 7)
        {
            transition->values[0] += transition->values[3];
            transition->values[1] += transition->values[4];
            transition->values[2] += transition->values[5];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            result[2] = transition->values[2];
            return result;
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
            return result;
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
            return result;
        }

        --mode;
        float eased = (float)t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, eased);
        }
        result[0] = eased *
            (transition->values[3] - transition->values[0]) + transition->values[0];
        result[1] = eased *
            (transition->values[4] - transition->values[1]) + transition->values[1];
        result[2] = eased *
            (transition->values[5] - transition->values[2]) + transition->values[2];
        return result;
}


    float *InterpolateScale(AnmVmScaleTransition *transition, float result[2])
    {
        if (AdvanceTransition(&transition->timer, &transition->duration,
            transition->mode))
        {
            // The target's two-component scale helper also uses mode 7 for
            // its relative endpoint.
            const unsigned int endpoint = (transition->mode == 7) ? 0 : 2;
            result[0] = transition->values[endpoint];
            result[1] = transition->values[endpoint + 1];
            return result;
        }

        unsigned int mode = transition->mode;
        if (mode == 7)
        {
            transition->values[0] += transition->values[2];
            transition->values[1] += transition->values[3];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            return result;
        }

        if (mode == 17)
        {
            transition->values[0] += transition->values[6];
            transition->values[1] += transition->values[7];
            result[0] = transition->values[0];
            result[1] = transition->values[1];
            transition->values[6] += transition->values[2];
            transition->values[7] += transition->values[3];
            return result;
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
            return result;
        }

        --mode;
        float eased = (float)t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, eased);
        }
        result[0] = eased *
            (transition->values[2] - transition->values[0]) + transition->values[0];
        result[1] = eased *
            (transition->values[3] - transition->values[1]) + transition->values[1];
        return result;
}


    // The view rotation is an angular scalar: relative and eased paths wrap,
    // and ordinary interpolation follows the shortest signed arc.
    float *InterpolateAngle(AnmVmScalarTransition *transition, float *result)
    {
        if (AdvanceTransition(&transition->timer, &transition->duration,
            transition->mode))
        {
            *result = transition->values[(transition->mode == 7) ? 0 : 1];
            return result;
        }

        unsigned int mode = transition->mode;
        if (mode == 7)
        {
            transition->values[0] = NormalizeAngle(
                transition->values[0] + transition->values[1]);
            *result = transition->values[0];
            return result;
        }

        if (mode == 17)
        {
            transition->values[0] = NormalizeAngle(
                transition->values[0] + transition->values[3]);
            transition->values[3] = NormalizeAngle(
                transition->values[3] + transition->values[1]);
            *result = transition->values[0];
            return result;
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
            float angle = NormalizeAngle(startTerm + targetTerm);
            const float startTangentTerm = NormalizeAngle(
                (float)((1.0 - t) * (1.0 - t) * t * transition->values[2]));
            angle = NormalizeAngle(startTangentTerm + angle);
            const float targetTangentTerm = NormalizeAngle(
                (float)(tMinusOne * t * t * transition->values[3]));
            *result = NormalizeAngle(targetTangentTerm + angle);
            return result;
        }

        --mode;
        float eased = (float)t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, eased);
        }
        const float difference = ShortestAngleDifference(
            transition->values[1], transition->values[0]);
        const float normalizedDifference = NormalizeAngle(difference);
        const float scaledDifference = NormalizeAngle(
            normalizedDifference * eased);
        *result = NormalizeAngle(scaledDifference + transition->values[0]);
        return result;
    }

    // Texture scroll offsets use the ordinary scalar helper at 0x0040CCF0.
    // Its Hermite terms are accumulated in the target's evaluation order,
    // without angle normalization or shortest-arc interpolation.
    float InterpolateLinearScalar(AnmVmScalarTransition *transition)
    {
        if (AdvanceTransition(&transition->timer, &transition->duration,
            transition->mode))
        {
            return transition->values[(transition->mode == 7) ? 0 : 1];
        }

        unsigned int mode = transition->mode;
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

        --mode;
        float eased = (float)t;
        if (mode <= 15U)
        {
            eased = EaseTransition(mode, eased);
        }
        return (transition->values[1] - transition->values[0]) * eased +
            transition->values[0];
    }





}

void __stdcall UpdateVmForBinding(AnmVm *vm);

void AnmResource::BindVmScript(AnmVm *vm, int scriptIndex)
{
    if (scriptOffsets[scriptIndex] == 0 || isInvalid != 0)
    {
        memset(vm, 0, sizeof(AnmVm));
        return;
    }

    vm->ResetForReuse();
    vm->anmScriptIndex = (short)scriptIndex;
    vm->anmFileId = (short)fileId;
    vm->flags &= ~0xC00U;
    vm->resource = this;
    vm->stateFlags = (vm->stateFlags & ~0x40U) | 0x80U;
    const unsigned int scriptData = scriptOffsets[scriptIndex];
    vm->scriptDataOffset = scriptData;
    vm->scriptDataOffsetCopy = scriptData;

    if (!vm->timer070.initialized)
    {
        vm->timer070.currentF = 0.0f;
        vm->timer070.current = 0;
        vm->timer070.previous = -999999;
        vm->timer070.gameSpeed = reinterpret_cast<float *>(0x004B6F58);
        vm->timer070.initialized = 1;
    }
    (void)*reinterpret_cast<AnmManager *volatile *>(0x004D2E50);
    vm->timer070.currentF = 0.0f;
    vm->timer070.current = 0;
    vm->timer070.previous = -1;
    *reinterpret_cast<volatile unsigned int *>(&vm->flags) &= ~1U;

    UpdateVmForBinding(vm);
    ++GetGlobalAnmManager()->activeVmCount;

    vm->scale2X = 1.0f;
    vm->scale2Y = 1.0f;
    if ((vm->stateFlags & 0xC0U) == 0x80U)
    {
        vm->stateFlags = (vm->stateFlags & ~0x80U) | 0x40U;
    }
}

void AnmResource::BindVmAtPosition(
    AnmVm *vm,
    int scriptIndex,
    const ZunVec3 *position)
{
    if (scriptOffsets[scriptIndex] == 0 || isInvalid != 0)
    {
        memset(vm, 0, sizeof(AnmVm));
        return;
    }

    vm->ResetForReuse();
    vm->secondaryPosition = *position;
    vm->anmScriptIndex = (short)scriptIndex;
    vm->anmFileId = (short)fileId;
    vm->flags &= ~0xC00U;
    vm->resource = this;
    vm->stateFlags = (vm->stateFlags & ~0x40U) | 0x80U;
    const unsigned int scriptData = scriptOffsets[scriptIndex];
    vm->scriptDataOffset = scriptData;
    vm->scriptDataOffsetCopy = scriptData;

    if (!vm->timer070.initialized)
    {
        vm->timer070.currentF = 0.0f;
        vm->timer070.current = 0;
        vm->timer070.previous = -999999;
        vm->timer070.gameSpeed = reinterpret_cast<float *>(0x004B6F58);
        vm->timer070.initialized = 1;
    }
    (void)*reinterpret_cast<AnmManager *volatile *>(0x004D2E50);
    vm->timer070.currentF = 0.0f;
    vm->timer070.current = 0;
    vm->timer070.previous = -1;
    *reinterpret_cast<volatile unsigned int *>(&vm->flags) &= ~1U;

    UpdateVmForBinding(vm);
    ++GetGlobalAnmManager()->activeVmCount;

    if ((vm->stateFlags & 0xC0U) == 0x80U)
    {
        vm->stateFlags = (vm->stateFlags & ~0x80U) | 0x40U;
    }
}

__declspec(noinline) int AnmVm::BindAnmFile(
    AnmResource *resource,
    int fileIndex)
{
    AnmVm *vm = this;
    if (resource->anmFileHeader == 0 || resource->isInvalid != 0)
    {
        return -1;
    }

    vm->anmFileIndex = (short)fileIndex;
    register AnmFileEntry *entry = &resource->anmFiles[fileIndex];
    vm->anmFileEntry = entry;
    register AnmFileEntry *entryForLaterFields = entry;

    const float left = entry->field024;
    vm->textureRect[1][0] = left;
    vm->textureRect[0][0] = left;
    const float bottom = entry->field02C;
    vm->textureRect[1][2] = bottom;
    vm->textureRect[0][2] = bottom;
    const float top = entryForLaterFields->field028;
    vm->textureRect[0][3] = top;
    vm->textureRect[0][1] = top;
    const float right = entry->field030;
    vm->textureRect[1][3] = right;
    vm->textureRect[1][1] = right;

    vm->uvScrollX = entryForLaterFields->field038;
    vm->uvScrollY = entry->field034;

    float *transformMatrix = vm->transformMatrix[0];
    transformMatrix[14] = 0.0f;
    transformMatrix[13] = 0.0f;
    transformMatrix[12] = 0.0f;
    transformMatrix[11] = 0.0f;
    transformMatrix[9] = 0.0f;
    transformMatrix[8] = 0.0f;
    transformMatrix[7] = 0.0f;
    transformMatrix[6] = 0.0f;
    transformMatrix[4] = 0.0f;
    transformMatrix[3] = 0.0f;
    transformMatrix[2] = 0.0f;
    transformMatrix[1] = 0.0f;
    transformMatrix[15] = 1.0f;
    transformMatrix[10] = 1.0f;
    transformMatrix[5] = 1.0f;
    transformMatrix[0] = 1.0f;

    vm->spriteScaleMatrix[3][3] = 1.0f;
    vm->spriteScaleMatrix[2][2] = 1.0f;
    vm->spriteScaleMatrix[1][1] = 1.0f;
    vm->spriteScaleMatrix[0][0] = 1.0f;
    vm->spriteScaleMatrix[3][2] = 0.0f;
    vm->spriteScaleMatrix[3][1] = 0.0f;
    vm->spriteScaleMatrix[3][0] = 0.0f;
    vm->spriteScaleMatrix[2][3] = 0.0f;
    vm->spriteScaleMatrix[2][1] = 0.0f;
    vm->spriteScaleMatrix[2][0] = 0.0f;
    vm->spriteScaleMatrix[1][3] = 0.0f;
    vm->spriteScaleMatrix[1][2] = 0.0f;
    vm->spriteScaleMatrix[1][0] = 0.0f;
    vm->spriteScaleMatrix[0][3] = 0.0f;
    vm->spriteScaleMatrix[0][2] = 0.0f;
    vm->spriteScaleMatrix[0][1] = 0.0f;

    register AnmFileEntry *scaleEntry = vm->anmFileEntry;
    transformMatrix[0] = vm->uvScrollX * (1.0f / 256.0f);
    vm->transformMatrix[1][1] = vm->uvScrollY * (1.0f / 256.0f);

    vm->spriteScaleMatrix[0][0] =
        (vm->uvScrollX / scaleEntry->field020) * scaleEntry->field03C;
    memcpy(vm->textureTransformMatrix, transformMatrix, sizeof(vm->textureTransformMatrix));
    vm->spriteScaleMatrix[1][1] =
        (vm->uvScrollY / scaleEntry->field01C) * scaleEntry->field040;

    return 0;
}




namespace
{
    // The target divides its binary64 2*pi constant by a float-rounded
    // segment count, then stores the quotient as float.
    const double kAnmVmGeometryTwoPi = 6.28318530717958647692;


}




// Keep the frame owner and its list helpers in the same translation unit.
namespace
{
    inline void InsertAnmVmNodeAfter(AnmVmListNode *head, AnmVmListNode *node)
    {
        if (head->next != 0)
        {
            node->next = head->next;
            head->next->previous = node;
        }
        head->next = node;
        node->previous = head;
    }
}

AnmId AnmVm::RegisterWorldVmAtTail()
{
    AnmManager *manager = g_AnmManager;
    AnmVmListNode *node = &managerNode;
    node->vm = this;
    node->next = 0;
    node->previous = 0;

    if (manager->worldVms == 0)
    {
        manager->worldVms = node;
    }
    else
    {
        AnmVmListNode *tail = manager->worldVmsTail;
        InsertAnmVmNodeAfter(tail, node);
    }
    manager->worldVmsTail = node;

    ++manager->nextVmId;
    if (manager->nextVmId == 0)
    {
        ++manager->nextVmId;
    }
    id = manager->nextVmId;
    AnmId result = { manager->nextVmId };
    return result;
}

AnmId AnmVm::RegisterWorldVmAtHead()
{
    AnmManager *manager = g_AnmManager;
    AnmVmListNode *node = &managerNode;
    node->vm = this;
    node->next = 0;
    node->previous = 0;

    AnmVmListNode *head = manager->worldVms;
    if (head == 0)
    {
        manager->worldVmsTail = node;
    }
    else
    {
        InsertAnmVmNodeAfter(node, head);
    }
    manager->worldVms = node;

    ++manager->nextVmId;
    if (manager->nextVmId == 0)
    {
        ++manager->nextVmId;
    }
    id = manager->nextVmId;
    AnmId result = { manager->nextVmId };
    return result;
}

AnmId AnmVm::RegisterUiVmAtTail()
{
    AnmManager *manager = g_AnmManager;
    AnmVmListNode *node = &managerNode;
    node->vm = this;
    node->next = 0;
    node->previous = 0;

    if (manager->uiVms == 0)
    {
        manager->uiVms = node;
    }
    else
    {
        AnmVmListNode *tail = manager->uiVmsTail;
        InsertAnmVmNodeAfter(tail, node);
    }
    manager->uiVmsTail = node;

    ++manager->nextVmId;
    if (manager->nextVmId == 0)
    {
        ++manager->nextVmId;
    }
    id = manager->nextVmId;
    AnmId result = { manager->nextVmId };
    return result;
}

AnmId AnmVm::RegisterUiVmAtHead()
{
    AnmManager *manager = g_AnmManager;
    AnmVmListNode *node = &managerNode;
    node->vm = this;
    node->next = 0;
    node->previous = 0;

    AnmVmListNode *head = manager->uiVms;
    if (head == 0)
    {
        manager->uiVmsTail = node;
    }
    else
    {
        InsertAnmVmNodeAfter(node, head);
    }
    manager->uiVms = node;

    ++manager->nextVmId;
    if (manager->nextVmId == 0)
    {
        ++manager->nextVmId;
    }
    id = manager->nextVmId;
    AnmId result = { manager->nextVmId };
    return result;
}

// Partial reconstruction of the 13,604-byte per-VM update owner at
// 0x00459D90. This source mapping is not an exact match: the remaining target
// branches, ordering and side effects still require full control-flow review.
int AnmVm::Update()
{
    AnmVm *const vm = this;
    // Keep one child-id output slot per target spawn opcode. Their distinct
    // addresses prevent LTCG from folding five original callsites together.
    unsigned int childSpawnId88;
    unsigned int childSpawnId96;
    unsigned int childSpawnId91;
    unsigned int childSpawnId90;
    unsigned int childSpawnId92;
    if (scriptDataOffsetCopy == 0)
    {
        return 1;
    }

    if ((flags & 0x00080000U) != 0)
    {
        return 0;
    }

    const float savedGameSpeed = g_TimeScale;
    if ((stateFlags & 1U) != 0)
    {
        g_TimeScale = 1.0f;
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
            // The target jumps past the normal speed-restoration block on
            // this frozen-stage edge.
            return 0;
        }
    }

AnmVmUpdateScriptLoop:
    for (;;)
    {
            const AnmVmInstructionHeader *const instruction =
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
    // Both opcode zero and opcode 64 are explicit no-ops in the target's
    // dispatch table; they advance through the common instruction tail.
    case 0:
    case 64:
        break;

    case 4:
        SetUpdateTimerCurrent(&timer070, ReadInstructionInteger(instruction, 12));
        scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 8);
        continue;

    case 5:
        integerDestination = IntegerDestination(*this, instruction);
        --(*integerDestination);
        integerValue = IntegerSource(*this, instruction, 8, 1U);
        if (integerValue > 0)
        {
            SetUpdateTimerCurrent(&timer070, ReadInstructionInteger(instruction, 16));
            scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 12);
        }
        else
        {
            scriptDataOffsetCopy = NextInstructionAddress(instruction);
        }
        continue;

    case 69:
        flags &= ~1U;
        if (currentFrame == 0)
        {
            goto AnmVmUpdateWaitNoFrame;
        }
        goto AnmVmUpdateWaitScan;

    case 63:
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

    case 28:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left == right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 29:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left == right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 30:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left != right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 31:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left != right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 32:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left < right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 33:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left < right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 34:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left <= right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 35:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left <= right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 36:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left > right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 37:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left > right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 38:
    {
        const int left = IntegerSource(*this, instruction, 8, 1U);
        const int right = IntegerSource(*this, instruction, 12, 2U);
        if (left >= right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case 39:
    {
        const float left = FloatSource(*this, instruction, 8, 1U);
        const float right = FloatSource(*this, instruction, 12, 2U);
        if (left >= right) goto AnmVmUpdateConditionalBranch;
        goto AnmVmUpdateInstructionAdvance;
    }

    case -1:
    case 1:
        flags &= ~1U;
        // Fall through to the shared script-completion case.
    case 2:
        scriptDataOffsetCopy = 0;
        g_TimeScale = savedGameSpeed;
        return 1;

    case 86:
        stateFlags = (stateFlags & ~1U) |
            ((unsigned int)IntegerSource(*this, instruction, 8, 1U) & 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 88:
    {
        SpawnChildVm(vm->resource, &childSpawnId88, IntegerSource(*this, instruction, 8, 1U), this, 0);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 96:
    {
        SpawnChildVm(vm->resource, &childSpawnId96, IntegerSource(*this, instruction, 8, 1U), this, 0);
        AnmVm *child = ResolveAnmVmId(&childSpawnId96);
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(*this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(*this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 91:
    {
        SpawnChildVm(vm->resource, &childSpawnId91, IntegerSource(*this, instruction, 8, 1U), this, 2);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 90:
    {
        SpawnChildVm(vm->resource, &childSpawnId90, IntegerSource(*this, instruction, 8, 1U), this, 4);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 92:
    {
        SpawnChildVm(vm->resource, &childSpawnId92, IntegerSource(*this, instruction, 8, 1U), this, 6);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 111:
        stateFlags = (stateFlags & ~0xC0U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 6) & 0xC0U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 97:
    {
        unsigned int childId;
        SpawnChildVmAtPosition(vm->resource, &childId, IntegerSource(*this, instruction, 8, 1U), this);
        AnmVm *child = ResolveAnmVmId(&childId);
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(*this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(*this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 95:
    {
        unsigned int childId;
        SpawnChildVmAtPosition(vm->resource, &childId, IntegerSource(*this, instruction, 8, 1U), this);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

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
            if ((instruction->flags & 1U) != 0)
            {
                fileIndex = ResolveIntegerOperand(fileIndex);
            }
            fileIndex = anmFileResolver(this, fileIndex);
        }

        AnmResource *fileResource = vm->resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }
        BindAnmFile(fileResource, fileIndex);
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 102:
    {
        flags |= 1U;
        const AnmVmInstructionHeader * volatile variantInstruction = instruction;
        int fileIndex;
        if (anmFileResolver == 0)
        {
            const int firstIndex = IntegerSource(*this, instruction, 8, 1U);
            const unsigned int variantCount =
                (unsigned int)IntegerSource(*this, instruction, 12, 2U);
            const unsigned int randomValue =
                reinterpret_cast<Rng *>(0x004D2AE0)->GetU32();
            fileIndex = firstIndex + (int)(randomValue % variantCount);
        }
        else
        {
            const int firstIndex = IntegerSource(*this, instruction, 8, 1U);
            const unsigned int variantCount =
                (unsigned int)IntegerSource(*this, instruction, 12, 2U);
            const unsigned int randomValue =
                reinterpret_cast<Rng *>(0x004D2AE0)->GetU32();
            fileIndex = firstIndex + (int)(randomValue % variantCount);
            fileIndex = anmFileResolver(this, fileIndex);
        }

        AnmResource *fileResource = vm->resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }
        BindAnmFile(fileResource, fileIndex);
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = NextInstructionAddress(variantInstruction);
        continue;
    }

    case 50:
        scaleX = FloatSource(*this, instruction, 8, 1U);
        scaleY = FloatSource(*this, instruction, 12, 2U);
        flags |= 8U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 106:
        unknown058 = FloatSource(*this, instruction, 8, 1U);
        unknown05C = FloatSource(*this, instruction, 12, 2U);
        flags |= 0x10U;
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

    case 77:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&secondaryColor);
        colorBytes[3] = (unsigned char)IntegerSource(*this, instruction, 8, 1U);
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

    case 49:
        unknown024 = FloatSource(*this, instruction, 8, 1U);
        unknown028 = FloatSource(*this, instruction, 12, 2U);
        unknown02C = FloatSource(*this, instruction, 16, 4U);
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

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

    case 55:
    {
        const unsigned int duration = (unsigned int)IntegerSource(*this, instruction, 12, 2U);
        const unsigned char parameter1 = InstructionBytes(instruction)[8];
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(duration, 0, alpha, parameter1);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 66:
        flags ^= (((unsigned int)ReadInstructionInteger(instruction, 8) << 5) ^ flags) & 0x000000E0U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

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

    case 75:
    {
        const int frameOffset = IntegerSource(*this, instruction, 8, 1U);
        timer070.Add((float)-frameOffset);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 72:
        flags = (flags & ~0x00000001U) |
            ((unsigned int)ReadInstructionInteger(instruction, 8) & 1U);
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

    case 70:
        textureOffsetX = FloatSource(*this, instruction, 8, 1U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 71:
        textureOffsetY = FloatSource(*this, instruction, 8, 1U);
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

    case 89:
        stateFlags = (stateFlags & ~4U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 2) & 4U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

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

        // The target resolves Z, Y, X; resolving an indirect random operand
        // advances the VM RNG, so preserve that order before assigning fields.
        const float viewTargetZ = FloatSource(*this, instruction, 24, 0x10U);
        const float viewTargetY = FloatSource(*this, instruction, 20, 8U);
        const float viewTargetX = FloatSource(*this, instruction, 16, 4U);
        viewTarget.x = viewTargetX;
        viewTarget.y = viewTargetY;
        viewTarget.z = viewTargetZ;
        ResetTransitionTimer(&viewPosition);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

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

        const float viewTargetZ = FloatSource(*this, instruction, 32, 0x40U);
        const float viewTargetY = FloatSource(*this, instruction, 28, 0x20U);
        const float viewTargetX = FloatSource(*this, instruction, 24, 0x10U);
        viewTarget.x = viewTargetX;
        viewTarget.y = viewTargetY;
        viewTarget.z = viewTargetZ;
        ResetTransitionTimer(&viewPosition);
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
            current, target,
            IntegerSource(*this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
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
            current, target,
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

    case 59:
    {
        const float targetZ = FloatSource(*this, instruction, 24, 0x10U);
        const float targetY = FloatSource(*this, instruction, 20, 8U);
        const float targetX = FloatSource(*this, instruction, 16, 4U);
        const int duration = IntegerSource(*this, instruction, 8, 1U);
        positionTransition.duration = duration;

        const float *reference = reinterpret_cast<const float *>(0x004D2E54);
        // The target copies these transition lanes as raw dwords; memcpy
        // preserves the bitwise stores instead of routing them through x87.
        memcpy(&positionTransition.values[6], reference, sizeof(float) * 3);
        memcpy(&positionTransition.values[9], reference, sizeof(float) * 3);
        positionTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);

        memcpy(&positionTransition.values[0], &unknown024, sizeof(float) * 3);
        const float target[3] = {targetX, targetY, targetZ};
        memcpy(&positionTransition.values[3], target, sizeof(target));
        ResetTransitionTimer(&positionTransition);
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 113:
        viewTransition.duration =
            IntegerSource(*this, instruction, 8, 1U);
        viewTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        viewTransition.values[1] =
            NormalizeAngle(FloatSource(*this, instruction, 16, 4U));
        viewTransition.values[0] = NormalizeAngle(unknown02C);
        viewTransition.values[2] = NormalizeAngle(0.0f);
        viewTransition.values[3] = viewTransition.values[2];
        ResetViewTransitionTimer(&viewTransition);
        flags |= 4U;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 60:
    {
        const float targetY = FloatSource(*this, instruction, 20, 8U);
        const float targetX = FloatSource(*this, instruction, 16, 4U);
        const float target[2] = {targetX, targetY};
        const float current[2] = { scaleX, scaleY };
        const int duration = IntegerSource(*this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        SetScaleTransition(current, target, duration, mode);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 107:
    {
        const float targetY = FloatSource(*this, instruction, 20, 8U);
        const float targetX = FloatSource(*this, instruction, 16, 4U);
        const float target[2] = {targetX, targetY};
        const float current[2] = {unknown058, unknown05C};
        SetSecondaryScaleTransition(
            current,
            target,
            IntegerSource(*this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        flags |= 0x10U;
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
        ResetScalarTransitionTimer(&textureOffsetXTransition);
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
        ResetScalarTransitionTimer(&textureOffsetYTransition);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 67:
        flags ^= (((unsigned int)ReadInstructionInteger(instruction, 8) << 24) ^ flags) & 0x1F000000U;
        if ((flags & 0x1F000000U) == 0x0A000000U)
        {
            StartEffect10();
        }
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

    case 114:
    {
        flags = (flags & 0xEEFFFFFFU) | 0x0E000000U;
        const int vertexCount = IntegerSource(*this, instruction, 8, 1U);
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 98:
        UpdateTextureRectFromView();
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 99:
        stateFlags = (stateFlags & ~0x20U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 5) & 0x20U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 103:
        flags = (flags & 0xEFFFFFFFU) | 0x0F000000U;
        uvScrollX = FloatSource(*this, instruction, 8, 1U);
        uvScrollY = FloatSource(*this, instruction, 12, 2U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

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

    case 112:
        stateFlags = (stateFlags & ~0x100U) |
            (((unsigned int)IntegerSource(*this, instruction, 8, 1U) << 8) & 0x100U);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
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

    case 18:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left + right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 19:
    {
        const float left = FloatSource(*this, instruction, 12, 2U);
        const float right = FloatSource(*this, instruction, 16, 4U);
        *FloatDestination(*this, instruction) = left + right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 20:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left - right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 21:
    {
        const float left = FloatSource(*this, instruction, 12, 2U);
        const float right = FloatSource(*this, instruction, 16, 4U);
        *FloatDestination(*this, instruction) = left - right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 22:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left * right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 23:
    {
        const float left = FloatSource(*this, instruction, 12, 2U);
        const float right = FloatSource(*this, instruction, 16, 4U);
        *FloatDestination(*this, instruction) = left * right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 24:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left / right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 25:
    {
        const float left = FloatSource(*this, instruction, 12, 2U);
        const float right = FloatSource(*this, instruction, 16, 4U);
        *FloatDestination(*this, instruction) = left / right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 26:
    {
        const int left = IntegerSource(*this, instruction, 12, 2U);
        const int right = IntegerSource(*this, instruction, 16, 4U);
        *IntegerDestination(*this, instruction) = left % right;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 27:
    {
        const float right = FloatSource(*this, instruction, 16, 4U);
        const float left = FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)fmod((double)left, (double)right);
        goto AnmVmUpdateStoreUnaryFloat;
    }

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
        floatValue = (float)fmod((double)left, (double)right);
        goto AnmVmUpdateStoreUnaryFloat;
    }

    case 40:
    {
        if ((stateFlags & 2U) != 0)
        {
            const unsigned int limit =
                (unsigned int)IntegerSource(*this, instruction, 12, 2U);
            const unsigned int value = limit == 0
                ? 0
                : reinterpret_cast<Rng *>(0x004D2AD8)->GetU32() % limit;
            *IntegerDestination(*this, instruction) = (int)value;
            scriptDataOffsetCopy = NextInstructionAddress(instruction);
            continue;
        }

        const unsigned int limit =
            (unsigned int)IntegerSource(*this, instruction, 12, 2U);
        const unsigned int value = limit == 0
            ? 0
            : reinterpret_cast<Rng *>(0x004D2AE0)->GetU32() % limit;
        *IntegerDestination(*this, instruction) = (int)value;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 41:
    {
        const volatile unsigned char *vmStateFlags =
            reinterpret_cast<const volatile unsigned char *>(&stateFlags);
        const bool useUiRandom = (*vmStateFlags & 2U) != 0;
        float limit = ReadInstructionFloat(instruction, 12);
        float value;
        if (useUiRandom)
        {
            if ((InstructionOperandFlags(instruction) & 2U) != 0)
            {
                limit = ResolveFloatOperand(limit);
            }
            value = reinterpret_cast<Rng *>(0x004D2AD8)->GetFloat(limit);
        }
        else
        {
            if ((InstructionOperandFlags(instruction) & 2U) != 0)
            {
                limit = ResolveFloatOperand(limit);
            }
            value = reinterpret_cast<Rng *>(0x004D2AE0)->GetFloat(limit);
        }
        float *destination = FloatDestination(*this, instruction);
        *destination = value;
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 42:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)sin(value);
        goto AnmVmUpdateStoreUnaryFloat;
    }

    case 43:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)cos(value);
        goto AnmVmUpdateStoreUnaryFloat;
    }

    case 44:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)tan(value);
        goto AnmVmUpdateStoreUnaryFloat;
    }

    case 45:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)acos(value);
        goto AnmVmUpdateStoreUnaryFloat;
    }

    case 46:
    {
        const double value = (double)FloatSource(*this, instruction, 12, 2U);
        floatValue = (float)atan(value);
        goto AnmVmUpdateStoreUnaryFloat;
    }

AnmVmUpdateStoreUnaryFloat:
    *FloatDestination(*this, instruction) = floatValue;
    scriptDataOffsetCopy = NextInstructionAddress(instruction);
    continue;

    case 47:
    {
        const float angle = FloatSource(*this, instruction, 8, 1U);
        *FloatDestination(*this, instruction) = AddAngles(angle, 0.0f);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;
    }

    case 85:
        flags = (flags & 0x7FFFFFFFU) |
            ((unsigned int)InstructionBytes(instruction)[8] << 31);
        scriptDataOffsetCopy = NextInstructionAddress(instruction);
        continue;

    case 68:
        scriptState = (int)InstructionBytes(instruction)[8];
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

    case 87:
        stateFlags = (stateFlags & ~2U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 1) & 2U);
        break;

    default:
        break;
    }

AnmVmUpdateInstructionAdvance:
            scriptDataOffsetCopy = NextInstructionAddress(instruction);
            continue;

AnmVmUpdateConditionalBranch:
            SetUpdateTimerCurrent(&timer070, ReadInstructionInteger(instruction, 20));
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
        SetUpdateTimerCurrent(&timer070, marker->frame);
        flags |= 1U;
        scriptDataOffsetCopy = NextInstructionAddress(marker);
    }
    goto AnmVmUpdateScriptLoop;

AnmVmUpdateAfterScript:
    if (scriptDataOffsetCopy == 0)
    {
        g_TimeScale = savedGameSpeed;
        return 1;
    }

    {
        const float frameGameSpeed = g_TimeScale;
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
    union AnmVmUpdateScratch
    {
        float floats[12];
        int integers[3];
    } scratch;

    if ((stateFlags & 0x20U) != 0)
    {
        bool cornersValid = true;
        switch ((flags >> 24) & 0x1FU)
        {
        case 0:
            BuildAnmVmViewCornersMode0(this, scratch.floats, scratch.floats + 3, scratch.floats + 6, scratch.floats + 9);
            break;
        case 1:
            BuildAnmVmViewCornersMode1(this, scratch.floats, scratch.floats + 3, scratch.floats + 6, scratch.floats + 9);
            break;
        case 2:
        case 3:
            BuildAnmVmViewCornersMode23(this, scratch.floats, scratch.floats + 3, scratch.floats + 6, scratch.floats + 9);
            break;
        default:
            cornersValid = false;
            break;
        }

        if (cornersValid)
        {
            const float textureWidth = 640.0f;
            const float textureHeight = 480.0f;
            float *textureCoordinates = &textureRect[0][0];
            textureCoordinates[0] = scratch.floats[0] / textureWidth;
            textureCoordinates[1] = scratch.floats[1] / textureHeight;
            if (textureCoordinates[0] < 0.0f) textureCoordinates[0] = 0.0f;
            if (textureCoordinates[1] < 0.0f) textureCoordinates[1] = 0.0f;
            textureCoordinates[2] = scratch.floats[3] / textureWidth;
            textureCoordinates[3] = scratch.floats[4] / textureHeight;
            if (textureCoordinates[2] < 0.0f) textureCoordinates[2] = 0.0f;
            if (textureCoordinates[3] < 0.0f) textureCoordinates[3] = 0.0f;
            textureCoordinates[4] = scratch.floats[6] / textureWidth;
            textureCoordinates[5] = scratch.floats[7] / textureHeight;
            if (textureCoordinates[4] < 0.0f) textureCoordinates[4] = 0.0f;
            if (textureCoordinates[5] < 0.0f) textureCoordinates[5] = 0.0f;
            textureCoordinates[6] = scratch.floats[9] / textureWidth;
            textureCoordinates[7] = scratch.floats[10] / textureHeight;
            if (textureCoordinates[6] < 0.0f) textureCoordinates[6] = 0.0f;
            if (textureCoordinates[7] < 0.0f) textureCoordinates[7] = 0.0f;
        }

    }


    if (viewMode != 0)
    {
        AnmVmVectorTransition *transition =
            reinterpret_cast<AnmVmVectorTransition *>(&viewPosition);
        if ((flags & 0x200U) != 0)
        {
            InterpolateVector(transition, scratch.floats);
            memcpy(&alternatePosition, scratch.floats, sizeof(alternatePosition));
        }
        else
        {
            InterpolateVector(transition, scratch.floats);
            memcpy(&secondaryPosition, scratch.floats, sizeof(secondaryPosition));
        }
    }

    if (vm->colorTransition.duration != 0)
    {
        InterpolateColorVector(&vm->colorTransition, scratch.integers);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&vm->color);
        channels[0] = (unsigned char)scratch.integers[0];
        channels[1] = (unsigned char)scratch.integers[1];
        channels[2] = (unsigned char)scratch.integers[2];
    }

    if (effectTransition.duration != 0)
    {
        unsigned char *channels = reinterpret_cast<unsigned char *>(&color);
        channels[3] = (unsigned char)InterpolateInteger(&effectTransition);
    }

    if (scaleTransition.duration != 0)
    {
        InterpolateScale(&scaleTransition, scratch.floats);
        memcpy(&scaleX, scratch.floats, sizeof(float) * 2);
        flags |= 8U;
    }

    if (secondaryScaleTransition.duration != 0)
    {
        InterpolateScale(&secondaryScaleTransition, scratch.floats);
        memcpy(&scale2X, scratch.floats, sizeof(float) * 2);
        flags |= 0x10U;
    }

    if (positionTransition.duration != 0)
    {
        InterpolateVector(&positionTransition, scratch.floats);
        memcpy(&unknown024, scratch.floats, sizeof(ZunVec3));
        flags |= 4U;
    }

    if (viewTransition.duration != 0)
    {
        InterpolateAngle(&viewTransition, scratch.floats);
        memcpy(&unknown02C, scratch.floats, sizeof(float));
        flags |= 4U;
    }

    if (vm->secondaryColorTransition.duration != 0)
    {
        InterpolateColorVector(&vm->secondaryColorTransition, scratch.integers);
        unsigned char *channels = reinterpret_cast<unsigned char *>(&vm->secondaryColor);
        channels[0] = (unsigned char)scratch.integers[0];
        channels[1] = (unsigned char)scratch.integers[1];
        channels[2] = (unsigned char)scratch.integers[2];
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
    if (geometryMode != 9U && geometryMode != 13U && geometryMode != 14U)
    {
        break;
    }

    AnmVmEffect10Vertex *vertices;
    // Keep the count signed: the target performs its divisor calculations
    // even for zero/negative counts, and only the later loop test rejects them.
    int count;
    int divisor;
    float angleStep;
    float vStep;
    float angle;
    float v;
    if (geometryMode == 9U)
    {
        count = integerVariables[0];
        angle = unknown02C;
        divisor = count - 1;
        vertices = reinterpret_cast<AnmVmEffect10Vertex *>(extraData);
        const float divisorAsFloat = (float)divisor;
        angleStep = (float)(
            kAnmVmGeometryTwoPi / (double)divisorAsFloat);
        v = 0.0f;
        vStep = integerVariables[1] / divisorAsFloat;
    }
    else
    {
        count = integerVariables[0];
        angle = NormalizeAngle(unknown02C - unknown024 * 0.5f);
        divisor = count - 1;
        vertices = reinterpret_cast<AnmVmEffect10Vertex *>(extraData);
        const float divisorAsFloat = (float)divisor;
        angleStep = unknown024 / divisorAsFloat;
        v = 0.0f;
        vStep = integerVariables[1] / divisorAsFloat;
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
        // Keep the target's operand evaluation order: secondary first,
        // alternate second, then primary position. Volatile reads prevent
        // LTCG from folding this path into the mode-9 addition sequence.
        const volatile float &alternateX = alternatePosition.x;
        const volatile float &alternateY = alternatePosition.y;
        const volatile float &alternateZ = alternatePosition.z;
        centerX = alternateX + secondaryPosition.x;
        centerY = alternateY + secondaryPosition.y;
        centerZ = alternateZ + secondaryPosition.z;
        centerX = position.x + centerX;
        centerY = position.y + centerY;
        centerZ = position.z + centerZ;
    }

    // Mode 14 replaces the first normalized angle after local-center
    // accumulation but before the parent-center contribution, as in Update.
    if (geometryMode == 14U)
    {
        angle = NormalizeAngle(unknown02C);
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
        AnmVmEffect10Vertex *pairVertices = vertices;
        for (int i = 0; i < divisor; ++i)
        {
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
            pairVertices += 2;
        }

        pairVertices[0] = vertices[0];
        pairVertices[1] = vertices[1];
        pairVertices[0].v = v + textureScrollY;
        pairVertices[1].v = v + textureScrollY;
        break;
    }

    AnmVmEffect10Vertex *pairVertices = vertices;
    for (int i = 0; i < count; ++i)
    {
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
        pairVertices += 2;
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
        // Preserve VC10's __ftol2_sse conversion and its runtime rounding mode.
        timer070.current = (int)timer070.currentF;
    }
    g_TimeScale = savedGameSpeed;
    return 0;
}
