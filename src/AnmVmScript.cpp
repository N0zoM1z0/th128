#include "AnmVm.hpp"
#include "AnmResource.hpp"
#include "AnmManager.hpp"
#include "AngleMath.hpp"

#include <math.h>
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

    double FloorViewCoordinate(double value)
    {
        // FUN_0045F110 routes the view's left/top edge through FUN_0048FDF0,
        // which is the target's floor(double) helper.
        return floor(value);
    }

    void BuildViewCorners(const AnmVm *vm, float corners[4][3])
    {
        const AnmVm *parent = vm->parentVm;
        const bool inheritParent =
            parent != 0 && (vm->stateFlags & 0x100U) == 0;

        float width = vm->uvScrollX * vm->scaleX * vm->scale2X;
        float height = vm->uvScrollY * vm->scaleY * vm->scale2Y;
        float parentX = 0.0f;
        float parentY = 0.0f;
        float parentZ = 0.0f;

        if (inheritParent)
        {
            width *= parent->scaleX * parent->scale2X;
            height *= parent->scaleY * parent->scale2Y;
            parentX = parent->position.x +
                parent->secondaryPosition.x + parent->alternatePosition.x;
            parentY = parent->position.y +
                parent->secondaryPosition.y + parent->alternatePosition.y;
            parentZ = parent->position.z +
                parent->secondaryPosition.z + parent->alternatePosition.z;
        }

        const float centerX = vm->position.x +
            vm->secondaryPosition.x + vm->alternatePosition.x;
        const float centerY = vm->position.y +
            vm->secondaryPosition.y + vm->alternatePosition.y;
        const float centerZ = vm->position.z +
            vm->secondaryPosition.z + vm->alternatePosition.z + parentZ;
        const unsigned int viewMode = (vm->flags >> 24) & 0x1FU;
        const unsigned int horizontalAnchor = (vm->flags >> 20) & 3U;
        const unsigned int verticalAnchor = (vm->flags >> 22) & 3U;

        if (viewMode == 1U)
        {
            float angle = vm->unknown02C;
            if (inheritParent)
            {
                angle += parent->unknown02C;
            }

            const float cosine = (float)cos((double)angle);
            const float sine = (float)sin((double)angle);
            const float originX = centerX + parentX;
            const float originY = centerY + parentY;

            float left;
            float right;
            float top;
            float bottom;
            switch (horizontalAnchor)
            {
            case 0:
                left = -width * 0.5f;
                right = width * 0.5f;
                break;
            case 1:
                left = 0.0f;
                right = width;
                break;
            case 2:
                left = -width;
                right = 0.0f;
                break;
            default:
                left = right = 0.0f;
                break;
            }

            switch (verticalAnchor)
            {
            case 0:
                top = -height * 0.5f;
                bottom = height * 0.5f;
                break;
            case 1:
                top = 0.0f;
                bottom = height;
                break;
            case 2:
                top = -height;
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
                corners[i][0] = originX +
                    cosine * xOffsets[i] - sine * yOffsets[i];
                corners[i][1] = originY +
                    cosine * yOffsets[i] + sine * xOffsets[i];
                corners[i][2] = centerZ;
            }
            return;
        }

        float left;
        float right;
        float top;
        float bottom;
        switch (horizontalAnchor)
        {
        case 0:
        {
            const float centeredLeft = centerX - width * 0.5f;
            left = viewMode == 0U
                ? (float)FloorViewCoordinate((double)centeredLeft)
                : centeredLeft;
            right = left + width;
            break;
        }
        case 1:
            left = centerX;
            right = centerX + width;
            break;
        case 2:
            left = centerX - width;
            right = centerX;
            break;
        default:
            left = right = 0.0f;
            break;
        }

        switch (verticalAnchor)
        {
        case 0:
        {
            const float centeredTop = centerY - height * 0.5f;
            top = viewMode == 0U
                ? (float)FloorViewCoordinate((double)centeredTop)
                : centeredTop;
            bottom = top + height;
            break;
        }
        case 1:
            top = centerY;
            bottom = centerY + height;
            break;
        case 2:
            top = centerY - height;
            bottom = centerY;
            break;
        default:
            top = bottom = 0.0f;
            break;
        }

        left += parentX;
        right += parentX;
        top += parentY;
        bottom += parentY;

        corners[0][0] = left;
        corners[0][1] = top;
        corners[1][0] = right;
        corners[1][1] = top;
        corners[2][0] = left;
        corners[2][1] = bottom;
        corners[3][0] = right;
        corners[3][1] = bottom;
        for (unsigned int i = 0; i != 4; ++i)
        {
            corners[i][2] = centerZ;
        }
    }

    void UpdateTextureRectFromViewForVm(AnmVm *vm)
    {
        float corners[4][3];
        BuildViewCorners(vm, corners);

        float *textureCoordinates = &vm->textureRect[0][0];
        for (unsigned int i = 0; i != 4; ++i)
        {
            const float pixels[2] = {corners[i][0], corners[i][1]};
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
        AnmResource *resource = parent->resource;
        AnmManager *manager = GetGlobalAnmManager();
        if (resource == 0 || manager == 0)
        {
            return 0;
        }

        ++resource->activeVmCount;
        AnmVm *child = AllocateAnmVm();
        if (child == 0)
        {
            return 0;
        }

        child->stateFlags |= 2U;
        child->position.x = 0.0f;
        child->position.y = 0.0f;
        child->position.z = 0.0f;
        child->scriptState = parent->scriptState;
        resource->BindVmScript(child, scriptIndex);
        child->parentVm = parent;

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
        AnmResource *resource = parent->resource;
        AnmManager *manager = GetGlobalAnmManager();
        if (resource == 0 || manager == 0)
        {
            return 0;
        }

        ++resource->activeVmCount;
        AnmVm *child = AllocateAnmVm();
        if (child == 0)
        {
            return 0;
        }

        child->stateFlags |= 2U;
        child->position = parent->position;
        child->scriptState = parent->scriptState;
        resource->BindVmScript(child, scriptIndex);
        child->unknown024 = parent->unknown024;
        child->unknown028 = parent->unknown028;
        child->unknown02C = parent->unknown02C;
        child->alternatePosition = parent->secondaryPosition;
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

void AnmVm::UpdateTextureRectFromView()
{
    UpdateTextureRectFromViewForVm(this);
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
        const float current[3] = {unknown024, unknown028, unknown02C};
        SetPositionTransition(
            target,
            current,
            IntegerSource(this, instruction, 8, 1U),
            (unsigned int)ReadInstructionInteger(instruction, 12));
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
            const unsigned int randomValue = NextRandomValue();
            fileIndex = firstIndex +
                (variantCount != 0 ? (int)(randomValue % variantCount) : 0);
        }
        else
        {
            // Resolve both encoded operands and consume the same random value
            // as the built-in selector before delegating to the VM callback.
            (void)IntegerSource(this, instruction, 8, 1U);
            (void)IntegerSource(this, instruction, 12, 2U);
            NextRandomValue();
            fileIndex = anmFileResolver(this);
        }

        AnmResource *fileResource = resource;
        if (fileIndex < 0)
        {
            fileIndex = 0x139;
            fileResource = GetDefaultAnmResource();
        }

        if (fileResource != 0)
        {
            BindAnmFile(fileIndex, fileResource);
        }
        lastScriptFrame = timer070.current;
        scriptDataOffsetCopy = nextInstruction;
        return true;
    }

    case 93:
    {
        textureOffsetXTransition.duration =
            (unsigned int)IntegerSource(this, instruction, 8, 1U);
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
            (unsigned int)IntegerSource(this, instruction, 8, 1U);
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
            (unsigned int)IntegerSource(this, instruction, 8, 1U);
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

        if (instruction->opcode == 0xFFFFU || instruction->opcode == 1U)
        {
            flags &= ~1U;
            scriptDataOffsetCopy = 0;
            return executed;
        }
        if (instruction->opcode == 2U)
        {
            scriptDataOffsetCopy = 0;
            return executed;
        }
        if (!ExecuteBasicInstruction(instruction))
        {
            break;
        }
        ++executed;
    }

    return executed;
}
