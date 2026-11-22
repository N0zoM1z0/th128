#include "AnmVm.hpp"

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y)
{
    vm->flags |= 8;
    vm->scaleX = x;
    vm->scaleY = y;
}

void __stdcall AnmVmSetScaleX(AnmVm *vm, float x)
{
    vm->flags |= 8;
    vm->scaleX = x;
}

void __stdcall AnmVmSetScaleY(AnmVm *vm, float y)
{
    vm->flags |= 8;
    vm->scaleY = y;
}

void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y)
{
    vm->flags |= 8;
    vm->scale2X = x;
    vm->scale2Y = y;
}

void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y)
{
    vm->flags |= 8;
    vm->uvScrollX = x;
    vm->uvScrollY = y;
}
