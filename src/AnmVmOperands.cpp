#include "AnmVm.hpp"

#include <windows.h>

namespace
{
    const unsigned int kVmOperandBase = 10000;
    const unsigned int kVmIntegerOperandEnd = 10022;

    int RoundToNearestEven(float value)
    {
        if (value != value || value >= 2147483648.0f || value < -2147483648.0f)
        {
            return (int)0x80000000;
        }

        int integral = (int)value;
        float fraction = value - (float)integral;

        if (fraction > 0.5f || (fraction == 0.5f && (integral & 1) != 0))
        {
            ++integral;
        }
        else if (fraction < -0.5f || (fraction == -0.5f && (integral & 1) != 0))
        {
            --integral;
        }
        return integral;
    }

    unsigned int NextVmRandomValue(const AnmVm *vm)
    {
        volatile unsigned int *runtimeFlags =
            reinterpret_cast<volatile unsigned int *>(0x004D33F8);
        volatile unsigned char *lockDepth =
            reinterpret_cast<volatile unsigned char *>(0x004D37A2);
        CRITICAL_SECTION *criticalSection =
            reinterpret_cast<CRITICAL_SECTION *>(0x004D3768);

        const bool useCriticalSection = ((*runtimeFlags & 0x8000U) != 0);
        if (useCriticalSection)
        {
            EnterCriticalSection(criticalSection);
            ++(*lockDepth);
        }

        volatile unsigned short *randomState =
            reinterpret_cast<volatile unsigned short *>(
                (vm->stateFlags & 2U) != 0 ? 0x004D2AD8 : 0x004D2AE0);

        *reinterpret_cast<volatile unsigned int *>(randomState + 2) += 2;

        unsigned short first = (unsigned short)((*randomState ^ 0x9630U) - 0x6553U);
        unsigned short second = (unsigned short)(
            (((unsigned int)(first >> 14) + (unsigned int)first * 4U) ^ 0x9630U)
            - 0x6553U);

        *randomState = (unsigned short)(
            (unsigned int)(second >> 14) + (unsigned int)second * 4U);

        if (useCriticalSection)
        {
            LeaveCriticalSection(criticalSection);
            --(*lockDepth);
        }

        return ((unsigned int)first << 16) | (unsigned int)second;
    }

    double RandomUnit(const AnmVm *vm)
    {
        return (double)NextVmRandomValue(vm) * (1.0 / 4294967296.0);
    }

    float ReadTargetFloat(unsigned int address)
    {
        return *reinterpret_cast<const float *>(address);
    }
}

int AnmVm::ResolveIntegerOperand(int operand)
{
    if ((unsigned int)operand - kVmOperandBase >= 0x17U)
    {
        return operand;
    }

    switch ((unsigned int)operand)
    {
    case 10000:
        return integerVariables[0];
    case 10001:
        return integerVariables[1];
    case 10002:
        return integerVariables[2];
    case 10003:
        return integerVariables[3];
    case 10004:
        return RoundToNearestEven(floatVariables[0]);
    case 10005:
        return RoundToNearestEven(floatVariables[1]);
    case 10006:
        return RoundToNearestEven(floatVariables[2]);
    case 10007:
        return RoundToNearestEven(floatVariables[3]);
    case 10008:
        return integerVariables2[0];
    case 10009:
        return integerVariables2[1];
    case 10022:
        return (int)NextVmRandomValue(this);
    default:
        return operand;
    }
}

float AnmVm::ResolveFloatOperand(float operand)
{
    const int variable = RoundToNearestEven(operand);
    if ((unsigned int)variable - kVmOperandBase >= 0x1AU)
    {
        return operand;
    }

    switch ((unsigned int)variable)
    {
    case 10000:
        return (float)integerVariables[0];
    case 10001:
        return (float)integerVariables[1];
    case 10002:
        return (float)integerVariables[2];
    case 10003:
        return (float)integerVariables[3];
    case 10004:
        return floatVariables[0];
    case 10005:
        return floatVariables[1];
    case 10006:
        return floatVariables[2];
    case 10007:
        return floatVariables[3];
    case 10008:
        return (float)integerVariables2[0];
    case 10009:
        return (float)integerVariables2[1];
    case 10010:
        return (float)((RandomUnit(this) * 2.0 - 1.0) * 3.1415927410125732421875);
    case 10011:
        return (float)RandomUnit(this);
    case 10012:
        return (float)(RandomUnit(this) * 2.0 - 1.0);
    case 10013:
        return secondaryPosition.x;
    case 10014:
        return secondaryPosition.y;
    case 10015:
        return secondaryPosition.z;
    case 10016:
        return ReadTargetFloat(0x004D329C);
    case 10017:
        return ReadTargetFloat(0x004D32A0);
    case 10018:
        return ReadTargetFloat(0x004D32A4);
    case 10019:
        return ReadTargetFloat(0x004D32C0);
    case 10020:
        return ReadTargetFloat(0x004D32C4);
    case 10021:
        return ReadTargetFloat(0x004D32C8);
    case 10022:
        return (float)NextVmRandomValue(this);
    case 10023:
        return unknown024;
    case 10024:
        return unknown028;
    case 10025:
        return unknown02C;
    default:
        return operand;
    }
}

int *AnmVm::ResolveIntegerOperandAddress(int *operand)
{
    switch (*operand)
    {
    case 10000:
        return &integerVariables[0];
    case 10001:
        return &integerVariables[1];
    case 10002:
        return &integerVariables[2];
    case 10003:
        return &integerVariables[3];
    case 10008:
        return &integerVariables2[0];
    case 10009:
        return &integerVariables2[1];
    default:
        return operand;
    }
}

float *AnmVm::ResolveFloatOperandAddress(float *operand)
{
    const int variable = RoundToNearestEven(*operand);
    switch (variable)
    {
    case 10004:
        return &floatVariables[0];
    case 10005:
        return &floatVariables[1];
    case 10006:
        return &floatVariables[2];
    case 10007:
        return &floatVariables[3];
    case 10013:
        return &secondaryPosition.x;
    case 10014:
        return &secondaryPosition.y;
    case 10015:
        return &secondaryPosition.z;
    case 10023:
        return &unknown024;
    case 10024:
        return &unknown028;
    case 10025:
        return &unknown02C;
    default:
        return operand;
    }
}
