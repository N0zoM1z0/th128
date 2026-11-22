#pragma once

#include "ZunTimer.hpp"

// Animation VM, 0x4E8 bytes. Offsets are TH128 target observations; field
// names follow later games' AnmVm and are provisional. The thirteen embedded
// timers are the ones the constructor's member initialization touches.
struct AnmVm
{
    unsigned int id;                        // +0x000
    unsigned char unknown004[0x40 - 0x4];
    float scaleX;                           // +0x040
    float scaleY;                           // +0x044
    float scale2X;                          // +0x048
    float scale2Y;                          // +0x04C
    unsigned char unknown050[0x60 - 0x50];
    float uvScrollX;                        // +0x060
    float uvScrollY;                        // +0x064
    unsigned char unknown068[0x70 - 0x68];
    ZunTimer timer070;                      // +0x070
    unsigned char unknown084[0xD4 - 0x84];
    ZunTimer timer0D4;                      // +0x0D4
    unsigned char unknown0E8[0x120 - 0xE8];
    ZunTimer timer120;                      // +0x120
    unsigned char unknown134[0x14C - 0x134];
    ZunTimer timer14C;                      // +0x14C
    unsigned char unknown160[0x198 - 0x160];
    ZunTimer timer198;                      // +0x198
    unsigned char unknown1AC[0x1C4 - 0x1AC];
    ZunTimer timer1C4;                      // +0x1C4
    unsigned char unknown1D8[0x200 - 0x1D8];
    ZunTimer timer200;                      // +0x200
    unsigned char unknown214[0x23C - 0x214];
    ZunTimer timer23C;                      // +0x23C
    unsigned char unknown250[0x288 - 0x250];
    ZunTimer timer288;                      // +0x288
    unsigned char unknown29C[0x2B4 - 0x29C];
    ZunTimer timer2B4;                      // +0x2B4
    unsigned char unknown2C8[0x2E0 - 0x2C8];
    ZunTimer timer2E0;                      // +0x2E0
    unsigned char unknown2F4[0x30C - 0x2F4];
    ZunTimer timer30C;                      // +0x30C
    unsigned char unknown320[0x3FC - 0x320];
    ZunTimer timer3FC;                      // +0x3FC
    unsigned char unknown410[0x418 - 0x410];
    short anmFileIndex;                     // +0x418, -1 when unbound
    unsigned char unknown41A[0x4B0 - 0x41A];
    unsigned int flags;                     // +0x4B0, bit 3 marks the transform dirty
    unsigned char unknown4B4[0x4E8 - 0x4B4];

    AnmVm();
};
typedef char AnmVmSize[(sizeof(AnmVm) == 0x4E8) ? 1 : -1];

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetScaleX(AnmVm *vm, float x);
void __stdcall AnmVmSetScaleY(AnmVm *vm, float y);
void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y);
