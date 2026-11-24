// Non-runnable LTCG context for the four ANM VM operand resolver methods.
#include "../src/AnmVm.hpp"

extern "C" float __stdcall AnmVmOperandsProbe(
    AnmVm *vm, float *floatOperand, int *integerOperand)
{
    const int integerValue = vm->ResolveIntegerOperand(*integerOperand);
    int *resolvedInteger = vm->ResolveIntegerOperandAddress(integerOperand);
    float *resolvedFloat = vm->ResolveFloatOperandAddress(floatOperand);
    return vm->ResolveFloatOperand(*resolvedFloat) +
        (float)integerValue + (float)*resolvedInteger;
}
