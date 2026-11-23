#pragma once

#include <stddef.h>
#include "ZunMath.hpp"
#include "ZunTimer.hpp"

struct AnmVm;
struct AnmResource;

struct AnmVmListNode
{
    AnmVm *vm;
    AnmVmListNode *next;
    AnmVmListNode *previous;
};
typedef char AnmVmListNodeSize[(sizeof(AnmVmListNode) == 12) ? 1 : -1];

// Animation VM, 0x4E8 bytes. Offsets are TH128 target observations; field
// names follow later games' AnmVm where possible. The two embedded links are
// distinct: managerNode joins the global world/UI lists, while childNode is
// the sentinel/link used to associate child VMs with their parent.
struct AnmVm
{
    unsigned int id;                        // +0x000
    AnmVmListNode managerNode;               // +0x004
    AnmVmListNode childNode;                 // +0x010
    unsigned char unknown01C[0x20 - 0x1C];
    int scriptState;                        // +0x020
    float unknown024;                       // +0x024, script-visible float variable
    float unknown028;                       // +0x028, script-visible float variable
    float unknown02C;                       // +0x02C, script-visible float variable
    unsigned char unknown030[0x40 - 0x30];
    float scaleX;                           // +0x040
    float scaleY;                           // +0x044
    float scale2X;                          // +0x048
    float scale2Y;                          // +0x04C
    unsigned char unknown050[8];
    float unknown058;                       // +0x058, reset to 1.0
    float unknown05C;                       // +0x05C, reset to 1.0
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
    unsigned char unknown320[0x330 - 0x320];
    float transformMatrix[4][4];            // +0x330, reset to identity
    unsigned char unknown370[0x3F0 - 0x370];
    int unknown3F0;                          // +0x3F0, reset to -1
    unsigned char unknown3F4[4];
    short currentFrame;                   // +0x3F8
    short unknown3FA;
    ZunTimer timer3FC;                      // +0x3FC
    unsigned char unknown410[0x418 - 0x410];
    short anmFileIndex;                     // +0x418, -1 when unbound
    short anmFileId;                        // +0x41A
    unsigned char unknown41C[2];
    short anmScriptIndex;                   // +0x41E
    unsigned int scriptDataOffset;          // +0x420
    unsigned int scriptDataOffsetCopy;      // +0x424
    unsigned int unknown428;
    AnmResource *resource;                  // +0x42C
    int integerVariables[4];                // +0x430..+0x43C
    float floatVariables[4];                // +0x440..+0x44C
    int integerVariables2[2];                // +0x450..+0x454
    ZunVec3 secondaryPosition;              // +0x458
    ZunVec3 position;                       // +0x464
    unsigned char unknown470[0x4AC - 0x470];
    void *extraData;                        // +0x4AC, freed by the destructor
    unsigned int flags;                     // +0x4B0, bit 3 marks the transform dirty
    unsigned int stateFlags;                // +0x4B4, bit 6 set by reset
    unsigned char unknown4B8[0x4C8 - 0x4B8];
    void (__fastcall *stopCallback)(AnmVm *, short); // +0x4C8
    unsigned char unknown4CC[0x4E8 - 0x4CC];

    AnmVm();
    void ResetForReuse();

    int ResolveIntegerOperand(int operand);
    float ResolveFloatOperand(float operand);
    int *ResolveIntegerOperandAddress(int *operand);
    float *ResolveFloatOperandAddress(float *operand);
};
typedef char AnmVmSize[(sizeof(AnmVm) == 0x4E8) ? 1 : -1];
typedef char AnmVmManagerNodeOffset[(offsetof(AnmVm, managerNode) == 0x04) ? 1 : -1];
typedef char AnmVmChildNodeOffset[(offsetof(AnmVm, childNode) == 0x10) ? 1 : -1];
typedef char AnmVmScriptIndexOffset[(offsetof(AnmVm, anmScriptIndex) == 0x41E) ? 1 : -1];
typedef char AnmVmResourceOffset[(offsetof(AnmVm, resource) == 0x42C) ? 1 : -1];
typedef char AnmVmSecondaryPositionOffset[(offsetof(AnmVm, secondaryPosition) == 0x458) ? 1 : -1];
typedef char AnmVmPositionOffset[(offsetof(AnmVm, position) == 0x464) ? 1 : -1];
typedef char AnmVmIntegerVariablesOffset[(offsetof(AnmVm, integerVariables) == 0x430) ? 1 : -1];
typedef char AnmVmFloatVariablesOffset[(offsetof(AnmVm, floatVariables) == 0x440) ? 1 : -1];
typedef char AnmVmIntegerVariables2Offset[(offsetof(AnmVm, integerVariables2) == 0x450) ? 1 : -1];
typedef char AnmVmStopCallbackOffset[(offsetof(AnmVm, stopCallback) == 0x4C8) ? 1 : -1];

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetScaleX(AnmVm *vm, float x);
void __stdcall AnmVmSetScaleY(AnmVm *vm, float y);
void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y);
