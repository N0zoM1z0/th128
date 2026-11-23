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

struct AnmVmColorTransition
{
    int values[12];
    ZunTimer timer;
    unsigned int duration;
    unsigned int mode;
};
typedef char AnmVmColorTransitionSize[(sizeof(AnmVmColorTransition) == 0x4C) ? 1 : -1];

struct AnmVmVectorTransition
{
    float values[12];
    ZunTimer timer;
    unsigned int duration;
    unsigned int mode;
};
typedef char AnmVmVectorTransitionSize[(sizeof(AnmVmVectorTransition) == 0x4C) ? 1 : -1];

struct AnmVmScaleTransition
{
    float values[8];
    ZunTimer timer;
    unsigned int duration;
    unsigned int mode;
};
typedef char AnmVmScaleTransitionSize[(sizeof(AnmVmScaleTransition) == 0x3C) ? 1 : -1];

struct AnmVmScalarTransition
{
    float values[4];
    ZunTimer timer;
    unsigned int duration;
    unsigned int mode;
};
typedef char AnmVmScalarTransitionSize[(sizeof(AnmVmScalarTransition) == 0x2C) ? 1 : -1];

struct AnmVmIntegerTransition
{
    int values[4];
    ZunTimer timer;
    unsigned int duration;
    unsigned int mode;
};
typedef char AnmVmIntegerTransitionSize[(sizeof(AnmVmIntegerTransition) == 0x2C) ? 1 : -1];

// Animation VM, 0x4E8 bytes. Offsets are TH128 target observations; field
// names follow later games' AnmVm where possible. The two embedded links are
// distinct: managerNode joins the global world/UI lists, while childNode is
// the sentinel/link used to associate child VMs with their parent.
struct AnmVm
{
    unsigned int id;                        // +0x000
    AnmVmListNode managerNode;               // +0x004
    AnmVmListNode childNode;                 // +0x010
    AnmVm *drawListNext;                    // +0x01C, per-state render-list link
    int scriptState;                        // +0x020
    float unknown024;                       // +0x024, script-visible float variable
    float unknown028;                       // +0x028, script-visible float variable
    float unknown02C;                       // +0x02C, script-visible float variable
    ZunVec3 scriptVector;                   // +0x030
    AnmVm *parentVm;                         // +0x03C, parent for linked child VMs
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
    AnmVmColorTransition colorTransition;   // +0x0F0
    AnmVmIntegerTransition effectTransition; // +0x13C
    AnmVmVectorTransition positionTransition; // +0x168
    AnmVmScalarTransition viewTransition;   // +0x1B4
    AnmVmScaleTransition scaleTransition;   // +0x1E0
    AnmVmScaleTransition secondaryScaleTransition; // +0x21C
    AnmVmColorTransition secondaryColorTransition; // +0x258
    AnmVmIntegerTransition secondaryEffectTransition; // +0x2A4
    AnmVmScalarTransition textureOffsetXTransition; // +0x2D0
    AnmVmScalarTransition textureOffsetYTransition; // +0x2FC
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
    ZunVec3 drawVertices[4];                // +0x47C, cached sprite quad positions
    void *extraData;                        // +0x4AC, freed by the destructor
    unsigned int flags;                     // +0x4B0, bit 3 marks the transform dirty
    unsigned int stateFlags;                // +0x4B4, bit 6 set by reset
    unsigned char unknown4B8[4];
    int (__fastcall *extraDataUpdateCallback)(AnmVm *); // +0x4BC
    void (__fastcall *extraDataDrawCallback)(AnmVm *);   // +0x4C0
    void (__fastcall *onDestroyCallback)(AnmVm *);       // +0x4C4
    void (__fastcall *stopCallback)(AnmVm *, short);     // +0x4C8
    unsigned char unknown4CC[0x4DC - 0x4CC];
    int (__thiscall *frameCallback)(AnmVm *); // +0x4DC, can terminate this update
    int (__fastcall *anmFileResolver)(AnmVm *); // +0x4E0
    unsigned int unknown4E4;

