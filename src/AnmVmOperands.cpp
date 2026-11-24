#include "AnmVm.hpp"
#include "Rng.hpp"


namespace
{
    const unsigned int kVmOperandBase = 10000;
    const unsigned int kVmIntegerOperandEnd = 10022;


    __forceinline Rng *VmRandomGenerator(const AnmVm *vm)
    {
        if ((vm->stateFlags & 2U) != 0)
        {
            return reinterpret_cast<Rng *>(0x004D2AD8);
        }
        return reinterpret_cast<Rng *>(0x004D2AE0);
    }

    __forceinline unsigned int NextVmRandomValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetU32();
    }

    __forceinline float RandomUnitValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetFloat();
    }

    __forceinline float RandomSignedValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetFloatSigned();
    }

    __forceinline float ReadTargetFloat(unsigned int address)
    {
        return *reinterpret_cast<const float *>(address);
    }
}

int AnmVm::ResolveIntegerOperand(int operand)
{
    const unsigned int variable = (unsigned int)operand - kVmOperandBase;
    switch (variable)
    {
    case 0:
        return integerVariables[0];
    case 1:
        return integerVariables[1];
    case 2:
        return integerVariables[2];
    case 3:
        return integerVariables[3];
    case 4:
        return (int)floatVariables[0];
    case 5:
        return (int)floatVariables[1];
    case 6:
        return (int)floatVariables[2];
    case 7:
        return (int)floatVariables[3];
    case 8:
        return integerVariables2[0];
    case 9:
        return integerVariables2[1];
    case 22:
        if ((stateFlags & 2U) != 0)
        {
            return (int)reinterpret_cast<Rng *>(0x004D2AD8)->GetU32();
        }
        return (int)reinterpret_cast<Rng *>(0x004D2AE0)->GetU32();
    default:
        return operand;
    }
}

__declspec(noinline) float AnmVm::ResolveFloatOperand(float operand)
{
    // Use the same VC10 float-to-int conversion as the target operand helper.
    const int variable = (int)operand - (int)kVmOperandBase;
    switch (variable)
    {
    case 0:
        return (float)integerVariables[0];
    case 1:
        return (float)integerVariables[1];
    case 2:
        return (float)integerVariables[2];
    case 3:
        return (float)integerVariables[3];
    case 4:
        return floatVariables[0];
    case 5:
        return floatVariables[1];
    case 6:
        return floatVariables[2];
    case 7:
        return floatVariables[3];
    case 8:
        return (float)integerVariables2[0];
    case 9:
        return (float)integerVariables2[1];
    case 11:
        if ((stateFlags & 2U) != 0)
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AD8)->GetFloat();
        }
        else
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AE0)->GetFloat();
        }
        return operand;
    case 12:
        if ((stateFlags & 2U) != 0)
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AD8)->GetFloatSigned();
        }
        else
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AE0)->GetFloatSigned();
        }
        return operand;
    case 10:
        if ((stateFlags & 2U) != 0)
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AD8)->GetFloatSigned(3.1415927410125732421875f);
        }
        else
        {
            *reinterpret_cast<volatile float *>(&operand) =
                reinterpret_cast<Rng *>(0x004D2AE0)->GetFloatSigned(3.1415927410125732421875f);
        }
        return operand;
    case 13:
        return secondaryPosition.x;
    case 14:
        return secondaryPosition.y;
    case 15:
        return secondaryPosition.z;
    case 23:
        return unknown024;
    case 24:
        return unknown028;
    case 25:
        return unknown02C;
    case 16:
        return ReadTargetFloat(0x004D329C);
    case 17:
        return ReadTargetFloat(0x004D32A0);
    case 18:
        return ReadTargetFloat(0x004D32A4);
    case 19:
        return ReadTargetFloat(0x004D32C0);
    case 20:
        return ReadTargetFloat(0x004D32C4);
    case 21:
        return ReadTargetFloat(0x004D32C8);
    case 22:
    {
        unsigned int randomValue;
        if ((stateFlags & 2U) != 0)
        {
            randomValue = reinterpret_cast<Rng *>(0x004D2AD8)->GetU32();
        }
        else
        {
            randomValue = reinterpret_cast<Rng *>(0x004D2AE0)->GetU32();
        }
        return (float)randomValue;
    }
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
    const int variable = (int)*operand;
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

unsigned int AnmVm::NextRandomValue()
{
    return NextVmRandomValue(this);
}

double AnmVm::RandomUnit()
{
    return (double)RandomUnitValue(this);
}
