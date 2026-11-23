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

    unsigned short InstructionOperandFlags(
        const AnmVmInstructionHeader *instruction)
    {
        return ReadInstructionUnsignedShort(instruction, 6);
    }

    int IntegerSource(
        AnmVm *vm,
        const AnmVmInstructionHeader *instruction,
        unsigned int offset,
        unsigned short flag)
    {
        int value = ReadInstructionInteger(instruction, offset);
        if ((InstructionOperandFlags(instruction) & flag) != 0)
        {
            value = vm->ResolveIntegerOperand(value);
        }
        return value;
    }

    float FloatSource(
        AnmVm *vm,
        const AnmVmInstructionHeader *instruction,
        unsigned int offset,
        unsigned short flag)
    {
        float value = ReadInstructionFloat(instruction, offset);
        if ((InstructionOperandFlags(instruction) & flag) != 0)
        {
            value = vm->ResolveFloatOperand(value);
        }
        return value;
    }

    unsigned int NextInstructionAddress(const AnmVmInstructionHeader *instruction)
    {
        return (unsigned int)(InstructionBytes(instruction) + instruction->size);
    }

    __forceinline void UpdateTextureRectFromViewForFrame(AnmVm *vm)
    {
        float corners[12];
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
        default:
            return;
        }

        float *textureCoordinates = &vm->textureRect[0][0];
        for (unsigned int i = 0; i != 4; ++i)
        {
            const float pixels[2] = {corners[i * 3], corners[i * 3 + 1]};
            float normalized[2];
            NormalizeAnmCoordinates(normalized, pixels);
            textureCoordinates[i * 2] = normalized[0];
            textureCoordinates[i * 2 + 1] = normalized[1];
        }
    }

    void LinkChildVm(AnmVm *parent, AnmVm *child)
    {
        AnmVmListNode *head = &parent->childNode;
        AnmVmListNode *node = &child->childNode;
        node->vm = child;
        node->previous = head;
        node->next = head->next;
        if (head->next != 0)
        {
            head->next->previous = node;
        }
        head->next = node;
    }

    unsigned int SpawnChildVm(
        AnmVm *parent,
        int scriptIndex,
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
        unsigned int id;
        if ((listMode & 4U) != 0)
        {
            id = (listMode & 2U) != 0
                ? manager->RegisterUiVmAtHead(child)
                : manager->RegisterUiVmAtTail(child);
        }
        else
        {
            id = (listMode & 2U) != 0
                ? manager->RegisterWorldVmAtHead(child)
                : manager->RegisterWorldVmAtTail(child);
        }

        LinkChildVm(parent, child);
        return id;
    }

    unsigned int SpawnChildVmAtPosition(AnmVm *parent, int scriptIndex)
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
        return manager->RegisterWorldVmAtTail(child);
    }

    AnmResource *GetDefaultAnmResource()
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
    if (((flags >> 24) & 0x1FU) > 3U)
    {
        return;
    }

    float corners[12];
    BuildAnmVmViewCorners(this, corners);
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

    timer070.SetCurrent(timer070.current);
    waitScriptAddress = scriptDataOffsetCopy;
    timer070.SetCurrent(match->frame);
    flags |= 1U;
    scriptDataOffsetCopy = NextInstructionAddress(match);
    return true;
}

