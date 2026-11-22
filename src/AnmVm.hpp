#pragma once

// Partial view of the animation VM. Offsets are TH128 target observations;
// field names follow later games' AnmVm and are provisional.
struct AnmVm
{
    unsigned char unknown0[0x40];
    float scaleX;          // +0x40
    float scaleY;          // +0x44
    float scale2X;         // +0x48
    float scale2Y;         // +0x4C
    unsigned char unknown50[0x10];
    float uvScrollX;       // +0x60
    float uvScrollY;       // +0x64
    unsigned char unknown68[0x4B0 - 0x68];
    unsigned int flags;    // +0x4B0; bit 3 marks the transform dirty
};

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetScaleX(AnmVm *vm, float x);
void __stdcall AnmVmSetScaleY(AnmVm *vm, float y);
void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y);