    AnmVm();
    void ResetForReuse();
    int BindAnmFile(int fileIndex, AnmResource *resource);
    void SetEffectState(unsigned int duration, unsigned char mode, unsigned char parameter0, unsigned char parameter1);
    void StartEffect10();
    void SetColorTransition(
        const unsigned char *target,
        const unsigned char *current,
        int duration,
        unsigned char mode);
    void SetScaleTransition(
        const float *target,
        const float *current,
        int duration,
        unsigned char mode);
    void SetSecondaryScaleTransition(
        const float *target,
        const float *current,
        int duration,
        unsigned char mode);
    void SetPositionTransition(
        const float *target,
        const float *current,
        int duration,
        unsigned int mode);
    void SetSecondaryColorTransition(
        const unsigned char *target,
        const unsigned char *current,
        int duration,
        unsigned char mode);
    void SetSecondaryEffectState(unsigned int duration, unsigned int mode, unsigned char parameter0, unsigned char parameter1);
    bool ExecuteWaitCommand(bool clearActiveFlag);
    bool ExecuteBasicInstruction(const AnmVmInstructionHeader *instruction);
    unsigned int ExecuteReadyInstructions();
    void UpdateTransitions();
    void UpdateTextureRectFromView();
    void UpdateCustomGeometry();
    void UpdateWorldMatrix(float matrix[4][4]);
    int Update();

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
typedef char AnmVmDrawListNextOffset[(offsetof(AnmVm, drawListNext) == 0x1C) ? 1 : -1];
typedef char AnmVmDrawVerticesOffset[(offsetof(AnmVm, drawVertices) == 0x47C) ? 1 : -1];
typedef char AnmVmParentVmOffset[(offsetof(AnmVm, parentVm) == 0x3C) ? 1 : -1];
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
typedef char AnmVmExtraDataUpdateCallbackOffset[(offsetof(AnmVm, extraDataUpdateCallback) == 0x4BC) ? 1 : -1];
typedef char AnmVmExtraDataDrawCallbackOffset[(offsetof(AnmVm, extraDataDrawCallback) == 0x4C0) ? 1 : -1];
typedef char AnmVmOnDestroyCallbackOffset[(offsetof(AnmVm, onDestroyCallback) == 0x4C4) ? 1 : -1];
typedef char AnmVmStopCallbackOffset[(offsetof(AnmVm, stopCallback) == 0x4C8) ? 1 : -1];
typedef char AnmVmFrameCallbackOffset[(offsetof(AnmVm, frameCallback) == 0x4DC) ? 1 : -1];
typedef char AnmVmFileResolverOffset[(offsetof(AnmVm, anmFileResolver) == 0x4E0) ? 1 : -1];
typedef char AnmVmLastScriptFrameOffset[(offsetof(AnmVm, lastScriptFrame) == 0x414) ? 1 : -1];
typedef char AnmVmWaitScriptAddressOffset[(offsetof(AnmVm, waitScriptAddress) == 0x410) ? 1 : -1];
typedef char AnmVmScriptVectorOffset[(offsetof(AnmVm, scriptVector) == 0x30) ? 1 : -1];
typedef char AnmVmColorOffset[(offsetof(AnmVm, color) == 0x3F0) ? 1 : -1];
typedef char AnmVmSecondaryColorOffset[(offsetof(AnmVm, secondaryColor) == 0x3F4) ? 1 : -1];
typedef char AnmVmTextureOffsetX[(offsetof(AnmVm, textureOffsetX) == 0x328) ? 1 : -1];
typedef char AnmVmAlternatePositionOffset[(offsetof(AnmVm, alternatePosition) == 0x470) ? 1 : -1];
typedef char AnmVmEffectTransitionOffset[(offsetof(AnmVm, effectTransition) == 0x13C) ? 1 : -1];
typedef char AnmVmViewPositionOffset[(offsetof(AnmVm, viewPosition) == 0xA4) ? 1 : -1];
typedef char AnmVmViewModeOffset[(offsetof(AnmVm, viewMode) == 0xE8) ? 1 : -1];
typedef char AnmVmColorTransitionOffset[(offsetof(AnmVm, colorTransition) == 0xF0) ? 1 : -1];
typedef char AnmVmPositionTransitionOffset[(offsetof(AnmVm, positionTransition) == 0x168) ? 1 : -1];
typedef char AnmVmViewTransitionOffset[(offsetof(AnmVm, viewTransition) == 0x1B4) ? 1 : -1];
typedef char AnmVmScaleTransitionOffset[(offsetof(AnmVm, scaleTransition) == 0x1E0) ? 1 : -1];
typedef char AnmVmSecondaryScaleTransitionOffset[(offsetof(AnmVm, secondaryScaleTransition) == 0x21C) ? 1 : -1];
typedef char AnmVmSecondaryColorTransitionOffset[(offsetof(AnmVm, secondaryColorTransition) == 0x258) ? 1 : -1];
typedef char AnmVmEffectTransitionOffset[(offsetof(AnmVm, effectTransition) == 0x13C) ? 1 : -1];
typedef char AnmVmSecondaryEffectTransitionOffset[(offsetof(AnmVm, secondaryEffectTransition) == 0x2A4) ? 1 : -1];
typedef char AnmVmTextureOffsetXTransitionOffset[(offsetof(AnmVm, textureOffsetXTransition) == 0x2D0) ? 1 : -1];
typedef char AnmVmTextureOffsetYTransitionOffset[(offsetof(AnmVm, textureOffsetYTransition) == 0x2FC) ? 1 : -1];

void ApplyAnmVmRenderState(AnmVm *vm);

void __stdcall AnmVmSetScale(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetScaleX(AnmVm *vm, float x);
void __stdcall AnmVmSetScaleY(AnmVm *vm, float y);
void __stdcall AnmVmSetScale2(AnmVm *vm, float x, float y);
void __stdcall AnmVmSetUvScroll(AnmVm *vm, float x, float y);
