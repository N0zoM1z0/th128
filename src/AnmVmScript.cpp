#include "AnmVm.hpp"
#include "AnmResource.hpp"
#include "AngleMath.hpp"

#include <math.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

namespace
{
    const unsigned int kMaximumInstructionsPerTick = 0x10000;

    const unsigned char *InstructionBytes(const AnmVmInstructionHeader *instruction)
    {
        return reinterpret_cast<const unsigned char *>(instruction);
    }

    int ReadInstructionInteger(const AnmVmInstructionHeader *instruction, unsigned int offset)
    {
        return *reinterpret_cast<const int *>(InstructionBytes(instruction) + offset);
    }

    unsigned short ReadInstructionUnsignedShort(
        const AnmVmInstructionHeader *instruction,
        unsigned int offset)
    {
        return *reinterpret_cast<const unsigned short *>(InstructionBytes(instruction) + offset);
    }

    float ReadInstructionFloat(const AnmVmInstructionHeader *instruction, unsigned int offset)
    {
        return *reinterpret_cast<const float *>(InstructionBytes(instruction) + offset);
    }

    int *IntegerDestination(AnmVm *vm, const AnmVmInstructionHeader *instruction)
    {
        int *destination = reinterpret_cast<int *>(
            const_cast<unsigned char *>(InstructionBytes(instruction) + 8));
        if ((instruction->flags & 1U) != 0)
        {
            destination = vm->ResolveIntegerOperandAddress(destination);
        }
        return destination;
    }

    float *FloatDestination(AnmVm *vm, const AnmVmInstructionHeader *instruction)
    {
        float *destination = reinterpret_cast<float *>(
            const_cast<unsigned char *>(InstructionBytes(instruction) + 8));
        if ((instruction->flags & 1U) != 0)
        {
            destination = vm->ResolveFloatOperandAddress(destination);
        }
        return destination;
    }

    int IntegerSource(AnmVm *vm, const AnmVmInstructionHeader *instruction, unsigned int offset, unsigned char flag)
    {
        int value = ReadInstructionInteger(instruction, offset);
        if ((instruction->flags & flag) != 0)
        {
            value = vm->ResolveIntegerOperand(value);
        }
        return value;
    }

    float FloatSource(AnmVm *vm, const AnmVmInstructionHeader *instruction, unsigned int offset, unsigned char flag)
    {
        float value = ReadInstructionFloat(instruction, offset);
        if ((instruction->flags & flag) != 0)
        {
            value = vm->ResolveFloatOperand(value);
        }
        return value;
    }

    unsigned int NextInstructionAddress(const AnmVmInstructionHeader *instruction)
    {
        return (unsigned int)(InstructionBytes(instruction) + instruction->size);
    }


}

void AnmVm::SetEffectState(
    void *resource,
    unsigned char index,
    unsigned char parameter0,
    unsigned char parameter1)
{
    effectResource = resource;
    effectIndex = index;
    effectParameter0 = parameter0;
    effectParameter1 = parameter1;
    effectProgress0 = 0;
    effectProgress1 = 0;
    timer14C.Reset();
}

void AnmVm::SetSecondaryEffectState(
    void *resource,
    unsigned int index,
    unsigned char parameter0,
    unsigned char parameter1)
{
    secondaryEffectResource = resource;
    secondaryEffectIndex = index & 0xFFU;
    secondaryEffectParameter0 = parameter0;
    secondaryEffectParameter1 = parameter1;
    timer2B4.Reset();
}

