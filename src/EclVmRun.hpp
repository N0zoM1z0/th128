#pragma once

#include <stddef.h>
#include "ZunMath.hpp"

namespace th128 {

struct EclRunInstr {
    int time;                       // +0x00
    short opcode;                   // +0x04
    unsigned short size;            // +0x06
    unsigned short paramMask;       // +0x08
    unsigned char difficultyMask;   // +0x0A
    unsigned char paramCount;       // +0x0B
    unsigned int unk0C;             // +0x0C
    union {
        int iargs[1];
        float fargs[1];
        unsigned char bargs[1];
    };                              // +0x10
};

struct EclRunStack {
    unsigned char data[0x1000];     // +0x0000
    int sp;                         // +0x1000
    int bp;                         // +0x1004

    int Push(const void *val, char typeTag);
    int Pop(void *outVal, char expectedType);
    int AllocFrame(int size);
    int FreeFrame();
};

struct EclRunInterp {
    float start;                    // +0x00
    float goal;                     // +0x04
    float vel1;                     // +0x08
    float vel2;                     // +0x0C
    int timer[5];                   // +0x10..+0x23
    int duration;                   // +0x24
    int mode;                       // +0x28
};

class EclRunVm;

struct EclRunContext {
    float time;                     // +0x0000
    EclRunInstr *curInstr;          // +0x0004
    EclRunStack stack;              // +0x0008 (size 0x1008)
    int asyncId;                    // +0x1010
    EclRunVm *owner;                // +0x1014
    int unk1018;                    // +0x1018
    unsigned char difficultyMask;   // +0x101C
    unsigned char pad101D[3];       // +0x101D..+0x101F
    EclRunInterp interps[8];        // +0x1020..+0x117F (8 * 0x2C = 0x160)
    EclRunInstr *interpInstrs[8];   // +0x1180..+0x119F
    unsigned int flags;             // +0x11A0

    EclRunContext();

    int Run(float deltaTime);
    int CallSub(EclRunContext *target, int argStartIndex);
    int GetIntArg(int argIndex);
    float GetFloatArg(int argIndex);
    int GetIntVar(int id);
    float GetFloatVar(float id);
    int *GetIntArg0Ptr();
    float *GetFloatArgPtr(int argIndex);
    float *GetFloatArg1Ptr(EclRunInstr *instr);
};

struct EclRunNode {
    EclRunContext *ctx;             // +0x00
    EclRunNode *next;               // +0x04
    EclRunNode *prev;               // +0x08
};

struct EclRunSub {
    const char *name;
    void *code;
};

struct EclRunResource {
    void *vtable;                   // +0x00
    int fileCount;                  // +0x04
    int subCount;                   // +0x08
    void *files[32];                // +0x0C
    EclRunSub *subs;                // +0x8C

    EclRunInstr *FindSub(const char *name);
    EclRunVm *CreateVm(const char *subName);
};

class EclRunVm {
public:
    EclRunVm();
    virtual int OnSubHook();
    virtual int GetIntVar(int id);
    virtual int *GetIntVarPtr(int id);
    virtual float GetFloatVar(int id);
    virtual float *GetFloatVarPtr(int id);

    EclRunContext *activeContext;   // +0x0004
    EclRunContext mainContext;      // +0x0008 (size 0x11A4)
    EclRunResource *resource;       // +0x11AC
    EclRunNode listHead;            // +0x11B0 (size 0x0C)

    int RunAllContexts(float deltaTime);
    int SpawnAsyncContext(int asyncId, int argStartIndex);
    EclRunNode *FindAsyncContext(int asyncId);
    void StopAsyncContexts();
    int JumpToSub(const char *subName);
};

typedef char EclRunContextSizeCheck[(sizeof(EclRunContext) == 0x11A4) ? 1 : -1];
typedef char EclRunVmSizeCheck[(sizeof(EclRunVm) == 0x11BC) ? 1 : -1];

struct EclRunVec3 {
    float x;
    float y;
    float z;

    void FromAngle(float angle, float radius);
};

float __stdcall EclRunNormalizeAngle(float angle);
float __stdcall EclRunNormalizeAngleDiff(float a, float b);
void EclRunRotateVec2(EclRunVec3 *out, const EclRunVec3 *in, float angle);
float EclRunInterpScalar(EclRunInterp *interp);
void EclRunResetInterpTimer(void *interp);
void __cdecl EclRunSubNotFoundLog(const char *fmt, ...);

} // namespace th128
