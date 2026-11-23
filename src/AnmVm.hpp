#pragma once

#include <stddef.h>
#include "ZunMath.hpp"
#include "ZunTimer.hpp"

struct AnmVm;
struct AnmResource;
struct AnmFileEntry;

struct AnmVmInstructionHeader
{
    unsigned short opcode;
    unsigned short size;
    short frame;
    unsigned char flags;
    unsigned char reserved;
};
typedef char AnmVmInstructionHeaderSize[(sizeof(AnmVmInstructionHeader) == 8) ? 1 : -1];

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
    ZunVec3 scriptVector;                   // +0x030
    unsigned char unknown03C[4];
    float scaleX;                           // +0x040
    float scaleY;                           // +0x044
    float scale2X;                          // +0x048
    float scale2Y;                          // +0x04C
    float unknown050;                       // +0x050
    float unknown054;                       // +0x054
    float unknown058;                       // +0x058, reset to 1.0
    float unknown05C;                       // +0x05C, reset to 1.0
    float uvScrollX;                        // +0x060
    float uvScrollY;                        // +0x064
    float textureScrollX;                  // +0x068
    float textureScrollY;                  // +0x06C
    ZunTimer timer070;                      // +0x070
    float textureRect[2][4];              // +0x084, duplicate source rectangle
    ZunVec3 viewPosition;                   // +0x0A4
    ZunVec3 viewTarget;                     // +0x0B0
    ZunVec3 viewReferenceA;                 // +0x0BC
    ZunVec3 viewReferenceB;                 // +0x0C8
    ZunTimer timer0D4;                      // +0x0D4
    int viewMode;                           // +0x0E8
    int viewFlags;                          // +0x0EC
    unsigned int colorTransitionValues[6];  // +0x0F0..+0x104
    unsigned int colorTransitionOffsets[6]; // +0x108..+0x11C
    ZunTimer timer120;                      // +0x120
    unsigned int colorTransitionDuration;   // +0x134
    unsigned int colorTransitionMode;       // +0x138
    unsigned int effectParameter0;          // +0x13C
    unsigned int effectParameter1;          // +0x140
    unsigned int effectProgress0;           // +0x144
    unsigned int effectProgress1;           // +0x148
    ZunTimer timer14C;                      // +0x14C
    void *effectResource;                   // +0x160
    unsigned int effectIndex;               // +0x164
    ZunVec3 positionTransitionStart;        // +0x168
    ZunVec3 positionTransitionTarget;       // +0x174
    ZunVec3 positionTransitionReferenceA;   // +0x180
    ZunVec3 positionTransitionReferenceB;   // +0x18C
    ZunTimer timer198;                      // +0x198
    unsigned int positionTransitionDuration; // +0x1AC
    unsigned int positionTransitionMode;    // +0x1B0
    unsigned char unknown1B4[0x1C4 - 0x1B4];
    ZunTimer timer1C4;                      // +0x1C4
    unsigned char unknown1D8[0x1E0 - 0x1D8];
    float scaleTransitionStart[2];          // +0x1E0
    float scaleTransitionTarget[2];         // +0x1E8
    unsigned char unknown1F0[0x200 - 0x1F0];
    ZunTimer timer200;                      // +0x200
    unsigned int scaleTransitionDuration;   // +0x214
    unsigned int scaleTransitionMode;       // +0x218
    unsigned char unknown21C[0x23C - 0x21C];
    ZunTimer timer23C;                      // +0x23C
    unsigned char unknown250[8];
    unsigned int secondaryColorTransitionValues[6];  // +0x258..+0x26C
    unsigned int secondaryColorTransitionOffsets[6]; // +0x270..+0x284
    ZunTimer timer288;                      // +0x288
    unsigned int secondaryColorTransitionDuration; // +0x29C
    unsigned int secondaryColorTransitionMode;     // +0x2A0
    unsigned int secondaryEffectParameter0;        // +0x2A4
    unsigned int secondaryEffectParameter1;        // +0x2A8
    unsigned char unknown2AC[0x2B4 - 0x2AC];
    ZunTimer timer2B4;                      // +0x2B4
    void *secondaryEffectResource;           // +0x2C8
    unsigned int secondaryEffectIndex;        // +0x2CC
    unsigned char unknown2D0[0x2E0 - 0x2D0];
    ZunTimer timer2E0;                      // +0x2E0
    unsigned char unknown2F4[0x30C - 0x2F4];
    ZunTimer timer30C;                      // +0x30C
    unsigned char unknown320[8];
    float textureOffsetX;                   // +0x328
    float textureOffsetY;                   // +0x32C
    float transformMatrix[4][4];            // +0x330, reset to identity
    float textureTransformMatrix[4][4];    // +0x370, copied from transformMatrix
    float spriteScaleMatrix[4][4];         // +0x3B0, sprite-to-texture scale
    unsigned int color;                     // +0x3F0, reset to opaque white
    unsigned int secondaryColor;            // +0x3F4
    short currentFrame;                   // +0x3F8
    short unknown3FA;
    ZunTimer timer3FC;                      // +0x3FC
    unsigned int waitScriptAddress;         // +0x410
    int lastScriptFrame;                     // +0x414
    short anmFileIndex;                     // +0x418, -1 when unbound
    short anmFileId;                        // +0x41A
    unsigned char unknown41C[2];
    short anmScriptIndex;                   // +0x41E
    unsigned int scriptDataOffset;          // +0x420
    unsigned int scriptDataOffsetCopy;      // +0x424
    AnmFileEntry *anmFileEntry;             // +0x428
    AnmResource *resource;                  // +0x42C
    int integerVariables[4];                // +0x430..+0x43C
    float floatVariables[4];                // +0x440..+0x44C
    int integerVariables2[2];                // +0x450..+0x454
    ZunVec3 secondaryPosition;              // +0x458
    ZunVec3 position;                       // +0x464
    ZunVec3 alternatePosition;               // +0x470
    unsigned char unknown47C[0x4AC - 0x47C];
    void *extraData;                        // +0x4AC, freed by the destructor
    unsigned int flags;                     // +0x4B0, bit 3 marks the transform dirty
    unsigned int stateFlags;                // +0x4B4, bit 6 set by reset
    unsigned char unknown4B8[0x4C8 - 0x4B8];
    void (__fastcall *stopCallback)(AnmVm *, short); // +0x4C8
    unsigned char unknown4CC[0x4E0 - 0x4CC];
    int (__fastcall *anmFileResolver)(AnmVm *); // +0x4E0
    unsigned int unknown4E4;

    AnmVm();
    void ResetForReuse();
    int BindAnmFile(int fileIndex, AnmResource *resource);
    void SetEffectState(void *resource, unsigned char index, unsigned char parameter0, unsigned char parameter1);
    void SetSecondaryEffectState(void *resource, unsigned int index, unsigned char parameter0, unsigned char parameter1);
    bool ExecuteWaitCommand(bool clearActiveFlag);
    bool ExecuteBasicInstruction(const AnmVmInstructionHeader *instruction);
    unsigned int ExecuteReadyInstructions();

    int ResolveIntegerOperand(int operand);
    float ResolveFloatOperand(float operand);
    int *ResolveIntegerOperandAddress(int *operand);
    float *ResolveFloatOperandAddress(float *operand);
    unsigned int NextRandomValue();
    double RandomUnit();
};
typedef char AnmVmSize[(sizeof(AnmVm) == 0x4E8) ? 1 : -1];
typedef char AnmVmManagerNodeOffset[(offsetof(AnmVm, managerNode) == 0x04) ? 1 : -1];
typedef char AnmVmChildNodeOffset[(offsetof(AnmVm, childNode) == 0x10) ? 1 : -1];
typedef char AnmVmScriptIndexOffset[(offsetof(AnmVm, anmScriptIndex) == 0x41E) ? 1 : -1];
typedef char AnmVmResourceOffset[(offsetof(AnmVm, resource) == 0x42C) ? 1 : -1];
typedef char AnmVmFileEntryOffset[(offsetof(AnmVm, anmFileEntry) == 0x428) ? 1 : -1];
typedef char AnmVmTextureRectOffset[(offsetof(AnmVm, textureRect) == 0x84) ? 1 : -1];
typedef char AnmVmTextureMatrixOffset[(offsetof(AnmVm, textureTransformMatrix) == 0x370) ? 1 : -1];
typedef char AnmVmTextureOffsetX[(offsetof(AnmVm, textureOffsetX) == 0x328) ? 1 : -1];
typedef char AnmVmTextureOffsetY[(offsetof(AnmVm, textureOffsetY) == 0x32C) ? 1 : -1];
typedef char AnmVmSpriteScaleMatrixOffset[(offsetof(AnmVm, spriteScaleMatrix) == 0x3B0) ? 1 : -1];
typedef char AnmVmSecondaryPositionOffset[(offsetof(AnmVm, secondaryPosition) == 0x458) ? 1 : -1];
typedef char AnmVmPositionOffset[(offsetof(AnmVm, position) == 0x464) ? 1 : -1];
typedef char AnmVmIntegerVariablesOffset[(offsetof(AnmVm, integerVariables) == 0x430) ? 1 : -1];
typedef char AnmVmFloatVariablesOffset[(offsetof(AnmVm, floatVariables) == 0x440) ? 1 : -1];
typedef char AnmVmIntegerVariables2Offset[(offsetof(AnmVm, integerVariables2) == 0x450) ? 1 : -1];
typedef char AnmVmStopCallbackOffset[(offsetof(AnmVm, stopCallback) == 0x4C8) ? 1 : -1];
typedef char AnmVmFileResolverOffset[(offsetof(AnmVm, anmFileResolver) == 0x4E0) ? 1 : -1];
typedef char AnmVmLastScriptFrameOffset[(offsetof(AnmVm, lastScriptFrame) == 0x414) ? 1 : -1];
typedef char AnmVmWaitScriptAddressOffset[(offsetof(AnmVm, waitScriptAddress) == 0x410) ? 1 : -1];
typedef char AnmVmScriptVectorOffset[(offsetof(AnmVm, scriptVector) == 0x30) ? 1 : -1];
typedef char AnmVmColorOffset[(offsetof(AnmVm, color) == 0x3F0) ? 1 : -1];
typedef char AnmVmSecondaryColorOffset[(offsetof(AnmVm, secondaryColor) == 0x3F4) ? 1 : -1];
typedef char AnmVmTextureOffsetX[(offsetof(AnmVm, textureOffsetX) == 0x328) ? 1 : -1];
typedef char AnmVmAlternatePositionOffset[(offsetof(AnmVm, alternatePosition) == 0x470) ? 1 : -1];
typedef char AnmVmEffectParameterOffset[(offsetof(AnmVm, effectParameter0) == 0x13C) ? 1 : -1];
typedef char AnmVmEffectResourceOffset[(offsetof(AnmVm, effectResource) == 0x160) ? 1 : -1];
typedef char AnmVmViewPositionOffset[(offsetof(AnmVm, viewPosition) == 0xA4) ? 1 : -1];
typedef char AnmVmViewModeOffset[(offsetof(AnmVm, viewMode) == 0xE8) ? 1 : -1];
typedef char AnmVmColorTransitionValuesOffset[(offsetof(AnmVm, colorTransitionValues) == 0xF0) ? 1 : -1];
typedef char AnmVmColorTransitionOffsetsOffset[(offsetof(AnmVm, colorTransitionOffsets) == 0x108) ? 1 : -1];
typedef char AnmVmPositionTransitionOffset[(offsetof(AnmVm, positionTransitionStart) == 0x168) ? 1 : -1];
typedef char AnmVmPositionTransitionDurationOffset[(offsetof(AnmVm, positionTransitionDuration) == 0x1AC) ? 1 : -1];
typedef char AnmVmScaleTransitionOffset[(offsetof(AnmVm, scaleTransitionStart) == 0x1E0) ? 1 : -1];
typedef char AnmVmScaleTransitionDurationOffset[(offsetof(AnmVm, scaleTransitionDuration) == 0x214) ? 1 : -1];
typedef char AnmVmSecondaryColorTransitionOffset[(offsetof(AnmVm, secondaryColorTransitionValues) == 0x258) ? 1 : -1];
typedef char AnmVmSecondaryEffectResourceOffset[(offsetof(AnmVm, secondaryEffectResource) == 0x2C8) ? 1 : -1];

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetScaleX(AnmVm *vm, float x);
void __stdcall AnmVmSetScaleY(AnmVm *vm, float y);
void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y);