bool AnmVm::ExecuteWaitCommand(bool clearActiveFlag)
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

    while (marker != 0 && marker->opcode != 0xFFFFU)
    {
        if (marker->opcode == 0x40U)
        {
            if (marker->frame == currentFrame)
            {
                match = marker;
                break;
            }
            if (marker->frame == -1)
            {
                fallback = marker;
            }
        }

        if (marker->size < sizeof(AnmVmInstructionHeader))
        {
            break;
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

    timer070.SetCurrent(timer070.current);
    waitScriptAddress = scriptDataOffsetCopy;
    timer070.SetCurrent(match->frame);
    flags |= 1U;
    scriptDataOffsetCopy = NextInstructionAddress(match);
    return true;
}

bool AnmVm::ExecuteBasicInstruction(const AnmVmInstructionHeader *instruction)
{
    if (instruction == 0 || instruction->size < sizeof(AnmVmInstructionHeader))
    {
        return false;
    }

    const unsigned int nextInstruction = NextInstructionAddress(instruction);
    int *integerDestination;
    float *floatDestination;
    int integerValue;
    float floatValue;

    switch (instruction->opcode)
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

        if (fileIndex < 0 || BindAnmFile(fileIndex, resource) != 0)
        {
            return false;
        }
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 4:
        timer070.SetCurrent(ReadInstructionInteger(instruction, 12));
        scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 8);
        return true;

    case 5:
        integerDestination = IntegerDestination(this, instruction);
        --(*integerDestination);
        integerValue = IntegerSource(this, instruction, 8, 1U);
        if (integerValue > 0)
        {
            timer070.SetCurrent(ReadInstructionInteger(instruction, 16));
            scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 12);
        }
        else
        {
            scriptDataOffsetCopy = nextInstruction;
        }
        return true;

    case 6:
        *IntegerDestination(this, instruction) =
            IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 7:
        *FloatDestination(this, instruction) =
            FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 8:
        integerDestination = IntegerDestination(this, instruction);
        *integerDestination += IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 9:
        floatDestination = FloatDestination(this, instruction);
        *floatDestination += FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 10:
        integerDestination = IntegerDestination(this, instruction);
        *integerDestination -= IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 11:
        floatDestination = FloatDestination(this, instruction);
        *floatDestination -= FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 12:
        integerDestination = IntegerDestination(this, instruction);
        *integerDestination *= IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 13:
        floatDestination = FloatDestination(this, instruction);
        *floatDestination *= FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 14:
        integerDestination = IntegerDestination(this, instruction);
        *integerDestination /= IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 15:
        floatDestination = FloatDestination(this, instruction);
        *floatDestination /= FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 16:
        integerDestination = IntegerDestination(this, instruction);
        *integerDestination %= IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 17:
        floatDestination = FloatDestination(this, instruction);
        floatValue = FloatSource(this, instruction, 8, 1U);
        *floatDestination = (float)fmod(
            (double)floatValue,
            (double)FloatSource(this, instruction, 12, 2U));
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 18:
        floatValue = ReadInstructionFloat(instruction, 12);
        if ((instruction->flags & 2U) != 0)
        {
            floatValue = (float)ResolveIntegerOperand(ReadInstructionInteger(instruction, 12));
        }
        *IntegerDestination(this, instruction) =
            (int)floatValue + IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 19:
        *FloatDestination(this, instruction) =
            FloatSource(this, instruction, 12, 2U) +
            FloatSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 20:
        floatValue = ReadInstructionFloat(instruction, 16);
        if ((instruction->flags & 4U) != 0)
        {
            floatValue = (float)ResolveIntegerOperand(ReadInstructionInteger(instruction, 16));
        }
        *IntegerDestination(this, instruction) =
            IntegerSource(this, instruction, 12, 2U) - (int)floatValue;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 21:
        *FloatDestination(this, instruction) =
            FloatSource(this, instruction, 12, 2U) -
            FloatSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 22:
        floatValue = ReadInstructionFloat(instruction, 12);
        if ((instruction->flags & 2U) != 0)
        {
            floatValue = (float)ResolveIntegerOperand(ReadInstructionInteger(instruction, 12));
        }
        *IntegerDestination(this, instruction) =
            (int)floatValue * IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 23:
        *FloatDestination(this, instruction) =
            FloatSource(this, instruction, 12, 2U) *
            FloatSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 24:
        floatValue = ReadInstructionFloat(instruction, 12);
        if ((instruction->flags & 2U) != 0)
        {
            floatValue = (float)ResolveIntegerOperand(ReadInstructionInteger(instruction, 12));
        }
        integerValue = (int)floatValue;
        *IntegerDestination(this, instruction) =
            integerValue / IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 25:
        *FloatDestination(this, instruction) =
            FloatSource(this, instruction, 12, 2U) /
            FloatSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 26:
        floatValue = ReadInstructionFloat(instruction, 12);
        if ((instruction->flags & 2U) != 0)
        {
            floatValue = (float)ResolveIntegerOperand(ReadInstructionInteger(instruction, 12));
        }
        integerValue = (int)floatValue;
        *IntegerDestination(this, instruction) =
            integerValue % IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 27:
        *FloatDestination(this, instruction) =
            (float)fmod(
                (double)FloatSource(this, instruction, 12, 2U),
                (double)FloatSource(this, instruction, 16, 4U));
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    {
        bool takeBranch = false;
        const unsigned short opcode = instruction->opcode;
        if ((opcode & 1U) == 0)
        {
            const int left = IntegerSource(this, instruction, 8, 1U);
            const int right = IntegerSource(this, instruction, 12, 2U);
            switch (opcode)
            {
            case 28: takeBranch = (left == right); break;
            case 30: takeBranch = (left != right); break;
            case 32: takeBranch = (left < right); break;
            case 34: takeBranch = (left <= right); break;
            case 36: takeBranch = (left > right); break;
            case 38: takeBranch = (left >= right); break;
            }
        }
        else
        {
            const float left = FloatSource(this, instruction, 8, 1U);
            const float right = FloatSource(this, instruction, 12, 2U);
            switch (opcode)
            {
            case 29: takeBranch = (left == right); break;
            case 31: takeBranch = (left != right); break;
            case 33: takeBranch = (left < right); break;
            case 35: takeBranch = (left <= right); break;
            case 37: takeBranch = (left > right); break;
            case 39: takeBranch = (left >= right); break;
            }
        }

        if (takeBranch)
        {
            timer070.SetCurrent(ReadInstructionInteger(instruction, 20));
            scriptDataOffsetCopy = scriptDataOffset + ReadInstructionInteger(instruction, 16);
        }
        else
        {
            scriptDataOffsetCopy = nextInstruction;
        }
        return true;
    }

    case 40:
    {
        const unsigned int limit = (unsigned int)IntegerSource(this, instruction, 12, 2U);
        const unsigned int value = (limit == 0) ? 0 : NextRandomValue() % limit;
        *IntegerDestination(this, instruction) = (int)value;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 41:
    {
        const float limit = FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)(RandomUnit() * limit);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 42:
    {
        const double value = (double)FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)sin(value);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 43:
    {
        const double value = (double)FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)cos(value);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 44:
    {
        const double value = (double)FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)tan(value);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 45:
    {
        const double value = (double)FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)asin(value);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 46:
    {
        const double value = (double)FloatSource(this, instruction, 12, 2U);
        *FloatDestination(this, instruction) = (float)atan(value);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 47:
    {
        const float angle = FloatSource(this, instruction, 8, 1U);
        *FloatDestination(this, instruction) = NormalizeAngle(angle);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 48:
    {
        const float x = FloatSource(this, instruction, 8, 1U);
        const float y = FloatSource(this, instruction, 12, 2U);
        const float z = FloatSource(this, instruction, 16, 4U);
        if ((flags & 0x200U) == 0)
        {
            secondaryPosition.x = x;
            secondaryPosition.y = y;
            secondaryPosition.z = z;
        }
        else
        {
            alternatePosition.x = x;
            alternatePosition.y = y;
            alternatePosition.z = z;
        }
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 49:
        unknown024 = FloatSource(this, instruction, 8, 1U);
        unknown028 = FloatSource(this, instruction, 12, 2U);
        unknown02C = FloatSource(this, instruction, 16, 4U);
        flags |= 4U;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 50:
        scaleX = FloatSource(this, instruction, 8, 1U);
        scaleY = FloatSource(this, instruction, 12, 2U);
        flags |= 8U;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 51:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&color);
        colorBytes[3] = (unsigned char)IntegerSource(this, instruction, 8, 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 52:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&color);
        colorBytes[2] = (unsigned char)IntegerSource(this, instruction, 8, 1U);
        colorBytes[1] = (unsigned char)IntegerSource(this, instruction, 12, 2U);
        colorBytes[0] = (unsigned char)IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 53:
        scriptVector.x = FloatSource(this, instruction, 8, 1U);
        scriptVector.y = FloatSource(this, instruction, 12, 2U);
        scriptVector.z = FloatSource(this, instruction, 16, 4U);
        flags |= 4U;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 54:
        unknown050 = FloatSource(this, instruction, 8, 1U);
        unknown054 = FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 70:
        textureOffsetX = FloatSource(this, instruction, 8, 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 71:
        textureOffsetY = FloatSource(this, instruction, 8, 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 72:
        flags = (flags & ~0x00000001U) |
            ((unsigned int)ReadInstructionInteger(instruction, 8) & 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 73:
        flags = (flags & ~0x00001000U) |
            (((unsigned int)ReadInstructionInteger(instruction, 8) << 12) & 0x00001000U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 74:
        flags = (flags & ~0x00004000U) |
            (((unsigned int)ReadInstructionInteger(instruction, 8) << 14) & 0x00004000U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 80:
        flags = (flags & ~0x00030000U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 16) & 0x00030000U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 82:
        flags = (flags & ~0x40000000U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 30) & 0x40000000U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 83:
    {
        const ZunVec3 oldPosition = position;
        position.x = 0.0f;
        position.y = 0.0f;
        position.z = 0.0f;
        secondaryPosition = oldPosition;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 85:
        flags = (flags & 0x7FFFFFFFU) |
            ((unsigned int)InstructionBytes(instruction)[8] << 31);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 86:
        stateFlags = (stateFlags & ~1U) |
            ((unsigned int)IntegerSource(this, instruction, 8, 1U) & 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 87:
        stateFlags = (stateFlags & ~2U) |
            (((unsigned int)InstructionBytes(instruction)[8] << 1) & 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 89:
        stateFlags = (stateFlags & ~4U) |
            (((unsigned int)IntegerSource(this, instruction, 8, 1U) << 2) & 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 99:
        stateFlags = (stateFlags & ~0x20U) |
            (((unsigned int)IntegerSource(this, instruction, 8, 1U) << 5) & 0x20U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 55:
    {
        void *resource = reinterpret_cast<void *>(
            (instruction->flags & 2U) != 0
                ? (unsigned int)ResolveIntegerOperand(ReadInstructionInteger(instruction, 12))
                : (unsigned int)ReadInstructionInteger(instruction, 12));
        const unsigned char parameter1 = InstructionBytes(instruction)[8];
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(resource, 0, alpha, parameter1);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 58:
    {
        void *resource = reinterpret_cast<void *>(
            (instruction->flags & 1U) != 0
                ? (unsigned int)ResolveIntegerOperand(ReadInstructionInteger(instruction, 8))
                : (unsigned int)ReadInstructionInteger(instruction, 8));
        const unsigned char index = InstructionBytes(instruction)[12];
        const unsigned char parameter1 = (unsigned char)IntegerSource(this, instruction, 16, 4U);
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(resource, index, alpha, parameter1);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 56:
    {
        viewMode = IntegerSource(this, instruction, 8, 1U);
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

        viewTarget.x = FloatSource(this, instruction, 16, 4U);
        viewTarget.y = FloatSource(this, instruction, 20, 8U);
        viewTarget.z = FloatSource(this, instruction, 24, 0x10U);
        timer0D4.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 57:
    {
        const unsigned char sourceBlue = (unsigned char)color;
        const unsigned char sourceGreen = (unsigned char)(color >> 8);
        const unsigned char sourceRed = (unsigned char)(color >> 16);
        const unsigned char targetAlpha =
            (unsigned char)IntegerSource(this, instruction, 24, 0x10U);
        const unsigned char targetGreen =
            (unsigned char)IntegerSource(this, instruction, 20, 8U);
        const unsigned char targetBlue =
            (unsigned char)IntegerSource(this, instruction, 16, 4U);

        colorTransitionDuration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        colorTransitionMode = InstructionBytes(instruction)[12];
        colorTransitionValues[0] = sourceBlue;
        colorTransitionValues[1] = sourceGreen;
        colorTransitionValues[2] = sourceRed;
        colorTransitionValues[3] = targetAlpha;
        colorTransitionValues[4] = targetGreen;
        colorTransitionValues[5] = targetBlue;
        memset(colorTransitionOffsets, 0, sizeof(colorTransitionOffsets));
        timer120.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 59:
    {
        positionTransitionDuration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        positionTransitionMode = ReadInstructionInteger(instruction, 12);
        positionTransitionStart.x = unknown024;
        positionTransitionStart.y = unknown028;
        positionTransitionStart.z = unknown02C;
        positionTransitionTarget.x = FloatSource(this, instruction, 16, 4U);
        positionTransitionTarget.y = FloatSource(this, instruction, 20, 8U);
        positionTransitionTarget.z = FloatSource(this, instruction, 24, 0x10U);
        positionTransitionReferenceA.x = *reinterpret_cast<const float *>(0x004D2E54);
        positionTransitionReferenceA.y = *reinterpret_cast<const float *>(0x004D2E58);
        positionTransitionReferenceA.z = *reinterpret_cast<const float *>(0x004D2E5C);
        positionTransitionReferenceB = positionTransitionReferenceA;
        timer198.Reset();
        flags |= 4U;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 60:
    {
        scaleTransitionDuration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        scaleTransitionMode = InstructionBytes(instruction)[12];
        scaleTransitionStart[0] = scaleX;
        scaleTransitionStart[1] = scaleY;
        scaleTransitionTarget[0] = FloatSource(this, instruction, 16, 4U);
        scaleTransitionTarget[1] = FloatSource(this, instruction, 20, 8U);
        timer200.Reset();
        flags |= 8U;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 61:
        flags ^= 0x400U;
        flags |= 8U;
        scaleX = -scaleX;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 62:
        flags ^= 0x800U;
        flags |= 8U;
        scaleY = -scaleY;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 65:
    {
        const unsigned int xFlags = ReadInstructionUnsignedShort(instruction, 8);
        const unsigned int yFlags = ReadInstructionUnsignedShort(instruction, 10);
        flags = (flags & ~0x00300000U) | ((xFlags << 20) & 0x00300000U);
        flags = (flags & ~0x00C00000U) | ((yFlags << 22) & 0x00C00000U);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 66:
        flags = (flags & ~0x000000E0U) |
            (((unsigned int)ReadInstructionInteger(instruction, 8) << 5) & 0x000000E0U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 67:
        flags = (flags & ~0x1F000000U) |
            (((unsigned int)IntegerSource(this, instruction, 8, 1U) << 24) & 0x1F000000U);
        if ((flags & 0x1F000000U) == 0x0A000000U)
        {
            StartEffect10();
        }
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 63:
        return ExecuteWaitCommand(false);

    case 68:
        scriptState = (int)InstructionBytes(instruction)[8];
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 69:
        return ExecuteWaitCommand(true);

    case 81:
        timer3FC.SetCurrent(timer3FC.current);
        scriptDataOffsetCopy = waitScriptAddress;
        return true;

    case 76:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&secondaryColor);
        colorBytes[2] = (unsigned char)IntegerSource(this, instruction, 8, 1U);
        colorBytes[1] = (unsigned char)IntegerSource(this, instruction, 12, 2U);
        colorBytes[0] = (unsigned char)IntegerSource(this, instruction, 16, 4U);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 77:
    {
        unsigned char *colorBytes = reinterpret_cast<unsigned char *>(&secondaryColor);
        colorBytes[3] = (unsigned char)IntegerSource(this, instruction, 8, 1U);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 78:
    {
        const unsigned char sourceBlue = (unsigned char)secondaryColor;
        const unsigned char sourceGreen = (unsigned char)(secondaryColor >> 8);
        const unsigned char sourceRed = (unsigned char)(secondaryColor >> 16);
        const unsigned char targetAlpha =
            (unsigned char)IntegerSource(this, instruction, 24, 0x10U);
        const unsigned char targetGreen =
            (unsigned char)IntegerSource(this, instruction, 20, 8U);
        const unsigned char targetBlue =
            (unsigned char)IntegerSource(this, instruction, 16, 4U);

        secondaryColorTransitionDuration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        secondaryColorTransitionMode = InstructionBytes(instruction)[12];
        secondaryColorTransitionValues[0] = sourceBlue;
        secondaryColorTransitionValues[1] = sourceGreen;
        secondaryColorTransitionValues[2] = sourceRed;
        secondaryColorTransitionValues[3] = targetAlpha;
        secondaryColorTransitionValues[4] = targetGreen;
        secondaryColorTransitionValues[5] = targetBlue;
        memset(secondaryColorTransitionOffsets, 0, sizeof(secondaryColorTransitionOffsets));
        timer288.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 79:
    {
        void *resource = reinterpret_cast<void *>(
            (unsigned int)IntegerSource(this, instruction, 12, 2U));
        const unsigned int index = (unsigned int)IntegerSource(this, instruction, 16, 4U);
        const unsigned char parameter1 = InstructionBytes(instruction)[8];
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetSecondaryEffectState(resource, index, alpha, parameter1);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 84:
    {
        const int vertexCount = IntegerSource(this, instruction, 8, 1U);
        flags = (flags & 0xE9FFFFFFU) | 0x09000000U;
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 101:
    {
        const int vertexCount = IntegerSource(this, instruction, 8, 1U);
        flags = (flags & 0xEDFFFFFFU) | 0x0D000000U;
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    default:
        return false;
    }
}

unsigned int AnmVm::ExecuteReadyInstructions()
{
    if (scriptDataOffsetCopy == 0 || (flags & 0x80000U) != 0)
    {
        return 0;
    }

    if (currentFrame != 0 && !ExecuteWaitCommand(false))
    {
        return 0;
    }

    unsigned int executed = 0;
    while (executed < kMaximumInstructionsPerTick)
    {
        const AnmVmInstructionHeader *instruction =
            reinterpret_cast<const AnmVmInstructionHeader *>(scriptDataOffsetCopy);

        if (instruction->frame > timer070.current)
        {
            break;
        }
        if (!ExecuteBasicInstruction(instruction))
        {
            break;
        }
        ++executed;
    }

    return executed;
}
