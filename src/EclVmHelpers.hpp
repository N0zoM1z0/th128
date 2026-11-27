#pragma once

#include "EclResource.hpp"

struct EclInstr
{
    int time;                       // +0x00
    unsigned short opcode;          // +0x04
    unsigned short size;            // +0x06
    unsigned short paramMask;       // +0x08
    unsigned char difficultyMask;   // +0x0A
    unsigned char paramCount;       // +0x0B
    unsigned int unk0C;             // +0x0C
    union
    {
        int iargs[1];
        float fargs[1];
        unsigned char bargs[1];
    };                              // +0x10
};

struct EclStack
{
    unsigned char data[0x1000];     // +0x0000
    int sp;                         // +0x1000
    int bp;                         // +0x1004

    void *GetLocalPtr(int offset);
    void *GetPtr(int offset);
    int GetSp();
    void SetSp(int newSp);
    int Push(const void *val, char typeTag);
    int Pop(void *outVal, char expectedType);
    int AllocFrame(int size);
    int FreeFrame();
};

class EclVm;

struct EclContextSlot
{
    void *ptr;
    unsigned char pad[0x28];
};

struct EclContext
{
    float time;                     // +0x0000
    EclInstr *curInstr;             // +0x0004
    EclStack stack;                 // +0x0008 (size 0x1008)
    int asyncId;                    // +0x1010
    EclVm *owner;                   // +0x1014
    int unk1018;                    // +0x1018
    unsigned char difficultyMask;   // +0x101C
    unsigned char pad101D[0x27];    // +0x101D..+0x1043
    EclContextSlot slots[8];        // +0x1044..+0x11A3 (8 * 0x2C = 0x160)

    EclContext();

    int Run(float deltaTime);
    int CallSub(EclContext *target, int argStartIndex);
    int GetIntArg(int argIndex);
    float GetFloatArg(int argIndex);
    int GetIntVar(int id);
    float GetFloatVar(float id);
    int *GetIntArg0Ptr();
    float *GetFloatArgPtr(int argIndex);
    float *GetFloatArg1Ptr(EclInstr *instr);
};

struct EclContextNode
{
    EclContext *ctx;                // +0x00
    EclContextNode *next;           // +0x04
    EclContextNode *prev;           // +0x08
};

class EclVm
{
public:
    EclVm();
    virtual void OnSubHook();
    virtual int GetIntVar(int id);
    virtual int *GetIntVarPtr(int id);
    virtual float GetFloatVar(int id);
    virtual float *GetFloatVarPtr(int id);

    EclContext *activeContext;      // +0x0004
    EclContext mainContext;         // +0x0008 (size 0x11A4)
    SptResourceInf *resource;       // +0x11AC
    EclContextNode listHead;        // +0x11B0 (size 0x0C)

    SptResourceInf *GetResource();
    void SetResource(SptResourceInf *res);
    void SetCurInstr(EclInstr *instr);
    EclInstr *GetCurInstr();
    void SetTime(float t);

    int RunAllContexts(float deltaTime);
    int SpawnAsyncContext(int asyncId, int argStartIndex);
    EclContextNode *FindAsyncContext(int asyncId);
    void StopAsyncContexts();
    int BindSub(SptResourceInf *res, const char *subName);
    int ResetToSub(SptResourceInf *res, const char *subName);
    int JumpToSub(const char *subName);
};

float __stdcall NormalizeAngleDiff(float a, float b);
void __cdecl EclSubNotFoundLog(const char *fmt, ...);