__forceinline bool AnmVm::ExecuteBasicInstruction(const AnmVmInstructionHeader *instruction)
{
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

        AnmResource *fileResource = resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }
        BindAnmFile(fileIndex, fileResource);
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
        const unsigned int duration = (unsigned int)IntegerSource(this, instruction, 12, 2U);
        const unsigned char parameter1 = InstructionBytes(instruction)[8];
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(duration, 0, alpha, parameter1);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 58:
    {
        const unsigned int duration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        const unsigned char parameter1 = (unsigned char)IntegerSource(this, instruction, 16, 4U);
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&color)[3];
        SetEffectState(duration, mode, alpha, parameter1);
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

        const unsigned char target[3] = {targetAlpha, targetGreen, targetBlue};
        const unsigned char current[3] = {sourceBlue, sourceGreen, sourceRed};
        SetColorTransition(
            target, current,
            IntegerSource(this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 59:
    {
        const float target[3] = {
            FloatSource(this, instruction, 16, 4U),
            FloatSource(this, instruction, 20, 8U),
            FloatSource(this, instruction, 24, 0x10U)
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
            IntegerSource(this, instruction, 8, 1U);
        positionTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        positionTransition.timer.Reset();
        flags |= 4U;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 60:
    {
        const float target[2] = {
            FloatSource(this, instruction, 16, 4U),
            FloatSource(this, instruction, 20, 8U)
        };
        const float current[2] = { scaleX, scaleY };
        const int duration = IntegerSource(this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        SetScaleTransition(target, current, duration, mode);
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

    case 75:
    {
        const int frameOffset = IntegerSource(this, instruction, 8, 1U);
        timer070.Add((float)-frameOffset);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

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

        const unsigned char target[3] = {targetAlpha, targetGreen, targetBlue};
        const unsigned char current[3] = {sourceBlue, sourceGreen, sourceRed};
        SetSecondaryColorTransition(
            target, current,
            IntegerSource(this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 79:
    {
        const unsigned int duration = (unsigned int)IntegerSource(this, instruction, 8, 1U);
        const unsigned char mode = InstructionBytes(instruction)[12];
        const unsigned char parameter1 = (unsigned char)IntegerSource(this, instruction, 16, 4U);
        const unsigned char alpha = reinterpret_cast<unsigned char *>(&secondaryColor)[3];
        SetSecondaryEffectState(duration, mode, alpha, parameter1);
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

    case 88:
        SpawnChildVm(
            this,
            IntegerSource(this, instruction, 8, 1U),
            0);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 90:
        SpawnChildVm(
            this,
            IntegerSource(this, instruction, 8, 1U),
            4);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 91:
        SpawnChildVm(
            this,
            IntegerSource(this, instruction, 8, 1U),
            2);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 92:
        SpawnChildVm(
            this,
            IntegerSource(this, instruction, 8, 1U),
            6);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 95:
        SpawnChildVmAtPosition(
            this,
            IntegerSource(this, instruction, 8, 1U));
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 96:
    {
        const unsigned int childId = SpawnChildVm(
            this,
            IntegerSource(this, instruction, 8, 1U),
            0);
        AnmManager *manager = GetGlobalAnmManager();
        AnmVm *child = manager != 0 ? manager->GetVmById(childId) : 0;
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 97:
    {
        const unsigned int childId = SpawnChildVmAtPosition(
            this,
            IntegerSource(this, instruction, 8, 1U));
        AnmManager *manager = GetGlobalAnmManager();
        AnmVm *child = manager != 0 ? manager->GetVmById(childId) : 0;
        if (child != 0)
        {
            child->alternatePosition.x = FloatSource(this, instruction, 12, 2U);
            child->alternatePosition.y = FloatSource(this, instruction, 16, 4U);
        }
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 98:
        UpdateTextureRectFromView();
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 100:
    {
        const float referenceAX =
            FloatSource(this, instruction, 12, 2U);
        const float referenceAY =
            FloatSource(this, instruction, 16, 4U);
        const float referenceAZ =
            FloatSource(this, instruction, 20, 8U);
        const float referenceBX =
            FloatSource(this, instruction, 36, 0x80U);
        const float referenceBY =
            FloatSource(this, instruction, 40, 0x100U);
        const float referenceBZ =
            FloatSource(this, instruction, 44, 0x200U);
        const int duration = IntegerSource(this, instruction, 8, 1U);

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

        viewTarget.x = FloatSource(this, instruction, 24, 0x10U);
        viewTarget.y = FloatSource(this, instruction, 28, 0x20U);
        viewTarget.z = FloatSource(this, instruction, 32, 0x40U);
        timer0D4.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 102:
    {
        flags |= 1U;
        int fileIndex;
        if (anmFileResolver == 0)
        {
            const int firstIndex = IntegerSource(this, instruction, 8, 1U);
            const unsigned int variantCount =
                (unsigned int)IntegerSource(this, instruction, 12, 2U);
            const unsigned int randomValue = NextGlobalAnmRandomValue();
            fileIndex = firstIndex + (int)(randomValue % variantCount);
        }
        else
        {
            // The target consumes both encoded operands and one value from
            // the shared global RNG before invoking the resolver callback.
            (void)IntegerSource(this, instruction, 8, 1U);
            (void)IntegerSource(this, instruction, 12, 2U);
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
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 93:
    {
        textureOffsetXTransition.duration =
            IntegerSource(this, instruction, 8, 1U);
        textureOffsetXTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        textureOffsetXTransition.values[0] = textureOffsetX;
        textureOffsetXTransition.values[1] =
            FloatSource(this, instruction, 16, 4U);
        textureOffsetXTransition.values[2] = 0.0f;
        textureOffsetXTransition.values[3] = 0.0f;
        textureOffsetXTransition.timer.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 94:
    {
        textureOffsetYTransition.duration =
            IntegerSource(this, instruction, 8, 1U);
        textureOffsetYTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        textureOffsetYTransition.values[0] = textureOffsetY;
        textureOffsetYTransition.values[1] =
            FloatSource(this, instruction, 16, 4U);
        textureOffsetYTransition.values[2] = 0.0f;
        textureOffsetYTransition.values[3] = 0.0f;
        textureOffsetYTransition.timer.Reset();
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 103:
        flags = (flags & 0xEFFFFFFFU) | 0x0F000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        uvScrollY = FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 104:
        flags = (flags & 0xF0FFFFFFU) | 0x10000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        integerVariables[0] = IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 105:
        flags = (flags & 0xF1FFFFFFU) | 0x11000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        integerVariables[0] = IntegerSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 106:
        unknown058 = FloatSource(this, instruction, 8, 1U);
        unknown05C = FloatSource(this, instruction, 12, 2U);
        flags |= 0x10U;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 107:
    {
        const float target[2] = {
            FloatSource(this, instruction, 16, 4U),
            FloatSource(this, instruction, 20, 8U)
        };
        const float current[2] = {unknown058, unknown05C};
        SetSecondaryScaleTransition(
            target,
            current,
            IntegerSource(this, instruction, 8, 1U),
            InstructionBytes(instruction)[12]);
        flags |= 0x10U;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 108:
        flags = (flags & 0xF3FFFFFFU) | 0x13000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        uvScrollY = FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 109:
        flags = (flags & 0xF4FFFFFFU) | 0x14000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        uvScrollY = FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 110:
        flags = (flags & 0xF5FFFFFFU) | 0x15000000U;
        uvScrollX = FloatSource(this, instruction, 8, 1U);
        uvScrollY = FloatSource(this, instruction, 12, 2U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 111:
        stateFlags = (stateFlags & ~0xC0U) |
            (((unsigned int)IntegerSource(this, instruction, 8, 1U) << 6) & 0xC0U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 112:
        stateFlags = (stateFlags & ~0x100U) |
            (((unsigned int)IntegerSource(this, instruction, 8, 1U) << 8) & 0x100U);
        scriptDataOffsetCopy = nextInstruction;
        return true;

    case 114:
    {
        const int vertexCount = IntegerSource(this, instruction, 8, 1U);
        flags = (flags & 0xEEFFFFFFU) | 0x0E000000U;
        extraData = malloc(vertexCount * 0x38);
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 113:
        viewTransition.duration =
            IntegerSource(this, instruction, 8, 1U);
        viewTransition.mode =
            (unsigned int)ReadInstructionInteger(instruction, 12);
        viewTransition.values[0] = NormalizeAngle(unknown02C);
        viewTransition.values[1] =
            NormalizeAngle(FloatSource(this, instruction, 16, 4U));
        viewTransition.values[2] = 0.0f;
        viewTransition.values[3] = 0.0f;
        viewTransition.timer.Reset();
        flags |= 4U;
        scriptDataOffsetCopy = nextInstruction;
        return true;

    default:
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }
}

__forceinline void AnmVm::ExecuteReadyInstructions()
{
    if (scriptDataOffsetCopy == 0 || (flags & 0x80000U) != 0)
    {
        return;
    }

    if (currentFrame != 0 && !ExecuteWaitCommand(false))
    {
        return;
    }

    for (;;)
    {
        const AnmVmInstructionHeader *instruction =
            reinterpret_cast<const AnmVmInstructionHeader *>(scriptDataOffsetCopy);

        if (instruction->frame > timer070.current)
        {
            return;
        }

        if (instruction->opcode == 0xFFFFU || instruction->opcode == 1U)
        {
            flags &= ~1U;
            scriptDataOffsetCopy = 0;
            return;
        }
        if (instruction->opcode == 2U)
        {
            scriptDataOffsetCopy = 0;
            return;
        }
        if (!ExecuteBasicInstruction(instruction))
        {
            return;
        }
    }
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

        timer->SetCurrent(*duration);
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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float h00 = (2.0f * t + 1.0f) * tMinusOne * tMinusOne;
            const float h01 = t * t * (3.0f - 2.0f * t);
            const float h10 = (1.0f - t) * (1.0f - t) * t;
            const float h11 = tMinusOne * t * t;
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

        const float eased = EaseTransition(mode, t);
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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float h00 = (2.0f * t + 1.0f) * tMinusOne * tMinusOne;
            const float h01 = t * t * (3.0f - 2.0f * t);
            const float h10 = (1.0f - t) * (1.0f - t) * t;
            const float h11 = tMinusOne * t * t;
            const double value =
                (double)h00 * transition->values[0] +
                (double)h01 * transition->values[1] +
                (double)h10 * transition->values[2] +
                (double)h11 * transition->values[3];
            return ZunRoundToInt(value);
        }

        const float eased = EaseTransition(mode, t);
        const int delta = transition->values[1] - transition->values[0];
        return ZunRoundToInt((double)delta * eased + transition->values[0]);
    }

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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float h00 = (2.0f * t + 1.0f) * tMinusOne * tMinusOne;
            const float h01 = t * t * (3.0f - 2.0f * t);
            const float h10 = (1.0f - t) * (1.0f - t) * t;
            const float h11 = tMinusOne * t * t;
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

        const float eased = EaseTransition(mode, t);
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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float h00 = (2.0f * t + 1.0f) * tMinusOne * tMinusOne;
            const float h01 = t * t * (3.0f - 2.0f * t);
            const float h10 = (1.0f - t) * (1.0f - t) * t;
            const float h11 = tMinusOne * t * t;
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

        const float eased = EaseTransition(mode, t);
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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float targetTerm = NormalizeAngle(
                (t * t * (3.0f - 2.0f * t)) * transition->values[1]);
            const float startTerm = NormalizeAngle(
                (tMinusOne * tMinusOne * (2.0f * t + 1.0f)) *
                transition->values[0]);
            float result = NormalizeAngle(startTerm + targetTerm);
            const float startTangentTerm = NormalizeAngle(
                ((1.0f - t) * (1.0f - t) * t) * transition->values[2]);
            result = NormalizeAngle(startTangentTerm + result);
            const float targetTangentTerm = NormalizeAngle(
                (tMinusOne * t * t) * transition->values[3]);
            return NormalizeAngle(targetTangentTerm + result);
        }

        const float eased = EaseTransition(mode, t);
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

        const float t = transition->timer.currentF /
            (float)transition->duration;
        if (mode == 8)
        {
            const float tMinusOne = t - 1.0f;
            const float targetTerm =
                (t * t * (3.0f - 2.0f * t)) * transition->values[1];
            const float startTerm =
                (tMinusOne * tMinusOne * (2.0f * t + 1.0f)) *
                transition->values[0];
            float result = startTerm + targetTerm;
            const float startTangentTerm =
                ((1.0f - t) * (1.0f - t) * t) * transition->values[2];
            result += startTangentTerm;
            const float targetTangentTerm =
                (tMinusOne * t * t) * transition->values[3];
            return result + targetTangentTerm;
        }

        const float eased = EaseTransition(mode, t);
        return (transition->values[1] - transition->values[0]) * eased +
            transition->values[0];
    }

}

__forceinline void AnmVm::UpdateTransitions()
{
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
}

namespace
{
    const float kAnmVmGeometryTwoPi = 6.283185482025146484375f;

    __forceinline void WriteAnmVmGeometryVertexPair(
        AnmVmEffect10Vertex *vertices,
        float centerX,
        float centerY,
        float centerZ,
        float angle,
        float outerRadius,
        float innerRadius,
        unsigned int color,
        float outerU,
        float innerU,
        float v)
    {
        vertices[0].SetPolarForUpdate(angle, outerRadius);
        vertices[0].x += centerX;
        vertices[0].y += centerY;
        vertices[0].z = centerZ;
        vertices[0].reciprocalW = 1.0f;
        vertices[0].color = color;
        vertices[0].u = outerU;
        vertices[0].v = v;

        vertices[1].SetPolarForUpdate(angle, innerRadius);
        vertices[1].x += centerX;
        vertices[1].y += centerY;
        vertices[1].z = centerZ;
        vertices[1].reciprocalW = 1.0f;
        vertices[1].color = color;
        vertices[1].u = innerU;
        vertices[1].v = v;
    }
}

__forceinline void AnmVm::UpdateCustomGeometry()
{
    const unsigned int geometryMode = (flags >> 24) & 0x1FU;
    if ((geometryMode != 9U && geometryMode != 13U && geometryMode != 14U) ||
        extraData == 0)
    {
        return;
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
        return;
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
    const float outerRadius = scaleX * 0.5f + scaleY;
    const float innerRadius = scaleY - scaleX * 0.5f;
    const float outerU = textureRect[0][0] + textureScrollX;
    const float innerU = textureRect[0][2] + textureScrollX;

    if (geometryMode == 9U)
    {
        const unsigned int segmentCount = count - 1U;
        if (segmentCount == 0)
        {
            vertices[0].v = textureScrollY;
            vertices[1].v = textureScrollY;
            return;
        }

        const float angleStep = kAnmVmGeometryTwoPi /
            (float)segmentCount;
        const float vStep = (float)integerVariables[1] /
            (float)segmentCount;
        float angle = unknown02C;
        float v = 0.0f;

        for (unsigned int i = 0; i != segmentCount; ++i)
        {
            WriteAnmVmGeometryVertexPair(
                vertices + i * 2,
                centerX,
                centerY,
                centerZ,
                angle,
                outerRadius,
                innerRadius,
                vertexColor,
                outerU,
                innerU,
                v + textureScrollY);
            v += vStep;
            angle = AddAngles(angle, angleStep);
        }

        AnmVmEffect10Vertex *closingVertices = vertices + segmentCount * 2;
        closingVertices[0] = vertices[0];
        closingVertices[1] = vertices[1];
        closingVertices[0].v = v + textureScrollY;
        closingVertices[1].v = v + textureScrollY;
        return;
    }

    const unsigned int divisor = count - 1U;
    const float angleStep = unknown024 / (float)divisor;
    const float vStep = (float)integerVariables[1] / (float)divisor;
    const float initialAngle = geometryMode == 13U
        ? unknown02C - unknown024 * 0.5f
        : unknown02C;
    float angle = NormalizeAngle(initialAngle);
    float v = 0.0f;

    for (unsigned int i = 0; i != count; ++i)
    {
        WriteAnmVmGeometryVertexPair(
            vertices + i * 2,
            centerX,
            centerY,
            centerZ,
            angle,
            outerRadius,
            innerRadius,
            vertexColor,
            outerU,
            innerU,
            v + textureScrollY);
        v += vStep;
        angle = AddAngles(angle, angleStep);
    }
}


// Keep the frame owner in this translation unit with the script interpreter.
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

    __forceinline void UpdateContinuousMotion(AnmVm *vm, float gameSpeed)
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
        // The target branches to its zero-return epilogue before the shared
        // game-speed restore. Preserve that state leak on this stage gate.
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
        UpdateTextureRectFromViewForFrame(this);
    }

    UpdateTransitions();
    UpdateCustomGeometry();

    if (frameCallback != 0 && frameCallback(this) != 0)
    {
        // The target's callback-termination edge returns directly: it skips
        // both the timer increment and the normal game-speed restoration.
        return 1;
    }

    timer070.Increment();
    *gameSpeed = savedGameSpeed;
    return 0;
}
