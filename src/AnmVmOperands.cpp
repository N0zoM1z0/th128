#include "AnmVm.hpp"
#include "Rng.hpp"


namespace
{
    const unsigned int kVmOperandBase = 10000;
    const unsigned int kVmIntegerOperandEnd = 10022;


    Rng *VmRandomGenerator(const AnmVm *vm)
    {
        return reinterpret_cast<Rng *>(
            (vm->stateFlags & 2U) != 0 ? 0x004D2AD8 : 0x004D2AE0);
    }

    unsigned int NextVmRandomValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetU32();
    }

    float RandomUnitValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetFloat();
    }

    float RandomSignedValue(const AnmVm *vm)
    {
        return VmRandomGenerator(vm)->GetFloatSigned();
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
        return (int)floatVariables[0];
    case 10005:
        return (int)floatVariables[1];
    case 10006:
        return (int)floatVariables[2];
    case 10007:
        return (int)floatVariables[3];
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
    const int variable = (int)operand;
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
        return RandomSignedValue(this) * 3.1415927410125732421875f;
    case 10011:
        return RandomUnitValue(this);
    case 10012:
        return RandomSignedValue(this);
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
